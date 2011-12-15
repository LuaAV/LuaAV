#import "LuaAVConsole.h"
#import "LuaAVLog.h"
#include "luaav.h"
#include "lua_utility.h"
#include "luaav_app.h"

#if __LP64__ || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

static LuaAVConsole * g_console = nil;


void post_console_error(const char *msg) {
	[LuaAVLog flush];
	//[g_console stdoutRedirect:[NSString stringWithCString:msg encoding:NSASCIIStringEncoding]];
	[g_console postError:[NSString stringWithCString:msg encoding:NSASCIIStringEncoding]];
}


extern const char * luaav_app_path();

@implementation ScriptData 

- initWithName:(NSString *)newName withPath:(NSString *)newPath withObject:(luaav_state*)object {
    self = [super init];
    if (self) {
		[self setName: newName];
		[self setPath: newPath];
		self->state = object;
    }
    return self;
}

- (void)dealloc {
    [name release];
    name = nil;
    [super dealloc];
}

- (void)setName:(NSString *)newName {
    if (name != newName) {
        [name release];
        name = [newName copy];
    }
}

- (void)setPath:(NSString *)newPath {
    if (path != newPath) {
        [path release];
        path = [newPath copy];
    }
}

- (NSString *)name {
    return [[name retain] autorelease];
}

- (NSString *)path {
    return [[path retain] autorelease];
}

- (luaav_state *)state
{
	return state;
}

@end

//@implementation ScriptsTableView
//
//- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
//{
//    if ((NSDragOperationGeneric & [sender draggingSourceOperationMask]) == NSDragOperationGeneric)
//    {
//        return NSDragOperationGeneric;
//    }
//	return NSDragOperationNone;
//}
//
//- (void)draggingEnded:(id <NSDraggingInfo>)sender
//{
//    //we don't do anything in our implementation
//    //this could be ommitted since NSDraggingDestination is an infomal
//        //protocol and returns nothing
//	printf("drag!\n");
//}
//
//- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)flag
//{
//	return YES;
//}
//
//- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
//{
//    return YES;
//}
//
//- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
//{
//    NSPasteboard *paste = [sender draggingPasteboard];
//    NSArray *types = [NSArray arrayWithObjects:NSFilenamesPboardType, NSURLPboardType, nil];
//    NSString *desiredType = [paste availableTypeFromArray:types];
//    NSData *carriedData = [paste dataForType:desiredType];
//    if (nil == carriedData)
//    {
//        printf("drag\n");
//		if ([desiredType isEqualToString:NSFilenamesPboardType])
//        {
//            //we have a list of file names in an NSData object
//            NSArray *fileArray = 
//                [paste propertyListForType:@"NSFilenamesPboardType"];
//                //be caseful since this method returns id.  
//                //We just happen to know that it will be an array.
//            NSString *path = [fileArray objectAtIndex:0];
//                //assume that we can ignore all but the first path in the list
//				
//			return YES;
//        }
//    }
//    return NO;
//}
//
//- (id)initWithFrame:(NSRect)frame
//{
//    self = [super initWithFrame:frame];
//    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, NSURLPboardType, nil]];
//    return self;
//}
//
//- (void)dealloc
//{
//    [self unregisterDraggedTypes];
//    [super dealloc];
//}
//
//@end

@implementation LuaAVConsole

- (id)init
{
	if (self = [super init])
	{
		//[self startLogging];
		isLuaOutputTextViewScrolling = YES;
		isLogging = NO;
		g_console = self;
		luaav_error_handler(post_console_error);
		
		// init script storage list
		scripts = [[NSMutableArray alloc] init];
		selectedScript = -1;
	}
	return self;
}

- (void)initScriptview
{
	[scriptsTable setDataSource:self];
	[scriptsTable setTarget:self];
	[scriptsTable setDoubleAction: @selector(scriptsTableDoubleClick)];
}


+ (id)console
{
	return g_console;
}

- (id)window
{
	return LuaAVWindow;
}

- (void)copyText
{
	[luaOutputTextView copy:self];
}

- (void)selectAllText
{
	[luaOutputTextView selectAll:self];
}

/*
	StdOut redirection to log & NSTextView
*/
- (void)startLogging:(const char *)path
{
	[luaOutputTextView setFont: [NSFont userFixedPitchFontOfSize:11.0]];
	
	// redirect stdout & stderr using this LuaAVResponder as the listener:
	[[LuaAVLog stderrRedirect] addListener:self];
	[[LuaAVLog stdoutRedirect] addListener:self];

	outputLogFile = [[[NSString alloc] initWithUTF8String:path] retain];
	// clear log file
	[@"==== (cleared) ====\n" writeToFile:outputLogFile atomically:YES encoding:NSUTF8StringEncoding error:NULL];
	
	// header:
	NSString * startOutputLogString = [NSString stringWithFormat: @"LuaAV v%@ (%s): %@\nlog: %@\n", 
		[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"],
		luaav_revision(),
		[[NSDate date] descriptionWithCalendarFormat:nil timeZone:nil locale:nil],
		outputLogFile
	];
	[self stdoutRedirect: startOutputLogString];
	
	isLogging = YES;
}

- (void)stopLogging
{
	if(isLogging) {
		[[LuaAVLog stdoutRedirect] readPipe];
		[[LuaAVLog stderrRedirect] readPipe];
		[[LuaAVLog stdoutRedirect] removeListener:self];
		[[LuaAVLog stderrRedirect] removeListener:self];
		isLogging = NO;
	}
}

- (void)postError:(NSString *)text
{
	NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] initWithString:text];
	NSTextView * textView = luaOutputTextView;
	NSTextStorage * outputTextStorage = [textView textStorage];
	
	/*
	NSDictionary *fontTraits = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:400.0], NSFontWeightTrait, [NSNumber numberWithFloat:400.0], NSFontSlantTrait,nil];
	NSDictionary *fontAttributes = [NSDictionary dictionaryWithObjectsAndKeys:fontTraits, NSFontTraitsAttribute, nil];
	NSFontDescriptor *fdesc = [[[luaOutputTextView font] fontDescriptor] fontDescriptorByAddingAttributes:fontAttributes];
	*/
	/*
	NSFontDescriptor *fdesc = [
		[[luaOutputTextView font] fontDescriptor] 
			fontDescriptorByAddingAttributes:
				[NSDictionary dictionaryWithObjectsAndKeys: 
					NSFontTraitsAttribute, 
						[NSDictionary dictionaryWithObjectsAndKeys: 
							NSFontWeightTrait, 
								.0,
							nil
						],
					nil
				]
		];
	*/
	
//	NSFontDescriptor *fdesc = [[luaOutputTextView font] fontDescriptor];
	/*
	NSFont *err_font = [NSFont 
		fontWithDescriptor:fdesc
		size:[[luaOutputTextView font] pointSize]
	];	
	*/
	NSFont *err_font = [luaOutputTextView font];
	
	
	[attributedString 
		addAttribute:NSFontAttributeName
		value:err_font
		range:NSMakeRange(0, [[attributedString string] length])
	];
	
	
	[attributedString
		addAttribute:NSBackgroundColorAttributeName
		value:[NSColor colorWithCalibratedRed:.31 green:.32 blue:.34 alpha:1.]
		range:NSMakeRange(0, [[attributedString string] length])
	];
	[attributedString
		addAttribute:NSForegroundColorAttributeName
		value:[NSColor colorWithCalibratedRed:.878 green:.878 blue:.878 alpha:1.]
		range:NSMakeRange(0, [[attributedString string] length])
	];
	
	
	// append message to text view:
    [outputTextStorage appendAttributedString:attributedString];
	[attributedString release];
	
	// keep storage size reasonable:
	if ([outputTextStorage length] > 30000) {
		[outputTextStorage deleteCharactersInRange:NSMakeRange(0, [outputTextStorage length] - 16384)];
	}
    
	// scroll to end:
	if (isLuaOutputTextViewScrolling) {
		[textView scrollRangeToVisible:NSMakeRange([outputTextStorage length], 0)];
	}
}

- (void)stdoutRedirect:(NSString *)text
{
	/*
	[string addAttribute:NSForegroundColorAttributeName
               value:[NSColor blueColor]
               range:selectedRange];
	*/
	NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] initWithString:text];
	NSTextView * textView = luaOutputTextView;
	NSTextStorage * outputTextStorage = [textView textStorage];
	
	[attributedString addAttribute:NSFontAttributeName
						value:[luaOutputTextView font]
						range:NSMakeRange(0, [[attributedString string] length]) ];
	
	
	// append message to text view:
    [outputTextStorage appendAttributedString:attributedString];
	[attributedString release];
	
	// keep storage size reasonable:
	if ([outputTextStorage length] > 30000)
		[outputTextStorage deleteCharactersInRange:NSMakeRange(0, [outputTextStorage length] - 16384)];
    
	// scroll to end:
	if (isLuaOutputTextViewScrolling)
		[textView scrollRangeToVisible:NSMakeRange([outputTextStorage length], 0)];
    
	FILE * f = fopen([outputLogFile UTF8String], "a");
	if (f) {
		fprintf(f, "%s", [text UTF8String]);
		fclose(f);
	}
}

