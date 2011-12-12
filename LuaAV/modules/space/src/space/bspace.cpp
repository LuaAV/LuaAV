#include "bspace.h"
#include "bnav.h"

namespace space {

BSpace :: BSpace()
:	m_collision_config(0),
	m_dispatcher(0),
	m_broadphase(0),
	m_solver(0),
	m_world(0)
{
	m_collision_config = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collision_config);
	m_broadphase = new btDbvtBroadphase();
	
	m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	
	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(
		m_dispatcher,
		m_broadphase,
		m_solver,
		m_collision_config
	);
	
	m_world->setGravity(btVector3(0, 0, 0));
}

BSpace :: ~BSpace() {
	/*//remove the rigidbodies from the dynamics world and delete them
	for(int i = m_world->getNumCollisionObjects()-1; i >= 0; i--) {
		btCollisionObject *obj = m_world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if(body && body->getMotionState()) {
			delete body->getMotionState();
		}
		m_world->removeCollisionObject(obj);
		delete obj;
	}
	
	for(int j=0; j < m_collision_shapes.size(); j++) {
		btCollisionShape *shape = m_collision_shapes[j];
		delete shape;
	}
	*/
	delete m_world;
	delete m_solver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collision_config;
}

void BSpace :: add_nav(BNav *nav) {
	m_world->addRigidBody(
		nav->get_body()
	);
}

void BSpace :: remove_nav(BNav *nav) {
	m_world->removeRigidBody(
		nav->get_body()
	);
}

BNav * BSpace :: create_sphere(btVector3 &pos, btScalar radius, btScalar mass) {
	btSphereShape *sphere = new btSphereShape(radius);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(pos);
	
	// rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool is_dynamic = (mass != 0.f);
	btVector3 local_inertia(0, 0, 0);
	if(is_dynamic) {
		sphere->calculateLocalInertia(mass, local_inertia);
	}
	
	//using motionstate is recommended, it provides interpolation capabilities, 
	// and only synchronizes 'active' objects
	btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass, 
		motion_state, 
		sphere, 
		local_inertia
	);
	
	// Dynamics
	btRigidBody *body = new btRigidBody(rbInfo);
	
	return new BNav(this, BNav::SPHERE, sphere, body);
}

}	// space::
