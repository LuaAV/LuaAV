#ifndef SPACE_QUAT_H
#define SPACE_QUAT_H

/*
	Quat::slerp taken from

	 Visualizing Quaternions:
     Appendix E:  Tables E.1 through E.7: pages 443-450
     C and Mathematica code.

This  Appendix summarizes the  essential quaternion  utilities (Tables
E.1 through E.7) needed to implement many of the concepts presented in
the book.  Selected programs are duplicated in the text as applicable.


   Copyright 2006, Trustees of Indiana University
   These files may be freely copied for personal and
   educational uses provided these lines are included.

   Full License available at: visualizingquaternions_license.txt

*/

#include <math.h>
#include "vec3.h"

#define FLOAT_EPS		(10e-6f)
#define QUAT_ACCURACY_MAX (1.000001)
#define QUAT_ACCURACY_MIN (0.999999)
#define QUAT_EPSILON (0.000001)
#define QUAT_RAD2DEG (57.29577951308)
#define QUAT_DEG2RAD (0.01745329252)
#define QUAT_DEG2RAD_BY2 (0.00872664626)
#define QUAT_PIBY2 (1.57079632679)
#define QUAT_PI (3.14159265358979)

//#define TEM template <typename T>

#define VEC3_NORMALIZE(v) { \
	T s = FLOAT_EPS; \
	T m = (v)[0]*(v)[0] + (v)[1]*(v)[1] + (v)[2]*(v)[2]; \
	if (m > FLOAT_EPS) \
	{ \
		s = sqrt(m); \
		(v)[0] /= s; \
		(v)[1] /= s; \
		(v)[2] /= s; \
	} \
	else \
	{ \
		(v)[0] = s; \
		(v)[1] = s; \
		(v)[2] = s; \
	} \
}

#ifndef ABS
	#define ABS(x) ((x)<0?-(x):(x))
#endif

/*
	Needs add (bisector of 2 quats)
	Extract Versor/Tensor (Polar) Scalar/Right (Rectangular) pairs
	Angle(q)
	Axis(q)
	Index(q)
	Needs dq operator
*/

namespace space {

/*!
	\class Quat

	A quaternion class.
*/
template <typename T = double>
class Quat {
protected:
	typedef Vec3<T>		vec3;

public:
	// 4-complex data:
	T w, x, y, z;

	Quat(T w = (T)1, T x = (T)0, T y = (T)0, T z = (T)0)
	: w(w), x(x), y(y), z(z)
	{ normalize(); };

	Quat(const Quat& q)
	: w(q.w), x(q.x), y(q.y), z(q.z)
	{}

	void set(T w = (T)1, T x = (T)0, T y = (T)0, T z = (T)0) {
		this->w = w; this->x = x; this->y = y; this->z = z;
	}

	void set(const Quat& q) {
		w = q.w; x = q.x; y = q.y; z = q.z;
	}

	Quat operator+ (Quat &v) const {
		Quat q(*this);
		q.add(v);
		return q;
	}

	Quat operator* (const Quat &v) const {
		Quat q(*this);
		q.mul(v);
		return q;
	}

	Quat& operator= (const Quat& q) {
		w = q.w; x = q.x; y = q.y; z = q.z;
		return *this;
	}

	/*!
		Take the conjugate of a quaternion
	*/
	Quat conjugate() {
		return Quat(w, -x, -y, -z);
	}

	/*!
		Normalize
	*/
	void normalize() {
		//normcount = 0;
		double unit = w*w + x*x + y*y + z*z;
		if (unit*unit < QUAT_EPSILON) {
			// unit too close to epsilon, set to default transform
			w = (T)1;
			x = y = z = (T)0;
			return;
		}
		if (unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN) {
			T invmag = (T)1/sqrt(unit);
			w *= invmag;
			x *= invmag;
			y *= invmag;
			z *= invmag;
		}
	}

	/*!
		Reset back to the identity quaternion
	*/
	void reset() {
		w = (T)1;
		x = y = z = (T)0;
	}

	void inverse(Quat & result) {
		// same as conjugate if normalized as q^-1 = q_conj/q_mag^2
		normalize();
		result.w = w;
		result.x = -x;
		result.y = -y;
		result.z = -z;
	}

	void add(const Quat &q1) {
		w += q1.w;
		x += q1.x;
		y += q1.y;
		z += q1.z;
	}

	void sub(const Quat &q1) {
		w -= q1.w;
		x -= q1.x;
		y -= q1.y;
		z -= q1.z;
	}

