#include "luaav_app_codepad.h"
#include "luaav.h"
#include "lua_utility.h"
#include "luaav_app.h"

#import <Cocoa/Cocoa.h>
#import "MarkerLineNumberView.h"
#import "LuaAVCodepadView.h"
#import "LuaAVNSViewCategory.h"
#import "PSMTabBarControl.h"

@interface CodePadCocoa : NSWindow {
	PSMTabBarControl *_tabViewControl;
	NSTabView *_tabs;
}

// class methods
+ (NSMutableArray *)windows;
+ (void)addView:(NSView *)view;
+ (void)removeView:(NSView *)view;

// create
- (id)initWithContentRect:(NSRect)contentRect;

// functionality
- (void) addTab:(NSView *)view;

// properties
- (NSTabView *)tabs;
- (LuaAVCodepadView *)currentCodepadView;

- (void)tabNameDidChange:(NSNotification *)notification;

// menu actions
- (void)undoText;
- (void)redoText;
- (void)cutText;
- (void)copyText;
- (void)pasteText;
- (void)selectAllText;
- (void)eval;


@end

// MyNSViewCategory.h:
@interface NSView (myCustomMethods)

- (void)fixLeftEdge:(BOOL)fixed;
- (void)fixRightEdge:(BOOL)fixed;
- (void)fixTopEdge:(BOOL)fixed;
- (void)fixBottomEdge:(BOOL)fixed;
- (void)fixWidth:(BOOL)fixed;
- (void)fixHeight:(BOOL)fixed;

@end