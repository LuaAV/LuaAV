#include "HSpace.h"
#include "lua_utility.h"
#include "lua_glue.h"
#include "lua_array.h"

#ifndef MIN
#define MIN(a, b) ((a < b) ? (a) : (b))
#endif

int lua_HSpace_cells(lua_State *L) {
	HSpace *s = Glue<HSpace>::checkto(L, 1);
	float pos[3];
	if(
		lua::to_vec_t<float>(L, 2, 3, pos) && 
		lua::is<float>(L, 3)
	) {
		float d = lua::to<float>(L, 3);
		int n = MIN((int)(d*d), s->mMaxd);
		int cpos = s->unpackPos(pos[0], pos[1], pos[2]);
		
		lua_newtable(L);
		int nres = 0;
		for(int i=0; i < s->mD[n]; i++) {
			int rpos = s->mG[i];
			int apos = rpos+cpos;
			int npos[3];
			s->coords(apos, npos[0], npos[1], npos[2]);
			if(
				npos[0] >= 0 && npos[1] >= 0 && npos[2] >= 0 && 
				npos[0] < s->mSize && npos[1] < s->mSize && npos[2] < s->mSize
			) {
				lua::push_vec_t<int>(L, 3, npos);
				lua_rawseti(L, -2, nres+1);
				nres++;
			}
		}
	}
	else {
		luaL_error(L, "HSpace.cells: invalid arguments");
	}
	return 1;
}

int lua_HSpace_push_HObject(lua_State *L, HObject *o) {
	lua_newtable(L);
		lua_pushnumber(L, o->x);
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, o->y);
		lua_rawseti(L, -2, 2);
		lua_pushnumber(L, o->z);
		lua_rawseti(L, -2, 3);
	return 1;
}

int lua_HSpace_cell(lua_State *L) {
	HSpace *s = Glue<HSpace>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		int cidx = lua::to<int>(L, 2);
		HCell &cell = s->mCells[cidx];
		lua_newtable(L);
			int cx, cy, cz;
			s->cellCoords(cidx, cx, cy, cz);
			
			lua_newtable(L);
				lua_pushnumber(L, cx);
				lua_rawseti(L, -2, 1);
				lua_pushnumber(L, cy);
				lua_rawseti(L, -2, 2);
				lua_pushnumber(L, cz);
				lua_rawseti(L, -2, 3);
			lua_setfield(L, -2, "pos");
		
		lua_newtable(L);
		HObject *o = cell.objects;
		if(o) {
			int t = 1;
			HObject *so = o;
			do {
				lua_HSpace_push_HObject(L, o);
				lua_rawseti(L, -2, t);
				t++;
				o = o->next;
			} while(o != so);
		}
		lua_setfield(L, -2, "objects");
	}
	else {
		luaL_error(L, "HSpace.cell: invalid arguments");
	}
	return 1;
}

int lua_HSpace_query(lua_State *L) {
	HSpace *s = Glue<HSpace>::checkto(L, 1);
	float pos[3] = {0., 0., 0.};
	if(
		lua::to_vec_t(L, 2, 3, pos) && 
		lua::is<float>(L, 3)
	) {
		float radius = lua::to<float>(L, 3);
		HObject *objects[16];
		int nres = s->query(pos[0], pos[1], pos[2], radius, 16, objects);
		lua_newtable(L);
		for(int i=0; i < nres; i++) {
			lua_HSpace_push_HObject(L, objects[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "HSpace.query: invalid arguments");
	}
	return 1;
}

int lua_HSpace_query_range(lua_State *L) {
	HSpace *s = Glue<HSpace>::checkto(L, 1);
	float pos[3] = {0., 0., 0.};
	if(
		lua::to_vec_t(L, 2, 3, pos) && 
		lua::is<float>(L, 3) && 
		lua::is<float>(L, 4)
	) {
		float r1 = lua::to<float>(L, 3);
		float r2 = lua::to<float>(L, 4);
		HObject *objects[16];
		int nres = s->queryRange(pos[0], pos[1], pos[2], r1, r2, 16, objects);
		lua_newtable(L);
		for(int i=0; i < nres; i++) {
			lua_HSpace_push_HObject(L, objects[i]);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		luaL_error(L, "HSpace.query_range: invalid arguments");
	}
	return 1;
}

int lua_HSpace_hash(lua_State *L) {
	HSpace *s = Glue<HSpace>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	s->hash(lat);
	return 0;
}


template<> const char * Glue<HSpace>::usr_name() { return "HSpace"; }
template<> bool Glue<HSpace>::usr_has_index() { return true; }

template<> void Glue<HSpace>::usr_push(lua_State * L, HSpace * u) {
	Glue<HSpace>::usr_attr_push(L);
	Glue<HSpace>::usr_attr_prototype(L);
}

template<> HSpace * Glue<HSpace>::usr_new(lua_State * L) {
	int size = lua::opt<int>(L, 1, 5);
	HSpace *space = new HSpace(size);
	return space;
}

template<> void Glue<HSpace>::usr_index(lua_State * L, HSpace * u) {
	Glue<HSpace>::usr_attr_index(L);
}

template<> void Glue<HSpace>::usr_newindex(lua_State * L, HSpace * u) {
	Glue<HSpace>::usr_attr_newindex(L);
}

template<> int Glue<HSpace>::usr_tostring(lua_State * L, HSpace * u) {
	lua_pushfstring(L, "%s: %p", Glue<HSpace>::usr_name(), u); 
	return 1;
}

template<> void Glue<HSpace>::usr_gc(lua_State * L, HSpace * u) { 
	delete u;
}

template<> void Glue<HSpace>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "cells", lua_HSpace_cells },
		{ "cell", lua_HSpace_cell },
		{ "query", lua_HSpace_query },
		{ "query_range", lua_HSpace_query_range },
		{ "hash", lua_HSpace_hash },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<HSpace>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_space_HSpace(lua_State *L) {
	Glue<HSpace>::define(L);
	Glue<HSpace>::register_class(L);
	luaL_getmetatable(L, Glue<HSpace>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif

