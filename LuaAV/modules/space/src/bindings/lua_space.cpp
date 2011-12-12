#include "lua_space.h"
#include "lua_utility.h"

/*! Spatial computation module
	Space contains a range of spatial computation functions dealing with basic 
	vector and rotation math as well as spatial partitioning and collision 
	functionality.
	
	@module space
*/



#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_space_Nav(lua_State *L);
extern int luaopen_space_Space(lua_State *L);
extern int luaopen_space_quat(lua_State *L);
extern int luaopen_space_vec3(lua_State *L);

//extern int luaopen_space_BSpace(lua_State *L);

int luaopen_space(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg space_lib[] = {
		{NULL, NULL}
	};
	luaL_register(L, libname, space_lib);
	
	lua::preloadlib(L, "space.Nav", luaopen_space_Nav);
	lua::preloadlib(L, "space.Space", luaopen_space_Space);
	lua::preloadlib(L, "space.quat", luaopen_space_quat);
	lua::preloadlib(L, "space.vec3", luaopen_space_vec3);
	
	//lua::preloadlib(L, "space.BSpace", luaopen_space_BSpace);

	return 1;
}

#ifdef __cplusplus
}
#endif
