#include "lua_bnav.h"
#include "lua_utility.h"

int lua_bnav_pos(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 pos;
	if(lua::to_vec_t<btScalar>(L, 2, 3, pos.m_floats)) {
		s->get_body()->getWorldTransform().setOrigin(pos);
	}
	else {
		lua::push_vec_t<btScalar>(
			L, 
			3, 
			s->get_body()->getWorldTransform().getOrigin().m_floats
		);
		return 1;
	}
	return 0;
}

int lua_bnav_vel(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 vel;
	if(lua::to_vec_t<btScalar>(L, 2, 3, vel.m_floats)) {
		s->get_body()->setLinearVelocity(vel);
	}
	else {
		lua::push_vec_t<btScalar>(
			L, 
			3, 
			s->get_body()->getLinearVelocity().m_floats
		);
		return 1;
	}
	return 0;
}

int lua_bnav_radius(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btSphereShape *sphere = s->get_sphere();
	if(sphere) {
		lua::push<btScalar>(L, sphere->getRadius());
	}
	else {
		luaL_error(L, "BNav.radius: BNav is not a sphere, has no radius");
	}
	return 1;
}

int lua_bnav_quat(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	
	if(lua_istable(L, 2)) {
		lua_rawgeti(L, 2, 1);
		btScalar w = lua_tonumber(L, -1);
		lua_rawgeti(L, 2, 2);
		btScalar x = lua_tonumber(L, -1);
		lua_rawgeti(L, 2, 3);
		btScalar y = lua_tonumber(L, -1);
		lua_rawgeti(L, 2, 4);
		btScalar z = lua_tonumber(L, -1);
		lua_pop(L, 4);
		
		btQuaternion q(x, y, z, w);
		s->get_body()->getWorldTransform().setRotation(q);
		
		return 0;
	}
	else {
		btQuaternion q = s->get_body()->getWorldTransform().getRotation();
		lua_newtable(L);
		lua_pushnumber(L, q.w());
		lua_rawseti(L, -2, 1);
		lua_pushnumber(L, q.x());
		lua_rawseti(L, -2, 2);
		lua_pushnumber(L, q.y());
		lua_rawseti(L, -2, 3);
		lua_pushnumber(L, q.z());
		lua_rawseti(L, -2, 4);
	}
	return 1;
}

// YPR, AEB in relation to the z-axis
int lua_bnav_euler(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 angles;
	s->get_body()->getWorldTransform().getBasis().getEulerYPR(
		angles.m_floats[2],
		angles.m_floats[0],
		angles.m_floats[1]
	);
	lua::push_vec_t<btScalar>(L, 3, angles);
	return 1;
}

int lua_bnav_xaxis(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	lua::push_vec_t<btScalar>(
		L, 
		3, 
		s->get_body()->getWorldTransform().getBasis().getColumn(0).m_floats
	);
	return 1;
}

int lua_bnav_yaxis(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	lua::push_vec_t<btScalar>(
		L, 
		3, 
		s->get_body()->getWorldTransform().getBasis().getColumn(1).m_floats
	);
	return 1;
}

int lua_bnav_zaxis(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	lua::push_vec_t<btScalar>(
		L, 
		3, 
		s->get_body()->getWorldTransform().getBasis().getColumn(2).m_floats
	);
	return 1;
}

//  m_linearVelocity += impulse *m_linearFactor * m_inverseMass;
int lua_bnav_impulse(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 v;
	if(lua::to_vec<btScalar>(L, 2, 3, v.m_floats)) {
		s->get_body()->applyCentralImpulse(v);
	}
	else {
		luaL_error(L, "BNav.impulse: invalid arguments");
	}
	return 0;
}

//  m_totalForce += force*m_linearFactor;
int lua_bnav_force(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 v;
	if(lua::to_vec<btScalar>(L, 2, 3, v.m_floats)) {
		btVector3 pos(0, 0, 0);
		lua::to_vec<btScalar>(L, 3, 3, pos.m_floats);
		s->get_body()->applyForce(v, pos);
	}
	else {
		luaL_error(L, "BNav.force: invalid arguments");
	}
	return 0;
}

// m_angularVelocity += m_invInertiaTensorWorld * torque * m_angularFactor;
int lua_bnav_torque_impulse(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 v;
	if(lua::to_vec<btScalar>(L, 2, 3, v.m_floats)) {
		s->get_body()->applyTorqueImpulse(v);
	}
	else {
		luaL_error(L, "BNav.torque_impulse: invalid arguments");
	}
	return 0;
}

