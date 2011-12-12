#error "luaav_doc.h is a dummy file for documentation"

/*! LuaAV library manipulation functions
	
	
	@module LuaAV
*/
int luaopen_luaav(lua_State * L);

/*! Return the running version of LuaAV
	@ret subversion revision number (as string)
*/
int version(lua_State *L);

// platform
// string representing executing platform (OSX, LINUX, etc.)

///*! Create a new LuaAVState
//	
//	@param [string] 
//	@ret LuaAVState
//*/
//int new(lua_State *L);

// LuaAVState:close()
// LuaAVState:dofile()
// LuaAVState:dostring()
// LuaAVState:gostring()

// LuaAV.debug:
//		{"sleep", luaav_debug_sleep },
//		{"elapsed", luaav_debug_elapsed },
//		{"mainevents", luaav_debug_mainevents },
//		{"scheduler", luaav_debug_scheduler },