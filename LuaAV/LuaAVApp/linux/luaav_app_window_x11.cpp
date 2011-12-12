#include "luaav_app_window.h"
#include "lua_utility.h"
#include "luaav_app_screens_x11.h"
#include "al_pq.hpp"
#include "luaav.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string.h>

#include <map>
#include <string>

using std::map;
using std::string;

namespace luaav {

#define XWINPOLL_PERIOD    (0.02666)

// Window XWindows Imlementation
class WindowImpl {
protected:
	typedef Window	Generic;

	// XWindows polling userdata
    static map< ::Window, WindowImpl *> cXWinMap;
	static map< string, ::Atom> cAtomMap;

public:

	static bool cEnabled;

	WindowImpl(Window *window)
	:	mGeneric(window),
        mX11Window(0),
        mGLXView(0),
        mARROW(0),
        mIBEAM(0),
        mCROSSHAIR(0),
        mCLOSED_HAND(0),
        mPOINTING_HAND(0),
        mRESIZE_LEFT(0),
        mRESIZE_RIGHT(0),
        mRESIZE_LEFTRIGHT(0),
        mRESIZE_UP(0),
        mRESIZE_DOWN(0),
        mRESIZE_UPDOWN(0),
        mDISAPPEARING_ITEM(0),
		mClickTime(0),
		mGotFirstClick(false)
	{}

    ~WindowImpl() {
        unmapXWin();

        if(mARROW)              XFreeCursor(screens::g_display, mARROW);
        if(mIBEAM)              XFreeCursor(screens::g_display, mIBEAM);
        if(mCROSSHAIR)          XFreeCursor(screens::g_display, mCROSSHAIR);
        if(mCLOSED_HAND)        XFreeCursor(screens::g_display, mCLOSED_HAND);
        if(mPOINTING_HAND)      XFreeCursor(screens::g_display, mPOINTING_HAND);
        if(mRESIZE_LEFT)        XFreeCursor(screens::g_display, mRESIZE_LEFT);
        if(mRESIZE_RIGHT)       XFreeCursor(screens::g_display, mRESIZE_RIGHT);
        if(mRESIZE_LEFTRIGHT)   XFreeCursor(screens::g_display, mRESIZE_LEFTRIGHT);
        if(mRESIZE_UP)          XFreeCursor(screens::g_display, mRESIZE_UP);
        if(mRESIZE_DOWN)        XFreeCursor(screens::g_display, mRESIZE_DOWN);
        if(mRESIZE_UPDOWN)      XFreeCursor(screens::g_display, mRESIZE_UPDOWN);
        if(mDISAPPEARING_ITEM)  XFreeCursor(screens::g_display, mDISAPPEARING_ITEM);
    }

    Generic * generic() {
		return mGeneric;
	}

    // manage an event polling clock (global to all WindowImpl instances)
    static void enable();
    static void disable();

	static Atom getAtom(const char *name);

    static int XCursorForCursorStyle(Generic::CursorStyle style);
    void enableCursor(Cursor &cursor, int style);

    // polling callback from timer
    static void doPoll();
    static WindowImpl * implForXWindow(::Window xwin);

    void handleExpose(XExposeEvent &xexpose);
    void handleFocus(XFocusChangeEvent &xfocus);
    void handleConfigureNotify(XConfigureEvent &xconfigure);
    void handleResizeRequest(XResizeRequestEvent &xresizerequest);
    void handleButton(XButtonEvent &xbutton, bool press);
    void handleMotion(XMotionEvent &xmotion, int button);
    void handleKey(XKeyEvent &xkey);
	static void handleEvent(WindowImpl *w, XEvent &report, int button);

	void mapXWin();
	void unmapXWin();

	::Window Xwindow() {
        return mX11Window;
	}

    Generic *mGeneric;
    ::Window mX11Window;
    GLXContext mGLXView;
    Atom wmDeleteWindow;

    Cursor mARROW;
    Cursor mIBEAM;
    Cursor mCROSSHAIR;
    Cursor mCLOSED_HAND;
    Cursor mPOINTING_HAND;
    Cursor mRESIZE_LEFT;
    Cursor mRESIZE_RIGHT;
    Cursor mRESIZE_LEFTRIGHT;
    Cursor mRESIZE_UP;
    Cursor mRESIZE_DOWN;
    Cursor mRESIZE_UPDOWN;
    Cursor mDISAPPEARING_ITEM;

	::Time  mClickTime;
	bool mGotFirstClick;
};

map< ::Window, WindowImpl *> WindowImpl::cXWinMap;
map< string, ::Atom> WindowImpl::cAtomMap;
bool WindowImpl::cEnabled = false;

static int pollEvents(al_sec now, void *ptr, luaav_state *S) {
	if(WindowImpl::cEnabled) {
		WindowImpl::doPoll();
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(XWINPOLL_PERIOD, XWINPOLL_PERIOD, pollEvents, ptr, S);
	}
	return 0;
}

void WindowImpl :: enable() {
    if(!cEnabled) {
		cEnabled = true;
        luaav_state *S = luaav_app_state();
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(XWINPOLL_PERIOD, XWINPOLL_PERIOD, pollEvents, (void *)NULL, S);
	}
}

void WindowImpl :: disable() {
    if(cEnabled) {
        cEnabled = false;
	}
}

Atom WindowImpl :: getAtom(const char *name) {
	map< string, ::Atom>::iterator it = cAtomMap.find(string(name));
	if(it == cAtomMap.end()) {
		Atom a = XInternAtom(screens::g_display, name, False);
		if(a == None) {
			fprintf(stderr, "X11: unable to create Atom %s\n", name);
		}
		else {
			cAtomMap.insert(std::pair<string, ::Atom>(string(name), a));
		}
		return a;
	}
	else {
		return it->second;
	}
}

int WindowImpl :: XCursorForCursorStyle(Generic::CursorStyle style) {
    switch(style) {
    case Generic::IBEAM:             return XC_xterm;
    case Generic::CROSSHAIR:         return XC_crosshair;
    case Generic::CLOSED_HAND:       return XC_draped_box;
    case Generic::POINTING_HAND:     return XC_hand1;
    case Generic::RESIZE_LEFT:       return XC_left_side;
    case Generic::RESIZE_RIGHT:      return XC_right_side;
    case Generic::RESIZE_LEFTRIGHT:  return XC_sb_h_double_arrow;
    case Generic::RESIZE_UP:         return XC_top_side;
    case Generic::RESIZE_DOWN:       return XC_bottom_side;
    case Generic::RESIZE_UPDOWN:     return XC_sb_v_double_arrow;
    case Generic::DISAPPEARING_ITEM: return XC_pirate;

    case Generic::ARROW:
    default:
        return XC_arrow;
    }
}

void WindowImpl :: enableCursor(Cursor &cursor, int style) {
    if(!cursor) cursor = XCreateFontCursor(screens::g_display, style);
    XDefineCursor(screens::g_display, mX11Window, cursor);
}

static int buttonDown = -1;
void WindowImpl :: doPoll() {
    bool processedMouseDrag = false;
    bool processedMouseMove = false;

    while(XPending(screens::g_display)) {
		//printf("EVENT\n");

		bool is_mouse_moved_event = false;
        XEvent report;
        XNextEvent(screens::g_display, &report);
        switch(report.type) {
        case ButtonPress:
            buttonDown = report.xbutton.button;
		//	printf("down: %d\n", report.xbutton.time);
            break;

        case ButtonRelease:
            buttonDown = -1;
			//printf("up: %d\n", report.xbutton.time);
            break;

        case MotionNotify:
		//	printf("motion\n");
            if(buttonDown >= 0) {
                if(processedMouseDrag) {
                    continue;
                }
                processedMouseDrag = true;
            }
            else {
                if(processedMouseMove) {
                    continue;
                }
                processedMouseMove = true;
				is_mouse_moved_event = true;
            }

        default:
            break;
        }

        WindowImpl *w = implForXWindow(report.xany.window);
        if(w) {
			if(is_mouse_moved_event && ! w->generic()->mMouseMove ) {
				continue;			
			}

            handleEvent(w, report, buttonDown);
        }
    }
}

WindowImpl * WindowImpl :: implForXWindow(::Window xwin) {
    WindowImpl *w = NULL;
    map< ::Window, WindowImpl *>::iterator it = cXWinMap.find(xwin);
    if(it != cXWinMap.end()) {
        w = it->second;
    }
    return w;
}

void WindowImpl :: mapXWin() {
    if(mX11Window) {
        cXWinMap[mX11Window] = this;
    }
}

void WindowImpl :: unmapXWin() {
    if(mX11Window) {
        map< ::Window, WindowImpl *>::iterator it = cXWinMap.find(mX11Window);
        if(it != cXWinMap.end()) {
            cXWinMap.erase(it);
        }
    }
}

class WindowSystem {
protected:
	WindowSystem();
	~WindowSystem();

public:
	static WindowSystem& get();

    void createResources(int *bufferattrs, GLXFBConfig *&FBConfig,
                XVisualInfo *&vi, XSetWindowAttributes &winAttrs);
	bool hasContext() const;

    Window * createWindow(const char *title, int x, int y, int w, int h);
//	XSetWindowAttributes& pixelFormat() { return mPixelFormat; }
    GLXContext rootContext() { return mRootContext; }
    GLXFBConfig * fbconfig() {return mFBConfig;}
    int * bufferAttributes(bool stereo);

    static int cAttributeList[];
    static int buffer_attributes[];

    static int errorHandler(Display *display, XErrorEvent *event);

private:
	XSetWindowAttributes mPixelFormat;
	GLXContext mRootContext;
	GLXFBConfig *mFBConfig;