	/*!
		Multiply two quaternions (equivalent to rotation)
	*/
	void mul(const Quat &q2) {
		Quat q1(*this);

		w = (q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z);
		x = (q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y);
		y = (q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z);
		z = (q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x);
		normalize();
	}

	/*!
		Reverse multiply two quaternions (multiplication is not commutative)
	*/
	void revMul(const Quat &q1) {
		Quat q2(*this);

		w = (q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z);
		x = (q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y);
		y = (q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z);
		z = (q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x);
		normalize();
	}

	float mag() {
		return sqrt(w*w + x*x + y*y + z*z);
	}


	/*!
		Convert axis-angle format to a quaternion
	*/
	void fromAxisAngle(T theta, T x1, T y1, T z1) {
		T t2 = theta * (T)0.00872664626; // * 0.5 * 180/PI
		T sinft2 = sin(t2);
		w = cos(t2);
		x = x1 * sinft2;
		y = y1 * sinft2;
		z = z1 * sinft2;
		normalize();
	}

	/*!
		Convert Euler angle representation to a quaternion

		http://vered.rose.utoronto.ca/people/david_dir/GEMS/GEMS.html
		Converting from Euler angles to a quaternion is slightly more tricky, as the order of operations
		must be correct. Since you can convert the Euler angles to three independent quaternions by
		setting the arbitrary axis to the coordinate axes, you can then multiply the three quaternions
		together to obtain the final quaternion.

		So if you have three Euler angles (a, b, c), then you can form three independent quaternions
		Qx = [ cos(a/2), (sin(a/2), 0, 0)]
		Qy = [ cos(b/2), (0, sin(b/2), 0)]
		Qz = [ cos(c/2), (0, 0, sin(c/2))]
		And the final quaternion is obtained by Qx * Qy * Qz.
	*/
	void fromEuler(T az, T el, T ba) {

		T c1 = cos(az * QUAT_DEG2RAD_BY2);
		T c2 = cos(el * QUAT_DEG2RAD_BY2);
		T c3 = cos(ba * QUAT_DEG2RAD_BY2);
		T s1 = sin(az * QUAT_DEG2RAD_BY2);
		T s2 = sin(el * QUAT_DEG2RAD_BY2);
		T s3 = sin(ba * QUAT_DEG2RAD_BY2);

		// equiv quat_multiply(&Qy, &Qx, &Q1); // since many terms are zero
		T tw = c1*c2;
		T tx = c1*s2;
		T ty = s1*c2;
		T tz = - s1*s2;

		// equiv quat_multiply(&Q1, &Qz, &Q2); // since many terms are zero
		w = tw*c3 - tz*s3;
		x = tx*c3 + ty*s3;
		y = ty*c3 - tx*s3;
		z = tw*s3 + tz*c3;
		normalize();
	}

	/*!
		Convert to a 4x4 rotation matrix
	*/
	void toMatrix(T *m) {
		vec3 &vx = (vec3&)(*m);
		toVectorX(vx);
		m[3] = (T)0;

		vec3 &vy = (vec3&)(*(m+4));
		toVectorY(vy);
		m[7] = (T)0;

		vec3 &vz = (vec3&)(*(m+8));
		toVectorZ(vz);
		m[11] = (T)0;

		m[12] = m[13] = m[14] = (T)0;
		m[15] = (T)1;
	}
	
	/*!
		Convert from a 4x4 rotation matrix
		
		0	4	8	12
		1	5	9	13
		2	6	10	14
		3	7	11	15
	*/
	void fromMatrix(T *m) {
		T trace = m[0]+m[5]+m[10];
		w = sqrt(1. + trace)*0.5;
		
		if(trace > 0.) {
			x = (m[6] - m[9])/(4.*w);
			y = (m[8] - m[2])/(4.*w);
			z = (m[1] - m[4])/(4.*w);
		}
		else {
			if(m[0] > m[5] && m[0] > m[10]) {
				// m[0] is greatest
				x = sqrt(1. + m[0]-m[5]-m[10])*0.5;
				w = (m[6] - m[9])/(4.*x);
				y = (m[1] + m[4])/(4.*x);
				z = (m[2] + m[8])/(4.*x);
			}
			else if(m[5] > m[0] && m[5] > m[10]) {
				// m[1] is greatest
				y = sqrt(1. + m[5]-m[0]-m[10])*0.5;
				w = (m[8] - m[2])/(4.*y);
				x = (m[1] + m[4])/(4.*y);
				z = (m[6] + m[9])/(4.*y);
			}
			else { //if(m[10] > m[0] && m[10] > m[5]) {
				// m[2] is greatest
				z = sqrt(1. + m[10]-m[0]-m[5])*0.5;
				w = (m[1] - m[4])/(4.*z);
				x = (m[2] + m[8])/(4.*z);
				y = (m[6] + m[9])/(4.*z);
			}
		}
	}


