
#if defined (__APPLE__) || defined (OSX)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
#endif

#if defined (__LINUX__) || defined (__linux__) || defined (UNIX)
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#ifdef WIN32

	#include <windows.h>

	#include <gl/gl.h>
	#include <gl/glu.h>

	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )

#endif

#include "luaav_app_window.h"
#include "types/al_pq.hpp"
//#include "allocore/types/al_MsgQueue.hpp"
#include "lua_glue.h"


namespace luaav {


Window :: Window(lua_State *state, WindowSystem *ws, const char *title, int x, int y, int w, int h)
 :	ws(ws),
	mTitle(title),
	mFrames(0),
	mAutoClear(true),
	mFps(WINDOW_DEFAULT_FPS),
	mFramePeriod(1./WINDOW_DEFAULT_FPS),
	mFirstFrame(true),
	mFullscreen(false),
	mFloating(false),
	mStereo(false),
	mBorder(true),
	mGrow(true),
	mCursor(true),
	mSync(false),
	mMouseMove(false),
	mFinalizing(false),
	mInitializing(false),
	mRebuilding(false),
	mChangedFlag("shift"),
	mShift(false),
	mCtrl(false),
	mAlt(false),
	mCmd(false),
	mCursorStyle(ARROW),
	mActive(false),
	mScheduled(false)
{
	mState = luaav_getrootstate(state);
	
	mOrigin[0] = x;
	mOrigin[1] = y;
	mSize[0] = w;
	mSize[1] = h;
	mLastSize[0] = w;
	mLastSize[1] = h;

	mClearColor[0] = 0.;
	mClearColor[1] = 0.;
	mClearColor[2] = 0.;
	mClearColor[3] = 1.;

	implInit();
}

Window :: ~Window () {
	close(false);
	implDestruct();
}

void Window :: rebuild() {
	if(mInitializing || mRebuilding) return;
	mRebuilding = true;
	//printf("rebuild() %p\n", this);

	// flag to know if prev_frame is valid
	luaav_rect prev_frame;
	bool prev_frame_valid = implInitRebuild(prev_frame);

	// if prev frame is valid, then we are also enabled
	if(prev_frame_valid) {
		disable();
	}

	// create the window
	luaav_rect frame;
	implChooseFrame(prev_frame_valid, prev_frame.x, prev_frame.y, frame);
	implCreate(frame);

	sync_attributes();

	// reveal the window
	show();
	enable();	// adds window to scheduler
	mRebuilding = false;

	mLastSize[0] = frame.width;
	mLastSize[1] = frame.height;

	mFrames = 0;
}

void Window :: enter_camera(luaav_pt &size) {
	float aspect = (float)size.x / (float)size.y;
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)aspect, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(	0, 0, 4,
					0, 0, 0,
					0, 1, 0);
}

void Window :: draw() {
	implMakeCurrent();

	luaav_pt size = implGetSize();
	glViewport(0, 0, size.x, size.y);

	if(	mLastSize[0] != size.x ||
		mLastSize[1] != size.y) {
		resize_lua();
	}

	mLastSize[0] = size.x;
	mLastSize[1] = size.y;

	
	if(mFrames <= 0) {
		if(mFirstFrame) {
			create_lua();
			mFirstFrame = false;
		}
	
		init_lua();

		// clear for first frame
		glDrawBuffer(GL_BACK);
		glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Perform drawing here
	glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    if(mStereo) {
        glDrawBuffer(GL_BACK_RIGHT);
        enter_camera(size);
        draw_lua("right");

        glDrawBuffer(GL_BACK_LEFT);
		glClear(GL_DEPTH_BUFFER_BIT);	// needed on ubuntu for some reason
        enter_camera(size);
        draw_lua("left");
    }
    else {
        enter_camera(size);
        draw_lua("mono");
    }

    implFlush();

	// erase here so we can draw into the buffer during intermediate callbacks like mouse and key
	if(mAutoClear) {
		glDrawBuffer(GL_BACK);
		glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	implEndDraw();

	mFrames++;
}

void Window :: enter_lua() {
	implMakeCurrent();
	luaav_pt size = implGetSize();
	enter_camera(size);
}

void Window :: create_lua() {
	lua_State *L = mState;
	
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "create");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);	// env table and udata
	}
}

void Window :: init_lua() {
	lua_State *L = mState;

	lua::findmodule(L, "muro");
	if(lua_istable(L, -1)) {
		lua_getfield(L, -1, "initctx");
		if(!lua_isfunction(L, -1)) {
			luaL_error(L, "Window.destchanged: couldn't find valid muro.initctx function");
		}
		lua::push<const char *>(L, mTitle.c_str());
		lua::pcall(L, 1);
	}
	lua_pop(L, 1);
	
	lua::findmodule(L, "opengl");
	if(lua_istable(L, -1)) {
		lua_getfield(L, -1, "initctx");
		if(!lua_isfunction(L, -1)) {
			luaL_error(L, "Window.destchanged: couldn't find valid opengl.initctx function");
		}
		lua::push<const char *>(L, mTitle.c_str());
		lua::pcall(L, 1);
	}
	lua_pop(L, 1);

	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "init");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);	// env table and udata
	}

	// also call resize when dest changes
	resize_lua();
}

void Window :: closing_lua() {
	// do Lua callback
	lua_State *L = mState;
	if(!L) return;

	lua::findmodule(L, "muro");
	if(lua_istable(L, -1)) {
		lua_getfield(L, -1, "closectx");
		if(!lua_isfunction(L, -1)) {
			luaL_error(L, "Window.destclosing: couldn't find valid muro.closectx function");
		}
		lua::push<const char *>(L, mTitle.c_str());
		lua::pcall(L, 1);
	}
	lua_pop(L, 1);
	
	lua::findmodule(L, "opengl");
	if(lua_istable(L, -1)) {
		lua_getfield(L, -1, "closectx");
		if(!lua_isfunction(L, -1)) {
			luaL_error(L, "Window.destclosing: couldn't find valid opengl.closectx function");
		}
		lua::push<const char *>(L, mTitle.c_str());
		lua::pcall(L, 1);
	}
	lua_pop(L, 1);
	
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "closing");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);	// env table and udata
	}
}

void Window :: resize_lua() {
	// trigger rebuilding callback in script
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "resize");
		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);	// env table and udata
	}
}

void Window :: draw_lua(const char *eye) {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "draw");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::push<const char *>(L, eye);
			lua::pcall(L, 2);
			
			// run a gc sweep here
			lua_gc(L, LUA_GCCOLLECT, 0);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);
	}
}

void Window :: mouse_lua(WindowEvent event, const char *btn, float x, float y, int nclicks) {

	// handle state changes based on events
	switch(event) {
		case MOUSE_ENTER:
			if(mCursor)		implSetCursorStyle(mCursorStyle);
			else			implSetCursor(mCursor);
			break;

		case MOUSE_EXIT:
			if(!mCursor)	implSetCursor(true);
			implSetCursorStyle(ARROW);
			break;

		default:
			break;
	}

	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "mouse");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);

			switch(event) {
				case MOUSE_DOWN:	lua::push<const char *>(L, "down"); break;
				case MOUSE_DRAG:	lua::push<const char *>(L, "drag"); break;
				case MOUSE_UP:		lua::push<const char *>(L, "up"); break;
				case MOUSE_MOVE:	lua::push<const char *>(L, "move"); break;
				case MOUSE_ENTER:	lua::push<const char *>(L, "enter"); break;
				case MOUSE_EXIT:	lua::push<const char *>(L, "exit"); break;
			}

			lua::push<const char *>(L, btn);
			lua::push<float>(L, x);
			lua::push<float>(L, y);
			lua::push<int>(L, nclicks);
			
			enter_lua(); // prepare drawing context
			lua::pcall(L, 6);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);
	}
}

void Window :: mouse_lua(WindowEvent event, const char *btn, float x, float y) {

	// handle state changes based on events
	switch(event) {
		case MOUSE_ENTER:
			if(mCursor)		implSetCursorStyle(mCursorStyle);
			else			implSetCursor(mCursor);
			break;

		case MOUSE_EXIT:
			if(!mCursor)	implSetCursor(true);
			implSetCursorStyle(ARROW);
			break;

		default:
			break;
	}

	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
	
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "mouse");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);

			switch(event) {
				case MOUSE_DOWN:	lua::push<const char *>(L, "down"); break;
				case MOUSE_DRAG:	lua::push<const char *>(L, "drag"); break;
				case MOUSE_UP:		lua::push<const char *>(L, "up"); break;
				case MOUSE_MOVE:	lua::push<const char *>(L, "move"); break;
				case MOUSE_ENTER:	lua::push<const char *>(L, "enter"); break;
				case MOUSE_EXIT:	lua::push<const char *>(L, "exit"); break;
			}

			lua::push<const char *>(L, btn);
			lua::push(L, x);
			lua::push(L, y);
			
			enter_lua(); // prepare drawing context
			lua::pcall(L, 5);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);
	}
}

