#ifndef LUA_OPENCL_UTIL_H
#define LUA_OPENCL_UTIL_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "al_OpenCL.hpp"
#include "lua_utility.h"

using al::cl::OpenCLEvent;

namespace lua {


// lua::is
template<> inline bool is<cl_platform_id>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<cl_device_id>(lua_State * L, int idx) { return lua_isnumber(L, idx); }


// lua::to
template<> inline cl_platform_id to(lua_State * L, int idx) { return (cl_platform_id)lua_tointeger(L, idx); }
template<> inline cl_device_id to(lua_State * L, int idx) { return (cl_device_id)lua_tointeger(L, idx); }


// lua::push
template<> inline void push<cl_platform_id>(lua_State * L, cl_platform_id v) {
	lua_pushinteger(L, (lua_Integer)v);
}
template<> inline void push<cl_device_id>(lua_State * L, cl_device_id v) {
	lua_pushinteger(L, (lua_Integer)v);
}
template<> inline void push<OpenCLEvent>(lua_State * L, OpenCLEvent v) {
	lua_pushinteger(L, (lua_Integer)v.get_event());
}


} // lua::


#endif // LUA_OPENCL_UTIL_H