	static WindowSystem *cTheWindowSystem;
};

#define mX11Window	(mImpl->mX11Window)
#define mGLXView	(mImpl->mGLXView)


void Window :: implInit() {
    // enable XWindows event polling on first window creation
    Implementation::enable();

    // init the implementation instance
	mImpl = new Implementation(this);
}

void Window :: implDestruct() {
	delete mImpl;
}

Window * Window :: implCreate(const char *title, int x, int y, int w, int h) {
	WindowSystem &ws = WindowSystem::get();
	return ws.createWindow(title, x, y, w, h);
}

bool Window :: implInitRebuild(luaav_rect &frame) {
	bool frame_valid = false;

	if(mX11Window) {
		frame_valid = true;

			//signal Muro
            destclosing();

			//trigger rebuilding callback in script
			/*lua_State *L = getState();
			instance_to_userdata(L, this);
			lua_getfenv(L, -1);
			lua_getfield(L, -1, "destclosing");
			if(lua_isfunction(L, -1)) {
				lua_pushvalue(L, -3);
				lua::pcall(L, 2);
			}
			else {
				lua_pop(L, 1);
			}
			lua_pop(L, 2);	//env table and udata
			*/

        frame = implGetFrame();

		glXMakeCurrent(screens::g_display, None, NULL);

		// disassociate the XWindow handle with the implementation
        mImpl->unmapXWin();

 		glXDestroyContext(screens::g_display, mGLXView);
 		XUnmapWindow(screens::g_display, mX11Window);
 		XDestroyWindow(screens::g_display, mX11Window);

 		mX11Window = 0;
 		mGLXView = 0;
	}
	return frame_valid;
}

void Window :: implChooseFrame(bool useorigin, int x, int y, luaav_rect &frame)  {
	if(mFullscreen) {
        screens::screen_rect(screens::main_screen(), &frame);
        if(!useorigin) {
            x = mOrigin[0];
            y = mOrigin[1];
        }

        int screen_count = screens::screen_count();
        for(int i=0; i < screen_count; i++) {
            luaav_rect r;
            screens::screen_rect(screens::screen(i), &r);

            if( r.x < x &&
                x < (r.x + r.width) &&
                r.y < y &&
                y < (r.y + r.height))
            {
                frame.x = r.x;
                frame.y = r.y;
                frame.width = r.width;
                frame.height = r.height;
                break;
            }
        }
	}
	else {
		frame.x = mOrigin[0];
		frame.y = mOrigin[1];
		frame.width = mSize[0];
		frame.height = mSize[1];
	}
}

/*
Atom FS_ATOM = 0;
Atom A_STATE = 0;
#define _NET_WM_STATE_REMOVE	0
#define _NET_WM_STATE_ADD		1
*/

typedef int (*SwapInterval)(int);
static SwapInterval SwapIntervalSGI = 0;

void Window :: implCreate(luaav_rect &frame) {
    int *buffer_attrs = NULL;
    GLXFBConfig *fbconfig = NULL;
    XVisualInfo *vi = NULL;
    XSetWindowAttributes winAttrs;
    bool custom_visual = mStereo;

    if(custom_visual) {
        buffer_attrs = ws->bufferAttributes(mStereo);
        ws->createResources(buffer_attrs, fbconfig, vi, winAttrs);

        if(!fbconfig) {
            fprintf(stderr, "Falling back to supported pixelformat");
            ws->createResources(WindowSystem::buffer_attributes, fbconfig, vi, winAttrs);

             // reset offending attrs
            mStereo = false;
        }

        delete[] buffer_attrs;
        buffer_attrs = NULL;
    }
    else {
        // create window
        ws->createResources(WindowSystem::buffer_attributes, fbconfig, vi, winAttrs);
 //       winAttrs = ws->pixelFormat();
 //       fbconfig = ws->fbconfig();
//        vi = glXGetVisualFromFBConfig(screens::g_display, fbconfig[0]);
    }
    if(!vi) {
        // error
    }

    int window_mask = CWColormap | CWEventMask | CWBorderPixel;
    if(mFullscreen || !mBorder) {
        winAttrs.override_redirect = True;
        window_mask |= CWOverrideRedirect;
    }
    else {
        winAttrs.override_redirect = False;
    }

    mCtxSize[0] = frame.width;
    mCtxSize[1] = frame.height;
    mX11Window = XCreateWindow(screens::g_display,
                        DefaultRootWindow(screens::g_display),
                        frame.x, frame.y,
                        frame.width, frame.height,
                        0,
                        vi->depth,
                        InputOutput,
                        vi->visual,
                        window_mask,
                        &winAttrs);

    if(!mX11Window) {
        printf("XError calling XCreatewindow\n");
    }

    if(mFullscreen || !mBorder) {
        XGrabKeyboard(screens::g_display, mX11Window, True, GrabModeAsync,
                                        GrabModeAsync, CurrentTime);
    }

    // associate the XWindow handle with the implementation
    mImpl->mapXWin();

    XStoreName(screens::g_display, mX11Window, mTitle.data());
//    XSetWindowBorderWidth(screens::g_display, mX11Window, mBorder ? 0 : 0); // not window border but within window border
    XSelectInput(screens::g_display, mX11Window,    ExposureMask |
                                                    StructureNotifyMask |
                                                    PointerMotionMask |
                                                    ButtonPressMask |
                                                    ButtonReleaseMask |
                                                    KeyPressMask |
                                                    KeyReleaseMask |
                                                    EnterWindowMask |
                                                    FocusChangeMask |
                                                    LeaveWindowMask);// |
                                                  //  ResizeRedirectMask);
    XMapWindow(screens::g_display, mX11Window);

    //  XIfEvent(screens::g_display, &event, WaitForNotify, (Char*)mGLXWindow);

    mGLXView = glXCreateContext(screens::g_display, vi, ws->rootContext(), GL_TRUE);

    if(mGLXView == NULL) {
       printf("XError calling glXCreateContext\n");
    }

    if(!glXIsDirect(screens::g_display, mGLXView)) {
        printf("Error: rendering in indirect mode\n");
    }

    // register for window manager closing notifications
	Atom wmDeleteWindow = WindowImpl::getAtom("WM_DELETE_WINDOW");
	mImpl->wmDeleteWindow = wmDeleteWindow;
	XSetWMProtocols(screens::g_display, mX11Window, &wmDeleteWindow, 1);


/*	if(!FS_ATOM) {
		FS_ATOM = XInternAtom(screens::g_display, "_NET_WM_STATE_FULLSCREEN", False);
		A_STATE = XInternAtom(screens::g_display, "_NET_WM_STATE", False);
	}
//	mImpl->wmDeleteWindow = wmDeleteWindow;
//	XSetWMProtocols(screens::g_display, mX11Window, &wmFS, 1);
	XEvent xev;
	xev.type = ClientMessage;
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = False;
	xev.xclient.display = screens::g_display;
	xev.xclient.window = mX11Window;
	xev.xclient.message_type = A_STATE;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = _NET_WM_STATE_ADD; // _NET_WM_STATE_REMOVE
	xev.xclient.data.l[1] = FS_ATOM;
	xev.xclient.data.l[2] = None;
	
*/
    implSetFrame(frame);

    if(mFullscreen || !mBorder) {
        XGrabKeyboard(screens::g_display, mX11Window, True, GrabModeAsync,
                                        GrabModeAsync, CurrentTime);
    }

	/*XSendEvent(screens::g_display, mX11Window, False, 
				SubstructureRedirectMask | SubstructureNotifyMask,
				&xev);*/

	if(SwapIntervalSGI == 0) {
		SwapIntervalSGI = (SwapInterval)glXGetProcAddress((const GLubyte *)"glXSwapIntervalSGI");
	}
}

void Window :: implNull() {
	// disassociate the XWindow handle with the implementation
    /*mImpl->unmapXWin();

	mX11Window = NULL;
	mGLXView = NULL;*/
	implRelease();
}

void Window :: implRelease() {
    glXMakeCurrent(screens::g_display, None, NULL);

    // disassociate the XWindow handle with the implementation
    mImpl->unmapXWin();

    if(mGLXView) {
        glXDestroyContext(screens::g_display, mGLXView);
        mGLXView = NULL;
    }
    if(mX11Window) {
        XUnmapWindow(screens::g_display, mX11Window);
        XDestroyWindow(screens::g_display, mX11Window);
        mX11Window = NULL;
    }

    // VERY IMPORTANT -> without this, a quick close/re-open operation will leave a window
    // hanging on the screen
    WindowImpl::doPoll();
}

void WindowImpl :: handleExpose(XExposeEvent &xexpose) {
        /*
    typedef struct {
	int type;
	unsigned long serial;	// # of last request processed by server
	Bool send_event;	    // true if this came from a SendEvent request
	Display *display;	    // Display the event was read from
	Window window;
	int x, y;
	int width, height;
	int count;		        // if nonzero, at least this many more
} XExposeEvent;


    if(count > 0) break;

    count
        Number of other expose events waiting in the server's events queue. This may be useful if we got several expose events in a row - we will usually avoid redrawing the window until we get the last of them (i.e. until count is 0).
    Window window
        The ID of the window this expose event was sent for (in case our application registered for events on several windows).
    int x, y
        The x and y coordinates (in pixels) from the top-left of the window, of the window's region that needs to be redrawn.
    int width, height
        The width and height (in pixels) of the window's region that needs to be redraw.
        */
}

void WindowImpl :: handleFocus(XFocusChangeEvent &xfocus) {
    if(mGeneric->mFullscreen || !mGeneric->mBorder) {
        XGrabKeyboard(screens::g_display, this->Xwindow(), True, GrabModeAsync,
                                        GrabModeAsync, CurrentTime);
    }
}

void WindowImpl :: handleConfigureNotify(XConfigureEvent &xconfigure) {
/*
typedef struct {
	int type;
	unsigned long serial;   // # of last request processed by server
	Bool send_event;        // true if this came from a SendEvent request
	Display *display;       // Display the event was read from
	Window event;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	Bool override_redirect;
} XConfigureEvent;

The x and y members are set to the coordinates relative to the parent window's origin and indicate the position of
the upper-left outside corner of the window. The width and height members are set to the inside size of the window,
not including the border. The border_width member is set to the width of the window's border, in pixels.
*/
 //   printf("%d %d %d %d\n", xconfigure.x, xconfigure.y, xconfigure.width, xconfigure.height);
    mGeneric->mCtxSize[0] = xconfigure.width;
    mGeneric->mCtxSize[1] = xconfigure.height;

/*
XSizeHints size_hints; memset(&size_hints, '\0', sizeof(XSizeHints));
	long supplied_return;
	//if(XGetWMSizeHints(screens::g_display, xconfigure.window, &size_hints, &supplied_return, WindowImpl::getAtom("WM_SIZE_HINTS"))) {
	if(XGetWMNormalHints(screens::g_display, xconfigure.window, &size_hints, &supplied_return)) {	
		printf("ERROR cccalling XGetWMSizeHints\n");
	}
	printf("supplied return: %d\n", supplied_return);
	if(supplied_return & USPosition) printf("USPosition\n");
	if(supplied_return & USSize) printf("USSize\n");
	if(supplied_return & PPosition) printf("PPosition\n");
	if(supplied_return & PSize) printf("PSize\n");

	if(supplied_return & PMinSize) printf("PMinSize\n");
	if(supplied_return & PMaxSize) printf("PMaxSize\n");
	if(supplied_return & PResizeInc) printf("PResizeInc\n");
	if(supplied_return & PAspect) printf("PAspect\n");

	if(supplied_return & PBaseSize) printf("PBaseSize\n");
	if(supplied_return & PWinGravity) printf("PWinGravity\n");
	
	printf("min: %d %d  max: %d %d\n", size_hints.min_width, size_hints.min_height, size_hints.max_width, size_hints.max_height);

	size_hints.min_width = 512;
	size_hints.min_height = 512;
	size_hints.max_width = 512;
	size_hints.max_height = 512;
	size_hints.flags = PMinSize | PMaxSize;
	XSetWMNormalHints(screens::g_display, xconfigure.window, &size_hints);
*/
//	XSetWMSizeHints(screens::g_display, xconfigure.window, &size_hints, WindowImpl::getAtom("WM_SIZE_HINTS"));
/*

	if(XGetWMSizeHints(screens::g_display, xconfigure.window, &size_hints, &supplied_return, WindowImpl::getAtom("WM_SIZE_HINTS"))) {
		printf("ERROR cccalling XGetWMSizeHints\n");
	}
	printf("min: %d %d  max: %d %d\n", size_hints.min_width, size_hints.min_height, size_hints.max_width, size_hints.max_height);*/
}

void WindowImpl :: handleResizeRequest(XResizeRequestEvent &xresizerequest) {
    /*
        typedef struct {
             int type;
             unsigned long serial;
             Bool send_event;
             Display *display;
             Window window;
             int width, height;
        } XResizeRequestEvent;

        The type member is set to the event type constant name that uniquely identifies it. For example, when the X server
        reports a GraphicsExpose event to a client application, it sends an XGraphicsExposeEvent structure with the type
        member set to GraphicsExpose. The display member is set to a pointer to the display the event was read on. The
        send_event member is set to True if the event came from a SendEvent protocol request. The serial member is set from
        the serial number reported in the protocol but expanded from the 16-bit least-significant bits to a full 32-bit
        value. The window member is set to the window that is most useful to toolkit dispatchers.

        The window member is set to the window whose size another client attempted to change. The width and height members
        are set to the inside size of the window, excluding the border.
    */
//    printf("%d %d\n", xresizerequest.width, xresizerequest.height);
    mGeneric->mCtxSize[0] = xresizerequest.width;
    mGeneric->mCtxSize[1] = xresizerequest.height;
}

void WindowImpl ::  handleButton(XButtonEvent &xbutton, bool press) {
    /*
        typedef struct {
            int type;               // ButtonPress or ButtonRelease
            unsigned long serial;   // # of last request processed by server
            Bool send_event;        // true if this came from a SendEvent request
            Display *display;       // Display the event was read from
            Window window;          // ``event'' window it is reported relative to
            Window root;            // root window that the event occurred on
            Window subwindow;       // child window
            Time time;              // milliseconds
            int x, y;               // pointer x, y coordinates in event window
            int x_root, y_root;     // coordinates relative to root
            unsigned int state;     // key or button mask
            unsigned int button;    // detail
            Bool same_screen;       // same screen flag
        } XButtonEvent;
        typedef XButtonEvent XButtonPressedEvent;
        typedef XButtonEvent XButtonReleasedEvent;

    When you receive these events, their structure members are set as follows.

    The type member is set to the event type constant name that uniquely identifies it. For example, when the X
    server reports a GraphicsExpose event to a client application, it sends an XGraphicsExposeEvent structure with
    the type member set to GraphicsExpose. The display member is set to a pointer to the display the event was
    read on. The send_event member is set to True if the event came from a SendEvent protocol request. The serial
    member is set from the serial number reported in the protocol but expanded from the 16-bit least-significant
     bits to a full 32-bit value. The window member is set to the window that is most useful to toolkit dispatchers.

    These structures have the following common members: window, root, subwindow, time, x, y, x_root, y_root,
    state, and same_screen. The window member is set to the window on which the event was generated and is
    referred to as the event window. As long as the conditions previously discussed are met, this is the window
    used by the X server to report the event. The root member is set to the source window's root window. The
    x_root and y_root members are set to the pointer's coordinates relative to the root window's origin at the
    time of the event.

    The same_screen member is set to indicate whether the event window is on the same screen as the root window
    and can be either True or False. If True, the event and root windows are on the same screen. If False, the
    event and root windows are not on the same screen.

    If the source window is an inferior of the event window, the subwindow member of the structure is set to the
    child of the event window that is the source window or the child of the event window that is an ancestor of
    the source window. Otherwise, the X server sets the subwindow member to None. The time member is set to the
    time when the event was generated and is expressed in milliseconds.

    If the event window is on the same screen as the root window, the x and y members are set to the coordinates
    relative to the event window's origin. Otherwise, these members are set to zero.

    The state member is set to indicate the logical state of the pointer buttons and modifier keys just prior
    to the event, which is the bitwise inclusive OR of one or more of the button or modifier key masks:
    Button1Mask, Button2Mask, Button3Mask, Button4Mask, Button5Mask, ShiftMask, LockMask, ControlMask, Mod1Mask,
    Mod2Mask, Mod3Mask, Mod4Mask, and Mod5Mask.

    Each of these structures also has a member that indicates the detail. For the XKeyPressedEvent and
    XKeyReleasedEvent structures, this member is called a keycode. It is set to a number that represents
    a physical key on the keyboard. The keycode is an arbitrary representation for any key on the keyboard
    (see sections 12.7 and 16.1).

    For the XButtonPressedEvent and XButtonReleasedEvent structures, this member is called button. It represents
    the pointer button that changed state and can be the Button1, Button2, Button3, Button4, or Button5 value.
    For the XPointerMovedEvent structure, this member is called is_hint. It can be set to NotifyNormal or NotifyHint.


    */

    Generic::WindowEvent event = press ? Generic::MOUSE_DOWN : Generic::MOUSE_UP;
	bool double_click = false;
	if(mGotFirstClick && event ==  Generic::MOUSE_DOWN) {
//		printf("db-click time: %d\n", (int)(xbutton.time - mClickTime));

		if((int)(xbutton.time - mClickTime) < 300) {
		//	printf("DOUBLE CLICK\n");
			double_click = true;
		}
	}

    switch(xbutton.button) {
        case Button1:
			if(double_click) {
				generic()->mouseClick(event, "left", xbutton.x, xbutton.y, 2);
			}
			else if(event == Generic::MOUSE_DOWN) {
				generic()->mouseClick(event, "left", xbutton.x, xbutton.y, 1);
			}
			else {
				generic()->mouse(event, "left", xbutton.x, xbutton.y);
			}
            break;

        case Button2:
            if(double_click) {
				generic()->mouseClick(event, "right", xbutton.x, xbutton.y, 2);
			}
			else if(event == Generic::MOUSE_DOWN) {
				generic()->mouseClick(event, "right", xbutton.x, xbutton.y, 1);
			}
			else {
				generic()->mouse(event, "right", xbutton.x, xbutton.y);
			}
            break;

	case Button3:
		//printf("button3\n");
		break;

	case Button4:
		if(event == Generic::MOUSE_DOWN) {
			generic()->mouseScroll(event, "wheel", xbutton.x, xbutton.y, -1, 0);
		}
	//	printf("button4: %d %d\n", xbutton.x, xbutton.y);
		break;

	case Button5:
		if(event == Generic::MOUSE_DOWN) {
			generic()->mouseScroll(event, "wheel", xbutton.x, xbutton.y, 1, 0);
		}
	//	printf("button5: %d %d\n", xbutton.x, xbutton.y);
		break;

    }
}

void WindowImpl :: handleMotion(XMotionEvent &xmotion, int button) {
    Generic::WindowEvent event = (button >= 0) ? Generic::MOUSE_DRAG : Generic::MOUSE_MOVE;
    switch(button) {
    case Button1:
        generic()->mouse(event, "left", xmotion.x, xmotion.y);
        break;

    case Button2:
        generic()->mouse(event, "right", xmotion.x, xmotion.y);
        break;

    case -1:
        generic()->mouse(event, "left", xmotion.x, xmotion.y);
        break;

    default:
        break;
    }
}

const char * test_key(KeySym sym) {
	switch(sym) {
	case XK_BackSpace: return "BackSpace";
	case XK_Tab: return "Tab";
	case XK_Linefeed: return "Linefeed";
	case XK_Clear: return "Clear";
	case XK_Return: return "Return";
	case XK_Pause: return "Pause";
	case XK_Scroll_Lock: return "Scroll_Lock";
	case XK_Sys_Req: return "Sys_Req";
	case XK_Escape: return "Escape";
	case XK_Delete: return "Delete";

	case XK_Left: return "Left";
	case XK_Up: return "Up";
	case XK_Right: return "Right";
	case XK_Down: return "Down";

	case XK_Shift_R:
	case XK_Shift_L: return "Shift";
	case XK_Control_R:
	case XK_Control_L: return "Control";
	case XK_Meta_R:
	case XK_Meta_L: return "Meta";
	case XK_Alt_R:
	case XK_Alt_L: return "Alt";	// OPTION ON OSX
	case XK_Super_R:
	case XK_Super_L: return "Super"; // COMMAND ON OSX
	case XK_Hyper_R:
	case XK_Hyper_L: return "Hyper";

	default: return "";
	}
}

int keycode_translate(char key, KeySym sym) {
	switch(sym) {
		case XK_Up: 	return 63232;
		case XK_Down: 	return 63233;
		case XK_Left: 	return 63234;
		case XK_Right: 	return 63235;

		default:
			return (int)key;
	}
}

bool handle_modifier(WindowImpl *w, bool down, KeySym sym) {
	switch(sym) {
		case XK_Shift_R:
		case XK_Shift_L:
			w->generic()->mShift = down ? true : false;
			return true;

		case XK_Control_R:
		case XK_Control_L:
			w->generic()->mCtrl = down ? true : false;
			return true;

		case XK_Alt_R:
		case XK_Alt_L:
			w->generic()->mAlt = down ? true : false;
			return true;

		case XK_Super_R:
		case XK_Super_L:
			w->generic()->mCmd = down ? true : false;
			return true;

		default:
			return false;
	}
}

void WindowImpl :: handleKey(XKeyEvent &xkey) {
    char key[64]; memset(key, '\0', 64);
    KeySym keysym;
    XLookupString(&xkey, key, 64, &keysym, NULL);
//	printf("key: %s %c %d\n", test_key(keysym), key[0], key[0]);
//	printf("event_type: %d %s\n", xkey.type, xkey.type == KeyPress ? "true" : "false");
//	printf("XKeysymToKeycode: %d\n", XKeysymToKeycode(screens::g_display, keysym));
//	printf("KK: %d\n", *((int *)key));
//	printf("XK_LEFT: %d %s\n", XK_Left, (keysym & 0xff00) == 0xff00 ? "true" : "false");
//	printf("%d   %d   %d\n", (keysym & 0xff), 63234-(keysym & 0xff), (keysym & 0xff) | 0x10000000);
	if(handle_modifier(this, xkey.type == KeyPress, keysym)) {
		generic()->modifiers();
	}
	else {
		int keycode = keycode_translate(key[0], keysym);
		if(xkey.type == KeyPress) {
		    generic()->key("down", keycode);
		}
		else {
		    generic()->key("up", keycode);
		}
	}
}

// http://users.actcom.co.il/~choo/lupg/tutorials/xlib-programming/xlib-programming.html#events_mainloop
// http://users.actcom.co.il/~choo/lupg/tutorials/xlib-programming/events.c
void WindowImpl :: handleEvent(WindowImpl *w, XEvent &report, int button) {
    if(!w) {
        return;  // safety
    }

    switch(report.type) {

    case Expose:
        //printf("Expose\n");
        if(report.xexpose.count <= 0)
            w->handleExpose(report.xexpose);
        break;

    case FocusIn:
    case FocusOut:
        w->handleFocus(report.xfocus);
        break;

    case ConfigureNotify:
    //    printf("ConfigureNotify\n");
        w->handleConfigureNotify(report.xconfigure);
        break;

    case ResizeRequest:
    //    printf("ResizeRequest\n");
        w->handleResizeRequest(report.xresizerequest);
        break;

    case ClientMessage:
    //    printf("ClientMessage\n");
        if(report.xclient.data.l[0] == w->wmDeleteWindow) {
            printf("Killing\n");
            Generic::finalize(w->generic());
        }
        break;

    case MappingNotify:
    //    printf("MappingNotify\n");
        /* Keyboard or Pointer mapping was changed by another
        * client; if keyboard, should call XRefreshKeyboardMapping,
        * unless keyboard events are not used */
        break;

    case MapNotify:
    //    printf("MapNotify\n");
        break;

    case UnmapNotify:
     //   printf("UnmapNotify\n");
        break;

    case DestroyNotify:
     //   printf("destroy\n");
        break;

    case ButtonPress:
     //   printf("ButtonPress\n");
        w->handleButton(report.xbutton, true);
		w->mGotFirstClick = true;   
		w->mClickTime = report.xbutton.time;		
		break;

    case ButtonRelease:
    //    printf("ButtonRelease\n");
        w->handleButton(report.xbutton, false);
        break;

    case MotionNotify:
		w->mGotFirstClick = false;
        // http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html
        w->handleMotion(report.xmotion, button);
        break;

    case EnterNotify:
        // The mouse pointer just entered one of our controlled windows.
        break;

    case LeaveNotify:
        // The mouse pointer just left one of our controlled windows.
        break;


    case KeyPress:
    case KeyRelease:
        // A key was just released on the keyboard while any of our windows had the keyboard focus.
        w->handleKey(report.xkey);
        break;

    default:
      //  printf("default: %x\n", report.type);
        break;
    }

	/*
	switch(report.type) {
		case Expose:			printf("Expose\n"); break;
		case FocusIn:			printf("FocusIn\n"); break;
		case FocusOut:			printf("FocusOut\n"); break;
		case ConfigureNotify:	printf("ConfigureNotify\n"); break;
		case ResizeRequest:		printf("ResizeRequest\n"); break;
		case ClientMessage:		printf("ClientMessage\n"); break;
		case MappingNotify:		printf("MappingNotify\n"); break;
		case MapNotify:			printf("MapNotify\n"); break;
		case UnmapNotify:		printf("UnmapNotify\n"); break;
		case DestroyNotify:		printf("DestroyNotify\n"); break;
		case ButtonPress:		printf("ButtonPress\n"); break;
		case ButtonRelease:		printf("ButtonRelease\n"); break;
		case MotionNotify:		printf("MotionNotify\n"); break;
		case EnterNotify:		printf("EnterNotify\n"); break;
		case LeaveNotify:		printf("LeaveNotify\n"); break;
		case KeyPress:			printf("KeyPress\n"); break;
		case KeyRelease:		printf("KeyRelease\n"); break;
		default: printf("default: %x\n", report.type); break;
    }
	*/
}

void Window :: implMakeCurrent() {
    Bool success = glXMakeCurrent(screens::g_display, mX11Window, mGLXView);
    if(!success) {

    }
}

void Window :: implEndDraw() {}

void Window :: implLock() {

}

void Window :: implUnlock() {

}

void Window :: implFlush() {
    glXSwapBuffers(screens::g_display, mX11Window);
}




void Window :: implShow() {

}

void Window :: implHide() {

}

void Window :: implRaise() {
	if(mX11Window) XRaiseWindow(screens::g_display, mX11Window);
}

void Window :: implLower() {
	if(mX11Window) XLowerWindow(screens::g_display, mX11Window);
}




void Window :: implSetTitle(const char *v) {
	if(mX11Window) {
        XStoreName(screens::g_display, mX11Window, mTitle.data());
	}
}

/*
void Window :: implSetClearColor(float r, float g, float b, float a) {

}
*/

void Window :: implSetFrame(luaav_rect frame) {

	if(mX11Window) {	
		XWindowChanges values;
		values.x = frame.x;
		values.y = frame.y;
		values.width = frame.width;
		values.height = frame.height;
		XConfigureWindow(screens::g_display,
		                mX11Window,
		                CWX | CWY | CWWidth | CWHeight,
		                &values);
	}
	else {
		mOrigin[0] = frame.x;
		mOrigin[1] = frame.y;
		mSize[0] = frame.width;
		mSize[1] = frame.height;
	}
}

luaav_rect Window :: implGetFrame() {
	luaav_rect frame;

	if(mX11Window) {
		GLXWindow root;
		int x, y;
		unsigned int width, height, border_width, depth;
		XGetGeometry(screens::g_display, mX11Window,
		                &root, &x, &y, &width, &height,
		                &border_width, &depth);

		frame.x = x;
		frame.y = y;
		frame.width = width;
		frame.height = height;
	}
	else {
		frame.x = mOrigin[0];
		frame.y = mOrigin[1];
		frame.width = mSize[0];
		frame.height = mSize[1];
	}
	return frame;
}

luaav_pt Window :: implGetSize() {
	luaav_pt pt;
	if(mX11Window) {
		GLXWindow root;
		int x, y;
		unsigned int width, height, border_width, depth;
		XGetGeometry(screens::g_display, mX11Window,
		                &root, &x, &y, &width, &height,
		                &border_width, &depth);

		pt.x = width;
		pt.y = height;
	}
	else {
		pt.x = mSize[0];
		pt.y = mSize[1];
	}
    return pt;
}

void Window :: implSetFullscreen(bool enable) {
	rebuild();
}

void Window :: implSetBorder(bool enable) {
	rebuild();
}

void Window :: implSetGrow(bool enable) {
	if(mX11Window) {
		XSizeHints size_hints; memset(&size_hints, '\0', sizeof(XSizeHints));

		if(enable) {
			size_hints.min_width = 0;
			size_hints.min_height = 0;
			size_hints.max_width = 10000000;
			size_hints.max_height = 10000000;
		}
		else {
			size_hints.min_width = mSize[0];
			size_hints.min_height = mSize[1];
			size_hints.max_width = mSize[0];
			size_hints.max_height = mSize[1];
		}

		size_hints.flags = PMinSize | PMaxSize;
		XSetWMNormalHints(screens::g_display, mX11Window, &size_hints);
	}
}

void Window :: implSetFloating(bool enable) {
	rebuild();
}

void Window :: implSetMouseMove(bool enable) {
//	if(mCocoaWindow) [mCocoaWindow setFullscreen: enable];
}

void Window :: implSetSync(bool enable) {
	if(SwapIntervalSGI != 0) {
	/*	int res = SwapIntervalSGI(enable ? 1 : 0);
		if(res != 0) {
			printf("error setting sync\n");
		}
	*/
	}
}

void Window :: implSetCursor(bool enable) {
//	if(mCocoaWindow) [mCocoaWindow setCursor: enable];
}

void Window :: implSetCursorStyle(CursorStyle style) {
    switch(style) {
    case IBEAM:             mImpl->enableCursor(mImpl->mIBEAM, Implementation::XCursorForCursorStyle(style)); break;
    case CROSSHAIR:         mImpl->enableCursor(mImpl->mCROSSHAIR, Implementation::XCursorForCursorStyle(style)); break;
    case CLOSED_HAND:       mImpl->enableCursor(mImpl->mCLOSED_HAND, Implementation::XCursorForCursorStyle(style)); break;
    case POINTING_HAND:     mImpl->enableCursor(mImpl->mPOINTING_HAND, Implementation::XCursorForCursorStyle(style)); break;
    case RESIZE_LEFT:       mImpl->enableCursor(mImpl->mRESIZE_LEFT, Implementation::XCursorForCursorStyle(style)); break;
    case RESIZE_RIGHT:      mImpl->enableCursor(mImpl->mRESIZE_RIGHT, Implementation::XCursorForCursorStyle(style)); break;
    case RESIZE_LEFTRIGHT:  mImpl->enableCursor(mImpl->mRESIZE_LEFTRIGHT, Implementation::XCursorForCursorStyle(style)); break;
    case RESIZE_UP:         mImpl->enableCursor(mImpl->mRESIZE_UP, Implementation::XCursorForCursorStyle(style)); break;
    case RESIZE_DOWN:       mImpl->enableCursor(mImpl->mRESIZE_DOWN, Implementation::XCursorForCursorStyle(style)); break;
    case RESIZE_UPDOWN:     mImpl->enableCursor(mImpl->mRESIZE_UPDOWN, Implementation::XCursorForCursorStyle(style)); break;
    case DISAPPEARING_ITEM: mImpl->enableCursor(mImpl->mDISAPPEARING_ITEM, Implementation::XCursorForCursorStyle(style)); break;

    case ARROW:
    default:
        mImpl->enableCursor(mImpl->mARROW, Implementation::XCursorForCursorStyle(style)); break;
    }
}

void Window :: implCloseFocusedWindow() {

}

int Window :: implActiveWindowCount() {
    return 1;
}

WindowSystem * WindowSystem::cTheWindowSystem = 0;

//GLXFBConfig
// fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), 0, &nelements);
//   vi = glXGetVisualFromFBConfig(dpy, fbc[0]);
// NOT USED RIGHT NOW???
int WindowSystem::cAttributeList[] = {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 24,
    GLX_DOUBLEBUFFER,
    None
};

int WindowSystem::buffer_attributes[] = {
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DOUBLEBUFFER, True,
    GLX_STEREO, False,
    None
};



int WindowSystem :: errorHandler(Display *display, XErrorEvent *event) {
/*
typedef struct {
    int type;
    Display *display;           // Display the event was read from
    unsigned long serial;       // serial number of failed request
    unsigned char error_code;   // error code of failed request
    unsigned char request_code; // Major op-code of failed request
    unsigned char minor_code;   // Minor op-code of failed request
    XID resourceid;             // resource id
} XErrorEvent;

*/
    char str[512];
    XGetErrorText(event->display, event->error_code, str, 512);
    printf("XError: %s\n", str);
    return 0;
}


WindowSystem :: WindowSystem()
:   mRootContext(0),
    mFBConfig(0)
{
    XSetErrorHandler(errorHandler);
    XVisualInfo *vi = NULL;

    createResources(buffer_attributes, mFBConfig, vi, mPixelFormat);

	// create root context, nothing to share withs, render directs
	mRootContext = glXCreateContext(screens::g_display, vi, 0, GL_TRUE);
	if(mRootContext == NULL) {
		//throw ResourceError("unable to create root context");
	}
}

WindowSystem :: ~WindowSystem() {
	if(mRootContext) glXDestroyContext(screens::g_display, mRootContext);
}

int * WindowSystem :: bufferAttributes(bool stereo) {
    int *buffer_attrs = new int[ sizeof(buffer_attributes)/sizeof(int) ];

   /* int i=0;
    while(buffer_attributes[i] != None) {
        if(buffer_attributes[i] == GLX_STEREO) {
            buffer_attrs[i] = GLX_STEREO;
            i++;
            buffer_attrs[i] = stereo ? True : False;
            i++;
        }
        else {
            buffer_attrs[i] = buffer_attributes[i];
            i++;
        }
    }
    buffer_attrs[i] = None;*/

    buffer_attrs[0] = GLX_DRAWABLE_TYPE;
    buffer_attrs[1] = GLX_WINDOW_BIT;
    buffer_attrs[2] = GLX_RENDER_TYPE;
    buffer_attrs[3] = GLX_RGBA_BIT;
    buffer_attrs[4] = GLX_DOUBLEBUFFER;
    buffer_attrs[5] = True;
    buffer_attrs[6] = GLX_STEREO;
    buffer_attrs[7] = True;
    buffer_attrs[8] = None;

    return buffer_attrs;
}

void WindowSystem :: createResources(int *bufferattrs, GLXFBConfig *&FBConfig,
                            XVisualInfo *&vi, XSetWindowAttributes &winAttrs) {
    int fbcount;
    FBConfig = glXChooseFBConfig(screens::g_display, DefaultScreen(screens::g_display),
                                        bufferattrs, &fbcount);

    if(fbcount == 0) {
		printf("Could not get requested frame buffer configuration.\n");
		return;
	}


    vi = glXGetVisualFromFBConfig(screens::g_display, FBConfig[0]);
/*
http://wwweic.eri.u-tokyo.ac.jp/computer/manual/lx/SGI_Developer/books/XLib_PG/sgi_html/ch08.html
*/

    winAttrs.colormap = XCreateColormap(screens::g_display,
                                            RootWindow(screens::g_display, vi->screen),
                                            vi->visual,
                                            AllocNone);
    winAttrs.border_pixel = 0;
    winAttrs.event_mask =   ExposureMask
                                | StructureNotifyMask
                                | KeyPressMask
                                | KeyReleaseMask
                                | ButtonPressMask
                                | ButtonReleaseMask
                                | PointerMotionMask
                       //         | Button1MotionMask
                       //         | Button2Motion
                      //          | Button3Motion
                       //         | ButtonMotion
                                | LeaveWindowMask
                                | EnterWindowMask
                                | ResizeRedirectMask;
}

WindowSystem& WindowSystem::get() {
	if (!WindowSystem::cTheWindowSystem)
		WindowSystem::cTheWindowSystem = new WindowSystem();
	return *WindowSystem::cTheWindowSystem;
}

Window * WindowSystem :: createWindow(const char *title, int x, int y, int w, int h) {
	return new Window(this, title, x, y, w, h);
}

/*
Context * WindowSystem :: createContext() {
	return new Context(*this);
}
*/

bool WindowSystem :: hasContext() const {
	return mRootContext != NULL;
}


} // luaav::
