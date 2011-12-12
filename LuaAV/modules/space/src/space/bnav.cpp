#include "bnav.h"

namespace space {

BNav :: BNav(BSpace *space, ShapeType tag, btCollisionShape *shape, btRigidBody *body)
:	m_space(space),
	m_shape_tag(tag),
	m_body(body)
{
	m_shape.shape = shape;
	m_body->setSleepingThresholds(0.04, 0.5);
	space->add_nav(this);
}

BNav :: ~BNav() {
	if(m_space) {
		//m_space->remove_nav(this);
	}
	// delete body
	// delete shape
}

}	// space::
