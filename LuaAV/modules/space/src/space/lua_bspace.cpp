#include "bspace.h"
#include "lua_utility.h"
#include "lua_glue.h"
#include "lua_bnav.h"

using space::BSpace;

int lua_bspace_sphere(lua_State *L) {
	BSpace *s = Glue<BSpace>::checkto(L, 1);

	btVector3 pos;
	if(lua::to_vec_t<btScalar>(L, 2, 3, pos.m_floats)) {
		btScalar radius = lua::opt<btScalar>(L, 3, 1.);
		btScalar mass = lua::opt<btScalar>(L, 4, 1.);
		BNav *nav = s->create_sphere(pos, radius, mass);
		Glue<BNav>::push(L, nav);
	}
	else {
		luaL_error(L, "BSpace.sphere: invalid arguments");
	}
	return 1;
}

int lua_bspace_step(lua_State *L) {
	BSpace *s = Glue<BSpace>::checkto(L, 1);
	btScalar dt = lua::opt<btScalar>(L, 2, 1./40.);
	s->get_world()->stepSimulation(dt);
	return 0;
}


template<> const char * Glue<BSpace>::usr_name() { return "BSpace"; }
template<> bool Glue<BSpace>::usr_has_index() { return true; }

template<> void Glue<BSpace>::usr_push(lua_State * L, BSpace * u) {
	Glue<BSpace>::usr_attr_push(L);
	Glue<BSpace>::usr_attr_prototype(L);
}

template<> BSpace * Glue<BSpace>::usr_new(lua_State * L) {
	return new BSpace();
}

template<> void Glue<BSpace>::usr_index(lua_State * L, BSpace * u) {
	Glue<BSpace>::usr_attr_index(L);
}

template<> void Glue<BSpace>::usr_newindex(lua_State * L, BSpace * u) {
	Glue<BSpace>::usr_attr_newindex(L);
}

template<> int Glue<BSpace>::usr_tostring(lua_State * L, BSpace * u) {
	lua_pushfstring(L, "%s: %p", Glue<BSpace>::usr_name(), u); 
	return 1;
}

template<> void Glue<BSpace>::usr_gc(lua_State * L, BSpace * u) { 
	delete u;
}

template<> void Glue<BSpace>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "sphere", lua_bspace_sphere },
		{ "step", lua_bspace_step },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<BSpace>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_space_BNav(lua_State *L);

int luaopen_space_BSpace(lua_State *L) {
	Glue<BSpace>::define(L);
	Glue<BSpace>::register_class(L);
	luaL_getmetatable(L, Glue<BSpace>::mt_name(L));
	
	lua::call<const char *>(L, luaopen_space_BNav, "space.BNav");
	lua_pop(L, 1);

	return 1;
}

#ifdef __cplusplus
}
#endif