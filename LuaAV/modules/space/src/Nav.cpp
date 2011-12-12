#include "Nav.h"
#include "Space.h"

#define ABS(a)	(((a) < 0) ? -a : a)
#define CLAMP(x,a,b) (x)<(a)?(a):(x)>(b)?(b):(x)

namespace space {

double wrap(double n, double m, double o) {
	n = n - o;
	// -ve n should be 'shifted' up into a +ve range
	if (n < 0)
		n = n - (floor(n/m) * m);
	return fmod(n, m) + o;
}

Nav :: Nav(Space *space, float x, float y, float z)
:	pos(x, y, z),
	dpos(0, 0, 0),
	radius(1.),
	
	proxy(0),
	mCollideEnable(false),
	mId(0),
	space(space)
{
	update();
}

Nav :: ~Nav() {
	if(space && mCollideEnable) {
		space->remove(this);
	}
}

void Nav :: update() {
	// accumulate orientation:
	quat.rotateby(&dquat);
	quat.normalize();
	
	quat.toVectorX(&xaxis.x, &xaxis.y, &xaxis.z);
	quat.toVectorY(&yaxis.x, &yaxis.y, &yaxis.z);
	quat.toVectorZ(&zaxis.x, &zaxis.y, &zaxis.z);
	
	pos.x += dpos.x*xaxis.x + dpos.y*yaxis.x + dpos.z*zaxis.x;
	pos.y += dpos.x*xaxis.y + dpos.y*yaxis.y + dpos.z*zaxis.y;
	pos.z += dpos.x*xaxis.z + dpos.y*yaxis.z + dpos.z*zaxis.z;
	
	// wrap
	Vec3 m = space->size();
	Vec3 o = space->halfsize();
	
	pos.x = wrap(pos.x, m.x, -o.x);
	pos.y = wrap(pos.y, m.y, -o.y);
	pos.z = wrap(pos.z, m.z, -o.z);
	
	if(mCollideEnable) {
		space->move(this, pos.x, pos.y, pos.z);
	}
}

void Nav :: set(const Vec3 &v) {
	pos.set(v);
	if(mCollideEnable) {
		space->move(this, pos.x, pos.y, pos.z);
	}
}

void Nav :: view(const Quat &v) {
	quat.w = v.w; quat.x = v.x; quat.y = v.y; quat.z = v.z;
}

void Nav :: turn(float ax, float ay, float az) {
	dquat.fromEuler(ax, ay, az);
}

void Nav :: turn(const Quat &v) {
	dquat.w = v.w; dquat.x = v.x; dquat.y = v.y; dquat.z = v.z;
}

void Nav :: move(const Vec3 &v) {
	dpos.set(v);
}

void Nav :: halt() {
	dpos.set(0., 0., 0.);
	dquat.reset();
}

void Nav :: home() {
	pos.set(0.0, 0.0, 0.0);
	quat.reset();
}

Quat toward(const Vec3 &pos, Quat &quat, const Vec3 &v, float amt) {
	Vec3 diff;
	Vec3::sub(diff, v, pos);
	
	if(amt < 0) {
		diff.set(-diff.x, -diff.y, -diff.z);
	}
	
	Vec3::normalize(diff);
	
	// rotate diff to local reference frame
	Vec3 diff0 = diff;
	quat.rotateVectorTransposed(&(diff0.x), &(diff.x));
	

	// find normal axis to these two (defines the plane to rotate in):
	Vec3 axis(-diff.y, diff.x, 0);
	float axis_mag_sqr = Vec3::dot(axis, axis);
	float along = diff.z;
	
	if(axis_mag_sqr < QUAT_EPSILON && along < 0) {
		axis.set(0, 1, 0);
		axis_mag_sqr = 1.;
	}
	
	Quat q;
	if(along < QUAT_ACCURACY_MIN && axis_mag_sqr > QUAT_EPSILON) {
		double theta = ABS(amt)*acos(CLAMP(along, -1., 1.))*0.5;
		double cc = cos(theta);
		double ss = sin(theta);
		
		q.w = cc;
		q.x = ss*axis.x;
		q.y = ss*axis.y;
		q.z = ss*axis.z;
	}
	else {
		q.w = 1;
		q.x = 0;
		q.y = 0;
		q.z = 0;
	}
	return q;
}

void Nav :: face(const Vec3 &dir) {
	Vec3 p;
	Vec3::add(p, pos, dir);
	Quat dq = toward(pos, quat, p, 1);
	
	quat.rotateby(&dq);
	quat.normalize();
	
	quat.toVectorX(&xaxis.x, &xaxis.y, &xaxis.z);
	quat.toVectorY(&yaxis.x, &yaxis.y, &yaxis.z);
	quat.toVectorZ(&zaxis.x, &zaxis.y, &zaxis.z);
}

void Nav :: turn_toward(const Vec3 &v, float amt) {
	Quat q = toward(pos, quat, v, amt);
	
	dquat.w = q.w;
	dquat.x = q.x;
	dquat.y = q.y;
	dquat.z = q.z;
	
	/*
	Vec3 diff;
	Vec3::sub(diff, v, pos);
	
	if(amt < 0) {
		diff.set(-diff.x, -diff.y, -diff.z);
	}
	
	Vec3::normalize(diff);
	
	// rotate diff to local reference frame
	Vec3 diff0 = diff;
	quat.rotateVectorTransposed(&(diff0.x), &(diff.x));
	

	// find normal axis to these two (defines the plane to rotate in):
	Vec3 axis(-diff.y, diff.x, 0);
	float axis_mag_sqr = Vec3::dot(axis, axis);
	float along = diff.z;
	
	if(axis_mag_sqr < QUAT_EPSILON && along < 0) {
		axis.set(0, 1, 0);
		axis_mag_sqr = 1.;
	}

	if(along < QUAT_ACCURACY_MIN && axis_mag_sqr > QUAT_EPSILON) {
		double theta = ABS(amt)*acos(CLAMP(along, -1., 1.))*0.5;
		double cc = cos(theta);
		double ss = sin(theta);
		
		dquat.w = cc;
		dquat.x = ss*axis.x;
		dquat.y = ss*axis.y;
		dquat.z = ss*axis.z;
	}
	else {
		dquat.w = 1;
		dquat.x = 0;
		dquat.y = 0;
		dquat.z = 0;
	}
	*/
}

void Nav :: collide_enable(bool v) {
	// ignore redundant settings
	if(v == mCollideEnable) return;

	// add/remove from collision space
	if(v) {
		space->insert(this, pos.x, pos.y, pos.z);
	}
	else {
		space->remove(this);
	}
	mCollideEnable = v;
}

int Nav :: nearbyN(neighand::NearestNeighbor<Nav> *nn, int nmax) {
	return space->NH.findNearestNeighbors(proxy, radius, nn, nmax);
}

int Nav :: nearbyR(neighand::NearestNeighbor<Nav> *nn, float r, int nmax) {
	return space->NH.findNearestNeighbors(proxy, r, nn, nmax);
}

}	// space::

