#include "luaav_app_window_opengl_qt.h"

#include "luaav_app_window.h"
#include "lua_utility.h"
#include "luaav_app_screens_x11.h"
#include "types/al_pq.hpp"
#include "luaav.h"

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string.h>

#include <map>
#include <string>

using std::map;
using std::string;

namespace luaav {

// Window Qt Imlementation
class WindowImpl {
protected:
	typedef Window	Generic;

public:

	WindowImpl(Window *window)
	:	mGeneric(window),
		mQtWindow(0)
	{}

    ~WindowImpl() {
    }

    Generic * generic() {
		return mGeneric;
	}


    Generic *mGeneric;
	OpenGLWindow *mQtWindow;
};


class WindowSystem {
protected:
	WindowSystem();
	~WindowSystem();

public:
	static WindowSystem& get();

	bool hasContext() const;

    Window * createWindow(lua_State *L, const char *title, int x, int y, int w, int h);

private:


	static WindowSystem *cTheWindowSystem;
};

#define mQtWindow	(mImpl->mQtWindow)


void Window :: implInit() {
    // init the implementation instance
	mImpl = new Implementation(this);
}

void Window :: implDestruct() {
	delete mImpl;
}

Window * Window :: implCreate(lua_State *state, const char *title, int x, int y, int w, int h) {
	WindowSystem &ws = WindowSystem::get();
	return ws.createWindow(state, title, x, y, w, h);
}

bool Window :: implInitRebuild(luaav_rect &frame) {
	bool frame_valid = false;

	if(mQtWindow) {
		frame_valid = true;

			//signal Muro
            closing_lua();

        frame = implGetFrame();

		delete mQtWindow;
		mQtWindow = 0;
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


void Window :: implCreate(luaav_rect &frame) {
	QGL::FormatOptions format = QGL::DoubleBuffer | 
								QGL::DepthBuffer | 
								QGL::Rgba | 
								QGL::StencilBuffer | 
								QGL::DirectRendering;// | 
								QGL::SampleBuffers;
	if(mStereo) {
		format |= QGL::StereoBuffers;
	}

	mQtWindow = new OpenGLWindow(
						this, 
						mFullscreen, 
						format, 
						mTitle.data(), 
						frame.x, frame.y, frame.width, frame.height
					);
	
	mCtxSize[0] = frame.width;
    mCtxSize[1] = frame.height;

	if(mFullscreen) {
		mQtWindow->setWindowState(Qt::WindowFullScreen);
	}
	else if(!mBorder) {
		mQtWindow->setWindowFlags(Qt::FramelessWindowHint);
	}

	mQtWindow->show();
}

void Window :: implNull() {
	implRelease();
}

void Window :: implRelease() {
	delete mQtWindow;
	mQtWindow = 0;
}


void Window :: implMakeCurrent() {
 	if(mQtWindow) {
		mQtWindow->makeCurrent();
	}
}

void Window :: implEndDraw() {}

void Window :: implLock() {

}

void Window :: implUnlock() {

}

void Window :: implFlush() {
	mQtWindow->swapBuffers();
}




void Window :: implShow() {

}

void Window :: implHide() {

}

void Window :: implRaise() {
//	if(mX11Window) XRaiseWindow(screens::g_display, mX11Window);
}

void Window :: implLower() {
//	if(mX11Window) XLowerWindow(screens::g_display, mX11Window);
}




void Window :: implSetTitle(const char *v) {
	if(mQtWindow) {
		mQtWindow->setWindowTitle(OpenGLWindow::tr(v));
	}
}

/*
void Window :: implSetClearColor(float r, float g, float b, float a) {

}
*/

void Window :: implSetFrame(luaav_rect frame) {
	if(mQtWindow) {	
		mQtWindow->setGeometry(frame.x, frame.y, frame.width, frame.height);
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

	if(mQtWindow) {
		QRect rect = mQtWindow->rect();
		frame.x = rect.x();
		frame.y = rect.y();
		frame.width = rect.width();
		frame.height = rect.height();
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
	if(mQtWindow) {
		QSize size = mQtWindow->size();
		pt.x = size.width();
		pt.y = size.height();
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
	if(mQtWindow) {
		if(enable) {
			luaav_pt pt = implGetSize();
			mQtWindow->setFixedSize(pt.x, pt.y);
		}
		else {
			mQtWindow->setMinimumSize(0, 0);
			mQtWindow->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
		}
	}
}

void Window :: implSetFloating(bool enable) {
	rebuild();
}

void Window :: implSetMouseMove(bool enable) {
	if(mQtWindow) {
		mQtWindow->setMouseTracking(enable);
	}
}

void Window :: implSetSync(bool enable) {
}

void Window :: implSetCursor(bool enable) {
	if(mQtWindow) {
		if(enable) {
			mQtWindow->setCursor(QCursor(Qt::ArrowCursor));
		}
		else {
			mQtWindow->setCursor(QCursor(Qt::BlankCursor));
		}
	}
}

void Window :: implSetCursorStyle(CursorStyle style) {
	if(mQtWindow) {
		switch(style) {
		case IBEAM:			mQtWindow->setCursor(QCursor(Qt::IBeamCursor)); break;
		case CROSSHAIR:		mQtWindow->setCursor(QCursor(Qt::CrossCursor)); break;
	    case CLOSED_HAND:	mQtWindow->setCursor(QCursor(Qt::ClosedHandCursor)); break;
		case POINTING_HAND:	mQtWindow->setCursor(QCursor(Qt::PointingHandCursor)); break;
		case RESIZE_LEFT:	mQtWindow->setCursor(QCursor(Qt::SizeHorCursor)); break;
		case RESIZE_RIGHT:	mQtWindow->setCursor(QCursor(Qt::SizeHorCursor)); break;
		case RESIZE_LEFTRIGHT:	mQtWindow->setCursor(QCursor(Qt::SizeHorCursor)); break;
		case RESIZE_UP:		mQtWindow->setCursor(QCursor(Qt::SizeVerCursor)); break;
		case RESIZE_DOWN:	mQtWindow->setCursor(QCursor(Qt::SizeVerCursor)); break;
		case RESIZE_UPDOWN:	mQtWindow->setCursor(QCursor(Qt::SizeVerCursor)); break;
	 //   case DISAPPEARING_ITEM	mQtWindow->setCursor(QCursor(Qt::ArrowCursor)); break;

		case ARROW:
		default:
			mQtWindow->setCursor(QCursor(Qt::ArrowCursor));
		}
	}
}

void Window :: implCloseFocusedWindow() {

}

int Window :: implActiveWindowCount() {
    return 1;
}

WindowSystem * WindowSystem::cTheWindowSystem = 0;


WindowSystem :: WindowSystem()
//:   mRootContext(0)
{
}

WindowSystem :: ~WindowSystem() {
//	if(mRootContext) glXDestroyContext(screens::g_display, mRootContext);
}

WindowSystem& WindowSystem::get() {
	if (!WindowSystem::cTheWindowSystem)
		WindowSystem::cTheWindowSystem = new WindowSystem();
	return *WindowSystem::cTheWindowSystem;
}

Window * WindowSystem :: createWindow(lua_State *L, const char *title, int x, int y, int w, int h) {
	return new Window(L, this, title, x, y, w, h);
}

bool WindowSystem :: hasContext() const {
//	return mRootContext != NULL;
	return false;
}


} // luaav::
