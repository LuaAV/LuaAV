#ifndef BSPACE_NAV_H
#define BSPACE_NAV_H 1

#include "bspace.h"

namespace space {

class BNav {
public:
	enum ShapeType {
		NONE = 0,
		SPHERE
	};

	BNav(BSpace *space, ShapeType tag, btCollisionShape *shape, btRigidBody *body);
	~BNav();
	
	ShapeType get_shape_tag() {return m_shape_tag;}
	btCollisionShape * get_shape() {return m_shape.shape;}
	btSphereShape * get_sphere() {
		return (m_shape_tag == SPHERE) ? m_shape.sphere : 0;
	}
	btRigidBody * get_body() {return m_body;}
	
protected:
	BSpace *m_space;
	
	ShapeType m_shape_tag;
	union {
		btCollisionShape *shape;
		btSphereShape *sphere;
	} m_shape;

	btRigidBody *m_body;
	
//	btPairCachingGhostObject *m_ghost;
};


} // space::

#endif // BSPACE_SPACE_H