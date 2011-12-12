#include "luaav_app.h"
#include "luaav_app_audiodriver.h"
#include "luaav_app_menu.h"
#include "luaav_app_screens.h"
#include "luaav_app_window.h"
#include "luaav_filewatcher.h"
#include "luaav_app_codepad.h"
#include "luaav_app_stateview.h"

#include "allocore/system/al_MainLoop.h"
#include <cstring>

extern "C" int luaopen_clang(lua_State *L);

extern int luaopen_app(lua_State * L);
extern int luaopen_app_codepad(lua_State * L);
extern const char * luaav_app_path();
extern const char * luaav_app_resource_path();

// platform-specfic paths
extern void luaav_app_paths_init(
	std::string &app_path,
	std::string &app_resource_path,
	std::string &app_home_path,
	std::string &app_config_path,
	std::string &app_log_path
);


// platform-specific app functions	


/*! Modules and functionality for building applications
	The app module abstracts out all OS-specific functionality in as consistent 
	a manner as possible.  app provides modules for dealing with windowing, file 
	system events, menus, etc.
	
	@module app
*/


/*! Call quit on the application	
	@name quit
*/
extern int quit(lua_State * L);

/*! Open an open file panel
	
	@param dir The directory to start from
	@ret The file or files selected (string or table) if any
	@name openpanel
*/
extern int app_openpanel(lua_State *L);

/*! Open an save file panel
	
	@param dir The directory to start from
	@ret The file saved if any
	@name savepanel
*/
extern int app_savepanel(lua_State *L);

/*! Open a URL in the default web browser
	
	@param url The URL to open
	@name openurl
*/
extern int app_openurl(lua_State *L);
#ifdef AL_LINUX
	extern int app_fork(lua_State *L);
#endif

#ifdef AL_OSX
	extern int app_add_module(lua_State *L);
	extern int app_clear_modules(lua_State *L);
#endif

/*! Copy and paste data from the clipboard
	If a data argument is given, clipboard will 
	paste it to the clipboard, otherwise, it will 
	copy whatever data is on the clipboard and return 
	it.
	
	@param [data] Optional data to past
	@ret The data on the clipboard
	@name clipboard
*/
extern int app_clipboard(lua_State *L);
extern int app_window_event(lua_State *L);

	
extern int luaopen_app_console(lua_State * L);

static LuaAVState * app_state_main = 0;
static const char * app_path_separator = "/";
static std::string app_path;
static std::string app_resource_path;
static std::string app_home_path;
static std::string app_config_path;
static std::string app_log_path;


/* main thread entry point */
void tick(al_nsec ns, void * u) {
//	LuaAVState * S = (LuaAVState *)u;
//	al_sec t = al_time_ns2s * ns;
//	printf("@%06.3f\n", t);
	luaav_main_tick();
}

void quit(void * u) {	
	
}

void luaav_app_init(const char *path) {
	// possibly overridden in app_paths_init
	if(path) {
		const char *last_slash = strrchr(path, '/');
		if(last_slash) {	
			char folder[512];
			strncpy(folder, path, last_slash-path);
			folder[last_slash-path] = '\0';
			app_path.assign(folder);
		}
	}

	// initialize LuaAV:
	luaav_init(NULL, NULL);
	luaav_app_paths_init(
		app_path,
		app_resource_path,
		app_home_path,
		app_config_path,
		app_log_path
	);
	luaav_app_menus_init();
	luaav_app_screens_init();
	luaav_add_main_module("app.file", luaopen_app_file );	
	
	luaav_add_opt_module("app.Window", luaopen_window);
	luaav_add_opt_module("app", luaopen_app);
	luaav_add_opt_module("app.console", luaopen_app_console);
#ifndef AL_OSX
	luaav_add_opt_module("app.CodePad", luaav::CodePad::register_udata);
	luaav_add_opt_module("app.StateView", luaav::StateView::register_udata);
#endif
	luaav_add_opt_module("clang", luaopen_clang);

	luaav_app_audio_init();
	
	app_state_main = luaav_state_create("main", NULL);	
	luaav_state_dofile(app_state_main, "LuaAV.lua", luaav_app_resource_path());
	
	al_main_attach(0.01, tick, app_state_main, quit);	
}

void luaav_app_quit() {
	printf("terminated\n");
	
	al_main_exit();
	luaav_app_audio_quit();	
	
	luaav_state_close(&app_state_main);	
	
	luaav_quit();
}



luaav_state * luaav_app_state() {
	return app_state_main;
}

const char * luaav_app_path() {
	return app_path.data();
}

const char * luaav_app_resource_path() {
	return app_resource_path.data();
}

int luaopen_app(lua_State * L) {
	const char * libname = lua::to<const char *>(L, 1);
	struct luaL_reg lib[] = {
		{"openpanel", app_openpanel },
		{"openurl", app_openurl },
#ifdef AL_LINUX
		{"fork", app_fork },
#endif
#ifdef AL_OSX
		{"add_module", app_add_module },
		{"clear_modules", app_clear_modules },
#endif
		{"savepanel", app_savepanel },
		{"clipboard", app_clipboard },
		{"quit", quit},
		{"window_event", app_window_event },
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);
	
	lua_pushstring(L, app_path.data());				lua_setfield(L, -2, "apppath");
	lua_pushstring(L, app_resource_path.data());	lua_setfield(L, -2, "resourcepath");
	lua_pushstring(L, app_home_path.data());		lua_setfield(L, -2, "homepath");
	lua_pushstring(L, app_config_path.data());		lua_setfield(L, -2, "configpath");
	lua_pushstring(L, app_log_path.data());			lua_setfield(L, -2, "logpath");
	lua_pushstring(L, app_path_separator);			lua_setfield(L, -2, "separator");
	lua_pushstring(L, LUA_AV_OS_NAME);				lua_setfield(L, -2, "platform");

	return 1;
}
