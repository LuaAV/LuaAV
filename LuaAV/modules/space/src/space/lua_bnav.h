#ifndef LUA_BSPACE_NAV_H
#define LUA_BSPACE_NAV_H 1

#include "lua_glue.h"
#include "bnav.h"

using space::BNav;

template<>
inline void Glue<BNav>::usr_push(lua_State * L, BNav * u) {
	Glue<BNav>::usr_attr_push(L);
	Glue<BNav>::usr_set_reference_map(L, u);
}

template<> 
inline const char * Glue<BNav>::usr_name() { return "BNav"; }

template<> 
inline bool Glue<BNav>::usr_has_index() { return true; }

template<> 
inline void Glue<BNav>::usr_index(lua_State * L, BNav * u) {
	Glue<BNav>::usr_attr_index(L);
}

template<> 
inline void Glue<BNav>::usr_newindex(lua_State * L, BNav * u) {
	Glue<BNav>::usr_attr_newindex(L);
}

template<> 
inline int Glue<BNav>::usr_tostring(lua_State * L, BNav * u) {
	lua_pushfstring(L, "%s: %p", Glue<BNav>::usr_name(), u); 
	return 1;
}

template<> 
inline void Glue<BNav>::usr_gc(lua_State * L, BNav * u) { 
	delete u;
}

#endif // LUA_BSPACE_NAV_H