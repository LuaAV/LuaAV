#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "lua_utility.h"
#include "lua_glue.h"
#include <RandomLib/Random.hpp>


int lua_random_Float32(lua_State *L) {
	RandomLib::Random *s = Glue<RandomLib::Random>::checkto(L, 1);
	lua::push<double>(L, s->Fixed());
	return 1;
}

int lua_random_reseed(lua_State *L) {
	RandomLib::Random *s = Glue<RandomLib::Random>::checkto(L, 1);
	s->Reseed();
	return 0;
}

int lua_random_reset(lua_State *L) {
	RandomLib::Random *s = Glue<RandomLib::Random>::checkto(L, 1);
	s->Reset();
	return 0;
}

int lua_random_seed(lua_State *L) {
	RandomLib::Random *s = Glue<RandomLib::Random>::checkto(L, 1);
	if(lua::is<RandomLib::RandomSeed::seed_type>(L, 2)) {
		RandomLib::RandomSeed::seed_type seed = lua::to<RandomLib::RandomSeed::seed_type>(L, 2);
		s->Reseed(seed);
	}
	else {
		luaL_error(L, "Random.seed: invalid arguments");
	}
	return 0;
}



template<> const char * Glue<RandomLib::Random>::usr_name() { return "Random"; }
template<> bool Glue<RandomLib::Random>::usr_has_index() { return true; }

template<> void Glue<RandomLib::Random>::usr_push(lua_State * L, RandomLib::Random * u) {
	Glue<RandomLib::Random>::usr_attr_push(L);
	Glue<RandomLib::Random>::usr_attr_prototype(L);
}

template<> RandomLib::Random * Glue<RandomLib::Random>::usr_new(lua_State * L) {
	RandomLib::Random *r = new RandomLib::Random();
	r->Reseed(); 
	return r;
}

template<> void Glue<RandomLib::Random>::usr_index(lua_State * L, RandomLib::Random * u) {
	Glue<RandomLib::Random>::usr_attr_index(L);
}

template<> void Glue<RandomLib::Random>::usr_newindex(lua_State * L, RandomLib::Random * u) {
	Glue<RandomLib::Random>::usr_attr_newindex(L);
}

template<> int Glue<RandomLib::Random>::usr_tostring(lua_State * L, RandomLib::Random * u) {
	lua_pushfstring(L, "%s: %p", Glue<RandomLib::Random>::usr_name(), u); 
	return 1;
}

template<> void Glue<RandomLib::Random>::usr_gc(lua_State * L, RandomLib::Random * u) { 
	delete u;
}

template<> void Glue<RandomLib::Random>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "Float32", lua_random_Float32 },
		{ "reseed", lua_random_reseed },
		{ "reset", lua_random_reset },
		{ "seed", lua_random_seed },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<RandomLib::Random>::usr_attr_mt(L, methods, getters, setters);
}
#ifdef __cplusplus
extern "C" {
#endif


int luaopen_random_Random(lua_State *L) {
	Glue<RandomLib::Random>::define(L);
	Glue<RandomLib::Random>::register_class(L);
	luaL_getmetatable(L, Glue<RandomLib::Random>::mt_name(L));

	return 1;
}


int luaopen_random(lua_State *L) {
//	const char * libname = lua_tostring(L, 1);
//	luaL_register(L, libname, color_lib);
	
	return 1;
}

#ifdef __cplusplus
}
#endif