- (void)stderrWrite:(NSString *)text
{
	NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] initWithString:text];
	NSTextView * textView = luaOutputTextView;
	NSTextStorage * outputTextStorage = [textView textStorage];
	
	[attributedString addAttribute:NSForegroundColorAttributeName
						value:[NSColor colorWithCalibratedRed:0.5 green:0.0 blue:0.0 alpha:1.]
						range:NSMakeRange(0, [[attributedString string] length]) ];
	[attributedString addAttribute:NSFontAttributeName
						value:[luaOutputTextView font]
						range:NSMakeRange(0, [[attributedString string] length]) ];
	
	// append message to text view:
    [outputTextStorage appendAttributedString:attributedString];
	[attributedString release];
	
	// keep storage size reasonable:
	if ([outputTextStorage length] > 30000)
		[outputTextStorage deleteCharactersInRange:NSMakeRange(0, [outputTextStorage length] - 16384)];
    
	// scroll to end:
	if (isLuaOutputTextViewScrolling)
		[textView scrollRangeToVisible:NSMakeRange([outputTextStorage length], 0)];
    
	FILE * f = fopen([outputLogFile UTF8String], "a");
	if (f) {
		fprintf(f, "%s", [text UTF8String]);
		fclose(f);
	}
}

- (void)stderrRedirect:(NSString *)text { 
	printf("%s", [text UTF8String]);
	[self stderrWrite:@"error: "];
	[self stderrWrite: [text stringByAppendingString:@"\n"]];
}

- (void)statusMessage: (const char *)message
{
	[luaStatusTextField setStringValue: [NSString stringWithUTF8String:message]];
}

- (void)clearLog
{
	NSTextView * textView = luaOutputTextView;
	NSTextStorage * outputTextStorage = [textView textStorage];
	[outputTextStorage deleteCharactersInRange:NSMakeRange(0, [outputTextStorage length])];
}

/*
	NOTE: THESE ARE NOT LINKED RIGHT NOW
	
	Delegate methods for NSWindow:
	 Use this one if you want to put up a 'are you sure?' kind of message:
*/
- (BOOL)windowShouldClose:(NSNotification *)aNotification
{
	printf("windowShouldClose\n");
	return YES;

}

- (void)windowWillClose:(NSNotification *)aNotification
{
	[self stopLogging];
}




/*
	Methods for the Interface Builder actions:
*/	
- (IBAction)clearLoggingWindow: (id)sender
{
	[self clearLog];
}

- (BOOL)isLogScrolling
{
	return isLuaOutputTextViewScrolling;
}

- (void)setLogScrolling:(BOOL)scrolling
{
	isLuaOutputTextViewScrolling = scrolling;
	if (isLuaOutputTextViewScrolling) {
		NSTextView * textView = luaOutputTextView;
		NSTextStorage * outputTextStorage = [textView textStorage];
		[textView scrollRangeToVisible:NSMakeRange([outputTextStorage length], 0)];
		
	}
}

- (IBAction)toggleScrollingLoggingWindow: (id)sender
{
	[self setLogScrolling:([self isLogScrolling] == NO)];
}

- (IBAction)help: (id)sender
{
	[[NSWorkspace sharedWorkspace] openFile:
		[[NSString stringWithUTF8String:luaav_app_path()] stringByAppendingPathComponent: @"/extra/doc/index.html"]
	];
}

- (void)inputTextDidChange:(NSNotification *)aNotification
{
	// nothign to do
}

- (IBAction)executeText:(id)sender 
{
	NSString * code = [luaInputTextView stringValue];
	
	if (![code isEqualToString:@""]) {
		const char * src = [code cStringUsingEncoding: NSNonLossyASCIIStringEncoding];
		lua_State *L = NULL;
		
		if([scripts count] > 0) {
			int row = [scriptsTable selectedRow];
			ScriptData *data = (ScriptData *)[scripts objectAtIndex:(NSUInteger)row];
			if (data)
				L = luaav_getstate([data state]);
		}
		else {
			L = luaav_getstate(luaav_app_state());
		}
		
		if(L) {
			if (lua::dostring(L, src)) {
				printf("error: %s\n", lua_tostring(L, -1));
			}
		}
		[luaInputTextView setStringValue:@""]; 
	}
}

