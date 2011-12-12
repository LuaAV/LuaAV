#include "lua_glue.h"
#include "lua_utility.h"
#include "World2d.h"
#include "b2Joint.h"

namespace lua {

template<> inline b2BodyType opt(lua_State * L, int idx, b2BodyType v) {
	return (b2BodyType)luaL_optinteger(L, idx, (int)v);
}

}


typedef b2Contact Contact2d;

class LuaContactListener : public ContactListener {
public:
	
	LuaContactListener(World2D *world) 
	:	ContactListener(world)
	{}
	
	void BeginContact(b2Contact* contact) {
		if(flags & eBeginContact) {
			lua_State *L = (lua_State *)world->userData;
			if(L && Glue<World2D>::usr_get_reference_map(L, world)) {
				lua_getfenv(L, -1);
				lua_getfield(L, -1, "BeginContact");
				
				if(lua_isfunction(L, -1)) {
					lua_pushvalue(L, -3);
					Glue<Contact2d>::push(L, contact);
					lua::pcall(L, 2);
				}
				else {
					lua_pop(L, 1);
				}

				lua_pop(L, 2);
			}
		}
	}

	void EndContact(b2Contact* contact) {
		if(flags & eEndContact) {
			lua_State *L = (lua_State *)world->userData;
			if(L && Glue<World2D>::usr_get_reference_map(L, world)) {
				lua_getfenv(L, -1);
				lua_getfield(L, -1, "EndContact");
				
				if(lua_isfunction(L, -1)) {
					lua_pushvalue(L, -3);
					Glue<Contact2d>::push(L, contact);
					lua::pcall(L, 2);
				}
				else {
					lua_pop(L, 1);
				}

				lua_pop(L, 2);
			}
		}
	}

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
		if(flags & ePreSolve) {
			lua_State *L = (lua_State *)world->userData;
			if(L && Glue<World2D>::usr_get_reference_map(L, world)) {
				lua_getfenv(L, -1);
				lua_getfield(L, -1, "PreSolve");
				
				if(lua_isfunction(L, -1)) {
					lua_pushvalue(L, -3);
					Glue<Contact2d>::push(L, contact);
					lua::pcall(L, 2);
				}
				else {
					lua_pop(L, 1);
				}

				lua_pop(L, 2);
			}
		}
	}

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		if(flags & ePostSolve) {
			lua_State *L = (lua_State *)world->userData;
			if(L && Glue<World2D>::usr_get_reference_map(L, world)) {
				lua_getfenv(L, -1);
				lua_getfield(L, -1, "PostSolve");
				
				if(lua_isfunction(L, -1)) {
					lua_pushvalue(L, -3);
					Glue<Contact2d>::push(L, contact);
					lua::pcall(L, 2);
				}
				else {
					lua_pop(L, 1);
				}

				lua_pop(L, 2);
			}
		}
	}
	
};


/*! World
	A world is the physics space in which all bodies and shapes are created in.  It 
	runs the physics and dynamics.  Callbacks can be attached to a world in 
	order to manage collisions and other events.
	
	@module box2d.World
*/

/*! Draw the world
	@LuaMethod METHOD
	@name M:draw
*/
int lua_world_draw(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	s->world->DrawDebugData();
	return 0;
}

/*! Set contact events to listen for
	The value is a bitfield whose possible bits are 
	World.BEGIN_CONTACT, World.END_CONTACT, World.PRE_SOLVE, 
	World.POST_SOLVE
	
	<luacode>
	-- list for pre- and post-solve contact events
	world.contextEvents = World.PRE_SOLVE + World.POST_SOLVE
	-- PreSolve callback
	function world:PreSolve(contact)
		-- do something (never use contact outside of this function)
	end
	-- PostSolve callback
	function world:PostSolve(contact)
		-- do something (never use contact outside of this function)
	end
	</luacode>
	
	@param texcoord Array of texture coordinate data
	@LuaMethod GETSET
	@name M.contactEvents
*/
int lua_world_contactEvents(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	if(lua::is<int>(L, 2)) {
		s->contactListener->flags = lua::to<int>(L, 2);
	}
	else {
		lua::push<int>(L, s->contactListener->flags);
		return 1;
	}
	
	return 0;
}