void Window :: scroll_lua(WindowEvent event, const char *btn, float x, float y, float dx, float dy) {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "scroll");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);

			lua::push<float>(L, x);
			lua::push<float>(L, y);
			lua::push<float>(L, dx);
			lua::push<float>(L, dy);
			
			enter_lua(); // prepare drawing context
			lua::pcall(L, 5);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);
	}
}

void Window :: modifiers_lua() {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "modifiers");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::push<const char *>(L, mChangedFlag.c_str());
			
			enter_lua(); // prepare drawing context
			lua::pcall(L, 2);
		}
		else {
			lua_pop(L, 1);
		}

		lua_pop(L, 2);
	}
}

void Window :: key_lua(const char *event, int key) {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "key");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::push<const char *>(L, event);
			lua::push<int>(L, key);
			
			enter_lua(); // prepare drawing context
			lua::pcall(L, 3);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}
}

void Window :: copy_lua() {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "copy");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}
}

void Window :: cut_lua() {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "cut");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}
}

void Window :: paste_lua() {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "paste");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}
}

void Window :: selectall_lua() {
	lua_State *L = mState;
	if(Glue<Window>::usr_get_reference_map(L, this)) {
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "selectall");

		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}
}

void Window :: finalize(Window *win) {
	if(!(win->mFinalizing || win->mRebuilding)) {
		win->close(true);

		win->mFinalizing = true;


		win->closing_lua();
		/*
		// trigger closing callback in script
		lua_State *L = win->getState();
		instance_to_userdata(L, win);
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "closing");
		if(lua_isfunction(L, -1)) {
			lua_pushvalue(L, -3);
			lua::pcall(L, 1);
		}
		else {
			lua_pop(L, 1);
		}
		lua_pop(L, 2);	// env table and udata
		*/
		

		lua_State *L = win->mState;
		lua_gc(L, LUA_GCCOLLECT, 1); // force a full collection (no need to worry about speed)
		lua_gc(L, LUA_GCCOLLECT, 1); // force a second collection for weak sentinel ref
		lua_gc(L, LUA_GCCOLLECT, 1); // force a third collection for weak sentinel ref
		win->mFinalizing = false;
	}
}

void Window :: enable() {
	if(mState) {
		mActive = true;
		if (!mScheduled) {
			luaav_state * S = luaav_fromstate(mState);
			al::PQHelper queue(luaav_state_mainpq(S));
			mScheduled = queue.send(mFramePeriod, mFramePeriod, Window::draw, this, S);
		}
	}
}

void Window :: disable() {
	// setting mActive to false will trigger removal from the scheduler
	mActive = false;
}

void Window :: sync_attributes() {
	implSetFloating(mFloating);
	implSetCursor(mCursor);
	implSetCursorStyle(mCursorStyle);
	implSetSync(mSync);
	implSetMouseMove(mMouseMove);
}

void Window :: close(bool user_did_close) {
	if(! mFinalizing) {
	
		mFinalizing = true;
		disable();

		//printf("%x Window.close userDidClose: %s\n", this, userDidClose ? "true" : "false");
		if(user_did_close) {
			implNull();
		}
		else {
			implRelease();
		}
		mFinalizing = false;
	}
}

void Window :: show() {
	implShow();
}

void Window :: hide() {
	implHide();
}

void Window :: raise() {
	implRaise();
}

void Window :: lower() {
	implLower();
}

void Window :: lock() {
	implLock();
}

void Window :: unlock() {
	implUnlock();
}

void Window :: flush() {
	implFlush();
}

void Window :: add_close_menuitem() {
	lua_State *L = luaav_getstate(luaav_app_state());
	lua::call<
			const char *
		>(L, "menuitem_exists", "(File).(Close Window)");

	bool item_exists = lua_isboolean(L, -1) ? (lua_toboolean(L, -1) ? true : false) : false;
	lua_pop(L, 1);
	if(!item_exists) {
		lua::call<
				const char *, 
				const char *, 
				lua_CFunction
			>(L, "addmenuitem", "(File)", "Close Window", menu_close);
	}
}