/*
	Interface to scriptsTable
*/
#pragma mark scriptsTable

// propagates button callbacks to LuaAV script
- (void)scriptCall:(id)sender withName:(const char *)name
{
	if([scripts count] > 0) {
		int idx = [sender selectedRow];
		[scriptsTable selectColumnIndexes:[NSIndexSet indexSetWithIndex:idx] byExtendingSelection:NO];
		const char * filename = [[[scripts objectAtIndex:idx] name] cStringUsingEncoding:NSNonLossyASCIIStringEncoding];
		lua_State * L = luaav_getstate(luaav_app_state());
		lua_getglobal(L, name);
		if (lua_isfunction(L, -1)) {
			lua::push<const char *>(L, filename);
			lua_pcall(L, 1, 0, 0);
		}
		else {
			lua_pop(L, 1);
		}
	}
}

-(IBAction)closeScript:(id)sender {
	[self scriptCall:sender withName:"fileclose"];
}

- (IBAction)reloadScript:(id)sender {
	[self scriptCall:sender withName:"fileopen"];
}

- (IBAction)editScript:(id)sender {
	[self scriptCall:sender withName:"fileedit"];
}

/* 
	Not sure how to detect what button was double clicked.
	For now, just ignore it.
*/
- (void)scriptsTableDoubleClick {
//	printf("- (void)scriptsTableDoubleClick;\n");
}

- (void)removeScript:(NSString *)name withPath:(NSString *)path {
	for(int i=0; i < [scripts count]; i++) {
		ScriptData *data = (ScriptData *)[scripts objectAtIndex:(NSUInteger)i];
		if([path compare: [data path]] == NSOrderedSame) {
			[scripts removeObjectAtIndex:i];
			int row = [scriptsTable selectedRow];
			if(row >= i) {
				if([scripts count] == 0) {
					[scriptsTable deselectAll:self];
				}
				else {
					int idx = (row-1) < 0 ? 0 : row-1;
					[scriptsTable selectRowIndexes:[NSIndexSet indexSetWithIndex:idx] byExtendingSelection:NO];
				}
			}
			[scriptsTable reloadData];
			break;
		}
	}
}

- (void)addScript:(NSString *)name withPath:(NSString *)path withObject:(luaav_state *)ptr {
	// Append a newly created data object, then reload the table contents.
	ScriptData * scriptData = [[ScriptData alloc] initWithName:name withPath:path withObject:ptr ];
	[scripts addObject:scriptData];
	[scriptData release];
	[scriptsTable reloadData];
	
	int idx = [scripts count]-1;
	[scriptsTable selectRowIndexes:[NSIndexSet indexSetWithIndex:idx] byExtendingSelection:NO];
}

- (NSString *)selectedScriptPath {
	int row = [scriptsTable selectedRow];
	if(row >= 0) {
		ScriptData *data = (ScriptData *)[scripts objectAtIndex:(NSUInteger)row];
		return [data path];
	}
	else {
		return nil;
	}
}

// ---------------------------------------------------------
//  Data source methods
// ---------------------------------------------------------

- (int)numberOfRowsInTableView:(NSTableView *)tv {
	int x = [scripts count];
    return x;
}

- (id)tableView:(NSTableView *)tv objectValueForTableColumn:(NSTableColumn *)tc row:(int)row {
	if ([[tc identifier] isEqualToString:@"Script"]) {
		return [[scripts objectAtIndex:row] name];
    }
	return nil;
}

- (IBAction)tableViewSelected:(id)sender {
    selectedScript = [sender selectedRow];
}

// this gets called when a user edits the table view:
- (void)tableView:(NSTableView *)tv setObjectValue:(id)objectValue forTableColumn:(NSTableColumn *)tc row:(int)row {
//	printf("setObjectValue %i %s\n", row, [tc identifier]);
//    if ([[tc identifier] isEqualToString:@"Script"]) {
//        [[scripts objectAtIndex:row] setTime: objectValue];
////    } else {
////        [[scripts objectAtIndex:row] setInfo: objectValue];
//    }
}

