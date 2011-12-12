#include "luaav_app_window_opengl_qt.h"
#include <stdio.h>
#include <QHBoxLayout>
#include <QGLFormat>
#include <QIcon>
#include <QX11Info>
#include "luaav_linux.h"
#include <cstring>

#include <X11/X.h>
#include <X11/Xlib.h>


const char * name_for_qt_button(Qt::MouseButtons btn) {
	switch(btn) {
	case Qt::LeftButton:	return "left";
	case Qt::RightButton:	return "right";
	case Qt::MidButton:		return "mid";
	default:				return "<unknown>";
	}
}

OpenGLWidget :: OpenGLWidget(luaav::Window *generic, QGL::FormatOptions format, QWidget *parent)
:	mGeneric(generic),
	QGLWidget(QGLFormat(format), parent)
{}

OpenGLWidget :: ~OpenGLWidget()
{}

void OpenGLWidget :: initializeGL() {
//	printf("initializeGL\n");
}

void OpenGLWidget :: paintGL() {

}

void OpenGLWidget :: resizeGL(int width, int height) {
	mGeneric->resize_lua();
}
/*
void OpenGLWidget :: keyPressEvent(QKeyEvent *e) {
	mGeneric->key("down", e->key());
}

void OpenGLWidget :: keyReleaseEvent(QKeyEvent *e) {
	mGeneric->key("up", e->key());
}
*/


OpenGLWindow :: OpenGLWindow(luaav::Window *generic, bool fullscreen, QGL::FormatOptions format, const char *title, int x, int y, int w, int h)
:	mGeneric(generic),
	mFormat(format),
	mGLContext(0),
	mGLXView(0),
	mDownBtn(Qt::NoButton)
{
	setWindowTitle(tr(title));
	setGeometry(x, y, w, h);
	setWindowIcon(QIcon(QPixmap(LUAAV_ICON_PATH)));

	if(fullscreen) {
		enableFullscreen(x, y, w, h);
	}
	else {
		mGLContext = new OpenGLWidget(mGeneric, mFormat, this);

		QHBoxLayout *layout = new QHBoxLayout;
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(mGLContext);
		setLayout(layout);
	}
}

OpenGLWindow :: ~OpenGLWindow() {
	if(mGLXView) {
		const QX11Info &xinfo = x11Info();
		Display *display = xinfo.display();
		glXMakeCurrent(display, None, NULL);
		glXDestroyContext(display, mGLXView);

		releaseKeyboard();
   	}
}

void OpenGLWindow :: makeCurrent() {
	if(mGLContext) {
		mGLContext->makeCurrent();
	}
	else if(mGLXView) {
		const QX11Info &xinfo = x11Info();
		Display *display = xinfo.display();
		Bool success = glXMakeCurrent(display, winId(), mGLXView);
	}
}

void OpenGLWindow :: swapBuffers() {
	if(mGLContext) {
		mGLContext->swapBuffers();
	}
	else if(mGLXView) {
		const QX11Info &xinfo = x11Info();
		Display *display = xinfo.display();
		glXSwapBuffers(display, winId());
	}
}

void OpenGLWindow :: enableFullscreen(int x, int y, int width, int height) {
	int buffer_attributes[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER, True,
		GLX_STEREO, False,
		None
	};

	if(mFormat & QGL::StereoBuffers) {
		buffer_attributes[9] = True;
	}

	const QX11Info &xinfo = x11Info();
	Display *display = xinfo.display();
	XVisualInfo *vi = NULL;
	Window window = 0;
	int fbcount = 0;
	GLXFBConfig *fbc = ::glXChooseFBConfig(
								display, 
								DefaultScreen(display), 
								buffer_attributes, 
								&fbcount
							);

	if(!fbc) {
		// ERROR
		fprintf(stderr, "unable to create GLXFBConfig\n");
		return;
	}

	vi = ::glXGetVisualFromFBConfig(display, fbc[0]);
	XSetWindowAttributes swa;
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.override_redirect = True;
	swa.colormap = XCreateColormap(
							display, 
							RootWindow(display, vi->screen), 
							vi->visual,
							AllocNone
					);

	window = XCreateWindow(
					display,
					RootWindow(display, vi->screen),
					x, y, width, height, 0, 
					vi->depth, 
					InputOutput, 
					vi->visual, 
					CWBorderPixel | CWColormap | CWOverrideRedirect, 
					&swa
				);

	if(!window) {
		// ERROR
		fprintf(stderr, "unable to create window\n");
		return;
	}


	create(window, true, true);	
}