	/*!
		Convert to axis-angle format
	*/
	void toAxisAngle(T & aa, T & ax, T & ay, T & az) {
		T unit = w*w;
		if (unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN) {
			T invSinAngle = 1.0/sqrt(1.0 - unit);
			aa = acos(w) * 114.59155902616; // * 2 * 180 / pi
			ax = x * invSinAngle;
			ay = y * invSinAngle;
			az = z * invSinAngle;
		} else {
			aa = (T)0;
			ax = x;
			ay = y;
			az = z;
		}
	}

	/*!
		Convert to Euler angle format
		http://www.mathworks.com/access/helpdesk/help/toolbox/aeroblks/quaternionstoeulerangles.html
	*/
	void toEuler(T & az, T & el, T & ba) {
		T sqw = w*w;
		T sqx = x*x;
		T sqy = y*y;
		T sqz = z*z;
		az = QUAT_RAD2DEG * asinf(-2.0f * (x*z - w*y));
		el = QUAT_RAD2DEG * atan2f(2.0f * (y*z + w*x),(sqw - sqx - sqy + sqz));
		ba = QUAT_RAD2DEG * atan2f(2.0f * (x*y + w*z), (sqw + sqx - sqy - sqz));
	}

	/*!
		Get the x-axis of a local coordinate system represented by a quaternion.
	*/
	void toVectorX(vec3 &v)	{
		v.x = 1.0 - 2.0*y*y - 2.0*z*z;
		v.y = 2.0*x*y + 2.0*z*w;
		v.z = 2.0*x*z - 2.0*y*w;
	}

	/*!
		Get the y-axis of a local coordinate system represented by a quaternion.
	*/
	void toVectorY(vec3 &v) {
		v.x = 2.0*x*y - 2.0*z*w;
		v.y = 1.0 - 2.0*x*x - 2.0*z*z;
		v.z = 2.0*y*z + 2.0*x*w;
	}

	/*!
		Get the z-axis of a local coordinate system represented by a quaternion.
	*/
	void toVectorZ(vec3 & v) {
		v.x = 2.0*x*z + 2.0*y*w;
		v.y = 2.0*y*z - 2.0*x*w;
		v.z = 1.0 - 2.0*x*x - 2.0*y*y;
	}


	/*!
		Rotate a 3D vector by the quaternion
	*/
	void rotateVector(vec3&v) {
		vec3 v_orig(v);
		vec3 vx, vy, vz;

		toVectorX(vx);
		toVectorY(vy);
		toVectorZ(vz);

		v = vx * v_orig.x;
		v += vy * v_orig.y;
		v += vz * v_orig.z;
	}

	/*!
		Spherical linear interpolation of a quaternion
		Does not change the applied quaternion

		@param result	Resulting interpolated quaternion
		@param target	The quaternion to interpolate toward
		@param amt		The amount to interpolate, range [0, 1]
	*/

	static Quat slerp(Quat &input, Quat &target, T amt) {
		Quat result;
		int bflip = 0;
		T dot_prod = input.w*target.w + input.x*target.x + input.y*target.y + input.z*target.z;
		T a, b;

		//clamp
		dot_prod = (dot_prod < -1) ? -1 : ((dot_prod > 1) ? 1 : dot_prod);

		// if B is on opposite hemisphere from A, use -B instead
		if (dot_prod < 0.0) {
			dot_prod = -dot_prod;
			bflip = 1;
		}

		T cos_angle = acos(dot_prod);
		if(ABS(cos_angle) > FLOAT_EPS) {
			T sine = sin(cos_angle);
			T inv_sine = 1./sine;

			a = sin(cos_angle*(1.-amt)) * inv_sine;
			b = sin(cos_angle*amt) * inv_sine;

			if (bflip) {
				b = -b;
			}
		}
		else {
			a = amt;
			b = 1.-amt;
		}

		result.w = a*input.w + b*target.w;
		result.x = a*input.x + b*target.x;
		result.y = a*input.y + b*target.y;
		result.z = a*input.z + b*target.z;

		result.normalize();
		return result;
	}

	/*!
		Spherical linear interpolation of a quaternion

		@param target	The quaternion to interpolate toward
		@param amt		The amount to interpolate, range [0, 1]
	*/

