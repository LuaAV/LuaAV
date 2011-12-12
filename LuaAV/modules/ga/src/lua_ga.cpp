#include "lua_utility.h"
#include "lua_glue.h"




#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_ga_e2(lua_State *L);
extern int luaopen_ga_e3(lua_State *L);

extern int luaopen_ga_c2(lua_State *L);

int luaopen_ga(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg ga_lib[] = {
		{NULL, NULL}
	};
	luaL_register(L, libname, ga_lib);
	
	lua::preloadlib(L, "ga.e2", luaopen_ga_e2);
	lua::preloadlib(L, "ga.e3", luaopen_ga_e3);
	
	lua::preloadlib(L, "ga.c2", luaopen_ga_c2);
	
	return 1;
}

#ifdef __cplusplus
}
#endif