int Window :: menu_close(lua_State *L) {
	implCloseFocusedWindow();
	/*if(implActiveWindowCount() > 0) {
		//remove menu item
	}*/
	return 0;
}

int Window :: draw(al_sec now, Window * win, luaav_state * S) {
	if (win->mActive) {
		win->draw();
		al::PQHelper queue(luaav_state_mainpq(S));
		//al_sec sleep = win->mFramePeriod;
		//al_sec sleep = win->mFramePeriod + luaav_state_reallatency(S); 
		al_sec sleep = MAX(win->mFramePeriod, luaav_state_reallatency(S));
		queue.send(sleep, win->mFramePeriod, Window::draw, win, S);
	} else {
		win->mScheduled = false;
	}
	return 0; // return 1 to defer this call... e.g. if win is not ready?
}

} // luaav::

using luaav::Window;


/*! An OpenGL Window.
	An OpenGL Window.  Manages OS event callbacks within the window and during
	window focus.

	Current callbacks are:
	<luacode>
		Window:create()
		Window:init()
		Window:closing()
		Window:key(event, key)
		Window:modifiers()
		Window:mouse(event, btn, x, y, nclicks)
		Window:scroll(x, y, dx, dy)
		Window:resize()
		Window:draw()
	</luacode>
	
	@module app.Window
*/

/*! Close the window
	@LuaMethod METHOD
	@name M:close
*/
int window_close(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		s->closing_lua();
		s->close(false);
	}
	else {
		luaL_error(L, "Window.close: invalid object");
	}
	
	return 0;
}


/*! Title
	@param title Window title
	@LuaMethod GETSET
	@name M.title
*/
int window_title(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<const char *>(L, 2)) {
			const char *v = lua::to<const char *>(L, 2);
			s->mTitle.assign(v);
			s->implSetTitle(v);
		}
		else {
			lua::push<const char *>(L, s->mTitle.data());
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.title: invalid object");
	}
	return 0;
}

/*! Autoclear
	@param autoclear Flag to automatically clear the window
	@LuaMethod GETSET
	@name M.autoclear
*/
int window_autoclear(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			s->mAutoClear = lua::to<bool>(L, 2);
		}
		else {
			lua::push<bool>(L, s->mAutoClear);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.autoclear: invalid object");
	}
	
	return 0;
}

/*! Clear color
	@param clearcolor RGBA color vector
	@LuaMethod GETSET
	@name M.clearcolor
*/
int window_clearcolor(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		float *cc = s->mClearColor;
		if(lua::to_vec_t(L, 2, 4, cc)) {
			return 0;
		}
		else {
			lua::push_vec_t(L, 4, cc);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.clearcolor: invalid object or arguments");
	}
	return 0;
}

/*! Origin
	@param origin XY vector
	@LuaMethod GETSET
	@name M.origin
*/
int window_origin(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::to_vec_t(L, 2, 2, s->mOrigin)) {
			luaav_rect frame = s->implGetFrame();
			frame.x = (int)s->mOrigin[0];
			frame.y = (int)s->mOrigin[1];
			s->implSetFrame(frame);

			return 0;
		}
		else {
			luaav_rect frame = s->implGetFrame();
			lua_newtable(L);
			lua::push(L, frame.x);
			lua_rawseti(L, -2, 1);
			lua::push(L,frame.y);
			lua_rawseti(L, -2, 2);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.origin: invalid object or arguments");
	}
	return 0;
}

/*! Dimensions
	@param dim XY vector
	@LuaMethod GETSET
	@name M.dim
*/
int window_dim(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::to_vec_t(L, 2, 2, s->mSize)) {
			luaav_rect frame = s->implGetFrame();
			frame.width = (int)s->mSize[0];
			frame.height = (int)s->mSize[1];
			s->implSetFrame(frame);

			return 0;
		}
		else {
			luaav_rect frame = s->implGetFrame();

			lua_newtable(L);
			lua::push(L, frame.width);
			lua_rawseti(L, -2, 1);
			lua::push(L, frame.height);
			lua_rawseti(L, -2, 2);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.dim: invalid object or arguments");
	}
	return 0;
}

