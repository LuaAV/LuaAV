#include "luaav_app_window.h"
#include "luaav_app_screens.h"
#include "luaav_app_screens_osx.h"
#include <map>
#include <string>

#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

// to avoid compilation errors
#if __LP64__ || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

std::map<int, std::string> cKeyMap;


@interface CustomGLView : NSView {
	NSOpenGLContext*	mContext;
	BOOL				mAcceptsFirstMouse;
}

- (id)initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format sharedContext:(NSOpenGLContext*)shared;

- (void)update;
- (void)makeCurrent;
- (void)sync:(BOOL)enable;

- (void)setAcceptsFirstMouse:(BOOL)enable;

@end

@implementation CustomGLView

- (id)initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format sharedContext:(NSOpenGLContext*)shared
{
    self = [super initWithFrame:frame];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_surfaceNeedsUpdate:) name:NSViewGlobalFrameDidChangeNotification object:self];
	mContext = [[NSOpenGLContext alloc] initWithFormat:format shareContext:shared];
	mAcceptsFirstMouse = NO;
	
    return self;
}

- (void)dealloc
{
	if (mContext) {
		[mContext clearDrawable];
		[mContext release];
	}
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSViewGlobalFrameDidChangeNotification object:self];
    [super dealloc];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	[[self window] rightMouseDown:theEvent];
}

- (void)update
{
	if([mContext view] == self) {
        [mContext update];
    }
}

- (void)flush
{
	[mContext flushBuffer];
}

- (void)makeCurrent
{
	[mContext makeCurrentContext];
}

- (void)sync:(BOOL)enable
{
	const GLint interval = enable ? 1 : 0;
	[mContext setValues:&interval forParameter: NSOpenGLCPSwapInterval];
}

- (void)setAcceptsFirstMouse:(BOOL)enable
{
	mAcceptsFirstMouse = enable;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return mAcceptsFirstMouse;
}

- (void)lockContext
{
    if ([mContext view] != self) {
        [mContext setView:self];
    }

    [mContext makeCurrentContext];
}

- (void)unlockContext
{
//	[NSOpenGLContext clearCurrentContext];
}

- (BOOL)lockFocusIfCanDraw
{
    if ([super lockFocusIfCanDraw] == NO) return NO;

	[self lockContext];
	return YES;
}

- (void)unlockFocus
{
	[self unlockContext];
	[super unlockFocus];
}

- (BOOL)isOpaque
{
    return YES;
}

- (void) _surfaceNeedsUpdate:(NSNotification*)notification
{
    [self update];
}

- (NSOpenGLContext*)openGLContext
{
	return mContext;
}


@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface DelegatedView : NSObject {
	CustomGLView*			mView;
	NSRect					mFrame;
	NSOpenGLPixelFormat*	mFormat;
	NSOpenGLContext*		mShared;
}

- (CustomGLView*)createWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format sharedContext:(NSOpenGLContext*)shared;

@end


@implementation DelegatedView

- (void)delegatedCreateWithFrame:(id)dummy
{
	mView = [[CustomGLView alloc] initWithFrame:mFrame pixelFormat:mFormat sharedContext:mShared];
}