/*! Create a body in the world
	Bodies can be created with the listed parameters or by passing in a table 
	with all of the body parameters.  The list of paramters is: position, type, 
	angle, linearVelocity, angularVelocity, linearDamping, angularDamping, 
	allowSleep, awake, fixedRotation, active, gravityScale.  Only position is 
	required.
	
	<luacode>
	-- The following are equivalent:
	local body1 = world:body({0, 0}, 0, World.DYNAMIC)
	local body2 = world:body{
		position = {0, 0},
		angle = 0,
		type = World.DYNAMIC,
	}
	</luacode>
	
	@param pos The position of the body
	@param angle The angle of the body
	@param [type] The body type, default DYNAMIC
	@ret The body
	@LuaMethod METHOD
	@name M:body
*/
int lua_world_body(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	b2BodyDef bd;
	
	if(lua::hasfield(L, 2, "position")) {
		bd.type = lua::optfield<b2BodyType>(L, 2, b2_dynamicBody, "type");
		lua::tofield_vec<float32>(L, 2, "position", 2, &bd.position.x);
		bd.angle = lua::optfield<float32>(L, 2, 0, "angle");
		lua::tofield_vec<float32>(L, 2, "linearVelocity", 2, &bd.linearVelocity.x);
		bd.angularVelocity = lua::optfield<float32>(L, 2, 0, "angularVelocity");
		bd.linearDamping = lua::optfield<float32>(L, 2, 0, "linearDamping");
		bd.angularDamping = lua::optfield<float32>(L, 2, 0, "angularDamping");
		
		bd.allowSleep = lua::optfield<bool>(L, 2, true, "allowSleep");
		bd.awake = lua::optfield<bool>(L, 2, true, "awake");
		bd.fixedRotation = lua::optfield<bool>(L, 2, false, "fixedRotation");
		bd.active = lua::optfield<bool>(L, 2, true, "active");
		
		bd.gravityScale = lua::optfield<float32>(L, 2, 1., "gravityScale");
	}
	else {
		lua::to_vec_t<float32>(L, 2, 2, &bd.position.x);
		bd.angle = lua::opt<float32>(L, 3, 0.);
		bd.type = lua::opt<b2BodyType>(L, 4, b2_dynamicBody);
	}
	
	Body2d *b = s->body(bd);
	Glue<Body2d>::push(L, b);
	
	return 1;
}

/*! Step the world
	@param [dt] Option step time, default is 1/33
	@param [viter] Velocity iterations, default is 6
	@param [piter] Position iterations, default is 4
	@LuaMethod METHOD
	@name M:step
*/
int lua_world_step(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	float dt = lua::opt<float>(L, 2, 0.03030303030303);
	float viter = lua::opt<int>(L, 3, 6);
	float piter = lua::opt<int>(L, 4, 4);
	s->userData = L;
	s->step(dt, viter, piter);
	s->userData = 0;
	return 0;
}

/*! Remove an object from the world
	@param object A body or a joint
	@LuaMethod METHOD
	@name M:remove
*/
int lua_world_remove(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	Body2d *b = Glue<Body2d>::to(L, 2);
	Joint2d *j = Glue<Joint2d>::to(L, 2);
	if(b) {
		s->world->DestroyBody(b->body);
		b->body = 0;
		b->world = 0;
	}
	else if(j) {
		s->world->DestroyJoint(j->joint);
		j->joint = 0;
		j->world = 0;
	}
	
	return 0;
}

/*! Create a revolute joint
	Joint parameters are collideConnected, lowerAngle, upperAngle,
	maxMotorTorque, motorSpeed, enableLimit, enableMotor
	
	<luacode>
	-- an angle limited revolute joint
	local revj = world:revoluteJoint(b1, b2, {
		collideConnected = false,
		lowerAngle = pi/2,
		upperAngle = 3*pi/2
	})
	</luacode>

	@param body1 The first body
	@param body2 The second body
	@param [t] The optional table of joint parameters
	@ret The revolute joint
	@LuaMethod METHOD
	@name M:revoluteJoint
*/
int lua_world_revoluteJoint(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	Body2d *b1 = Glue<Body2d>::checkto(L, 2);
	Body2d *b2 = Glue<Body2d>::checkto(L, 3);
	if(lua_istable(L, 4)) {
		b2Vec2 pos;
		lua::to_vec_t<float32>(L, 4, 2, &pos.x);
		
		b2RevoluteJointDef jd;
		jd.Initialize(b1->body, b2->body, pos);
		
		if(lua_istable(L, 5)) {
			jd.collideConnected = lua::optfield<bool>(L, 5, false, "collideConnected");
			jd.lowerAngle = lua::optfield<float32>(L, 5, 0., "lowerAngle");
			jd.upperAngle = lua::optfield<float32>(L, 5, 0., "upperAngle");
			jd.maxMotorTorque = lua::optfield<float32>(L, 5, 0., "maxMotorTorque");
			jd.motorSpeed = lua::optfield<float32>(L, 5, 0., "motorSpeed");
			jd.enableLimit = lua::optfield<bool>(L, 5, false, "enableLimit");
			jd.enableMotor = lua::optfield<bool>(L, 5, false, "enableMotor");
		}
		
		Joint2d *j = new Joint2d(s, s->world->CreateJoint(&jd));
		Glue<Joint2d>::push(L, j);
	}
	else {
		luaL_error(L, "World.revoluteJoint: invalid arguments");
	}
	return 1;
}

