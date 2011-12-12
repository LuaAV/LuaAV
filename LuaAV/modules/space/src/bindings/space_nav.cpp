#include "Nav.h"
#include "lua_utility.h"
#include "lua_glue.h"
#include "space_nav.h"

#define COLLIDE_FIELD	"collide_cb"

using space::Vec3;

/*! An oriented point with a radius
	A Nav is an oriented point in a space with a radius. Navs are essentially a position in 
	space with a quaternion. The radius of the Nav is used to determine collision 
	detection. All collisions are handled with a callback function.
	
	@module space.Nav
	@see space.Space
*/


/*! The position
	@LuaMethod GETSET
	@name M.pos
*/
int lua_nav_pos(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	Vec3 &pos = s->pos;
	if(lua::to_vec<float>(L, 2, 3, &pos.x)) {
		return 0;
	}
	else {
		lua::push_vec_t<float>(L, 3, &pos.x);
		return 1;
	}
	return 0;
}

/*! The velocity
	@LuaMethod GETSET
	@name M.vel
*/
int lua_nav_vel(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	Vec3 &dpos = s->dpos;
	if(lua::to_vec<float>(L, 2, 3, &dpos.x)) {
		return 0;
	}
	else {
		lua::push_vec_t<float>(L, 3, &dpos.x);
		return 1;
	}
	return 0;
}

/*! The orientation
	@LuaMethod GETSET
	@name M.orient
*/
int lua_nav_orient(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	Quat &q = s->quat;
	if(lua::to_vec<double>(L, 2, 4, &q.w)) {
		return 0;
	}
	else {
		lua::push_vec_t<double>(L, 4, &q.w);
		return 1;
	}
	return 0;
}

/*! The angular velocity
	@LuaMethod GETSET
	@name M.turn
*/
int lua_nav_turn(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	double turn[] = {0, 0, 0};
	int n = 4;
	if(lua::to_vec_n<double>(L, 2, n, turn)) {
		if(n == 3) {
			s->turn(turn[0], turn[1], turn[2]);
		}
		else {
			Quat &q = s->dquat;
			q.w = turn[0];
			q.x = turn[1];
			q.y = turn[2];
			q.z = turn[3];
		}
		return 0;
	}
	else {
		lua::push_vec_t<double>(L, 4, &(s->dquat.w));
		return 1;
	}
	return 0;
}

/*! The radius
	@LuaMethod GETSET
	@name M.radius
*/
int lua_nav_radius(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->radius = lua::to<float>(L, 2);
		return 0;
	}
	else {
		lua::push<float>(L, s->radius);
		return 1;
	}
	return 0;
}

/*! The local x-axis
	@LuaMethod GET
	@name M.x
*/
int lua_nav_x(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	lua::push_vec_t<float>(L, 3, &(s->xaxis.x));
	return 1;
}

/*! The local y-axis
	@LuaMethod GET
	@name M.y
*/
int lua_nav_y(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	lua::push_vec_t<float>(L, 3, &(s->yaxis.x));
	return 1;
}

/*! The local z-axis
	@LuaMethod GET
	@name M.z
*/
int lua_nav_z(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	lua::push_vec_t<float>(L, 3, &(s->zaxis.x));
	return 1;
}

/*! Face a point in space
	@param pos The point to face
	@name M:face
*/
int lua_nav_face(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	Vec3 dir;
	if(lua::to_vec<float>(L, 2, 3, &(dir.x))) {
		s->face(dir);
	}
	else {
		luaL_error(L, "Nav.face: invalid arguments");
	}
	return 0;
}


int lua_nav_set(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	Vec3 v;
	if(lua::to_vec<float>(L, 2, 3, &(v.x))) {
		s->set(v);
	}
	else {
		luaL_error(L, "Nav.set: invalid arguments");
	}
	return 0;
}

