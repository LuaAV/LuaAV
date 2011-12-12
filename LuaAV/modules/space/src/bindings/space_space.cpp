#include "space_nav.h"
#include "Space.h"
#include "lua_utility.h"
#include "lua_glue.h"

using space::Space;

/*! Partition a space containing spheres
	Space partitions a cubic volume into a regular grid in such a way that fast collision 
	detection between spherical objects (such as Navs) is possible.
	
	@module space.Space
	@see space.Nav
*/

/*! Create a point in the space
	@param pos The position of the point
	@ret A Nav at located at pos
	@see space.Nav
	@name M:point
*/
int lua_space_point(lua_State *L) {
	Space *s = Glue<Space>::checkto(L, 1);
	float pos[] = {0., 0., 0.};
	lua::to_vec<float>(L, 2,  3, pos);
	Nav *nav = new Nav(s, pos[0], pos[1], pos[2]);
	Glue<Nav>::push(L, nav);
	return 1;
}

int lua_space_minp(lua_State *L) {
	Space *s = Glue<Space>::checkto(L, 1);
	space::Vec3 c = s->mincorner();
	lua::push_vec_t<float>(L, 3, &(c.x));
	return 1;
}

int lua_space_maxp(lua_State *L) {
	Space *s = Glue<Space>::checkto(L, 1);
	space::Vec3 c = s->maxcorner();
	lua::push_vec_t<float>(L, 3, &(c.x));
	return 1;
}

int lua_space_ncells(lua_State *L) {
	Space *s = Glue<Space>::checkto(L, 1);
	lua::push<float>(L, s->ncells());
	return 1;
}

int lua_space_within(lua_State *L) {
	Space *s = Glue<Space>::checkto(L, 1);
	space::Vec3 pos;
	if(lua::to_vec_t<float>(L, 2, 3, &(pos.x))) {
		float r = lua::opt<float>(L, 3, 0.1);
		Nav *ignore = Glue<Nav>::to(L, 4);
		
		neighand::NearestNeighbor<Nav> nn[16];
		int nfound = s->within(pos.x, pos.y, pos.z, nn, r, 16);
		lua_newtable(L);
		int j=0;
		for(int i=0; i < nfound; i++) {
			if(nn[i].object != ignore) {
				if(Glue<Nav>::usr_get_reference_map(L, nn[i].object)) {
					lua_rawseti(L, -2, j+1);
				}
				j++;
			}
		}
		return 1;
	}
	return 0;
}


template<> const char * Glue<Space>::usr_name() { return "Space"; }
template<> bool Glue<Space>::usr_has_index() { return true; }

template<> void Glue<Space>::usr_push(lua_State * L, Space * u) {
	Glue<Space>::usr_attr_push(L);
	Glue<Space>::usr_attr_prototype(L);
}

template<> Space * Glue<Space>::usr_new(lua_State * L) {
	float minv = -lua::opt<float>(L, 1, 1);
	Space *space = new Space(minv, minv, minv, -2.*minv/16.);
	return space;
}

template<> void Glue<Space>::usr_index(lua_State * L, Space * u) {
	Glue<Space>::usr_attr_index(L);
}

template<> void Glue<Space>::usr_newindex(lua_State * L, Space * u) {
	Glue<Space>::usr_attr_newindex(L);
}

template<> int Glue<Space>::usr_tostring(lua_State * L, Space * u) {
	lua_pushfstring(L, "%s: %p", Glue<Space>::usr_name(), u); 
	return 1;
}

template<> void Glue<Space>::usr_gc(lua_State * L, Space * u) { 
	delete u;
}

template<> void Glue<Space>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "point", lua_space_point },
		{ "within", lua_space_within },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "minp", lua_space_minp },
		{ "maxp", lua_space_maxp },
		{ "ncells", lua_space_ncells },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<Space>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_space_Nav(lua_State *L);;

int luaopen_space_Space(lua_State *L) {
	Glue<Space>::define(L);
	Glue<Space>::register_class(L);
	luaL_getmetatable(L, Glue<Space>::mt_name(L));
	
	lua::call<const char *>(L, luaopen_space_Nav, "space.Nav");
	lua_pop(L, 1);

	return 1;
}

#ifdef __cplusplus
}
#endif