- (CustomGLView *)createWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format sharedContext:(NSOpenGLContext*)shared
{
	mView = 0;
	mFrame = frame;
	mFormat = format;
	mShared = shared;
	[self performSelectorOnMainThread: @selector(delegatedCreateWithFrame:) withObject:0 waitUntilDone:YES];
	return mView;
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


@interface LuaAVWindowController : NSObject {
	id				delegate;
	
	BOOL			mFullscreen;
	BOOL			mMouseMove;
	BOOL			mMouseButtonDown;
	luaav::Window	*mWin;
}

- (void)setDelegate:(id)val;
- (id)delegate;

// delegate interface
- (void)setParent:(luaav::Window *)win;
- (luaav::Window *)getParent;
- (void)setFullscreen:(BOOL)fs;
- (BOOL)fullscreen;
- (void)setMouseMove:(BOOL)mm;
- (BOOL)mouseMove;
- (NSPoint)mouseCoordForEventCoord:(NSPoint)p;

// delegated event handling
- (void)getModifiers:(NSEvent *)theEvent;
- (void)flagsChanged:(NSEvent*)theEvent;

- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;

- (void)sendMouseDownEvent:(NSEvent *)theEvent action:(luaav::Window::WindowEvent)event button:(const char *)button clicks:(int)nclicks;
- (void)sendMouseEvent:(NSEvent *)theEvent action:(luaav::Window::WindowEvent)event button:(const char *)button;

- (void)mouseEntered:(NSEvent *)theEvent;
- (void)mouseExited:(NSEvent *)theEvent;
- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseDragged:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;
- (void)rightMouseDown:(NSEvent *)theEvent;
- (void)rightMouseDragged:(NSEvent *)theEvent;
- (void)rightMouseUp:(NSEvent *)theEvent;
- (void)mouseMoved:(NSEvent *)theEvent;
- (void)otherMouseUp:(NSEvent *)theEvent;
- (void)scrollWheel:(NSEvent *)theEvent;

- (void)copyText;
- (void)cutText;
- (void)pasteText;
- (void)selectAllText;

@end


@implementation LuaAVWindowController

- (id)init
{
	if(self = [super init]) {
		delegate = nil;
		
		mFullscreen = NO;
		mMouseMove = NO;
		mMouseButtonDown = NO;
		mWin = NULL;
	}
	return self;
}

- (void)setDelegate:(id)val
{
	delegate = val;
}

- (id)delegate
{
	return delegate;
}

// delegate interface
- (void)setParent:(luaav::Window *)win
{
	mWin = win;
}

- (luaav::Window *)getParent
{
	return mWin;
}

- (void)setFullscreen:(BOOL)fs
{
	mFullscreen = fs;
}

- (BOOL)fullscreen
{
	return mFullscreen;
}

- (void)setMouseMove:(BOOL)mm
{
	mMouseMove = mm;
	[delegate setAcceptsMouseMovedEvents:mMouseMove];
}

- (BOOL)mouseMove
{
	return mMouseMove;
}

- (NSPoint)mouseCoordForEventCoord:(NSPoint)p
{
	// could cache this result in the future
	NSRect frame = NSMakeRect(0, 0, 100, 100);
	NSRect contentRect = [NSWindow contentRectForFrameRect:frame styleMask:NSTitledWindowMask];
	float titleBarHeight = 0;
	if(! mFullscreen && mWin->mBorder) {
		titleBarHeight = frame.size.height - contentRect.size.height;
	}
	
	NSPoint pp;
	pp.x = p.x;
	pp.y = [delegate frame].size.height - titleBarHeight - p.y;
	
	return pp;
}

- (void)getModifiers:(NSEvent *)theEvent
{
	//
	//	NSAlphaShiftKeyMask
	//	NSShiftKeyMask
	//	NSControlKeyMask
	//	NSAlternateKeyMask
	//	NSCommandKeyMask
	//	NSNumericPadKeyMask
	//	NSHelpKeyMask
	//	NSFunctionKeyMask
	//

	if(mWin) {
		NSUInteger modifierFlags = [theEvent modifierFlags];
		
		//event, key, shift, control, alternate, cmd
		bool shift = (modifierFlags & NSShiftKeyMask) ? true : false;
		bool ctrl = (modifierFlags & NSControlKeyMask) ? true : false;
		bool alt = (modifierFlags & NSAlternateKeyMask) ? true : false;
		bool cmd = (modifierFlags & NSCommandKeyMask) ? true : false;
		
		if(shift != mWin->mShift) mWin->mChangedFlag = "shift";
		else if(ctrl != mWin->mCtrl) mWin->mChangedFlag = "ctrl";
		else if(alt != mWin->mAlt) mWin->mChangedFlag = "alt";
		else if(cmd != mWin->mCmd) mWin->mChangedFlag = "cmd";
		
		mWin->mShift = shift;
		mWin->mCtrl = ctrl;
		mWin->mAlt = alt;
		mWin->mCmd = cmd;
	}
}

- (void)flagsChanged:(NSEvent*)theEvent
{
	[self getModifiers:theEvent];
	if(mWin) mWin->modifiers_lua();
}

- (void)keyDown:(NSEvent *)theEvent
{
	NSString *s = [theEvent charactersIgnoringModifiers];
//	NSString *s = [theEvent characters];
	[self getModifiers:theEvent];
	
	int code = [s characterAtIndex:0];
//	if(cKeyMap.find(code) != cKeyMap.end()) {
//		const char *key = cKeyMap[code].data();
//		if(mWin) mWin->key("down", code);
//	}
//	else {
		if(mWin) mWin->key_lua("down", code);
//	}
}

- (void)keyUp:(NSEvent *)theEvent
{
	NSString *s = [theEvent charactersIgnoringModifiers];
	//      NSString *s = [theEvent characters];
	[self getModifiers:theEvent];
	
	if(mWin) mWin->key_lua("up", [s characterAtIndex:0]);
}

- (void)sendMouseDownEvent:(NSEvent *)theEvent action:(luaav::Window::WindowEvent)event button:(const char *)button clicks:(int)nclicks
{
	[self getModifiers:theEvent];

	//	int clicks = [theEvent clickCount];
	NSPoint pos = [delegate mouseLocationOutsideOfEventStream];
	pos = [self mouseCoordForEventCoord:pos];

	if(mWin) mWin->mouse_lua(event, button, pos.x, pos.y, nclicks);
}

- (void)sendMouseEvent:(NSEvent *)theEvent action:(luaav::Window::WindowEvent)event button:(const char *)button
{
	[self getModifiers:theEvent];

	//	int clicks = [theEvent clickCount];
	NSPoint pos = [delegate mouseLocationOutsideOfEventStream];
	pos = [self mouseCoordForEventCoord:pos];

	if(mWin) mWin->mouse_lua(event, button, pos.x, pos.y);
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_ENTER button:""];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_EXIT button:""];
}

// handle mouse down events (left mouse button)
- (void)mouseDown:(NSEvent *)theEvent
{
	mMouseButtonDown = YES;
	int nclicks = [theEvent clickCount];
	[self sendMouseDownEvent:theEvent action: luaav::Window::MOUSE_DOWN button:"left" clicks:nclicks];
}

// handle mouse dragged events (left mouse button)
- (void)mouseDragged:(NSEvent *)theEvent
{
	if(mMouseButtonDown) {
		[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_DRAG button:"left"];
	}
}

// handle mouse up events (left mouse button)
- (void)mouseUp:(NSEvent *)theEvent
{
	mMouseButtonDown = NO;
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_UP button:"left"];
}


// handle mouse down events (right mouse button)
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_DOWN button:"right"];
}

// handle mouse dragged events (right mouse button)
- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_DRAG button:"right"];
}

// handle mouse up events (right mouse button)
- (void)rightMouseUp:(NSEvent *)theEvent
{
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_UP button:"right"];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	[self sendMouseEvent:theEvent action: luaav::Window::MOUSE_MOVE button:"left"];
}

// handle mouse up events (other mouse button)
- (void)otherMouseUp:(NSEvent *)theEvent
{
//    NSLog( @"Mouse O up");
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	[self getModifiers:theEvent];

	//	int clicks = [theEvent clickCount];
	NSPoint pos = [delegate mouseLocationOutsideOfEventStream];
	pos = [self mouseCoordForEventCoord:pos];
	
	NSPoint delta;
	delta.x = [theEvent deltaX];
	delta.y = [theEvent deltaY];

	if(mWin) mWin->scroll_lua(luaav::Window::MOUSE_SCROLL, "wheel", pos.x, pos.y, delta.x, delta.y);
}

- (void)copyText {
	if(mWin) mWin->copy_lua();
}

- (void)cutText {
	if(mWin) mWin->cut_lua();
}

- (void)pasteText {
	if(mWin) mWin->paste_lua();
}

