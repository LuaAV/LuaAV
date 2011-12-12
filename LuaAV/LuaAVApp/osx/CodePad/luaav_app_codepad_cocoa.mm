#import "luaav_app_codepad_cocoa.h"
#import "LuaAVCodepadView.h"
#import "luaav_app_screens_osx.h"

#define TABS_HEIGHT	(20)


static NSMutableArray *codepadWindows = nil;


@interface CodePadCocoa (Private)
- (void)windowDidResize:(NSNotification *)notification;
@end

@implementation CodePadCocoa

// class methods
+ (void)initialize {
    if (! codepadWindows) {
        codepadWindows = [[NSMutableArray alloc] init];
	}
}

+ (NSMutableArray *)windows {
	return codepadWindows;
}

- (void)windowWillClose:(NSNotification *)notification {
	NSWindow *closedWindow = [notification object];
	for(int i = 0; i < [codepadWindows count]; i++) {
		NSWindow *win = [codepadWindows objectAtIndex:i];
		if (closedWindow == win) 
			[codepadWindows removeObject:win];
	}	
}

+ (void)addView:(NSView *)view {
	if([codepadWindows count] == 0) {
		NSScreen *main = luaav::screens::main_screen();
		NSRect main_frame = [main frame];
		NSRect frame;
		frame.origin.x = 500;
		frame.origin.y = main_frame.size.height-550;
		frame.size.width = 350;
		frame.size.height = 500;
		
		CodePadCocoa *codepad = [[CodePadCocoa alloc] initWithContentRect:frame];		
		[codepadWindows addObject:codepad];

		//[codepad release];
	}
	CodePadCocoa *c = [codepadWindows objectAtIndex:0];
	[c addTab:view];
	
	[view setAutoresizesSubviews:YES];
	[view fixLeftEdge:YES];
	[view fixTopEdge:YES];
	[view fixWidth:NO];
	[c makeKeyAndOrderFront:nil];
}

+ (void)removeView:(NSView *)view {
	for(int i=0; i < [codepadWindows count]; i++) {
		CodePadCocoa *c = [codepadWindows objectAtIndex:i];
		NSTabView *tabs = [c tabs];
		for(int j=0; j < [tabs numberOfTabViewItems]; j++) {
			NSTabViewItem *item = [tabs tabViewItemAtIndex:j];
			if(view == [item view]) {
				[tabs removeTabViewItem:item];
				
				if([tabs numberOfTabViewItems] == 0) {
					[c close];
				}
				break;
			}
		}
	}
}


// create
- (id)initWithContentRect:(NSRect)contentRect {
	if(self = [super initWithContentRect:contentRect 
					styleMask: NSTitledWindowMask | NSClosableWindowMask |
								NSMiniaturizableWindowMask | NSResizableWindowMask
					backing:NSBackingStoreBuffered 
					defer:NO])
	{
		NSRect r = [self contentRectForFrameRect:[self frame]];
	
		[self setTitle: @"Code Pad"];
		_tabViewControl = [[PSMTabBarControl alloc] initWithFrame: NSMakeRect(0, r.size.height-TABS_HEIGHT, r.size.width, TABS_HEIGHT)];
		[_tabViewControl setStyleNamed:@"Metal"];
		
		_tabs = [[NSTabView alloc] initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height-TABS_HEIGHT)];
		[_tabs setAutoresizesSubviews:YES];
		[_tabs setTabViewType: NSNoTabsNoBorder];
		
		[_tabViewControl setTabView:_tabs];
		[_tabs setDelegate:_tabViewControl];
		
		[[self contentView] addSubview: _tabs];
		[[self contentView] addSubview: _tabViewControl];
		
		[[self contentView] setPostsFrameChangedNotifications:YES];
		[[NSNotificationCenter defaultCenter] 
				addObserver:_tabViewControl selector:@selector(frameDidChange:) 
				name:NSViewFrameDidChangeNotification
				object:[self contentView]];
		
		[[NSNotificationCenter defaultCenter] 
			addObserver:self selector:@selector(tabNameDidChange:) 
			name:@"Tab Name Change"
			object:nil];
		
		[self setDelegate:self];
		
		// window status
		[[NSNotificationCenter defaultCenter] 
				addObserver:_tabViewControl selector:@selector(windowStatusDidChange:) 
				name:NSWindowDidBecomeKeyNotification 
				object:self];
		[[NSNotificationCenter defaultCenter] 
				addObserver:_tabViewControl selector:@selector(windowStatusDidChange:) 
				name:NSWindowDidResignKeyNotification 
				object:self];
		/*[[NSNotificationCenter defaultCenter] 
				addObserver:_tabViewControl selector:@selector(windowDidMove:) 
				name:NSWindowDidMoveNotification
				object:self];
		*/
	}
	return self;
}