/*! Create a distance joint
	Joint parameters are collideConnected, frequencyHz, dampingRatio
	
	<luacode>
	local distj = world:distanceJoint(b1, b2, {
		collideConnected = true,
	})
	</luacode>

	@param body1 The first body
	@param body2 The second body
	@param [t] The optional table of joint parameters
	@ret The distance joint
	@LuaMethod METHOD
	@name M:distanceJoint
*/
int lua_world_distanceJoint(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	Body2d *b1 = Glue<Body2d>::checkto(L, 2);
	Body2d *b2 = Glue<Body2d>::checkto(L, 3);
	if(lua_istable(L, 4)) {
		b2Vec2 pos1;
		b2Vec2 pos2;
		lua::to_vec_t<float32>(L, 4, 2, &pos1.x);
		lua::to_vec_t<float32>(L, 5, 2, &pos2.x);
		
		b2DistanceJointDef jd;
		jd.Initialize(b1->body, b2->body, pos1, pos2);
		
		if(lua_istable(L, 6)) {
			jd.collideConnected = lua::optfield<bool>(L, 6, false, "collideConnected");
			jd.frequencyHz = lua::optfield<float32>(L, 6, 1., "frequencyHz");
			jd.dampingRatio = lua::optfield<float32>(L, 6, 0.2, "dampingRatio");
		}
		
		Joint2d *j = new Joint2d(s, s->world->CreateJoint(&jd));
		Glue<Joint2d>::push(L, j);
	}
	else {
		luaL_error(L, "World.distanceJoint: invalid arguments");
	}
	return 1;
}

/*! Create a prismatic joint
	Joint parameters are collideConnected, lowerTranslation, upperTranslation,
	maxMotorForce, motorSpeed, enableLimit, enableMotor

	@param body1 The first body
	@param body2 The second body
	@param [t] The optional table of joint parameters
	@ret The prismatic joint
	@LuaMethod METHOD
	@name M:prismaticJoint
*/
int lua_world_prismaticJoint(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	Body2d *b1 = Glue<Body2d>::checkto(L, 2);
	Body2d *b2 = Glue<Body2d>::checkto(L, 3);
	if(lua_istable(L, 4)) {
		b2Vec2 anchor;
		b2Vec2 axis;
		lua::to_vec_t<float32>(L, 4, 2, &anchor.x);
		lua::to_vec_t<float32>(L, 5, 2, &axis.x);
		
		b2PrismaticJointDef jd;
		jd.Initialize(b1->body, b2->body, anchor, axis);
		
		if(lua_istable(L, 6)) {
			jd.collideConnected = lua::optfield<bool>(L, 6, false, "collideConnected");
			jd.lowerTranslation = lua::optfield<float32>(L, 6, 0., "lowerTranslation");
			jd.upperTranslation = lua::optfield<float32>(L, 6, 0., "upperTranslation");
			jd.maxMotorForce = lua::optfield<float32>(L, 6, 0., "maxMotorForce");
			jd.motorSpeed = lua::optfield<float32>(L, 6, 0., "motorSpeed");
			jd.enableLimit = lua::optfield<bool>(L, 6, false, "enableLimit");
			jd.enableMotor = lua::optfield<bool>(L, 6, false, "enableMotor");
		}
		
		Joint2d *j = new Joint2d(s, s->world->CreateJoint(&jd));
		Glue<Joint2d>::push(L, j);
	}
	else {
		luaL_error(L, "World.prismaticJoint: invalid arguments");
	}
	return 1;
}

/*! Create a pulley joint
	
	@param body1 The first body
	@param body2 The second body
	@param groundAnchor1 The first ground anchor
	@param groundAnchor2 The second ground anchor
	@param anchor1 The first anchor
	@param anchor2 The second anchor
	@ret The pulley joint
	@LuaMethod METHOD
	@name M:pulleyJoint
*/
int lua_world_pulleyJoint(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	Body2d *b1 = Glue<Body2d>::checkto(L, 2);
	Body2d *b2 = Glue<Body2d>::checkto(L, 3);
	if(lua_istable(L, 4)) {
		b2Vec2 groundAnchor1;
		b2Vec2 groundAnchor2;
		b2Vec2 anchor1;
		b2Vec2 anchor2;
		lua::to_vec_t<float32>(L, 4, 2, &groundAnchor1.x);
		lua::to_vec_t<float32>(L, 5, 2, &groundAnchor2.x);
		lua::to_vec_t<float32>(L, 6, 2, &anchor1.x);
		lua::to_vec_t<float32>(L, 7, 2, &anchor1.x);
		float32 ratio = lua::opt<float32>(L, 8, 1.);
		
		b2PulleyJointDef jd;
		jd.Initialize(
			b1->body, b2->body, 
			groundAnchor1, groundAnchor2,
			anchor1, anchor1,
			ratio
		);
		
		Joint2d *j = new Joint2d(s, s->world->CreateJoint(&jd));
		Glue<Joint2d>::push(L, j);
	}
	else {
		luaL_error(L, "World.pulleyJoint: invalid arguments");
	}
	return 1;
}

