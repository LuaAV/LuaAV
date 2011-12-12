#ifndef LUA_SPACE_NAV_H 
#define LUA_SPACE_NAV_H 1

#include "lua_glue.h"
#include "Nav.h"

using space::Nav;

template<>
inline void Glue<Nav>::usr_push(lua_State * L, Nav * u) {
	Glue<Nav>::usr_attr_push(L);
	Glue<Nav>::usr_set_reference_map(L, u);
}

template<> 
inline const char * Glue<Nav>::usr_name() { return "Nav"; }

template<> 
inline bool Glue<Nav>::usr_has_index() { return true; }

template<> 
inline void Glue<Nav>::usr_index(lua_State * L, Nav * u) {
	Glue<Nav>::usr_attr_index(L);
}

template<> 
inline void Glue<Nav>::usr_newindex(lua_State * L, Nav * u) {
	Glue<Nav>::usr_attr_newindex(L);
}

template<> 
inline int Glue<Nav>::usr_tostring(lua_State * L, Nav * u) {
	lua_pushfstring(L, "%s: %p", Glue<Nav>::usr_name(), u); 
	return 1;
}

template<> 
inline void Glue<Nav>::usr_gc(lua_State * L, Nav * u) { 
	delete u;
}


#endif // LUA_SPACE_NAV_H