// delegate methods
- (void)windowDidResize:(NSNotification *)notification {
	NSRect r = [self contentRectForFrameRect:[self frame]];
	[_tabViewControl setFrame: NSMakeRect(0, r.size.height-TABS_HEIGHT, r.size.width, TABS_HEIGHT)];
	[_tabs setFrame:NSMakeRect(0, 0, r.size.width, r.size.height-TABS_HEIGHT)];
}


// functionality
- (void) addTab:(NSView *)view {
	// create a tab for the view
	NSString *tabName = nil;
	if([view isKindOfClass:[LuaAVCodepadView class]]) {
		LuaAVCodepadView *cpview = (LuaAVCodepadView *)view;
		tabName = [cpview label];
	}
	else {
		tabName = [NSString stringWithFormat:@"CodePad %d", [_tabs numberOfTabViewItems]];
	}
	NSTabViewItem *tvi = [[NSTabViewItem alloc] initWithIdentifier:tabName];	
	[tvi setLabel:tabName];
	[tvi setView:view];

	// at the tab to the tabs view
	[_tabs addTabViewItem:tvi];
	[_tabs selectTabViewItem:tvi]; // this is optional, but expected behavior
}

- (void)tabNameDidChange:(NSNotification *)notification {
	LuaAVCodepadView *v = (LuaAVCodepadView *)[notification object];
	for(int i = 0; i < [[_tabs tabViewItems] count]; i++) {
		NSTabViewItem *tvi = [[_tabs tabViewItems] objectAtIndex:0];
		
		if(v == [tvi view]) {
			[tvi setLabel:[v label]];
		}
	}
}



// properties
- (NSTabView *)tabs {
	return _tabs;
}

- (LuaAVCodepadView *)currentCodepadView {
	LuaAVCodepadView *cv = (LuaAVCodepadView *)[[_tabs selectedTabViewItem] view];
	return cv;
	
}


// menu actions
- (void)undoText {
	LuaAVCodepadView *current = [self currentCodepadView];
	LuaAVTextView *tv = [current textView];
	
	[[tv undoManager] undo];
	
	//	id view = [[tabs selectedTabViewItem] view];
}

- (void)redoText {
	LuaAVCodepadView *current = [self currentCodepadView];
	LuaAVTextView *tv = [current textView];
	
	[[tv undoManager] redo];
	//	id view = [[tabs selectedTabViewItem] view];
}

- (void)cutText {
	[[[self currentCodepadView] textView] cut:self];
}

- (void)copyText {
	[[[self currentCodepadView] textView] copy:self];
}

- (void)pasteText {
	[[[self currentCodepadView] textView] paste:self];
}

- (void)selectAllText {
	[[[self currentCodepadView] textView] selectAll:self];
}

- (void)eval {
	[[self currentCodepadView] eval];
}

- (void) dealloc {
	//TODO: why arent the views automatically being released?
	NSTabView *tabs = [self tabs];
	for(int j=0; j < [tabs numberOfTabViewItems]; j++) {
		NSTabViewItem *item = [tabs tabViewItemAtIndex:j];
		LuaAVCodepadView *v = (LuaAVCodepadView *)[item view];
		[tabs removeTabViewItem:item];
		[v detach];
	}

	[_tabViewControl release];
	[_tabs release];
	[super dealloc];	
}

@end