- (void)selectAllText {
	if(mWin) mWin->selectall_lua();
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


@interface CustomWindow : NSWindow {
	id		mController;
}


- (id)initWithTitle:(const char *)title contentRect:(NSRect)contentRect 
		styleMask:(unsigned int)windowStyle backing:(NSBackingStoreType)bufferingType 
		defer:(BOOL)deferCreation
		screen:(NSScreen *)screen;

- (void)setParent:(luaav::Window *)win;
- (luaav::Window *)getParent;
- (void)setFullscreen:(BOOL)fs;
- (BOOL)getFullscreen;
- (void)setMouseMove:(BOOL)mm;
- (NSPoint)mouseCoordForEventCoord:(NSPoint)p;
- (void)windowWillClose:(NSNotification *)notification;
- (void)copyText;
- (void)cutText;
- (void)pasteText;
- (void)selectAllText;
@end

@implementation CustomWindow

- (id)initWithTitle:(const char *)title contentRect:(NSRect)contentRect 
	styleMask:(unsigned int)windowStyle backing:(NSBackingStoreType)bufferingType 
	defer:(BOOL)deferCreation
	screen:(NSScreen *)screen
{
	if( self = [super initWithContentRect:contentRect 
					 styleMask:windowStyle 
					 backing:NSBackingStoreBuffered 
					 defer:NO
					 screen:screen])
	{
		[self setTitle: [NSString stringWithCString:title encoding:NSASCIIStringEncoding]];
		
		[[NSNotificationCenter defaultCenter] addObserver:self 
									selector:@selector(windowWillClose:) 
									name:NSWindowWillCloseNotification 
									object:self];
		
		mController = [[LuaAVWindowController alloc] init];
		[mController setDelegate:self];
	}
	
	return self;
}

- (void)copyText {
	if(mController) {
		[mController copyText];
	}
}

- (void)cutText {
	if(mController) {
		[mController cutText];
	}
}

- (void)pasteText {
	if(mController) {
		[mController pasteText];
	}
}

- (void)selectAllText {
	if(mController) {
		[mController selectAllText];
	}
}

- (void)setParent:(luaav::Window *)win
{
	if(mController) {
		[mController setParent:win];
	}
}

- (luaav::Window *)getParent
{
	return [mController getParent];
}

- (void)setFullscreen:(BOOL)fs
{
	if(mController) {
		[mController setFullscreen:fs];
	}
}

- (BOOL)getFullscreen
{
	return [mController fullscreen];
}

- (void)setMouseMove:(BOOL)mm
{
	[mController setMouseMove:mm];
}

- (BOOL)acceptsMouseMovedEvents
{
	
	return [mController mouseMove];
}

- (void)windowWillClose:(NSNotification *)notification
{
	id win = [notification object];
	luaav::Window::finalize( [win getParent] );
	[[NSNotificationCenter defaultCenter] removeObserver:win name:NSWindowWillCloseNotification object:win];
	[mController release];
	mController = nil;
}


- (NSPoint)mouseCoordForEventCoord:(NSPoint)p
{
	return [mController mouseCoordForEventCoord:p];
}

/* This is the preferred way to determine events */
// if you want to get events sent to you, you must make yourself a first responder
- (BOOL)acceptsFirstResponder
{
	printf("acceptsFirstResponder\n");
    return YES;
}
- (BOOL)becomeFirstResponder
{
	printf("becomeFirstResponder\n");
    return YES;
}

-(BOOL)canBecomeKeyWindow
{
	return YES;
}

- (void)flagsChanged:(NSEvent*)theEvent
{
	[mController flagsChanged:theEvent];
}

- (void)keyDown:(NSEvent *)theEvent
{
	[mController keyDown:theEvent];
}

- (void)keyUp:(NSEvent *)theEvent
{
	[mController keyUp:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	[mController mouseEntered:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	[mController mouseExited:theEvent];
}

// handle mouse down events (left mouse button)
- (void)mouseDown:(NSEvent *)theEvent
{
	[mController mouseDown:theEvent];
}

// handle mouse dragged events (left mouse button)
- (void)mouseDragged:(NSEvent *)theEvent
{
	[mController mouseDragged:theEvent];
}

// handle mouse up events (left mouse button)
- (void)mouseUp:(NSEvent *)theEvent
{
	[mController mouseUp:theEvent];
}

// handle mouse down events (right mouse button)
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[mController rightMouseDown:theEvent];
}

// handle mouse dragged events (right mouse button)
- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[mController rightMouseDragged:theEvent];
}

// handle mouse up events (right mouse button)
- (void)rightMouseUp:(NSEvent *)theEvent
{
	[mController rightMouseUp:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	[mController mouseMoved:theEvent];
}

// handle mouse up events (other mouse button)
- (void)otherMouseUp:(NSEvent *)theEvent
{
//    NSLog( @"Mouse O up");
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	[mController scrollWheel:theEvent];
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


@interface CustomPanel : NSPanel {
	id		mController;
}


- (id)initWithTitle:(const char *)title contentRect:(NSRect)contentRect 
		styleMask:(unsigned int)windowStyle backing:(NSBackingStoreType)bufferingType 
		defer:(BOOL)deferCreation
		screen:(NSScreen *)screen;

- (void)setParent:(luaav::Window *)win;
- (luaav::Window *)getParent;
- (void)setFullscreen:(BOOL)fs;
- (BOOL)getFullscreen;
- (void)setMouseMove:(BOOL)mm;
- (NSPoint)mouseCoordForEventCoord:(NSPoint)p;
- (void)windowWillClose:(NSNotification *)notification;
@end

@implementation CustomPanel

- (id)initWithTitle:(const char *)title contentRect:(NSRect)contentRect 
	styleMask:(unsigned int)windowStyle backing:(NSBackingStoreType)bufferingType 
	defer:(BOOL)deferCreation
	screen:(NSScreen *)screen
{
	if( self = [super initWithContentRect:contentRect 
					 styleMask:windowStyle 
					 backing:NSBackingStoreBuffered 
					 defer:NO
					 screen:screen])
	{
		[self setTitle: [NSString stringWithCString:title encoding:NSASCIIStringEncoding]];
		
		[[NSNotificationCenter defaultCenter] addObserver:self 
									selector:@selector(windowWillClose:) 
									name:NSWindowWillCloseNotification 
									object:self];
		
		mController = [[LuaAVWindowController alloc] init];
		[mController setDelegate:self];
		[self setFloatingPanel:YES];
	}
	
	return self;
}

- (void)setParent:(luaav::Window *)win
{
	if(mController) {
		[mController setParent:win];
	}
}

- (luaav::Window *)getParent
{
	return [mController getParent];
}

- (void)setFullscreen:(BOOL)fs
{
	if(mController) {
		[mController setFullscreen:fs];
	}
}

- (BOOL)getFullscreen
{
	return [mController fullscreen];
}

- (void)setMouseMove:(BOOL)mm
{
	[mController setMouseMove:mm];
}

- (BOOL)acceptsMouseMovedEvents
{
	
	return [mController mouseMove];
}

- (void)windowWillClose:(NSNotification *)notification
{
	id win = [notification object];
	luaav::Window::finalize( [win getParent] );
	[[NSNotificationCenter defaultCenter] removeObserver:win name:NSWindowWillCloseNotification object:win];
	[mController release];
	mController = nil;
}


- (NSPoint)mouseCoordForEventCoord:(NSPoint)p
{
	return [mController mouseCoordForEventCoord:p];
}

/* This is the preferred way to determine events */
// if you want to get events sent to you, you must make yourself a first responder
/*
- (BOOL)acceptsFirstResponder
{
	printf("acceptsFirstResponder\n");
    return YES;
}
- (BOOL)becomeFirstResponder
{
	printf("becomeFirstResponder\n");
    return YES;
}

-(BOOL)canBecomeKeyWindow
{
	return YES;
}
*/

- (void)flagsChanged:(NSEvent*)theEvent
{
	[mController flagsChanged:theEvent];
}

- (void)keyDown:(NSEvent *)theEvent
{
	[mController keyDown:theEvent];
}

- (void)keyUp:(NSEvent *)theEvent
{
	[mController keyUp:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	[mController mouseEntered:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	[mController mouseExited:theEvent];
}

// handle mouse down events (left mouse button)
- (void)mouseDown:(NSEvent *)theEvent
{
	[mController mouseDown:theEvent];
}

// handle mouse dragged events (left mouse button)
- (void)mouseDragged:(NSEvent *)theEvent
{
	[mController mouseDragged:theEvent];
}

// handle mouse up events (left mouse button)
- (void)mouseUp:(NSEvent *)theEvent
{
	[mController mouseUp:theEvent];
}

// handle mouse down events (right mouse button)
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[mController rightMouseDown:theEvent];
}

// handle mouse dragged events (right mouse button)
- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[mController rightMouseDragged:theEvent];
}

// handle mouse up events (right mouse button)
- (void)rightMouseUp:(NSEvent *)theEvent
{
	[mController rightMouseUp:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	[mController mouseMoved:theEvent];
}

// handle mouse up events (other mouse button)
- (void)otherMouseUp:(NSEvent *)theEvent
{
//    NSLog( @"Mouse O up");
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	[mController scrollWheel:theEvent];
}

@end


using std::map;
using std::string;

namespace luaav {

// Window Cocoa Imlementation
class WindowImpl {
protected:
	typedef Window	Generic;

public:
	WindowImpl(Window *window)
	:	mGeneric(window),
		mWindow(0),
		mView(0),
		mCursorHides(0)
	{
		static bool initialized = false;
		if(!initialized) {
			buildKeyMap();
			initialized = true;
		}
	}

    Generic * generic() {
		return mGeneric;
	}
	
    Generic *mGeneric;
	CustomWindow *mWindow;
	CustomGLView *mView;
	int mCursorHides;
	
	struct key_entry {
		int code;
		const char *name;
	};
	
	static void buildKeyMap() {
		for(int i=0; cKeyEntries[i].name; i++) {
			cKeyMap[ cKeyEntries[i].code ] = cKeyEntries[i].name;
		}
	}
	
//	static map<int, string>	cKeyMap;
	static key_entry cKeyEntries[];
};

WindowImpl::key_entry WindowImpl::cKeyEntries[] = {
	{9, "tab"},
	{27, "esc"},
	{127, "del"},
	{63232, "up"},
	{63233, "down"},
	{63234, "left"},
	{63235, "right"},
	{63236, "f1"},
	{63237, "f2"},
	{63238, "f3"},
	{63239, "f4"},
	{63240, "f5"},
	{63241, "f6"},
	{63242, "f7"},
	{63243, "f8"},
	{63244, "f9"},
	{63245, "f10"},
	{63246, "f11"},
	{63247, "f12"},
	{0, 0}
};

class WindowSystem {
protected:
	WindowSystem();
	~WindowSystem();

public:
	static WindowSystem& get();

	Window * create_window(lua_State *state, const char *title, int x, int y, int w, int h);
	
	bool hasContext() const;
	void handleEvents();

	NSOpenGLPixelFormat* pixelFormat() { return mPixelFormat; }
	NSOpenGLContext* rootContext() { return mRootContext; }

private:
	NSOpenGLPixelFormat *mPixelFormat;
	NSOpenGLContext *mRootContext;

	static WindowSystem *cTheWindowSystem;
};

#define mCocoaWindow	(mImpl->mWindow)
#define mCocoaView		(mImpl->mView)

void Window :: implInit() {
	mImpl = new Implementation(this);
}

void Window :: implDestruct() {
	delete mImpl;
}

Window * Window :: implCreate(lua_State *state, const char *title, int x, int y, int w, int h) {
	WindowSystem &ws = WindowSystem::get();
	Window *win = ws.create_window(state, title, x, y, w, h);
	win->add_close_menuitem();
	return win;
}

bool Window :: implInitRebuild(luaav_rect &frame) {
	bool frame_valid = false;
	if(mCocoaWindow) {
		frame_valid = true;
		
			// signal Muro
			closing_lua();
		
		frame = implGetFrame();
		
		[mCocoaWindow setContentView:nil];
		mCocoaView = nil;
		
		[mCocoaWindow close];
		[mCocoaWindow release];
		mCocoaWindow = nil;
	}
	return frame_valid;
}

void Window :: implChooseFrame(bool useorigin, int x, int y, luaav_rect &frame)  {
	if(mFullscreen) {
		NSScreen* screen = [NSScreen mainScreen];

		if(useorigin) {
			luaav_rect r;
			r.x = x;
			r.y = y;
			r.width = mSize[0];
			r.height = mSize[1];
			
			screen = screens::screen_for_rect(r);
		}
		screens::screen_rect(screen, frame);
	}
	else {
		frame.x = mOrigin[0];
		frame.y = mOrigin[1];
		frame.width = mSize[0];
		frame.height = mSize[1];
	}
}

// seems to prevent window from shifting screens in edge case
float fudge_factor(Window *win, luaav_rect &frame) {
	return (frame.y == 0 && win->mBorder && !win->mFullscreen) ? 1. : 0.;
}

void Window :: implCreate(luaav_rect &frame) {
	/*
		Relevant (not all) style mask constants
		
		NSBorderlessWindowMask = 0,			// <- Exclusive to other style mask constants

		NSTitledWindowMask = 1 << 0,
		NSClosableWindowMask = 1 << 1,
		NSMiniaturizableWindowMask = 1 << 2,
		NSResizableWindowMask = 1 << 3,
	*/
	
	if(mStereo) fprintf(stderr, "Warning: stereo not supported on OSX");
	
	// To do: add booleans for these attrs
	unsigned int style = 0;
	if(mBorder && !mFullscreen) {
		style = NSTitledWindowMask;
		style |= NSClosableWindowMask;
		style |= NSMiniaturizableWindowMask;
		if(mGrow) {
			style |= NSResizableWindowMask;
		}
	}
	else {
		style = NSBorderlessWindowMask;
	}

	NSScreen *screen = screens::screen_for_rect(frame);
	NSRect screenframe = [screen frame];
	
	NSScreen *main = screens::main_screen();
	NSRect mainframe = [main frame];
	
	luaav_pt cp = screens::from_luaav_pt(luaav_pt(frame.x, frame.y));
	
	float fudge = fudge_factor(this, frame);
	NSRect nsframe;
	nsframe.origin.x = frame.x - screenframe.origin.x;
//	nsframe.origin.y = mainframe.size.height - (frame.y + frame.height + fudge) - screenframe.origin.y;
	nsframe.origin.y = cp.y - frame.height - fudge - screenframe.origin.y;
	nsframe.size.width = frame.width;
	nsframe.size.height = frame.height;
	
//	printf("(%d, %d) -> (%d, %d)\n", frame.x, frame.y, cp.x, cp.y);
//	printf("nsframe.origin.y: %d -> %d\n", frame.y, (int)nsframe.origin.y);

//	printf("nsframe.origin.y = %d - (%d + %d + %d)\n", (int)mainframe.size.height, (int)frame.y, (int)frame.height, (int)fudge);
//	printf("NSFRAME screen: %d %d %d %d\n", (int)screenframe.origin.x, (int)screenframe.origin.y, (int)screenframe.size.width, (int)screenframe.size.height);
//	printf("NSFRAME: %d %d %d %d\n", (int)nsframe.origin.x, (int)nsframe.origin.y, (int)nsframe.size.width, (int)nsframe.size.height);

	if(mFloating) {
		mCocoaWindow = (CustomWindow *)[[CustomPanel alloc] initWithTitle:mTitle.data()
										contentRect:nsframe 
										styleMask:style|NSUtilityWindowMask
										backing:NSBackingStoreBuffered 
										defer:NO
										screen:screen];
	}
	else {
		mCocoaWindow = [[CustomWindow alloc] initWithTitle:mTitle.data()
										contentRect:nsframe 
										styleMask:style 
										backing:NSBackingStoreBuffered 
										defer:NO
										screen:screen];
	}
	
	[mCocoaWindow setParent:this];
	[mCocoaWindow retain];
	
	[mCocoaWindow display];
	if(mFullscreen) {
		luaav_rect main_frame;
		screens::screen_rect([NSScreen mainScreen], main_frame);
		
		// only hide menu bar if going fullscreen on main screen
		if(luaav_rects_are_equal(frame, main_frame)) {
			SetSystemUIMode(kUIModeAllSuppressed, 0); // kUIModeAllHidden // kUIModeAllSuppressed
		}
		[mCocoaWindow setFullscreen:true];
	}
	else {
		SetSystemUIMode(kUIModeNormal, 0);
		[mCocoaWindow setFullscreen:false];
	}

	if(mCocoaView) {
		[mCocoaWindow setContentView:mCocoaView];
		[mCocoaView release];
	}
	else {
		// create opengl view through delegation
		DelegatedView * delegate = [[DelegatedView alloc] init];
		mCocoaView = [delegate createWithFrame:nsframe pixelFormat:ws->pixelFormat() sharedContext:ws->rootContext()];
		[delegate release];
		
		if(mCocoaView == nil) {
			[mCocoaWindow release];
			// error, couldn't create view
		}
		
		[mCocoaWindow setContentView:mCocoaView];
	}
	
	// for click through behavior when window is floating
	if(mFloating) {
		[mCocoaView setAcceptsFirstMouse:YES];
	}
	
	// register for mouse enter/exit events
	[[mCocoaWindow contentView] addTrackingRect:[[mCocoaWindow contentView] bounds]
							owner:mCocoaWindow
							userData:nil
							assumeInside:NO];
							
	
//	nsframe = [mCocoaWindow frame];
//	printf("%d %d\n", (int)nsframe.origin.x, (int)nsframe.origin.y);
}

void Window :: implEndDraw() {}

void Window :: implNull() {
	mCocoaWindow = nil;
	mCocoaView = nil;
}

void Window :: implRelease() {
	if(mCocoaWindow) {
		[mCocoaWindow close];
		[mCocoaWindow setParent:NULL];
		[mCocoaWindow release];
		mCocoaWindow = nil;
	}

	if(mCocoaView) {
		[mCocoaView release];
		mCocoaView = nil;
	}
}

void Window :: implMakeCurrent() {
	if(mCocoaView) [mCocoaView makeCurrent];
}

void Window :: implLock() {
	if(mCocoaView) [mCocoaView lockFocusIfCanDraw];
}

void Window :: implUnlock() {
	if(mCocoaView) [mCocoaView unlockFocus];
}

void Window :: implFlush() {
	if(mCocoaView) [mCocoaView flush];
}




void Window :: implShow() {
	if(mCocoaWindow) [mCocoaWindow deminiaturize:nil];
}

void Window :: implHide() {
	if(mCocoaWindow) [mCocoaWindow miniaturize:nil];
}

void Window :: implRaise() {
	if(mCocoaWindow) [mCocoaWindow orderFront:nil];
}

void Window :: implLower() {
	if(mCocoaWindow) [mCocoaWindow orderBack:nil];
}


void Window :: implSetTitle(const char *v) {
	if(mCocoaWindow) {
		[mCocoaWindow setTitle: [NSString stringWithCString:v encoding:NSASCIIStringEncoding]];
	}
}

void Window :: implSetFrame(luaav_rect frame) {
//	printf("implSetFrame: %d %d %d %d\n", frame.x, frame.y, frame.width, frame.height);

	//NSRect screenframe = [screens::main_screen() frame];
//	luaav_pt cp = screens::from_luaav_pt(luaav_pt(frame.x, frame.y));
//	printf("(%d, %d) -> (%d, %d)\n", frame.x, frame.y, cp.x, cp.y);
	
//	NSRect screenframe = [screens::screen_for_rect(frame) frame];
	
	luaav_rect r = screens::to_luaav_rect(frame);
	float fudge = fudge_factor(this, frame);
	
//	float xheight = frame.y + fudge + frame.height;
//	xheight  = (xheight > screenframe.size.height) ? screenframe.size.height : xheight;
	NSPoint forigin;
	forigin.x = r.x;
	forigin.y = r.y - fudge;
	
	NSSize fsize;
	fsize.width = r.width;
	fsize.height = r.height;
	
//	printf("implSetFrame forigin: %d %d\n", (int)forigin.x, (int)forigin.y);
	
	//[mCocoaWindow setFrame:nsframe display:NO];// animate:YES];
	[mCocoaWindow setContentSize: fsize];
	[mCocoaWindow setFrameOrigin: forigin];
}

luaav_rect Window :: implGetFrame() {

//	NSRect screenframe = [[mCocoaWindow screen] frame];
	NSRect winframe = [mCocoaWindow frame];
	NSRect viewframe = [[mCocoaWindow contentView] frame];
	
//	NSScreen *main = screens::main_screen();
//	NSRect mainframe = [main frame];
	
	/*
	luaav_rect frame;
	frame.width = (int)viewframe.size.width;
	frame.height = (int)viewframe.size.height;
	frame.x = (int)winframe.origin.x;
	
	luaav_pt lp = screens::to_luaav_pt(luaav_pt((int)winframe.origin.x, (int)winframe.origin.y));
	frame.y = lp.y - viewframe.size.height;
	*/
	
//	printf("(%d, %d) -> (%d, %d)\n", (int)winframe.origin.x, (int)winframe.origin.y, lp.x, lp.y);
//	printf("frame.y: %d -> %d\n", (int)winframe.origin.y, frame.y);
	
//	frame.y = ((int)(screenframe.size.height+screenframe.origin.y)) - ((int)winframe.origin.y) - frame.height;
	
//	printf("implGetFrame screen: %d %d %d %d\n", (int)screenframe.origin.x, (int)screenframe.origin.y, (int)screenframe.size.width, (int)screenframe.size.height);
//	printf("implGetFrame win: %d %d %d %d\n", (int)winframe.origin.x, (int)winframe.origin.y, (int)winframe.size.width, (int)winframe.size.height);
	
//	printf("XX: %d\n", (int) -(winframe.origin.y - mainframe.size.height + screenframe.origin.y) - frame.height);
	
//	printf("NSFRAME: %d %d %d %d\n", (int)nsframe.origin.x, (int)nsframe.origin.y, (int)nsframe.size.width, (int)nsframe.size.height);

	return screens::to_luaav_rect(luaav_rect(
						(int)winframe.origin.x, 
						(int)winframe.origin.y,
						(int)viewframe.size.width,
						(int)viewframe.size.height
				));

//	return frame;
}

luaav_pt Window :: implGetSize() {
	luaav_pt pt;
	NSRect viewframe = [[mCocoaWindow contentView] frame];
	pt.x = viewframe.size.width;
	pt.y = viewframe.size.height;
	
	return pt;
}

void Window :: implSetFullscreen(bool enable) {
	if(mCocoaWindow) [mCocoaWindow setFullscreen: enable];
	rebuild();
}

void Window :: implSetBorder(bool enable) {
	rebuild();
}

void Window :: implSetGrow(bool enable) {
	luaav_pt size = implGetSize();
	mSize[0] = size.x;
	mSize[1] = size.y;
	rebuild();
}

void Window :: implSetFloating(bool enable) {
	rebuild();
}

void Window :: implSetMouseMove(bool enable) {
	if(mCocoaWindow) [mCocoaWindow setMouseMove: enable];
}

void Window :: implSetSync(bool enable) {
	if(mCocoaView) [mCocoaView sync: enable];
}

void Window :: implSetCursor(bool enable) {
	if(enable)	{
		while(mImpl->mCursorHides < 0) {
			[NSCursor unhide];
			mImpl->mCursorHides++;
		}
	}
	else {
		[NSCursor hide];
		mImpl->mCursorHides--;
	}
}

void Window :: implSetCursorStyle(CursorStyle style) {
	using namespace luaav;

	switch(style) {
	case Window::ARROW:				[[NSCursor arrowCursor] set];				break;
	case Window::IBEAM:				[[NSCursor IBeamCursor] set];				break;
//	case Window::CONTEXTUAL_MENU:	[[NSCursor contextualMenuCursor] set];		break;
	case Window::CROSSHAIR:			[[NSCursor crosshairCursor] set];			break;
	case Window::CLOSED_HAND:		[[NSCursor closedHandCursor] set];			break;
//	case Window::DRAG_COPY:			[[NSCursor dragCopyCursor ] set];			break;
//	case Window::DRAG_LINK:			[[NSCursor dragLinkCursor ] set];			break;
//	case Window::NO_OP:				[[NSCursor operationNotAllowedCursor] set];	break;
	case Window::OPEN_HAND:			[[NSCursor openHandCursor] set];			break;
	case Window::POINTING_HAND:		[[NSCursor pointingHandCursor] set];		break;
	case Window::RESIZE_LEFT:		[[NSCursor resizeLeftCursor] set];			break;
	case Window::RESIZE_RIGHT:		[[NSCursor resizeRightCursor] set];			break;
	case Window::RESIZE_LEFTRIGHT:	[[NSCursor resizeLeftRightCursor] set];		break;
	case Window::RESIZE_UP:			[[NSCursor resizeUpCursor] set];			break;
	case Window::RESIZE_DOWN:		[[NSCursor resizeDownCursor] set];			break;
	case Window::RESIZE_UPDOWN:		[[NSCursor resizeUpDownCursor] set];		break;
	case Window::DISAPPEARING_ITEM:	[[NSCursor disappearingItemCursor] set];	break;
	}
}

void Window :: implCloseFocusedWindow() {
	NSWindow *win = [NSApp mainWindow];
	if(win) {
		[win close];
	}
}

int Window :: implActiveWindowCount() {
	NSArray *windows = [NSApp windows];
	return [windows count];
}


WindowSystem * WindowSystem::cTheWindowSystem = 0;


/* Boolean pixel format attrs

	NSOpenGLPFAAllRenderers
		If present, this attribute indicates that the pixel format selection is open to all available renderers, 
		including debug and special-purpose renderers that are not OpenGL compliant.
	
	NSOpenGLPFADoubleBuffer
		If present, this attribute indicates that only double-buffered pixel formats are considered. Otherwise, 
		only single-buffered pixel formats are considered.
		
	NSOpenGLPFAStereo
		If present, this attribute indicates that only stereo pixel formats are considered. Otherwise, only 
		monoscopic pixel formats are considered.

	NSOpenGLPFAMinimumPolicy
		If present, this attribute indicates that the pixel format choosing policy is altered for the color, 
		depth, and accumulation buffers such that only buffers of size greater than or equal to the desired 
		size are considered.
	
	NSOpenGLPFAMaximumPolicy
		If present, this attribute indicates that the pixel format choosing policy is altered for the color, 
		depth, and accumulation buffers such that, if a nonzero buffer size is requested, the largest 
		available buffer is preferred.
	
	NSOpenGLPFAOffScreen
		If present, this attribute indicates that only renderers that are capable of rendering to an offscreen 
		memory area and have buffer depth exactly equal to the desired buffer depth are considered. The 
		NSOpenGLPFAClosestPolicy attribute is implied.
	
	NSOpenGLPFAFullScreen
		If present, this attribute indicates that only renderers that are capable of rendering to a full-screen 
		drawable are considered. The NSOpenGLPFASingleRenderer attribute is implied.
	
	NSOpenGLPFAColorFloat
		If present, this attribute indicates that only renderers that are capable using buffers storing floating 
		point pixels are considered. This should be accompanied by a NSOpenGLPFAColorSize of 64 (for half float 
		pixel components) or 128 (for full float pixel components). Note, not all hardware supports floating 
		point color buffers thus the returned pixel format could be NULL.
	
	NSOpenGLPFAMultisample
		If present and used with NSOpenGLPFASampleBuffers and NSOpenGLPFASamples, this attribute hints to OpenGL to 
		prefer multi-sampling. Multi-sampling will sample textures at the back buffer dimensions vice the multi-sample 
		buffer dimensions and use that single sample for all fragments with coverage on the back buffer location. 
		This means less total texture samples than with super-sampling (by a factor of the number of samples requested) 
		and will likely be faster though less accurate (texture sample wise) than super-sampling. If the underlying 
		video card does not have enough VRAM to support this feature, this hint does nothing.
		
		The NSOpenGLPFASampleBuffers and NSOpenGLPFASamples attributes must be configured to request anti-aliasing as follows:

			NSOpenGLPFAMultisample,
			NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1
			NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,

		If after adding these options, multisamping still does not work, try removing the NSOpenGLPFAPixelBuffer attribute 
		(if present). Some graphics cards may not support this option in specific versions of Mac OS X. If removing the 
		attribute still does not enable multisampling, try adding the NSOpenGLPFANoRecovery attribute. 
	
	NSOpenGLPFASupersample
		If present and used with NSOpenGLPFASampleBuffers and NSOpenGLPFASamples, this attribute hints to OpenGL to prefer 
		super-sampling. Super-sampling will process fragments with a texture sample per fragment and would likely be slower 
		than multi-sampling. If the pixel format is not requesting anti-aliasing, this hint does nothing.
	
	NSOpenGLPFASampleAlpha
		 If present and used with NSOpenGLPFASampleBuffers and NSOpenGLPFASampleBuffers, this attribute hints to OpenGL to 
		 update multi-sample alpha values to ensure the most accurate rendering. If pixel format is not requesting 
		 anti-aliasing then this hint does nothing.

	NSOpenGLPFASingleRenderer
		If present, this attribute indicates that a single rendering engine is chosen. On systems with multiple screens, 
		this disables OpenGL’s ability to drive different monitors through different graphics accelerator cards with a 
		single context. This attribute is not generally useful.
	
	NSOpenGLPFANoRecovery
		If present, this attribute indicates that OpenGL’s failure recovery mechanisms are disabled. Normally, if an 
		accelerated renderer fails due to lack of resources, OpenGL automatically switches to another renderer. This 
		attribute disables these features so that rendering is always performed by the chosen renderer. This attribute 
		is not generally useful.

	NSOpenGLPFAAccelerated
		If present, this attribute indicates that only hardware-accelerated renderers are considered. If not present, 
		accelerated renderers are still preferred.
	
	NSOpenGLPFAClosestPolicy
		If present, this attribute indicates that the pixel format choosing policy is altered for the color buffer such 
		that the buffer closest to the requested size is preferred, regardless of the actual color buffer depth of the 
		supported graphics device.
	
	NSOpenGLPFARobust
		If present, this attribute indicates that only renderers that do not have any failure modes associated with a 
		lack of video card resources are considered. This attribute is not generally useful.
	
	NSOpenGLPFABackingStore
		If present, this attribute indicates that OpenGL only considers renderers that have a back color buffer the 
		full size of the drawable (regardless of window visibility) and that guarantee the back buffer contents to 
		be valid after a call to NSOpenGLContext object’s flushBuffer.
	
	NSOpenGLPFAMPSafe
		If present, this attribute indicates that the renderer is multi-processor safe.
	
	NSOpenGLPFAMultiScreen
		If present, this attribute indicates that only renderers capable of driving multiple screens are considered. 
		This attribute is not generally useful.

	NSOpenGLPFACompliant
		If present, this attribute indicates that pixel format selection is only open to OpenGL-compliant renderers. 
		This attribute is implied unless NSOpenGLPFAAllRenderers is specified. This attribute is not useful in the 
		attribute array.
	
	NSOpenGLPFACompliant
		If present, this attribute indicates that pixel format selection is only open to OpenGL-compliant renderers. 
		This attribute is implied unless NSOpenGLPFAAllRenderers is specified. This attribute is not useful in the 
		attribute array.

	NSOpenGLPFAPixelBuffer
		If present, this attribute indicates that rendering to a pixel buffer is enabled.
*/

/*	Integer pixel format attrs

	NSOpenGLPFAAuxBuffers
		Value is a nonnegative integer that indicates the desired number of auxiliary buffers. Pixel formats 
		with the smallest number of auxiliary buffers that meets or exceeds the specified number are preferred.
	
	NSOpenGLPFAColorSize
		Value is a nonnegative buffer size specification. A color buffer that most closely matches the specified 
		size is preferred. If unspecified, OpenGL chooses a color size that matches the screen.
	
	NSOpenGLPFAAlphaSize
		Value is a nonnegative buffer size specification. An alpha buffer that most closely matches the specified 
		size is preferred.
		
	NSOpenGLPFADepthSize
		Value is a nonnegative depth buffer size specification. A depth buffer that most closely matches the 
		specified size is preferred.
	
	NSOpenGLPFAStencilSize
		Value is a nonnegative integer that indicates the desired number of stencil bitplanes. The smallest 
		stencil buffer of at least the specified size is preferred.
	
	NSOpenGLPFAAccumSize
		Value is a nonnegative buffer size specification. An accumulation buffer that most closely matches the 
		specified size is preferred.
	
	NSOpenGLPFASampleBuffers
		Value is a nonnegative number indicating the number of multisample buffers. 
	
	NSOpenGLPFASamples
		Value is a nonnegative indicating the number of samples per multisample buffer. 
	
	NSOpenGLPFARendererID
		Value is a nonnegative renderer ID number. OpenGL renderers that match the specified ID are preferred. 
		Constants to select specific renderers are provided in the CGLRenderers.h header of the OpenGL framework. 
		Of note is kCGLRendererGenericID which selects the Apple software renderer. The other constants select 
		renderers for specific hardware vendors.

	NSOpenGLPFAScreenMask
		Value is a bit mask of supported physical screens. All screens specified in the bit mask are guaranteed to 
		be supported by the pixel format. Screens not specified in the bit mask may still be supported. The bit mask 
		is managed by the CoreGraphics’s DirectDisplay, available in the CGDirectDisplay.h header of the 
		ApplicationServices umbrella framework. A CGDirectDisplayID must be converted to an OpenGL display mask 
		using the function CGDisplayIDToOpenGLDisplayMask. This attribute is not generally useful
		
	NSOpenGLPFAVirtualScreenCount
		The number of virtual screens in this format.
*/

WindowSystem :: WindowSystem() 
	: mPixelFormat(0), mRootContext(0)
{

	// renderer attributes
	NSOpenGLPixelFormatAttribute attrs[] = {
		NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)24,
		NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)8,
		NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16,
		NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,	//FSAA
		NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,		//FSAA
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFANoRecovery,
		(NSOpenGLPixelFormatAttribute)nil
	};

	// create pixel format
	mPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	//if (m_pixelFormat == nil) throw ResourceError("unable to choose matching pixel format");
	
	// create root context
	mRootContext = [[NSOpenGLContext alloc] initWithFormat:mPixelFormat shareContext:nil];
	if (mRootContext == nil) {
		[mPixelFormat release];
		//throw ResourceError("unable to create root context");
	}
}

WindowSystem :: ~WindowSystem() {
	[mRootContext release];
	[mPixelFormat release];
}

WindowSystem& WindowSystem::get() {
	if (!WindowSystem::cTheWindowSystem)
		WindowSystem::cTheWindowSystem = new WindowSystem();
	return *WindowSystem::cTheWindowSystem;
}

Window * WindowSystem :: create_window(lua_State *state, const char *title, int x, int y, int w, int h) {
	lua_State * P = luaav_getrootstate(state);
	return new Window(P, this, title, x, y, w, h);
}

/*
Context * WindowSystem :: createContext() {
	return new Context(*this);
}
*/

bool WindowSystem :: hasContext() const { 
	return [NSOpenGLContext currentContext] != nil;
}


} // luaav::
