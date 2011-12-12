#include "luaav.h"
#include "lua_utility.h"
#include "luaav_app.h"
#include "luaav_filewatcher.h"
#include "lua_glue.h"

using std::string;
using luaav::FileWatcher;



#define FW_KEY "FW_KEY"


template<> const char * Glue<FileWatcher>::usr_name() { return "FileWatcher"; }
template<> void Glue<FileWatcher>::usr_gc(lua_State * L, FileWatcher * u) { 
	luaav::FileWatcher::unwatchfile(u->filename.c_str());
	delete u;
}


int luaav_file_store_fw(lua_State *L, const char *filename, FileWatcher *fw) {
	string key(filename);
	key.append(FW_KEY);
	
	Glue<FileWatcher>::push(L, fw);
	lua_setfield(L, LUA_REGISTRYINDEX, key.c_str());
	return 0;
}

FileWatcher * luaav_file_get_fw(lua_State *L, const char *filename) {
	string key(filename);
	key.append(FW_KEY);
	
	lua_getfield(L, LUA_REGISTRYINDEX, key.c_str());
	
	FileWatcher *fw = 0;
	if(lua_islightuserdata(L, -1)) {
		fw = (FileWatcher *)Glue<FileWatcher>::checkto(L, -1);
	}
	lua_pop(L, 1);
	
	return fw;
}

int luaav_file_remove_fw(lua_State *L, const char *filename) {
	string key(filename);
	key.append(FW_KEY);
	
	lua_getfield(L, LUA_REGISTRYINDEX, key.c_str());
	if(lua_islightuserdata(L, -1)) {
		FileWatcher *fw = (FileWatcher *)lua_touserdata(L, -1);
		delete fw;
		
		lua_pushnil(L);
		lua_setfield(L, LUA_REGISTRYINDEX, key.c_str());
	}
	lua_pop(L, 1);
	return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/*! Filewatching functions
	Filewatching allows one to register for callbacks when a file is 
	modified.  These are OS-generated events.
	
	@module app.file
*/

static void luaav_file_reload_cb(const char *filename, void *ud) {
	FileWatcher *fw = (FileWatcher *)ud;
	lua_State *L = fw ? fw->L : luaav_getstate(luaav_app_state());
	lua_getglobal(L, "fileopen");
	if (lua_isfunction(L, -1)) {
		lua::push<const char *>(L, filename);
		lua_pcall(L, 1, 0, 0);
	}
	else {
		lua_pop(L, 1);
	}
}


/*! Watch a file

	@param filename The name of the file to watch
	@name watch
*/
static int luaav_file_watch(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	FileWatcher *fw = new FileWatcher(L, filename);
	luaav_file_store_fw(L, filename, fw);
	luaav::FileWatcher::watchfile(filename, luaav_file_reload_cb, fw);
	return 0;
}

/*! Check if a file is being watched

	@param filename The name of the file to check
	@name watching
*/
static int luaav_file_watching(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	bool watching = luaav::FileWatcher::watchingfile(filename);
	lua::push<bool>(L, watching);
	return 1;
}

/*! Unwatch a file

	@param filename The name of the file unwatch
	@name unwatch
*/
static int luaav_file_unwatch(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	luaav::FileWatcher::unwatchfile(filename);
	luaav_file_remove_fw(L, filename);
	return 0;
}

int luaopen_app_file(lua_State *L) {
	const char * libname = lua_tostring(L, -1);
	
	lua_newtable(L);
	struct luaL_reg methods[] = {
		{ "watch", luaav_file_watch },
		{ "watching", luaav_file_watching },
		{ "unwatch", luaav_file_unwatch },
		{ NULL, NULL },
	};
	luaL_register(L, libname, methods);
	
	Glue<FileWatcher>::define(L);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