@end

#pragma mark Lua API


int app_console_startlogging(lua_State * L) {
	const char * path = luaL_checkstring(L, 1);
	[[LuaAVConsole console] startLogging: path ];	
	return 0;
}

int app_console_stoplogging(lua_State * L) {
	[[LuaAVConsole console] stopLogging ];
	return 0;
}

int app_console_clearlog(lua_State * L) {
	[[LuaAVConsole console] clearLog];
	return 0;
}

int app_console_scrolling(lua_State * L) {
	if (lua::is<bool>(L, 1)) {
		[[LuaAVConsole console] setLogScrolling: lua_toboolean(L, 1)];
	}
	lua_pushboolean(L, [[LuaAVConsole console] isLogScrolling] == YES);
	return 1;
}

int app_console_statusbar(lua_State * L) {
	[[LuaAVConsole console] statusMessage:luaL_checkstring(L, 1)];
	return 0;
}

int app_console_addscript(lua_State * L) {
	NSString *filename = [NSString stringWithCString:luaL_checkstring(L, 1) encoding:NSASCIIStringEncoding];
	[[LuaAVConsole console] addScript:filename withPath:filename withObject:(luaav_state *)lua_touserdata(L, 2)];
	return 0;
}

int app_console_removescript(lua_State * L) {
	NSString *filename = [NSString stringWithCString:luaL_checkstring(L, 1) encoding:NSASCIIStringEncoding];
	[[LuaAVConsole console] removeScript:filename withPath:filename];
	return 0;
}

int app_console_selectedscript(lua_State * L) {
	NSString *filename = [[LuaAVConsole console] selectedScriptPath];
	if(filename) {
		lua::push<const char *>(L, (const char *)[filename cStringUsingEncoding: NSASCIIStringEncoding]);
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

int app_console_isfocused(lua_State * L) {
	id window = [[LuaAVConsole console] window];
	if(window) {
		lua_pushboolean(L, [window isKeyWindow] ? 1 : 0);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int app_console_copy(lua_State * L) {
	[[LuaAVConsole console] copyText];
	return 0;
}

int app_console_selectall(lua_State * L) {
	[[LuaAVConsole console] selectAllText];
	return 0;
}

int app_console_rect(lua_State * L) {
	NSRect screenframe = [[[[LuaAVConsole console] window] screen] frame];
	if(lua_istable(L, 1)) {
		NSRect r;
		lua_rawgeti(L, 1, 1);
		r.origin.x = lua_tonumber(L, -1);
		lua_rawgeti(L, 1, 2);
		r.origin.y = lua_tonumber(L, -1);
		lua_rawgeti(L, 1, 3);
		r.size.width = lua_tonumber(L, -1);
		lua_rawgeti(L, 1, 4);
		r.size.height = lua_tonumber(L, -1);
		lua_pop(L, 4);
		
		r.origin.y = (screenframe.size.height + screenframe.origin.y) - (r.size.height + r.origin.y);
		[[[LuaAVConsole console] window] setFrame:r display:YES];
		
		return 0;
	}
	else {
		NSRect r = [[[LuaAVConsole console] window] frame];	
		r.origin.y = (screenframe.size.height + screenframe.origin.y) - (r.size.height + r.origin.y);
		
		lua_newtable(L);
		lua_pushnumber(L, r.origin.x);
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, r.origin.y);
		lua_rawseti(L, -2, 2);
		lua_pushnumber(L, r.size.width);
		lua_rawseti(L, -2, 3);
		lua_pushnumber(L, r.size.height);
		lua_rawseti(L, -2, 4);
		return 1;
	}
}

int luaopen_app_console(lua_State * L) {
	const char * libname = lua_tostring(L, 1);
	struct luaL_reg lib[] = {
		{ "startlogging", app_console_startlogging },
		{ "stoplogging", app_console_stoplogging },
		{ "clearlog", app_console_clearlog },
		{ "scrolling", app_console_scrolling },
		{ "statusbar", app_console_statusbar },
		{ "addscript", app_console_addscript },
		{ "removescript", app_console_removescript },
		{ "selectedscript", app_console_selectedscript },
		{ "isfocused", app_console_isfocused },
		{ "copy", app_console_copy },
		{ "selectall", app_console_selectall },
		{ "rect", app_console_rect },
		{ NULL, NULL },
	};
	luaL_register(L, libname, lib);
	return 1;
}

