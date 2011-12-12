#import "luaav_app_codepad_cocoa.h"
#import "LuaAVCodepadView.h"

static NSMutableArray *codepadWindows = nil;


@interface CodePadCocoa (Private)
- (void)windowDidResize:(NSNotification *)notification;
@end

@implementation CodePadCocoa

+ (void)initialize {
    if (! codepadWindows) {
        codepadWindows = [[NSMutableArray alloc] init];
	}
}

+ (NSMutableArray *)getCodepadWindows {
	return codepadWindows;
}

#define TABS_HEIGHT	(22)

- (id)initWithContentRect:(NSRect)contentRect {
	if(self = [super initWithContentRect:contentRect 
					styleMask: NSTitledWindowMask | NSClosableWindowMask |
								NSMiniaturizableWindowMask | NSResizableWindowMask
					backing:NSBackingStoreBuffered 
					defer:NO])
	{
		NSRect r = [self contentRectForFrameRect:[self frame]];
	
		[self setTitle: @"Code Pad"];
		tabViewControl = [[PSMTabBarControl alloc] initWithFrame: NSMakeRect(0, r.size.height-TABS_HEIGHT, r.size.width, TABS_HEIGHT)];
		[tabViewControl setStyleNamed:@"Metal"];
		
		tabs = [[NSTabView alloc] initWithFrame:NSMakeRect(0, 0, r.size.width, r.size.height-TABS_HEIGHT)];
		[tabs setAutoresizesSubviews:YES];
		[tabs setTabViewType: NSNoTabsNoBorder];
		
		[tabViewControl setTabView:tabs];
		[tabs setDelegate:tabViewControl];
		
		[[self contentView] addSubview: tabs];
		[[self contentView] addSubview: tabViewControl];
		
		[self setDelegate:self];
		
		// window status
		[[NSNotificationCenter defaultCenter] 
				addObserver:tabViewControl selector:@selector(windowStatusDidChange:) 
				name:NSWindowDidBecomeKeyNotification 
				object:self];
		[[NSNotificationCenter defaultCenter] 
				addObserver:tabViewControl selector:@selector(windowStatusDidChange:) 
				name:NSWindowDidResignKeyNotification 
				object:self];
		/*[[NSNotificationCenter defaultCenter] 
				addObserver:tabViewControl selector:@selector(windowDidMove:) 
				name:NSWindowDidMoveNotification
				object:self];
		*/
	}
	return self;
}

+ (void)addView:(NSView *)view {
	if([codepadWindows count] == 0) {
		NSRect frame;
		frame.origin.x = 500;
		frame.origin.y = 500;
		frame.size.width = 350;
		frame.size.height = 500;
		
		CodePadCocoa *codepad = [[CodePadCocoa alloc] initWithContentRect:frame];		
		[codepadWindows addObject:codepad];

		[codepad release];
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
		CodePadCocoa *c = [codepadWindows objectAtIndex:0];
		NSTabView *tabs = [c getTabs];
		for(int j=0; j < [tabs numberOfTabViewItems]; j++) {
			NSTabViewItem *item = [tabs tabViewItemAtIndex:j];
			if(view == [item view]) {
				[tabs removeTabViewItem:item];
				
				if([tabs numberOfTabViewItems] == 0) {
					[codepadWindows removeObjectAtIndex:i];
				}
				break;
			}
		}
	}
}

- (NSTabView *)getTabs {
	return tabs;
}

- (void)windowDidResize:(NSNotification *)notification {
	NSRect r = [self contentRectForFrameRect:[self frame]];
	[tabViewControl setFrame: NSMakeRect(0, r.size.height-TABS_HEIGHT, r.size.width, TABS_HEIGHT)];
	[tabs setFrame:NSMakeRect(0, 0, r.size.width, r.size.height-TABS_HEIGHT)];
}

- (void) addTab:(NSView *)view {
	// create a tab for the view
	NSString *tabName = nil;
	if([view isKindOfClass:[LuaAVCodepadView class]]) {
		LuaAVCodepadView *cpview = (LuaAVCodepadView *)view;
		tabName = [cpview getLabel];
	}
	else {
		tabName = [NSString stringWithFormat:@"CodePad %d", [tabs numberOfTabViewItems]];
	}
	NSTabViewItem *tvi = [[NSTabViewItem alloc] initWithIdentifier:tabName];	
	[tvi setLabel:tabName];
	[tvi setView:view];

	// at the tab to the tabs view
	[tabs addTabViewItem:tvi];
	[tabs selectTabViewItem:tvi]; // this is optional, but expected behavior
}

- (void)undoText {
//	id view = [[tabs selectedTabViewItem] view];
}

- (void)redoText {
	//	id view = [[tabs selectedTabViewItem] view];
}

- (void)cutText {
	[[[self currentCodepadView] getTextView] cut:self];
}

- (void)copyText {
	[[[self currentCodepadView] getTextView] copy:self];
}

- (void)pasteText {
	[[[self currentCodepadView] getTextView] paste:self];
}

- (void)selectAllText {
	[[[self currentCodepadView] getTextView] selectAll:self];
}

- (void)eval {
	[[self currentCodepadView] eval];
}

- (LuaAVCodepadView *)currentCodepadView {
	return (LuaAVCodepadView *)[[tabs selectedTabViewItem] view];
}

- (void) dealloc {
	[tabViewControl release];
	[tabs release];
	[super dealloc];	
}

@end