// m_totalTorque += torque*m_angularFactor;
int lua_bnav_torque(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 v;
	if(lua::to_vec<btScalar>(L, 2, 3, v.m_floats)) {
		s->get_body()->applyTorque(v);
	}
	else {
		luaL_error(L, "BNav.torque: invalid arguments");
	}
	return 0;
}

int lua_bnav_damping(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	if(lua::is<btScalar>(L, 2)) {
		btScalar ldamp = lua::to<btScalar>(L, 2);
		btScalar adamp = lua::opt<btScalar>(L, 3, ldamp);
		s->get_body()->setDamping(ldamp, adamp);
	}
	else {
		luaL_error(L, "BNav.damping: invalid arguments");
	}
	return 0;
}

int lua_bnav_sleep_threshold(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	if(lua::is<btScalar>(L, 2)) {
		btScalar lthresh = lua::to<btScalar>(L, 0.04);
		btScalar athresh = lua::opt<btScalar>(L, 3, 0.5);
		s->get_body()->setSleepingThresholds(lthresh, athresh);
	}
	else {
		luaL_error(L, "BNav.damping: invalid arguments");
	}
	return 0;
}

int lua_bnav_active(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		bool v = lua::to<bool>(L, 2);
		if(v != s->get_body()->isActive()) {
			s->get_body()->activate(v);
		}
	}
	else {
		lua::push<bool>(L, s->get_body()->isActive());
		return 1;
	}
	return 0;
}

int lua_bnav_restitution(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	if(lua::is<btScalar>(L, 2)) {
		s->get_body()->setRestitution(lua::to<btScalar>(L, 2));
	}
	else {
		lua::push<btScalar>(L, s->get_body()->getRestitution());
		return 1;
	}
	return 0;
}

int lua_bnav_friction(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	if(lua::is<btScalar>(L, 2)) {
		s->get_body()->setFriction(lua::to<btScalar>(L, 2));
	}
	else {
		lua::push<btScalar>(L, s->get_body()->getFriction());
		return 1;
	}
	return 0;
}

int lua_bnav_local2global(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 v;
	if(lua::to_vec<btScalar>(L, 2, 3, v.m_floats)) {
		lua::push_vec_t<btScalar>(
			L, 
			3, 
			s->get_body()->getWorldTransform()*v
		);
	}
	else {
		luaL_error(L, "BNav.local2global: invalid arguments");
	}
	return 1;
}

int lua_bnav_global2local(lua_State *L) {
	BNav *s = Glue<BNav>::checkto(L, 1);
	btVector3 v;
	if(lua::to_vec<btScalar>(L, 2, 3, v.m_floats)) {
		lua::push_vec_t<btScalar>(
			L, 
			3, 
			s->get_body()->getWorldTransform().invXform(v)
		);
	}
	else {
		luaL_error(L, "BNav.global2local: invalid arguments");
	}
	return 1;
}

template<> void Glue<BNav>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "impulse", lua_bnav_impulse },
		{ "force", lua_bnav_force },
		{ "torque_impulse", lua_bnav_torque_impulse },
		{ "torque", lua_bnav_torque },
		{ "damping", lua_bnav_damping },
		{ "sleep_threshold", lua_bnav_sleep_threshold },
		{ "local2global", lua_bnav_local2global},
		{ "global2local", lua_bnav_global2local},
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "pos", lua_bnav_pos },
		{ "vel", lua_bnav_vel },
		{ "quat", lua_bnav_quat },
		{ "euler", lua_bnav_euler },
		{ "radius", lua_bnav_radius },
		{ "active", lua_bnav_active },
		{ "restitution", lua_bnav_restitution },
		{ "friction", lua_bnav_friction },
		{ "xaxis", lua_bnav_xaxis },
		{ "yaxis", lua_bnav_yaxis },
		{ "zaxis", lua_bnav_zaxis },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "pos", lua_bnav_pos },
		{ "vel", lua_bnav_vel },
		{ "quat", lua_bnav_quat },
		{ "active", lua_bnav_active },
		{ "restitution", lua_bnav_restitution },
		{ "friction", lua_bnav_friction },
		{ NULL, NULL}
	};
	Glue<BNav>::usr_attr_mt(L, methods, getters, setters);
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_space_BNav(lua_State *L) {
	Glue<BNav>::define(L);
	Glue<BNav>::register_class(L);
	luaL_getmetatable(L, Glue<BNav>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif