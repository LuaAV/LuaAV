#ifndef SPACE_POINT_H
#define SPACE_POINT_H 1

#include <math.h>
using namespace std;

#include "neighand.h"
#include "quat.h"

namespace space {

class Space;

#define FLOAT_EPS		(10e-6f)

struct Vec3 {
	Vec3()
	:	x(0), y(0), z(0)
	{}
	
	Vec3(float x, float y, float z)
	:	x(x), y(y), z(z)
	{}
	
	void set(float _x, float _y, float _z) {
		x = _x; y = _y; z = _z;
	}
	
	void set(const Vec3 &v) {
		x = v.x; y = v.y; z = v.z;
	}
	
	static void sub(Vec3 &v, const Vec3 &v1, const Vec3 &v2) {
		v.x = v1.x - v2.x;
		v.y = v1.y - v2.y;
		v.z = v1.z - v2.z;
	}
	
	static void add(Vec3 &v, const Vec3 &v1, const Vec3 &v2) {
		v.x = v1.x + v2.x;
		v.y = v1.y + v2.y;
		v.z = v1.z + v2.z;
	}
	
	static void normalize(Vec3 &v) {
		float magnitude_sqr = Vec3::dot(v, v);

		if (magnitude_sqr > FLOAT_EPS) {
			float scale = sqrt(magnitude_sqr);
			scale = 1.0f / scale;
			v.x *= scale;
			v.y *= scale;
			v.z *= scale;
		}
		else {
			v.x = FLOAT_EPS;
			v.y = FLOAT_EPS;
			v.z = FLOAT_EPS;
		}
	}
	
	static void cross(Vec3 &v, const Vec3 &v1, const Vec3 &v2) {
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
	}
	
	static float dot(const Vec3 &v1, const Vec3 &v2) {
		float result = v1.x * v2.x;
		result += v1.y * v2.y;
		result += v1.z * v2.z;

		return result;
	}

	float x;
	float y;
	float z;
};

class Nav {
public:
	Nav(Space *space, float x, float y, float z);	
	~Nav();

	Quat	quat;		/// orientation
	Quat	dquat;		/// angular velocity
	
	Vec3	pos;		/// position
	Vec3	dpos;		/// velocity
	
	Vec3	xaxis;
	Vec3	yaxis;
	Vec3	zaxis;
	
	float	radius;
	
	void update();
	
	void view(const Quat &v);
	void turn(float ax, float ay, float az);
	void turn(const Quat &v);
	void move(const Vec3 &v);
	void halt();
	void home();
	
	void face(const Vec3 &dir);
	void turn_toward(const Vec3 &v, float amt);
	
	bool collide_enable() {return mCollideEnable;}
	void collide_enable(bool v);
	
	int nearbyN(neighand::NearestNeighbor<Nav> *nn, int nmax=16);
	int nearbyR(neighand::NearestNeighbor<Nav> *nn, float r, int nmax=16);
	
	struct neighand::ObjectProxy<Nav> * proxy;
	
	void set_space(Space *s) {
		space = s;
	}
	
	void set(const Vec3 &v);
	
protected:
	bool	mCollideEnable;
	int		mId;
	Space	*space;
};

}	// space::

#endif	// SPACE_POINT_H