	void slerp(Quat &target, T amt) {
		int bflip = 0;
		T dot_prod = w*target.w + x*target.x + y*target.y + z*target.z;
		T a, b;

		// clamp
		dot_prod = (dot_prod < -1) ? -1 : ((dot_prod > 1) ? 1 : dot_prod);

		// if B is on opposite hemisphere from A, use -B instead
		if (dot_prod < 0.0) {
			dot_prod = -dot_prod;
			bflip = 1;
		}

		// from the geometric product's scalar term + s
		T cos_angle = acos(dot_prod);
		if(ABS(cos_angle) > FLOAT_EPS) {
			// spherical
			T sine = sin(cos_angle);
			T inv_sine = 1./sine;

			a = sin(cos_angle*(1.-amt)) * inv_sine;
			b = sin(cos_angle*amt) * inv_sine;

			if(bflip) {
				b = -b;
			}
		}
		else {
			// linear
			a = amt;
			b = 1.-amt;
		}

		w = a*w + b*target.w;
		x = a*x + b*target.x;
		y = a*y + b*target.y;
		z = a*z + b*target.z;

		normalize();
	}

	/*!
		Get the quaternion from a given point and quaterion toward another point
	*/
	void toward_point(vec3 &pos, Quat &q, vec3 &v, float amt) {
		vec3 diff, axis;
		vec3::sub(diff, v, pos);
		vec3::normalize(diff);
		
		if(amt < 0) {
			diff = diff*-1.;
		}
		
		vec3 zaxis;
		q.toVectorZ(zaxis);
		vec3::cross(axis, zaxis, diff);
		vec3::normalize(axis);
		
		float axis_mag_sqr = vec3::dot(axis, axis);
		float along = vec3::dot(zaxis, diff);
		
		if(axis_mag_sqr < 0.001 && along < 0) {
			vec3::cross(axis, zaxis, vec3(0., 0., 1.));
			vec3::normalize(axis);
			
			if(axis_mag_sqr < 0.001) {
				vec3::cross(axis, zaxis, vec3(0., 1., 0.));
				vec3::normalize(axis);
			}
			
			axis_mag_sqr = vec3::dot(axis, axis);
		}
		
		if(along < 0.9995 && axis_mag_sqr > 0.001) {
			float theta = ABS(amt)*acos(along)*QUAT_RAD2DEG;
//			printf("theta: %f  amt: %f\n", theta, amt);
			fromAxisAngle(theta, axis.x, axis.y, axis.z);
		}
		else {
			reset();
		}
	}

	// v1 and v2 must be normalized
	// alternatively expressed as Q = (1+gp(v1, v2))/sqrt(2*(1+dot(b, a)))
	static Quat rotor(vec3 &v1, vec3 &v2) {
		// get the normal to the plane (i.e. the unit bivector containing the v1 and v2)
		vec3 n;
		vec3::cross(n, v1, v2);
		vec3::normalize(n);	// normalize because the cross product can get slightly unnormalized

		// calculate half the angle between v1 and v2
		T dotmag = vec3::dot(v1, v2);
		T theta = acos(dotmag)*0.5;

		// calculate the scaled actual bivector generaed by v1 and v2
		vec3 bivec = n*sin(theta);
		Quat q(cos(theta), bivec.x, bivec.y, bivec.z);

		return q;
	}
	
	static Quat rotor(vec3 &v1, vec3 &v2, float amt) {
		// get the normal to the plane (i.e. the unit bivector containing the v1 and v2)
		float along = vec3::dot(v1, v2);
		if(along > 0.99999) {
			return Quat();
		}
		else if(along < -0.99999) {
			vec3 t = vec3::tangent(v1);
			float theta = QUAT_PI*amt;
			vec3 bivec = t*sin(theta);
			Quat q(cos(theta), bivec.x, bivec.y, bivec.z);
			return q;
		}
		else {
			vec3 n;
			vec3::cross(n, v1, v2);
			vec3::normalize(n);	// normalize because the cross product can get slightly unnormalized

			// calculate half the angle between v1 and v2
			T dotmag = vec3::dot(v1, v2);
			T theta = acos(dotmag)*0.5*amt;

			// calculate the scaled actual bivector generaed by v1 and v2
			vec3 bivec = n*sin(theta);
			Quat q(cos(theta), bivec.x, bivec.y, bivec.z);
			return q;
		}
	}
};

typedef Quat<float> Quatf;
typedef Quat<double> Quatd;


}	// space::

#endif	// SPACE_QUAT_H