/*! Create a gear joint
	
	@param joint1 The first joint
	@param joint2 The second joint
	@param ratio The gear ratio
	@ret The gear joint
	@LuaMethod METHOD
	@name M:gearJoint
*/
int lua_world_gearJoint(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	Joint2d *j1 = Glue<Joint2d>::checkto(L, 2);
	Joint2d *j2 = Glue<Joint2d>::checkto(L, 3);
	float32 ratio = lua::opt<float32>(L, 8, 1.);
		
	b2GearJointDef jd;
	jd.joint1 = j1->joint;
	jd.joint2 = j2->joint;
	jd.ratio = ratio;
	
	Joint2d *j = new Joint2d(s, s->world->CreateJoint(&jd));
	Glue<Joint2d>::push(L, j);
	return 1;
}

/*
int lua_world_mouse_joint(lua_State *L) {
	World2D *s = Glue<World2D>::checkto(L, 1);
	
		
	b2MouseJointDef jd;
	lua::to_vec_t<float32>(L, 2, 2, &jd.target.x);
	
	if(lua_istable(L, 3)) {
		jd.maxForce = lua::optfield<float32>(L, 3, 0., "maxForce");
		jd.frequencyHz = lua::optfield<>(L, 3, 5., "frequencyHz");
		jd.dampingRatio = lua::optfield<float32>(L, 3, 0.7, "dampingRatio");
		jd.timeStep = lua::optfield<float32>(L, 6, 0.016666666666667, "timeStep");
	}
	
	Joint2d *j = new Joint2d(s, s->world->CreateJoint(&jd));
	Glue<Joint2d>::push(L, j);
	return 1;
}
*/
template<> const char * Glue<World2D>::usr_name() { return "World"; }
template<> bool Glue<World2D>::usr_has_index() { return true; }

template<> void Glue<World2D>::usr_push(lua_State * L, World2D * u) {
	Glue<World2D>::usr_attr_push(L);
	Glue<World2D>::usr_attr_prototype(L);
	Glue<World2D>::usr_set_reference_map(L, u);
}

void unset_body(Body2d *b, b2Body *body) {
	b->world = 0;
	b->body = 0;
}

template<> World2D * Glue<World2D>::usr_new(lua_State * L) {
	World2D * world = new World2D(new LuaContactListener(0));
	world->contactListener->world = world;
	float gravity[] = {0, 0};
	lua::to_vec<float>(L, 1, 2, gravity);
	world->setGravity(gravity[0], gravity[1]);
	return world;
}

template<> void Glue<World2D>::usr_index(lua_State * L, World2D * u) {
	Glue<World2D>::usr_attr_index(L);
}

template<> void Glue<World2D>::usr_newindex(lua_State * L, World2D * u) {
	Glue<World2D>::usr_attr_newindex(L);
}

template<> int Glue<World2D>::usr_tostring(lua_State * L, World2D * u) {
	lua_pushfstring(L, "%s: %p", Glue<World2D>::usr_name(), u); 
	return 1;
}

template<> void Glue<World2D>::usr_gc(lua_State * L, World2D * u) { 
	delete u;
}

template<> void Glue<World2D>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "draw", lua_world_draw },
		{ "contactEvents", lua_world_contactEvents },
		{ "body", lua_world_body },
		{ "step", lua_world_step },
		{ "remove", lua_world_remove },
		{ "revoluteJoint", lua_world_revoluteJoint },
		{ "distanceJoint", lua_world_distanceJoint },
		{ "prismaticJoint", lua_world_prismaticJoint },
		{ "pulleyJoint", lua_world_pulleyJoint },
		{ "gearJoint", lua_world_gearJoint },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<World2D>::usr_attr_mt(L, methods, getters, setters);
}

/*! Body in a World
	A body is an object in a World responding to dynamics and phyiscs.  It 
	can take on different shapes, including shape composites.
	
	@module box2d.Body
*/