void OpenGLWindow :: setMouseTracking(bool enable) {
	QWidget::setMouseTracking(enable);
	if(mGLContext) {
		mGLContext->setMouseTracking(enable);
	}
}

void OpenGLWindow :: showEvent(QShowEvent *e) {
	QWidget::showEvent(e);
	if(!mGLContext) {
		int buffer_attributes[] = {
			GLX_X_RENDERABLE, True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_DOUBLEBUFFER, True,
			GLX_STEREO, False,
			None
		};

		if(mFormat & QGL::StereoBuffers) {
			buffer_attributes[9] = True;
		}

		const QX11Info &xinfo = x11Info();
		Display *display = xinfo.display();
		XVisualInfo *vi = NULL;
		int fbcount = 0;
		GLXFBConfig *fbc = ::glXChooseFBConfig(
									display, 
									DefaultScreen(display), 
									buffer_attributes, 
									&fbcount
								);

		if(!fbc) {
			// ERROR
			fprintf(stderr, "unable to create GLXFBConfig\n");
			return;
		}

		vi = ::glXGetVisualFromFBConfig(display, fbc[0]);

		mGLXView = glXCreateContext(display, vi, NULL, GL_TRUE);

		setFocus(Qt::OtherFocusReason);
		grabKeyboard();
	}
}

void OpenGLWindow :: keyPressEvent(QKeyEvent *e) {
	int key = e->key();
	switch(key) {
		case Qt::Key_Shift:
			mGeneric->mShift = true;
			mGeneric->modifiers_lua();
			break;
			
		case Qt::Key_Control:
			mGeneric->mCtrl = true;
			mGeneric->modifiers_lua();
			break;

		case Qt::Key_Meta:
			mGeneric->mCmd = true;
			mGeneric->modifiers_lua();
			break;

		case Qt::Key_Alt:
			mGeneric->mAlt = true;
			mGeneric->modifiers_lua();
			break;

		case Qt::Key_Escape:
			key = 27;
	
		default:
			mGeneric->key_lua("down", key);
			break;
	}
}

void OpenGLWindow :: keyReleaseEvent(QKeyEvent *e) {
	int key = e->key();
	switch(key) {
		case Qt::Key_Shift:
			mGeneric->mShift = false;
			mGeneric->modifiers_lua();
			break;
			
		case Qt::Key_Control:
			mGeneric->mCtrl = false;
			mGeneric->modifiers_lua();
			break;

		case Qt::Key_Meta:
			mGeneric->mCmd = false;
			mGeneric->modifiers_lua();
			break;

		case Qt::Key_Alt:
			mGeneric->mAlt = false;
			mGeneric->modifiers_lua();
			break;

		case Qt::Key_Escape:
			key = 27;
	
		default:
			mGeneric->key_lua("up", key);
			break;
	}
}

void OpenGLWindow :: mouseMoveEvent(QMouseEvent *e) {
	const QPoint &pos = e->pos();	
	if(e->buttons() == Qt::NoButton) {
		mGeneric->mouse_lua(Window::MOUSE_MOVE, "left", pos.x(), pos.y());
	}
	else {
		mGeneric->mouse_lua(Window::MOUSE_DRAG, name_for_qt_button(mDownBtn), pos.x(), pos.y());
	}
}

void OpenGLWindow :: mousePressEvent(QMouseEvent *e) {
	const QPoint &pos = e->pos();
	mDownBtn = e->button();
	mGeneric->mouse_lua(Window::MOUSE_DOWN, name_for_qt_button(e->button()), pos.x(), pos.y());
}

void OpenGLWindow :: mouseReleaseEvent(QMouseEvent *e) {
	const QPoint &pos = e->pos();
	mGeneric->mouse_lua(Window::MOUSE_UP, name_for_qt_button(e->button()), pos.x(), pos.y());
}

void OpenGLWindow :: wheelEvent(QWheelEvent *e) {
	const QPoint &pos = e->pos();
	int degrees = e->delta() / 8;
	int steps = degrees / 15;

	int dx = 0;
	int dy = 0;
	if(e->orientation() == Qt::Horizontal) {
		dx = steps;
	}
	else {
		dy = steps;
	}

	mGeneric->scroll_lua(Window::MOUSE_DRAG, "left", pos.x(), pos.y(), dx, dy);
}
