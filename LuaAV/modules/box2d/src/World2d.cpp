#include "World2d.h"
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>


void DebugDraw :: DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	glColor3fv(&color.r);
	glBegin(GL_LINE_LOOP);
	for(int i=0; i < vertexCount; i++) {
		glVertex2fv(&vertices[i].x);
	}
	glEnd();
}

void DebugDraw :: DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	glColor3fv(&color.r);
	glBegin(GL_POLYGON);
	for(int i=0; i < vertexCount; i++) {
		glVertex2fv(&vertices[i].x);
	}
	glEnd();
}

void DebugDraw :: DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	glColor3fv(&color.r);
	glBegin(GL_LINE_LOOP);
	float dtheta = 1./20*3.1415;
	float theta = 0.;
	for(int i=0; i < 40; i++) {
		glVertex2f(radius*cos(theta)+center.x, radius*sin(theta)+center.y);
		theta += dtheta;
	}
	glEnd();
}

void DebugDraw :: DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	glColor3fv(&color.r);
	glBegin(GL_POLYGON);
	float dtheta = 1./20*3.1415;
	float theta = 0.;
	for(int i=0; i < 40; i++) {
		glVertex2f(radius*cos(theta)+center.x, radius*sin(theta)+center.y);
		theta += dtheta;
	}
	glEnd();
}

void DebugDraw :: DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	glColor3fv(&color.r);
	glBegin(GL_LINES);
		glVertex2fv(&p1.x);
		glVertex2fv(&p2.x);
	glEnd();
}

void DebugDraw :: DrawTransform(const b2Transform& xf) {};

World2D :: World2D(ContactListener *contactListener)
:	contactListener(contactListener),
	world(NULL),
	userData(0)
{
	setup();
}

World2D :: ~World2D() {
	destroy();
	delete world;
	delete contactListener;
}

void World2D :: setup() {
	b2Vec2 gravity(0.0f, -10.f);
	bool doSleep = true;
	world = new b2World(gravity, doSleep);	
	world->SetDestructionListener(&destructionListener);
	world->SetContactListener(contactListener);
	world->SetDebugDraw(&debugDraw);
}

void World2D :: destroy() {
	b2Body *body = world->GetBodyList();
	while(body) {
		Body2d *b = (Body2d *)body->GetUserData();
		if(b) {
			b->world = 0;
			b->body = 0;
		}
		body = body->GetNext();
	}
}

void World2D :: setGravity(float x, float y) {
	b2Vec2  gravity(x, y);
	world->SetGravity(gravity);
}
	

void World2D :: step(float dt, int velIterations, int posIterations) {
	world->Step(dt, velIterations, posIterations);
	world->ClearForces();
}

Body2d * World2D :: body(b2BodyDef &bd) {
	Body2d *b = new Body2d(this, 0);
	bd.userData = b;
	b->body = world->CreateBody(&bd);
	return b;
}