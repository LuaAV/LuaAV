#ifndef WORLD_2D_H
#define WORLD_2D_H

#include <Box2D/Box2D.h>
#include <map>

using std::map;

#define k_maxContactPoints	2048
#define MAX_SHAPES			4096 

class World2D;

struct Body2d {
public:
	Body2d(World2D *world, b2Body *body) 
	: world(world), body(body)
	{
		
	}
	
	~Body2d() {}
	
	World2D *world;
	b2Body *body;
};

struct Joint2d {
public:
	Joint2d(World2D *world, b2Joint *joint) 
	: world(world), joint(joint)
	{}
	
	~Joint2d() {}
	
	World2D *world;
	b2Joint *joint;
};

class ContactListener : public b2ContactListener {
public:
	enum Events {
		eBeginContact	= 1<<0,
		eEndContact		= 1<<1,
		ePreSolve		= 1<<2,
		ePostSolve		= 1<<3
	};

	World2D *world;
	int flags;
	
	ContactListener(World2D *world) 
	:	world(world),
		flags(ePostSolve)
	{}
	
	void BeginContact(b2Contact* contact) {
		if(flags & eBeginContact) {
		
		}
	}

	void EndContact(b2Contact* contact) {
		if(flags & eEndContact) {
		
		}
	}

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
		if(flags & ePreSolve) {
			
		}
	}

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		if(flags & ePostSolve) {
			
		}
	}
	
};

class DestructionListener : public b2DestructionListener {

	void SayGoodbye(b2Joint* joint) {
		Joint2d *j = (Joint2d *)joint->GetUserData();
		if(j) {
			j->world = 0;
			j->joint = 0;
		}
	}
	
	void SayGoodbye(b2Fixture* fixture) {
		// nothing for now
	}
};


class DebugDraw : public b2Draw {
public:
	DebugDraw() : b2Draw() {
		m_drawFlags = e_shapeBit+e_jointBit;
	}
	
	virtual ~DebugDraw() {}

	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	virtual void DrawTransform(const b2Transform& xf);

};

class World2D {
public:
	friend class ContactListener;
	
	typedef void (*body_destroy_cb)(b2Body *body, void *ud);
	

	DestructionListener	destructionListener;
	ContactListener		*contactListener;
	b2World				*world;
	DebugDraw			debugDraw;
	void				*userData;


	World2D(ContactListener *contactListener);
	~World2D();

	void destroy();

	void setup();
	void setGravity(float x, float y);
	Body2d * body(b2BodyDef &bd);
	
	void step(float dt = 0.03030303030303, int velIterations = 6, int posIterations = 4);
};


#endif	// WORLD_2D_H