/*! Turn toward a point
	@param p The point
	@amt The amount to turn (positive values toward, negative values away)
	@name M:turn_toward
*/
int lua_nav_turn_toward(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	Vec3 pos;
	if(lua::to_vec_t<float>(L, 2, 3, &(pos.x))) {
		float amt = lua::opt<float>(L, 3, 1);
		s->turn_toward(pos, amt);
	}
	else {
		luaL_error(L, "Nav.turn_toward: invalid arguments");
	}
	return 0;
}

/*! Update position and orientation
	Updates position and orientation according to the current 
	velocity and angular velocity.  Also, any other Nav's within 
	the collision radius will be detected and passed through 
	the collision_cb callback if collide is enabled
	@name M:update
*/
int lua_nav_update(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	s->update();
	return 0;
}

int lua_nav_do_collide(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	if(s->collide_enable()) {
		lua_getfenv(L, 1);
		lua_pushstring(L, COLLIDE_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
		
		if(lua_isfunction(L, -1)) {
		
			neighand::NearestNeighbor<Nav> nn[16];
			int nfound = s->nearbyN(nn);
			if(nfound > 0) {
				lua_pushvalue(L, 1);	// arg 1: self
			
				// call collide_cb
				lua_newtable(L);		// arg 2: t
				for(int i=0; i < nfound; i++) {
					if(Glue<Nav>::usr_get_reference_map(L, nn[i].object)) {
						lua_rawseti(L, -2, i+1);
					}
				}
				
				int err = lua_pcall(L, 2, 0, 0);
				if(err) {
					luaL_error(L, "Nav.update: error calling collid_cb: %s", lua_tostring(L, -1));
					lua_pop(L, 1);
				}
			}
			else {
				lua_pop(L, 1);
			}
		}
		else {
			lua_pop(L, 1);
		}
	}
	return 0;
}

int lua_nav_within(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	if(s->collide_enable()) {
		float r = lua::opt<float>(L, 2, 0.1);
		
		neighand::NearestNeighbor<Nav> nn[16];
		int nfound = s->nearbyR(nn, r, 16);
		lua_newtable(L);
		for(int i=0; i < nfound; i++) {
			if(Glue<Nav>::usr_get_reference_map(L, nn[i].object)) {
				lua_rawseti(L, -2, i+1);
			}
		}
		return 1;
	}
	return 0;
}

/*! The collision enable flag
	@LuaMethod GETSET
	@name M.collide
*/
int lua_nav_collide(lua_State *L) {
	Nav *s = Glue<Nav>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->collide_enable(lua::to<bool>(L, 2));
	}
	else {
		lua::push<bool>(L, s->collide_enable());
	}
	return 0;
}

template<> void Glue<Nav>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "set", lua_nav_set },
		{ "face", lua_nav_face },
		{ "turn_toward", lua_nav_turn_toward },
		{ "update", lua_nav_update },
		{ "do_collide", lua_nav_do_collide },
		{ "within", lua_nav_within },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "pos", lua_nav_pos},
		{ "vel", lua_nav_vel },
		{ "orient", lua_nav_orient },
		{ "turn", lua_nav_turn },
		{ "radius", lua_nav_radius },
		{ "x", lua_nav_x },
		{ "y", lua_nav_y },
		{ "z", lua_nav_z },
		{ "collide", lua_nav_collide },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "pos", lua_nav_pos},
		{ "vel", lua_nav_vel },
		{ "orient", lua_nav_orient },
		{ "turn", lua_nav_turn },
		{ "radius", lua_nav_radius },
		{ "x", lua_nav_x },
		{ "y", lua_nav_y },
		{ "z", lua_nav_z },
		{ "collide", lua_nav_collide },
		{ NULL, NULL}
	};
	Glue<Nav>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_space_Nav(lua_State *L) {
	Glue<Nav>::define(L);
	Glue<Nav>::register_class(L);
	luaL_getmetatable(L, Glue<Nav>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif