#ifndef LUAAV_APP_WINDOW_OPENGL_QT_H
#define LUAAV_APP_WINDOW_OPENGL_QT_H

#include <QWidget>
#include <QGLWidget>
#include <QKeyEvent>
#include "luaav_app_window.h"

#include <GL/glx.h>

using luaav::Window;

class OpenGLWidget : public QGLWidget {
	Q_OBJECT

public:
	OpenGLWidget(luaav::Window *generic, QGL::FormatOptions format, QWidget *parent=0);
	~OpenGLWidget();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

//	void keyPressEvent(QKeyEvent *e);
//	void keyReleaseEvent(QKeyEvent *e);

	

	luaav::Window *mGeneric;
};



class OpenGLWindow : public QWidget {
	Q_OBJECT

public:
	OpenGLWindow(luaav::Window *generic, bool fullscreen, 
					QGL::FormatOptions format, const char *title, 
					int x, int y, int w, int h);
	~OpenGLWindow();

	void makeCurrent();
	void swapBuffers();
	void setMouseTracking(bool enable);


protected:
	void enableFullscreen(int x, int y, int width, int height);

	void showEvent(QShowEvent *e);
	
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

	void wheelEvent(QWheelEvent *e);

	luaav::Window *mGeneric;
	QGL::FormatOptions mFormat;
	OpenGLWidget *mGLContext;
	GLXContext mGLXView;
	Qt::MouseButtons mDownBtn;
};

#endif	// LUAAV_APP_WINDOW_OPENGL_QT_H