/*! Frames per second
	@param fps FPS
	@LuaMethod GETSET
	@name M.fps
*/
int window_fps(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<float>(L, 2)) {
			float v = lua::to<float>(L, 2);
			s->mFps = (v < 0.001) ? 0.001 : v;
			s->mFramePeriod = 1.f/s->mFps;
		}
		else {
			lua::push(L, s->mFps);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.fps: invalid object or arguments");
	}
	return 0;
}

/*! Vertical sync flag
	@param vsync Vsync flag
	@LuaMethod GETSET
	@name M.sync
*/
int window_sync(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mSync) {
				s->mSync = enable;
				s->implSetSync(enable);
			}
		}
		else {
			lua::push(L, s->mSync);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.sync: invalid object or arguments");
	}
	return 0;
}

/*! Fullscreen flag
	@param fullscreen Fullscreen flag
	@LuaMethod GETSET
	@name M.fullscreen
*/
int window_fullscreen(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mFullscreen) {
				s->mFullscreen = enable;
				s->implSetFullscreen(enable);
			}
			return 0;
		}
		else {
			lua::push(L, s->mFullscreen);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.fullscreen: invalid object or arguments");
	}
	return 0;
}

/*! Floating flag
	@param floating Floating flag
	@LuaMethod GETSET
	@name M.floating
*/
int window_floating(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mFloating) {
				s->mFloating = enable;
				s->implSetFloating(enable);
			}
			return 0;
		}
		else {
			lua::push(L, s->mFloating);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.floating: invalid object or arguments");
	}
	return 0;
}

/*! Stereo flag
	@param stereo Stereo flag
	@LuaMethod GETSET
	@name M.stereo
*/
int window_stereo(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mStereo) {
				s->mStereo = enable;
				s->rebuild();
			}
		}
		else {
			lua::push(L, s->mStereo);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.stereo: invalid object or arguments");
	}
	return 0;
}

/*! Border flag
	@param border Border flag
	@LuaMethod GETSET
	@name M.border
*/
int window_border(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mBorder) {
				s->mBorder = enable;
				s->implSetBorder(enable);
			}
		}
		else {
			lua::push(L, s->mBorder);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.border: invalid object or arguments");
	}
	return 0;
}

/*! Grow flag
	@param grow Grow flag
	@LuaMethod GETSET
	@name M.grow
*/
int window_grow(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mGrow) {
				s->mGrow = enable;
				s->implSetGrow(enable);
			}
		}
		else {
			lua::push(L, s->mGrow);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.grow: invalid object or arguments");
	}
	return 0;
}

/*! Flush and swap Window contents
	@LuaMethod METHOD
	@name M:flush
*/
int window_flush(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		s->draw();
	}
	else {
		luaL_error(L, "Window.flush: invalid object");
	}
	return 0;
}

/*! Mouse move event flag
	@param mousemove Mousemove flag
	@LuaMethod GETSET
	@name M.mousemove
*/
int window_mousemove(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mMouseMove) {
				s->mMouseMove = enable;
				s->implSetMouseMove(enable);
			}
		}
		else {
			lua::push(L, s->mMouseMove);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.mousemove: invalid object or arguments");
	}
	return 0;
}

/*! Cursor visible flag
	@param cursor Cursor flag
	@LuaMethod GETSET
	@name M.cursor
*/
int window_cursor(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<bool>(L, 2)) {
			bool enable = lua::to<bool>(L, 2);
			if(enable != s->mCursor) {
				s->mCursor = enable;
				s->implSetCursor(enable);
			}
		}
		else {
			lua::push(L, s->mCursor);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.cursor: invalid object or arguments");
	}
	return 0;
}

/*! Cursor style
	@param cursorstyle Window.ARROW, Window.IBEAM, Window.CROSSHAIR,
						Window.CLOSED_HAND, Window.OPEN_HAND, Window.POINTING_HAND,
						Window.RESIZE_LEFT, Window.RESIZE_RIGHT, Window.RESIZE_LEFTRIGHT,
						Window.RESIZE_UP, Window.RESIZE_DOWN, Window.RESIZE_UPDOWN,
						Window.DISAPPEARING_ITEM,
	@LuaMethod GETSET
	@name M.cursorstyle
*/
int window_cursorstyle(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		if(lua::is<int>(L, 2)) {
			Window::CursorStyle style = (Window::CursorStyle)lua::to<int>(L, 2);
			s->mCursorStyle = style;
			s->implSetCursorStyle(style);
		}
		else {
			lua::push<int>(L, (int)s->mCursorStyle);
			return 1;
		}
	}
	else {
		luaL_error(L, "Window.cursorstyle: invalid object or arguments");
	}
	return 0;
}

