#ifndef LUA_COMPILER_H
#define LUA_COMPILER_H 1

#include "lua_glue.h"
#include "lua_utility.h"
#include "system/al_Compiler.hpp"

using al::Compiler;
using al::JIT;

template<> inline const char * Glue<Compiler>::usr_name() { return "Compiler"; }
//template<> inline bool Glue<Compiler>::usr_has_index() { return true; }

template<> inline void Glue<Compiler>::usr_push(lua_State * L, Compiler * u) {
	//Glue<Compiler>::usr_attr_push(L);
}

template<> inline Compiler * Glue<Compiler>::usr_new(lua_State * L) {
	Compiler *c = new Compiler();
	return c;
}

template<> inline void Glue<Compiler>::usr_index(lua_State * L, Compiler * u) {
	//Glue<Compiler>::usr_attr_index(L);
}

template<> inline void Glue<Compiler>::usr_newindex(lua_State * L, Compiler * u) {
	//Glue<Compiler>::usr_attr_newindex(L);
}

template<> inline int Glue<Compiler>::usr_tostring(lua_State * L, Compiler * u) {
	lua_pushfstring(L, "%s: %p", Glue<Compiler>::usr_name(), u); 
	return 1;
}

template<> inline void Glue<Compiler>::usr_gc(lua_State * L, Compiler * u) { 
	delete u;
}


template<> inline const char * Glue<JIT>::usr_name() { return "JIT"; }
//template<> inline bool Glue<JIT>::usr_has_index() { return true; }

template<> inline void Glue<JIT>::usr_push(lua_State * L, JIT * u) {
	//u->retain();
	//printf("push JIT %p %d\n", u, u->refs());
	//Glue<JIT>::usr_attr_push(L);
	//Glue<JIT>::usr_set_reference_map(L, u);
}

template<> inline void Glue<JIT>::usr_index(lua_State * L, JIT * u) {
	//Glue<JIT>::usr_attr_index(L);
}

template<> inline void Glue<JIT>::usr_newindex(lua_State * L, JIT * u) {
	//Glue<JIT>::usr_attr_newindex(L);
}

template<> inline int Glue<JIT>::usr_tostring(lua_State * L, JIT * u) {
	lua_pushfstring(L, "%s: %p", Glue<JIT>::usr_name(), u); 
	return 1;
}

template<> inline void Glue<JIT>::usr_gc(lua_State * L, JIT * u) { 
	//printf("gc JIT %p %d\n", u, u->refs());
	u->release();
	JIT::sweep();
}

#endif // include guard
