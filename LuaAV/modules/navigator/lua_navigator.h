#ifndef LUA_3DCONNEXION_MOD_H
#define LUA_3DCONNEXION_MOD_H

#ifdef __cplusplus
extern "C" {
#endif

	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"

	extern int luaopen_navigator(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif