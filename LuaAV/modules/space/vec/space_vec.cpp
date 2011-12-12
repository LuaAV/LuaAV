#include "lua_space.h"
#include "vec3_udata.h"
#include "quat_udata.h"


#ifdef __cplusplus
extern "C" {
#endif

#define LUA_METHOD(name) {#name,space::lua_quat_##name}

int luaopen_space_quat(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	luaL_Reg quat_lib[] = {
		LUA_METHOD(normalize),
		LUA_METHOD(inverse),
		LUA_METHOD(mul),
		LUA_METHOD(revmul),
		LUA_METHOD(from_axis_angle),
		LUA_METHOD(from_euler),
		LUA_METHOD(to_axis_angle),
		LUA_METHOD(to_euler),
		LUA_METHOD(to_vector_x),
		LUA_METHOD(to_vector_y),
		LUA_METHOD(to_vector_z),
		LUA_METHOD(rotate_vector),
		LUA_METHOD(slerp),
		LUA_METHOD(toward_point),
		LUA_METHOD(rotor),
		{NULL, NULL}
	};
	luaL_register(L, libname, quat_lib);
	
	return 1;
}

#undef LUA_METHOD
#define LUA_METHOD(name) {#name, space::lua_vec3_##name}

int luaopen_space_vec3(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	luaL_Reg vec3_lib[] = {
		LUA_METHOD(add),
		LUA_METHOD(sub),
		LUA_METHOD(mul),
		LUA_METHOD(div),
		LUA_METHOD(scale),
		LUA_METHOD(dot),
		LUA_METHOD(mag_sqr),
		LUA_METHOD(mag),
		LUA_METHOD(cross),
		LUA_METHOD(normalize),
		LUA_METHOD(lerp),
		LUA_METHOD(easein2),
		LUA_METHOD(easeout2),
		LUA_METHOD(easeinout2),
		LUA_METHOD(casteljau),
		LUA_METHOD(hermite),
		LUA_METHOD(hermite2),
		LUA_METHOD(normal),
		LUA_METHOD(centroid3),
		LUA_METHOD(tangent),
		LUA_METHOD(rotaxis),
		LUA_METHOD(angle),
		LUA_METHOD(slerp),
		LUA_METHOD(min),
		LUA_METHOD(max),
		LUA_METHOD(mod),
		LUA_METHOD(quantize),
		LUA_METHOD(axis_angle),
		{NULL, NULL}
	};
	luaL_register(L, libname, vec3_lib);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
