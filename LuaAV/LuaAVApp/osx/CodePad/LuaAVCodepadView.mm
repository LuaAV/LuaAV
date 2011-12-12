#import "LuaAVCodepadView.h"
#import "luaav_app_codepad_cocoa.h"

#if __LP64__ || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

namespace luaav {
	
class CodePadImpl{
public:
	typedef CodePad		Generic;
	
	CodePadImpl(Generic *generic)
	:	mGeneric(generic),
		mCodePad(0)
	{}
	
	~CodePadImpl() {}
	
	Generic * generic() {return mGeneric;}
	
public:
	Generic *mGeneric;
	LuaAVCodepadView *mCodePad;
};
	
}	// luaav::

@implementation LuaAVCodepadView
@synthesize formats;

- (id)initWithFrame:(NSRect)frame name:(NSString *)aName;
{
	if(self = [super initWithFrame:frame]){
		// name and label
		NSArray *path = [aName pathComponents];
		_name = [[NSString alloc] initWithString:aName];
		if([path count] >= 1) {
			_label = [[NSString alloc] initWithString:[path objectAtIndex:[path count]-1]];
		}
		else {
			_label = [[NSString alloc] initWithString:_name];
		}
		
		// text highlighting formats
		formats = [[NSMutableDictionary alloc] init];
		
		// scrollview
		NSRect scrollRect = NSMakeRect(0, 0, frame.size.width, frame.size.height);
		scrollView = [[NSScrollView alloc] initWithFrame:scrollRect];
		
		// text view
		_textView = [[LuaAVTextView alloc] initWithFrame:scrollRect];
		[scrollView setDocumentView:_textView];
		
		textStorage = [_textView textStorage];
		[textStorage setDelegate:self];
		
		// line numbers
		lineNumberView = [[MarkerLineNumberView alloc] initWithScrollView:scrollView];
		
		// scroll view properties
		[scrollView setVerticalRulerView:lineNumberView];
		[scrollView setHasHorizontalRuler:NO];
		[scrollView setHasVerticalRuler:YES];
		[scrollView setRulersVisible:YES];
		[scrollView setHasVerticalScroller:YES];
		[scrollView fixLeftEdge:YES];
		[scrollView fixTopEdge:NO];
		[scrollView fixWidth:NO];
		[scrollView fixHeight:NO];
		
		// view properties
		[self fixLeftEdge:YES];
		[self fixTopEdge:NO];
		[self fixWidth:NO];
		[self fixHeight:NO];
		
		[self addSubview:scrollView];
	}
	
	return self;
}

// properties
- (NSString *)label {
	return _label;
}

- (NSString *)name {
	return _name;
}

- (void)setName:(NSString *)aName {
	if(_name != nil) [_name release];
	_name = [[NSString alloc] initWithString:aName];
	
	NSArray *path = [aName pathComponents];
	
	if(_label != nil) [_label release];
	if([path count] >= 1) {
		_label = [[NSString alloc] initWithString:[path objectAtIndex:[path count]-1]];
	}
	else {
		_label = [[NSString alloc] initWithString:_name];
	}
	[[NSNotificationCenter defaultCenter] postNotificationName:@"Tab Name Change" object:self userInfo:nil];
}

- (LuaAVTextView *)textView {
	return _textView;
}

- (void)setText:(const char *)text {
	NSRange range = NSMakeRange(0, [textStorage length]);
	[textStorage replaceCharactersInRange:range withString:[NSString stringWithUTF8String:text]];
}

- (NSString *)getText {
	return [textStorage string];
}


// functionality
- (void)format:(const char *)aName fromStart:(int)start toEnd:(int)end {
	NSDictionary *format = [formats objectForKey:[NSString stringWithCString:aName encoding:0]];
	NSRange r = NSMakeRange(start, end - start);
	[textStorage addAttribute:NSForegroundColorAttributeName 
						value:[format objectForKey:NSForegroundColorAttributeName] range:r];
}

- (void)insertText:(const char *)text atPos:(int)pos {
	NSAttributedString *s = [[NSAttributedString alloc] initWithString:[NSString stringWithUTF8String:text]];
	[textStorage insertAttributedString:s atIndex:(NSUInteger)pos];
	[s release];
}


- (int)cursorPos {
	return [[[_textView selectedRanges] objectAtIndex:0] rangeValue].location;
}

- (luaav::CodePadImpl *)parent {
	return _parent;
}

- (void)setParent:(luaav::CodePadImpl *)p {
	_parent = p;
}

- (void)getSelectionWithLine:(int *)line1 pos:(int *)pos1 line:(int *)line2 pos:(int *)pos2 {
	NSRange range = [_textView selectedRange];
	
	*line1 = [lineNumberView lineNumberForCharacterIndex:range.location 
												  inText:[textStorage string]];
	*line2 = [lineNumberView lineNumberForCharacterIndex:range.location+range.length 
												  inText:[textStorage string]];
	
	NSRange l1 = NSMakeRange(range.location, 0);
	NSRange line1_range = [[textStorage string] lineRangeForRange:l1];
	
	NSRange l2 = NSMakeRange(range.location+range.length, 0);
	NSRange line2_range = [[textStorage string] lineRangeForRange:l2];
	
	*pos1 = range.location - line1_range.location;
	*pos2 = range.location + range.length - line2_range.location;
}


- (void)textStorageDidProcessEditing:(NSNotification *)notification {
	int prev_nlines = [lineNumberView lineCount];
	[lineNumberView handleTextChanged];
	int nlines = [lineNumberView lineCount];
	int nlines_changed = nlines - prev_nlines;
	
	if(_parent) {
		NSRange editedRange = [textStorage editedRange];
		int line1 = [lineNumberView lineNumberForCharacterIndex:editedRange.location 
														 inText:[textStorage string]];
		int line2 = [lineNumberView lineNumberForCharacterIndex:editedRange.location+editedRange.length 
														 inText:[textStorage string]];
		
		if(nlines_changed > 1) {
			_parent->generic()->insert(line1, nlines_changed);
		}
		else if(nlines_changed < 0) {
			_parent->generic()->remove(line1, nlines_changed);
		}
		
		NSRange range1 = NSMakeRange(editedRange.location, 0);
		range1 = [[textStorage string] lineRangeForRange:range1];
		
		NSString *s = [[textStorage string] substringWithRange:range1];
		_parent->generic()->highlight(line1, [s UTF8String]);
		
		for(int i=line1+1; i <= line2; i++) {
			// get line string
			NSRange rn = NSMakeRange(range1.location+range1.length, 0);
			NSRange r = [[textStorage string] lineRangeForRange:rn];
			NSString *s = [[textStorage string] substringWithRange:r];
			
			// callback into Lua
			_parent->generic()->highlight(i, [s UTF8String]);
			
			// save for next iteration
			range1 = r;
		}
	}
	[_textView removeHighlight];
}


-(void)eval {
	if(_parent) {
			NSString *code;
		if([_textView selectedRange].length != 0) {
			code = [[_textView string] substringWithRange:[_textView selectedRange]];
		}else{
			NSRange editedRange = [textStorage editedRange];
			NSScanner *scanner = [NSScanner scannerWithString:[_textView string]];
			int line = [lineNumberView lineNumberForLocation:editedRange.location];
			int indexStart = [lineNumberView characterIndexForLine:line];
			[scanner setScanLocation:indexStart];
			[scanner scanUpToCharactersFromSet:[NSCharacterSet newlineCharacterSet] intoString:&code];
		}
		_parent->generic()->eval([code cStringUsingEncoding: NSASCIIStringEncoding]);
	}
}

- (void)detach {
	if(_parent) {
		_parent->mCodePad = 0;
	}
}

- (void) dealloc {
	[self detach];

	[_name release];
	[_label release];
	[lineNumberView release];
	[formats release];
	[_textView release];
	[scrollView release];
	[super dealloc];	
}

@end


#define mCocoaCodePad (mImpl->mCodePad)

namespace luaav {
	

void CodePad :: implInit() {
	mImpl = new Implementation(this);
}

void CodePad :: implDestruct() {
	delete mImpl;
	mImpl = 0;
}


void CodePad :: implCreate(const char *name, int x, int y, int w, int h) {
	if(!mCocoaCodePad) {
		mCocoaCodePad = [[LuaAVCodepadView alloc] initWithFrame:NSMakeRect(0, 0, w, h) name:[NSString stringWithUTF8String:name]];
		[mCocoaCodePad setParent:mImpl];
	
		[CodePadCocoa addView:mCocoaCodePad];
		[mCocoaCodePad release];
	}
}

void CodePad :: implDestroy() {
	if(mCocoaCodePad) {
		[CodePadCocoa removeView:mCocoaCodePad];
		[mCocoaCodePad release];
	}
}

const char * CodePad :: implGetCurrentName() {
	CodePadCocoa *win = (CodePadCocoa *)[[CodePadCocoa windows] objectAtIndex:0];
	if(win) {
		return [[[win currentCodepadView] name] UTF8String];
	}
	else {
		return "";
	}
}

void CodePad :: implSetName(const char *name) {
	if(mCocoaCodePad) {
		[mCocoaCodePad setName:[NSString stringWithUTF8String:name]];
	}
}

const char * CodePad :: implGetName() {
	if(mCocoaCodePad) {
		return [[mCocoaCodePad name] UTF8String];
	}
	else {
		return "";
	}
}

void CodePad :: implSetText(const char *text) {
	if(mCocoaCodePad) {
		[mCocoaCodePad setText:text];
	}
}

std::string CodePad :: implGetText() {
	if(mCocoaCodePad) {
		return std::string([[mCocoaCodePad getText] UTF8String]);
	}
	else {
		return std::string();
	}
}

// name = token type, color, name / value
void CodePad :: implCreateFormat(const char *name, float *color, std::map<std::string, int> &properties) {
	if(mCocoaCodePad) {
		NSMutableDictionary *props = [NSMutableDictionary dictionary];
		std::map<std::string, int>::iterator it = properties.begin();
		for(; it != properties.end(); ++it) {
			if(it->first == std::string("font_weight")) {
				//[props setObject:[NSNumber numberWithInt:it->second forKey:TODO:weightKey];
			}
			else if(it->first == std::string("italic")) {
				[props setObject:[NSNumber numberWithInt:it->second] forKey:NSObliquenessAttributeName];		
			}
			else if(it->first == std::string("underline")) {
				[props setObject:[NSNumber numberWithInt:it->second] forKey:NSUnderlineStyleAttributeName];
			}
		}
		[props setObject:[NSColor colorWithCalibratedRed:color[0] green:color[1] blue:color[2] alpha:1.0f] forKey:NSForegroundColorAttributeName];
		[[mCocoaCodePad formats] setObject:props forKey:[NSString stringWithCString:name encoding:0]];
	}
}			


void CodePad :: implFormat(const char *name, int start, int end) {
	if(mCocoaCodePad) {
		[mCocoaCodePad format:name fromStart:start toEnd:end];
	}
}

void CodePad :: implGetSelection(int &line1, int &pos1, int &line2, int &pos2) {
	if(mCocoaCodePad) {
		[mCocoaCodePad getSelectionWithLine: &line1 pos: &pos1 line: &line2 pos: &pos2];
	}
}

void CodePad :: implErrorHighlight(int line) {
	
}

void CodePad :: implClearErrorHighlight() {
	
}

void CodePad :: implPaste(int pos, const char *text) {
	if(mCocoaCodePad) {
		[mCocoaCodePad insertText:text atPos:pos];
	}
}
	
int CodePad :: implCursorPos() {
	if(mCocoaCodePad) {
		return [mCocoaCodePad cursorPos];
	}
	else {
		return 0;
	}
}
	
}	// luaav::