/*! Shift key flag
	@ret shift flag
	@LuaMethod GET
	@name M.shift
*/
int window_shift(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		lua::push(L, s->mShift);
	}
	else {
		luaL_error(L, "Window.shift: invalid object or arguments");
	}
	return 1;
}

/*! Ctrl key flag
	@ret ctrl flag
	@LuaMethod GET
	@name M.ctrl
*/
int window_ctrl(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		lua::push(L, s->mCtrl);
	}
	else {
		luaL_error(L, " Window.ctrl: invalid object or arguments");
	}
	return 1;
}

/*! Alt key flag
	@ret alt flag
	@LuaMethod GET
	@name M.alt
*/
int window_alt(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		lua::push(L, s->mAlt);
	}
	else {
		luaL_error(L, "Window.alt: invalid object or arguments");
	}
	return 1;
}

/*! Cmd key flag
	@ret Cmd flag
	@LuaMethod GET
	@name M.cmd
*/
int window_cmd(lua_State *L) {
	Window *s = Glue<Window>::to(L, 1);
	if(s) {
		lua::push(L, s->mCmd);
	}
	else {
		luaL_error(L, "Window.cmd: invalid object or arguments");
	}
	return 1;
}

template<> const char * Glue<Window>::usr_name() { return "Window"; }
template<> bool Glue<Window>::usr_has_index() { return true; }


template<> void Glue<Window>::usr_push(lua_State * L, Window * u) {
	Glue<Window>::usr_attr_push(L);
	Glue<Window>::usr_attr_prototype(L);
	Glue<Window>::usr_set_reference_map(L, u);
}

template<> Window * Glue<Window>::usr_new(lua_State * L) {
	Window *s = NULL;
	if(Glue<Window>::usr_attr_is_prototype(L)) {
		const char *title = lua::optfield<const char *>(L, 1, "", "title");

		lua_pushstring(L, "origin");
		lua_rawget(L, -2);
			int origin[] = {0, 0};
			lua::to_vec_t<int>(L, -1, 2, origin);
		lua_pop(L, 1);

		lua_pushstring(L, "dim");
		lua_rawget(L, -2);
			int dim[] = {512, 512};
			lua::to_vec_t<int>(L, -1, 2, dim);
		lua_pop(L, 1);

		s = s->implCreate(L, title, origin[0], origin[1], dim[0], dim[1]);
		s->mInitializing = true;	// to avoid constant rebuild on attribute setting
	}
	else {
		const char *title = lua::opt<const char *>(L, 1, "");
		
		int argc = lua::is<int>(L, 1) ? 1 : 2;
		int x = lua::opt<int>(L, argc, 0);
		int y = lua::opt<int>(L, argc+1, 0);
		int w = MAX(lua::opt<int>(L, argc+2, 512), 5);
		int h = MAX(lua::opt<int>(L, argc+3, 512), 5);

		s = Window::implCreate(L, title, x, y, w, h);
	}

	s->mInitializing = false;
	s->rebuild();

	return s;
}

template<> void Glue<Window>::usr_index(lua_State * L, Window * u) {
	Glue<Window>::usr_attr_index(L);
}

template<> void Glue<Window>::usr_newindex(lua_State * L, Window * u) {
	Glue<Window>::usr_attr_newindex(L);
}

template<> int Glue<Window>::usr_tostring(lua_State * L, Window * u) {
	lua_pushfstring(L, "%s: %p", Glue<Window>::usr_name(), u); 
	return 1;
}

template<> void Glue<Window>::usr_gc(lua_State * L, Window * u) {
	// window_close(L);
	delete u;
}


typedef struct _CursorStyle_Reg {
	const char *name;
	luaav::Window::CursorStyle style;
} CursorStyle_Reg;

typedef struct _WindowEvent_Reg {
	const char *name;
	luaav::Window::WindowEvent event;
} WindowEvent_Reg;


#define ENUM_STYLE_ENTRY(name) {#name, Window::name}

const CursorStyle_Reg lua_cursorstyles[] = {
	ENUM_STYLE_ENTRY(ARROW),
	ENUM_STYLE_ENTRY(IBEAM),
	ENUM_STYLE_ENTRY(CONTEXTUAL_MENU),
	ENUM_STYLE_ENTRY(CROSSHAIR),
	ENUM_STYLE_ENTRY(CLOSED_HAND),
	ENUM_STYLE_ENTRY(DRAG_COPY),
	ENUM_STYLE_ENTRY(DRAG_LINK),
	ENUM_STYLE_ENTRY(NO_OP),
	ENUM_STYLE_ENTRY(OPEN_HAND),
	ENUM_STYLE_ENTRY(POINTING_HAND),
	ENUM_STYLE_ENTRY(RESIZE_LEFT),
	ENUM_STYLE_ENTRY(RESIZE_RIGHT),
	ENUM_STYLE_ENTRY(RESIZE_LEFTRIGHT),
	ENUM_STYLE_ENTRY(RESIZE_UP),
	ENUM_STYLE_ENTRY(RESIZE_DOWN),
	ENUM_STYLE_ENTRY(RESIZE_UPDOWN),
	ENUM_STYLE_ENTRY(DISAPPEARING_ITEM),
	{0, (Window::CursorStyle)0}
};

const WindowEvent_Reg lua_windowevents[] = {
	ENUM_STYLE_ENTRY(MOUSE_DOWN),
	ENUM_STYLE_ENTRY(MOUSE_DRAG),
	ENUM_STYLE_ENTRY(MOUSE_UP),
	ENUM_STYLE_ENTRY(MOUSE_SCROLL),
	ENUM_STYLE_ENTRY(MOUSE_ENTER),
	ENUM_STYLE_ENTRY(MOUSE_EXIT),
	{0, (Window::WindowEvent)0}
};

/*
int get_cursor_styles(lua_State *L) {
	lua_newtable(L);
	for(int i=0; lua_cursorstyles[i].name; i++) {
		lua::push(L, lua_cursorstyles[i].name);
		lua_pushinteger(L, lua_cursorstyles[i].style);
		lua_rawset(L, -3);
	}
	return 1;
}
*/

template<> void Glue<Window>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "close", window_close },
		{ "flush", window_flush },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "title", window_title },
		{ "autoclear", window_autoclear },
		{ "clearcolor", window_clearcolor },
		{ "origin", window_origin },
		{ "dim", window_dim },
		{ "fps", window_fps },
		{ "sync", window_sync },
		{ "fullscreen", window_fullscreen },
		{ "floating", window_floating },
		{ "stereo", window_stereo },
		{ "border", window_border },
		{ "grow", window_grow },
		{ "mousemove", window_mousemove },
		{ "cursor", window_cursor },
		{ "cursorstyle", window_cursorstyle },
		{ "shift", window_shift },
		{ "ctrl", window_ctrl },
		{ "alt", window_alt },
		{ "cmd", window_cmd },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "title", window_title },
		{ "autoclear", window_autoclear },
		{ "clearcolor", window_clearcolor },
		{ "origin", window_origin },
		{ "dim", window_dim },
		{ "fps", window_fps },
		{ "sync", window_sync },
		{ "fullscreen", window_fullscreen },
		{ "floating", window_floating },
		{ "stereo", window_stereo },
		{ "border", window_border },
		{ "grow", window_grow },
		{ "mousemove", window_mousemove },
		{ "cursor", window_cursor },
		{ "cursorstyle", window_cursorstyle },
		{ NULL, NULL}
	};
	Glue<Window>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_window(lua_State *L) {
	Glue<Window>::define(L);
	Glue<Window>::register_class(L);
	luaL_getmetatable(L, Glue<Window>::mt_name(L));
	
	for(int i=0; lua_cursorstyles[i].name; i++) {
		lua::push<const char *>(L, lua_cursorstyles[i].name);
		lua::push<int>(L, (int)lua_cursorstyles[i].style);
		lua_rawset(L, -3);
	}
	for(int i=0; lua_windowevents[i].name; i++) {
		lua::push<const char *>(L, lua_windowevents[i].name);
		lua::push<int>(L, (int)lua_windowevents[i].event);
		lua_rawset(L, -3);
	}
	
	return 1;
}


#ifdef __cplusplus
}
#endif
