#ifndef LUA_ARRAY_H
#define LUA_ARRAY_H 1

#include "lua_glue.h"
#include "lua_utility.h"
#include "allocore/types/al_Array.hpp"


#define ARRAY_NAME		"Array"
#define ARRAY_META_NAME	"LuaAV.Array"
#define ARRAY_MAGIC_FIELD	"__magic"
#define ARRAY_MAGIC_VALUE	(0xA110)

///! just an alias to make it easier to read
typedef al::ArrayWrapper * ArrayHandle;

template<>
inline const char * Glue<al::ArrayWrapper>::mt_name(lua_State * L) {
	return ARRAY_META_NAME;
}

template<> 
inline const char * Glue<al::ArrayWrapper>::usr_name() {
	return ARRAY_NAME;
}

template<> 
inline ArrayHandle Glue<al::ArrayWrapper> :: usr_to(
	lua_State * L, 
	al::ArrayWrapper * u, 
	int idx
) { 
	lua_getmetatable(L, idx);
	lua_getfield(L, -1, "__magic");
	int magic = (int)lua_tonumber(L, -1);
	lua_pop(L, 2);
	return (magic == ARRAY_MAGIC_VALUE) ? u : NULL;
}

template<> 
inline void Glue<al::ArrayWrapper> :: usr_push(lua_State * L, ArrayHandle u) {
	// create per-userdata env table:
	Glue<al::ArrayWrapper>::usr_attr_push(L);	
	u->retain(); 
}

template<> 
inline void Glue<al::ArrayWrapper> :: usr_gc(lua_State * L, ArrayHandle u) { 
	u->release(); 
}

template<> 
inline bool Glue<al::ArrayWrapper>::usr_has_index() { 
	return true;
}

template<> 
inline void Glue<al::ArrayWrapper>::usr_index(lua_State * L, ArrayHandle u) {
	Glue<al::ArrayWrapper>::usr_attr_index(L);
}

template<> 
inline void Glue<al::ArrayWrapper>::usr_newindex(lua_State * L, ArrayHandle u) {
	Glue<al::ArrayWrapper>::usr_attr_newindex(L);
}


template<> 
inline ArrayHandle Glue<al::ArrayWrapper>::usr_new(lua_State * L) {
	ArrayHandle u = new al::ArrayWrapper;
	AlloArray *lat = u;

	// defaults
	int align = 4;
	AlloArrayHeader h;
	h.type = AlloFloat32Ty;
	h.components = 1;
	h.dimcount = 2;
	h.dim[0] = 1;
	h.dim[1] = 1;
	
	if(lua_istable(L, 1)) {
		h.components = lua::optfield(L, 1, 1, "components");
		h.type = lua::optfield<AlloTy>(L, 1, AlloFloat32Ty, "type");
		align = lua::optfield(L, 1, 4, "align");
		
		// check dim:
		lua_getfield(L, 1, "dim");
		if(lua_istable(L, -1)) {
			h.dimcount = MIN(lua_objlen(L, -1), ALLO_ARRAY_MAX_DIMS);
			for(int i=0; i< h.dimcount; i++) {
				lua_rawgeti(L, -1, i+1);
				h.dim[i] = luaL_optint(L, -1, 1);
				lua_pop(L, 1);
			}
		}
		else if(lua_isnumber(L, -1)) {
			h.dimcount = 1;
			h.dim[0] = luaL_optint(L, -1, 1);
		}
		lua_pop(L, 1); // dim		
	}
	else if(lua::is<uint8_t>(L, 1)) {
		h.components = lua::to<uint8_t>(L, 1);
		h.type = lua::opt<AlloTy>(L, 2, AlloFloat32Ty);
		
		uint32_t dim[] = {1, 1, 1, 1};
		int dimcount = ALLO_ARRAY_MAX_DIMS;
		lua::to_vec_tn<uint32_t>(L, 3, dimcount, dim);
		h.dimcount = (uint8_t)dimcount;
		for(int i=0; i < dimcount; i++) {
			h.dim[i] = dim[i];
		}
		
		align = lua::opt<int>(L, 4, 4);
	}
	
	allo_array_setstride(&h, align);
	allo_array_create(lat, &h);
	memset(lat->data.ptr, '\0', allo_array_size(lat));
	//allo_array_wrapper_retain(u);
	u->retain();
	
	return u;
}

inline ArrayHandle lua_array_to(lua_State *L, int idx) {
	ArrayHandle w = Glue<al::ArrayWrapper>::to(L, idx);
	return w; 
}

inline ArrayHandle lua_array_checkto(lua_State *L, int idx) {
	ArrayHandle w = Glue<al::ArrayWrapper>::checkto(L, idx);
	return w;
}

/// conveniently conform to the lua:: utility specializations:
namespace lua {
	template<> 
	inline void to(lua_State * L, int idx, al::ArrayWrapper * v) { 
		v = Glue<al::ArrayWrapper>::checkto(L, idx); 
	}
	
	template<> 
	inline ArrayHandle to(lua_State * L, int idx) { 
		return Glue<al::ArrayWrapper>::checkto(L, idx); 
	}
	
	template<> 
	inline bool is<al::ArrayWrapper>(lua_State * L, int idx) { 
		return NULL != Glue<al::ArrayWrapper>::to(L, idx); 
	}
	
	template<> 
	inline const al::ArrayWrapper * opt(lua_State * L, int idx, al::ArrayWrapper * v) {
		al::ArrayWrapper * r = Glue<al::ArrayWrapper>::to(L, idx);
		return r ? r : v;
	}
	
	template<> 
	inline al::ArrayWrapper * opt(lua_State * L, int idx, al::ArrayWrapper * v) {
		al::ArrayWrapper * r = Glue<al::ArrayWrapper>::to(L, idx);
		return r ? r : v;
	}

} // lua::

#endif // LUA_ARRAY_H
