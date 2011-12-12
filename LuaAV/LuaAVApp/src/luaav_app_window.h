#ifndef INCLUDE_LUAAV_WINDOW_H
#define INCLUDE_LUAAV_WINDOW_H 1

#include "luaav.h"
#include "luaav_app.h"
#include "luaav_utility.hpp"
#include "lua_utility.h"

#include <string>


#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_window(lua_State *L);

#ifdef __cplusplus
}
#endif

namespace luaav {

class WindowSystem;
class WindowImpl;

#define WINDOW_DEFAULT_FPS	33.


class LUAAV_API Window {
protected:
	typedef Window			Self;
	typedef WindowImpl		Implementation;

public:
	enum CursorStyle {
		ARROW = 0,
		IBEAM,
		CONTEXTUAL_MENU,
		CROSSHAIR,
		CLOSED_HAND,
		DRAG_COPY,
		DRAG_LINK,
		NO_OP,
		OPEN_HAND,
		POINTING_HAND,
		RESIZE_LEFT,
		RESIZE_RIGHT,
		RESIZE_LEFTRIGHT,
		RESIZE_UP,
		RESIZE_DOWN,
		RESIZE_UPDOWN,
		DISAPPEARING_ITEM,
	};

	enum WindowEvent {
		MOUSE_DOWN = 0,
		MOUSE_DRAG,
		MOUSE_UP,
		MOUSE_SCROLL,
		MOUSE_MOVE,
		MOUSE_ENTER,
		MOUSE_EXIT,
	};


public:
	Window(lua_State *state, WindowSystem  *ws, const char *title, int x, int y, int w, int h);
	~Window();

    void rect(luaav_rect *r);
	void rebuild();
	void enter_camera(luaav_pt &size);
	void draw();
	void enter_lua();
	
	// Lua Callbacks
	void create_lua();
	void init_lua();
	void closing_lua();	
	void resize_lua();
	void draw_lua(const char *eye);
	void mouse_lua(WindowEvent event, const char *btn, float x, float y, int nclicks);
	void mouse_lua(WindowEvent event, const char *btn, float x, float y);
	void scroll_lua(WindowEvent event, const char *btn, float x, float y, float dx, float dy);
	void modifiers_lua();
	void key_lua(const char *event, int key);
	void copy_lua();
	void cut_lua();
	void paste_lua();
	void selectall_lua();
	static void finalize(Window *win);


	void enable();
	void disable();

	void sync_attributes();

	static int draw(al_sec now, Window * win, luaav_state * S);

	void close(bool user_did_close);

	void make_current();
	void show();
	void hide();
	void raise();
	void lower();

	void lock();
	void unlock();
	void flush();

	// app menu interaction
	void add_close_menuitem();
	static int menu_close(lua_State *L);	// menu callback

	// creation
	void implInit();
	void implDestruct();
	static Window * implCreate(lua_State *state, const char *title, int x, int y, int w, int h);

	// window management
	bool implInitRebuild(luaav_rect &frame);
	void implChooseFrame(bool useorigin, int x, int y, luaav_rect &frame);
	void implCreate(luaav_rect &frame);

	// memory management
	void implNull();
	void implRelease();

	// view management
	void implMakeCurrent();
	void implLock();
	void implUnlock();
	void implFlush();
	void implEndDraw();

	// window positioning
	void implShow();
	void implHide();
	void implRaise();
	void implLower();

	// window attributes
	void implSetTitle(const char *v);
	void implSetFrame(luaav_rect frame);
	luaav_rect implGetFrame();
	luaav_pt implGetSize();
	void implSetFullscreen(bool enable);
	void implSetBorder(bool enable);
	void implSetGrow(bool enable);
	void implSetFloating(bool enable);
	void implSetMouseMove(bool enable);
	void implSetSync(bool enable);
	void implSetCursorStyle(Window::CursorStyle style);
	void implSetCursor(bool enable);

	// app-wide window functions
	static void implCloseFocusedWindow();
	static int implActiveWindowCount();

// make public for fast access in static lua functions
public:

	// pointer to the binding-specific implementation
	Implementation *mImpl;
	lua_State *mState;


	WindowSystem *ws;
	std::string mTitle;
	unsigned int mFrames;
	float mOrigin[2];
	float mSize[2];
	float mLastSize[2];
	float mCtxSize[2];
	bool mAutoClear;
	float mClearColor[4];
	float mFps;
	float mFramePeriod;
	bool mFirstFrame;
	bool mFullscreen;
	bool mFloating;
	bool mStereo;
	bool mBorder;
	bool mGrow;
	bool mCursor;
	bool mSync;
	bool mMouseMove;
	bool mFinalizing;
	bool mInitializing;
	bool mRebuilding;
	std::string mChangedFlag;
	bool mShift;
	bool mCtrl;
	bool mAlt;
	bool mCmd;
	CursorStyle mCursorStyle;
	bool mActive;
	bool mScheduled;
};


}	// luaav::

#endif	/* include guard */