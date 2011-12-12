#ifndef BSPACE_SPACE_H
#define BSPACE_SPACE_H 1

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

namespace space {

class BNav;

class BSpace {
public:

	BSpace();
	~BSpace();
	
	void add_nav(BNav *nav);
	void remove_nav(BNav *nav);
	
	BNav * create_sphere(btVector3 &pos, btScalar radius, btScalar mass);
	btDiscreteDynamicsWorld * get_world() {return m_world;}

protected:
//	btAlignedObjectArray<btCollisionShape *> m_collision_shapes;
	btDefaultCollisionConfiguration *m_collision_config;
	btCollisionDispatcher *m_dispatcher;
	btBroadphaseInterface *m_broadphase;
	btConstraintSolver *m_solver;
	btDiscreteDynamicsWorld *m_world;
};


} // space::

#endif // BSPACE_SPACE_H