/*! Add a box to a body
	A box can be created with the parameters below or a table of 
	parameters.  Valid parameters are center, size, angle, 
	friction, restitution, density, isSensor.  Size is a required parameter.
	
	<luacode>
	-- the following are equivalent:
	body1:box({1, 1}, {0, 0})
	body2:box{
		size = {1, 1},
		center = {0, 0},
	}
	</luacode>
	
	
	@param size The size of the box
	@param center The center of the box
	@param [angle] The box angle
	@LuaMethod METHOD
	@name M:box
*/
int lua_body_box(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		b2FixtureDef fd;
		b2Vec2 center(0., 0.);
		b2Vec2 size(1., 1.);
		float32 angle = 0;
		
		if(lua::hasfield(L, 2, "size")) {
			lua::tofield_vec<float32>(L, 2, "center", 2, &center.x);
			lua::tofield_vec<float32>(L, 2, "size", 2, &size.x);
			angle = lua::optfield<float32>(L, 2, 0, "angle");
			fd.friction = lua::optfield<float32>(L, 2, 0, "friction");
			fd.restitution = lua::optfield<float32>(L, 2, 0, "restitution");
			fd.density = lua::optfield<float32>(L, 2, 0, "density");
			// categoryBits
			// maskBits
			// groupIndex
			fd.isSensor = lua::optfield<bool>(L, 2, false, "isSensor");
		}
		else {
			lua::to_vec_t<float32>(L, 2, 2, &size.x);
			lua::to_vec_t<float32>(L, 3, 2, &center.x);
			angle = lua::opt<float32>(L, 4, 0.);
		}
		
		
		b2PolygonShape shape;
		shape.SetAsBox(size.x*0.5, size.y*0.5);//, center, angle);
		fd.shape = &shape;
		
		s->body->CreateFixture(&fd);
	}
	return 0;
}

/*! Add a circle to a body
	A circle can be created with the parameters below or a table of 
	parameters.  Valid parameters are center, radius, angle, 
	friction, restitution, density, isSensor.  Radius is a required parameter.
	
	<luacode>
	-- the following are equivalent:
	body1:circle(3)
	body2:circle{
		radius = 3
	}
	</luacode>
	
	
	@param size The size of the box
	@param center The center of the box
	@param [angle] The box angle
	@LuaMethod METHOD
	@name M:circle
*/
int lua_body_circle(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		b2FixtureDef fd;
		float32 radius = 0;
		
		if(lua_istable(L, 2)) {
			radius = lua::optfield<float32>(L, 2, 1, "radius");
			fd.friction = lua::optfield<float32>(L, 2, 0, "friction");
			fd.restitution = lua::optfield<float32>(L, 2, 0, "restitution");
			fd.density = lua::optfield<float32>(L, 2, 0, "density");
			// categoryBits
			// maskBits
			// groupIndex
			fd.isSensor = lua::optfield<bool>(L, 2, false, "isSensor");
		}
		else {
			radius = lua::opt<float32>(L, 2, 1.);
		}
		
		
		b2CircleShape shape;
		shape.m_radius = radius;
		fd.shape = &shape;
		
		
		s->body->CreateFixture(&fd);
	}
	return 0;
}

/*! Position
	@LuaMethod GET
	@name M.position
*/
int lua_body_position(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		const b2Vec2& v = s->body->GetPosition();
		lua::push_vec_t<float32>(L, 2, &v.x);
		return 1;
	}
	return 0;
}

/*! World center
	@LuaMethod GET
	@name M.worldCenter
*/
int lua_body_worldCenter(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		const b2Vec2& v = s->body->GetWorldCenter();
		lua::push_vec_t<float32>(L, 2, &v.x);
		return 1;
	}
	return 0;
}

/*! Angle
	@LuaMethod GET
	@name M.angle
*/
int lua_body_angle(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		lua::push<float32>(L, s->body->GetAngle());
		return 1;
	}
	return 0;
}

/*! Velocity
	@LuaMethod GETSET
	@name M.velocity
*/
int lua_body_velocity(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		b2Vec2 v;
		if(lua::to_vec<float>(L, 2, 2, &v.x)) {
			s->body->SetLinearVelocity(v);
			return 0;
		}
		else {
			const b2Vec2& vel = s->body->GetLinearVelocity();
			lua::push_vec_t<float>(L, 2, &vel.x);
			return 1;
		}
	}
	return 0;
}

/*! Angular velocity
	@LuaMethod GETSET
	@name M.angularVelocity
*/
int lua_body_angularVelocity(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		if(lua::is<float32>(L, 2)) {
			s->body->SetAngularVelocity(lua::to<float32>(L, 2));
		}
		else {
			lua::push<float32>(L, s->body->GetAngle());
			return 1;
		}
	}
	return 0;
}

