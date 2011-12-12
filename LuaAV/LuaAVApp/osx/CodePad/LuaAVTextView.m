#import "LuaAVTextView.h"

@implementation LuaAVTextView

- (id)initWithFrame:(NSRect)frame {
	if(self = [super initWithFrame:frame]) {
		[[self textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
		
		// this is what turns soft wrapping off... enabling soft wrapping would just involve
		// setting this flag to true
		[[self textContainer] setWidthTracksTextView:NO];
		
		[self setHorizontallyResizable:YES];
		
		[self setAllowsUndo:YES];
		[self setDelegate:self];
		undoManager = [[NSUndoManager alloc] init];
	}
	return self;
}


- (void) drawRect:(NSRect)rect {
	[super drawRect:rect];
	[COLOR(.8,.8,.8,.2) set];
	if(pointer != NULL)
		[NSBezierPath fillRect:*pointer];
}

- (void) highlightRange:(NSRange)range {
	highlightedArea = [self firstRectForCharacterRange:range];
	highlightedArea.size.width = [self frame].size.width;
	pointer = &highlightedArea;
}

- (void) removeHighlight {
	pointer = NULL;
}

- (void) undoTextChange:(NSRange)ramge string:(NSString *)str {
	//if([str isEqualToString:@"\n"]) 
		//[undoManager endUndoGrouping];
}
- (BOOL)textView:(NSTextView *)aTextView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString {
	[[undoManager prepareWithInvocationTarget:self] undoTextChange:affectedCharRange string:replacementString];

	return YES;
}
- (NSUndoManager *)undoManagerForTextView:(NSTextView *)aTextView {
	return undoManager;
}

- (void) dealloc {
	[undoManager release];
	[super dealloc];
}
@end