/*! Mass
	Mass can be set with either a numerical value or a table.  
	If the value is a table, it can have parameters mass and center.
	
	<luacode>
	body.mass = {
		mass = 1.,
		center = {0.1, 0},
	}
	-- or simply
	body.mass = 1.
	</luacode>
	
	@LuaMethod GETSET
	@name M.mass
*/
int lua_body_mass(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		if(lua::is<float32>(L, 2)) {
			b2MassData mass;
			mass.mass = lua::to<float32>(L, 2);
			mass.I = s->body->GetInertia();
			s->body->SetMassData(&mass);
		}
		else if(lua_istable(L, 2)) {
			b2MassData mass;
			mass.mass = lua::optfield<float32>(L, 2, 1., "mass");
			lua::tofield_vec<float32>(L, 2, "center", 2, &mass.center.x);
			mass.I = s->body->GetInertia();
			s->body->SetMassData(&mass);
		}
		else {
			lua::push<float32>(L, s->body->GetMass());
			return 1;
		}
	}
	return 0;
}

/*! Intertia
	@LuaMethod GET
	@name M.inertia
*/
int lua_body_inertia(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		lua::push<float32>(L, s->body->GetInertia());
		return 1;
	}
	return 0;
}

/*! Linear damping
	@LuaMethod GETSET
	@name M.linearDamping
*/
int lua_body_linearDamping(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		if(lua::is<float32>(L, 2)) {
			s->body->SetLinearDamping(lua::to<float32>(L, 2));
		}
		else {
			lua::push<float32>(L, s->body->GetLinearDamping());
			return 1;
		}
	}
	return 0;
}

/*! Angular damping
	@LuaMethod GETSET
	@name M.angularDamping
*/
int lua_body_angularDamping(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		if(lua::is<float32>(L, 2)) {
			s->body->SetAngularDamping(lua::to<float32>(L, 2));
		}
		else {
			lua::push<float32>(L, s->body->GetAngularDamping());
			return 1;
		}
	}
	return 0;
}

/*! Size
	This attribute is only valid if the body's 
	first shape is a box.
	
	@LuaMethod GET
	@name M.size
*/
int lua_body_size(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	
	if(s->body) {
		b2Vec2 v(0, 0);
		const b2Fixture* f = s->body->GetFixtureList();
		if(f) {
			const b2Shape * shape = f->GetShape();
			if(
				shape->GetType() == b2Shape::e_polygon && 
				((b2PolygonShape *)shape)->GetVertexCount() == 4
			) {	
				const b2Vec2& v0 = ((b2PolygonShape *)shape)->GetVertex(0);
				const b2Vec2& v2 = ((b2PolygonShape *)shape)->GetVertex(2);
				v = v2-v0;
			}
		}
		lua::push_vec_t<float32>(L, 2, &v.x);

		return 1;
	}
	
	return 0;
}

/*! Apply a force
	@param force The force vecotr
	@param point The point at which to apply the force
	@LuaMethod METHOD
	@name M:applyForce
*/
int lua_body_applyForce(lua_State *L) {
	Body2d *s = Glue<Body2d>::checkto(L, 1);
	if(s->body) {
		b2Vec2 force(0, 0);
		b2Vec2 point(0, 0);
		
		lua::to_vec_t(L, 2, 2, &force.x);
		lua::to_vec_t(L, 3, 2, &point.x);
		
		s->body->ApplyForce(force, point);
	}
	
	return 0;
}

template<>
inline void Glue<Body2d>::usr_push(lua_State * L, Body2d * u) {
	Glue<Body2d>::usr_attr_push(L);
	Glue<Body2d>::usr_set_reference_map(L, u);
}

template<> 
inline const char * Glue<Body2d>::usr_name() { return "Body"; }

template<> 
inline bool Glue<Body2d>::usr_has_index() { return true; }

template<> 
inline void Glue<Body2d>::usr_index(lua_State * L, Body2d * u) {
	Glue<Body2d>::usr_attr_index(L);
}

template<> 
inline void Glue<Body2d>::usr_newindex(lua_State * L, Body2d * u) {
	Glue<Body2d>::usr_attr_newindex(L);
}

template<> 
inline int Glue<Body2d>::usr_tostring(lua_State * L, Body2d * u) {
	lua_pushfstring(L, "%s: %p", Glue<Body2d>::usr_name(), u); 
	return 1;
}

template<> 
inline void Glue<Body2d>::usr_gc(lua_State * L, Body2d * u) { 
	if(u->body) {
		u->body->SetUserData(0);
	}
	u->world = 0;
	u->body = 0;
	delete u;
}

template<> void Glue<Body2d>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "box", lua_body_box},
		{ "circle", lua_body_circle},
		{ "applyForce", lua_body_applyForce },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "position", lua_body_position},
		{ "worldCenter", lua_body_worldCenter},
		{ "angle", lua_body_angle},
		{ "velocity", lua_body_velocity},
		{ "angularVelocity", lua_body_angularVelocity},
		{ "mass", lua_body_mass},
		{ "inertia", lua_body_inertia},
		{ "linearDamping", lua_body_linearDamping},
		{ "angularDamping", lua_body_angularDamping},
		{ "size", lua_body_size },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "velocity", lua_body_velocity},
		{ "angularVelocity", lua_body_angularVelocity},
		{ "mass", lua_body_mass},
		{ "linearDamping", lua_body_linearDamping},
		{ "angularDamping", lua_body_angularDamping},
		{ NULL, NULL}
	};
	Glue<Body2d>::usr_attr_mt(L, methods, getters, setters);
}


/*! Joint in a World
	A joint connect two objects in the World or one object to a location 
	in the World
	
	@module box2d.Joint
*/


/*! Body A
	@LuaMethod GET
	@name M.bodyA
*/
int lua_joint_bodyA(lua_State *L) {
	Joint2d *s = Glue<Joint2d>::checkto(L, 1);
	if(s->joint) {
		Body2d *b = (Body2d *)s->joint->GetBodyA()->GetUserData();
		if(b) {
			Glue<Body2d>::usr_get_reference_map(L, b);
			return 1;
		}
	}
	return 0;
}

/*! Body B
	@LuaMethod GET
	@name M.bodyB
*/
int lua_joint_bodyB(lua_State *L) {
	Joint2d *s = Glue<Joint2d>::checkto(L, 1);
	if(s->joint) {
		Body2d *b = (Body2d *)s->joint->GetBodyB()->GetUserData();
		if(b) {
			Glue<Body2d>::usr_get_reference_map(L, b);
			return 1;
		}
	}
	return 0;
}

/*! Anchor A position
	@LuaMethod GET
	@name M.anchorA
*/
int lua_joint_anchorA(lua_State *L) {
	Joint2d *s = Glue<Joint2d>::checkto(L, 1);
	if(s->joint) {
		const b2Vec2& v = s->joint->GetAnchorA();
		lua::push_vec_t<float32>(L, 2, &v.x);
		return 1;
	}
	return 0;
}

/*! Anchor B position
	@LuaMethod GET
	@name M.anchorB
*/
int lua_joint_anchorB(lua_State *L) {
	Joint2d *s = Glue<Joint2d>::checkto(L, 1);
	if(s->joint) {
		const b2Vec2& v = s->joint->GetAnchorB();
		lua::push_vec_t<float32>(L, 2, &v.x);
		return 1;
	}
	return 0;
}

template<>
inline void Glue<Joint2d>::usr_push(lua_State * L, Joint2d * u) {
	Glue<Joint2d>::usr_attr_push(L);
	Glue<Joint2d>::usr_set_reference_map(L, u);
}

template<> 
inline const char * Glue<Joint2d>::usr_name() { return "Joint"; }

template<> 
inline bool Glue<Joint2d>::usr_has_index() { return true; }

template<> 
inline void Glue<Joint2d>::usr_index(lua_State * L, Joint2d * u) {
	Glue<Joint2d>::usr_attr_index(L);
}

template<> 
inline void Glue<Joint2d>::usr_newindex(lua_State * L, Joint2d * u) {
	Glue<Joint2d>::usr_attr_newindex(L);
}

template<> 
inline int Glue<Joint2d>::usr_tostring(lua_State * L, Joint2d * u) {
	lua_pushfstring(L, "%s: %p", Glue<Joint2d>::usr_name(), u); 
	return 1;
}

template<> 
inline void Glue<Joint2d>::usr_gc(lua_State * L, Joint2d * u) { 
	if(u->joint) {
		u->joint->SetUserData(0);
	}
	u->world = 0;
	u->joint = 0;
	delete u;
}

template<> void Glue<Joint2d>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "bodyA", lua_joint_bodyA},
		{ "bodyB", lua_joint_bodyB},
		{ "anchorA", lua_joint_anchorA},
		{ "anchorB", lua_joint_anchorB},
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<Joint2d>::usr_attr_mt(L, methods, getters, setters);
}


/*! Contact in a World
	A contact represent the point at which two shapes collide in a world.
	
	@module box2d.Contact
*/


/*! Enabled flag
	@LuaMethod GETSET
	@name M.enabled
*/
int lua_contact_enabled(lua_State *L) {
	Contact2d *s = Glue<Contact2d>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->SetEnabled(lua::to<bool>(L, 2));
	}
	else {
		lua::push<bool>(L, s->IsEnabled());
		return 1;
	}
	return 0;
}

/*! Friction
	@LuaMethod GETSET
	@name M.friction
*/
int lua_contact_friction(lua_State *L) {
	Contact2d *s = Glue<Contact2d>::checkto(L, 1);
	if(lua::is<float32>(L, 2)) {
		s->SetFriction(lua::to<float32>(L, 2));
	}
	else {
		lua::push<float32>(L, s->GetFriction());
		return 1;
	}
	return 0;
}

/*! Restitution
	@LuaMethod GETSET
	@name M.restitution
*/
int lua_contact_restitution(lua_State *L) {
	Contact2d *s = Glue<Contact2d>::checkto(L, 1);
	if(lua::is<float32>(L, 2)) {
		s->SetRestitution(lua::to<float32>(L, 2));
	}
	else {
		lua::push<float32>(L, s->GetRestitution());
		return 1;
	}
	return 0;
}

/*! Body A
	@LuaMethod GET
	@name M.bodyA
*/
int lua_contact_bodyA(lua_State *L) {
	Contact2d *s = Glue<Contact2d>::checkto(L, 1);
	Body2d *b = (Body2d *)(s->GetFixtureA()->GetBody()->GetUserData());
	if(b) {
		Glue<Body2d>::usr_get_reference_map(L, b);
		
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

/*! Body B
	@LuaMethod GET
	@name M.bodyB
*/
int lua_contact_bodyB(lua_State *L) {
	Contact2d *s = Glue<Contact2d>::checkto(L, 1);
	Body2d *b = (Body2d *)(s->GetFixtureB()->GetBody()->GetUserData());
	if(b) {
		Glue<Body2d>::usr_get_reference_map(L, b);
		
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

template<>
inline void Glue<Contact2d>::usr_push(lua_State * L, Contact2d * u) {
	Glue<Contact2d>::usr_attr_push(L);
	Glue<Contact2d>::usr_set_reference_map(L, u);
}

template<> 
inline const char * Glue<Contact2d>::usr_name() { return "Contact"; }

template<> 
inline bool Glue<Contact2d>::usr_has_index() { return true; }

template<> 
inline void Glue<Contact2d>::usr_index(lua_State * L, Contact2d * u) {
	Glue<Contact2d>::usr_attr_index(L);
}

template<> 
inline void Glue<Contact2d>::usr_newindex(lua_State * L, Contact2d * u) {
	Glue<Contact2d>::usr_attr_newindex(L);
}

template<> 
inline int Glue<Contact2d>::usr_tostring(lua_State * L, Contact2d * u) {
	lua_pushfstring(L, "%s: %p", Glue<Contact2d>::usr_name(), u); 
	return 1;
}

template<> 
inline void Glue<Contact2d>::usr_gc(lua_State * L, Contact2d * u) { 
	// nothing
}

template<> void Glue<Contact2d>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "enabled", lua_contact_enabled },
		{ "friction", lua_contact_friction },
		{ "restitution", lua_contact_restitution },
		{ "bodyA", lua_contact_bodyA },
		{ "bodyB", lua_contact_bodyB },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "enabled", lua_contact_enabled },
		{ "friction", lua_contact_friction },
		{ "restitution", lua_contact_restitution },
		{ NULL, NULL}
	};
	Glue<Contact2d>::usr_attr_mt(L, methods, getters, setters);
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_box2d_Contact(lua_State *L) {
	Glue<Contact2d>::define(L);
	Glue<Contact2d>::register_class(L);
	luaL_getmetatable(L, Glue<Contact2d>::mt_name(L));

	return 1;
}

int luaopen_box2d_Joint(lua_State *L) {
	Glue<Joint2d>::define(L);
	Glue<Joint2d>::register_class(L);
	luaL_getmetatable(L, Glue<Joint2d>::mt_name(L));

	return 1;
}

int luaopen_box2d_Body(lua_State *L) {
	Glue<Body2d>::define(L);
	Glue<Body2d>::register_class(L);
	luaL_getmetatable(L, Glue<Body2d>::mt_name(L));

	return 1;
}

int luaopen_box2d_World(lua_State *L) {
	Glue<World2D>::define(L);
	Glue<World2D>::register_class(L);
	luaL_getmetatable(L, Glue<World2D>::mt_name(L));
	
	lua::pushfield<int>(L, -1, "BEGIN_CONTACT", ContactListener::eBeginContact);
	lua::pushfield<int>(L, -1, "END_CONTACT", ContactListener::eEndContact);
	lua::pushfield<int>(L, -1, "PRE_SOLVE", ContactListener::ePreSolve);
	lua::pushfield<int>(L, -1, "POST_SOLVE", ContactListener::ePostSolve);
	lua::pushfield<int>(L, -1, "STATIC", b2_staticBody);
	lua::pushfield<int>(L, -1, "KINETIC", b2_kinematicBody);
	lua::pushfield<int>(L, -1, "DYNAMIC", b2_dynamicBody);
	
	lua::call<const char *>(L, luaopen_box2d_Body, "box2d.Body");
	lua_pop(L, 1);
	
	lua::call<const char *>(L, luaopen_box2d_Joint, "box2d.Joint");
	lua_pop(L, 1);
	
	lua::call<const char *>(L, luaopen_box2d_Contact, "box2d.Contact");
	lua_pop(L, 1);

	return 1;
}

#ifdef __cplusplus
}
#endif
