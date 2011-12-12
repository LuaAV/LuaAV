#ifndef SPACE_VEC4_H
#define SPACE_VEC4_H 1

#include <math.h>
#include "vec3.h"

namespace space {

/*!
	\class Vec4

	A 4D vector class.  Contains overloaded operators for the usual arithmetic operations as well
	as a range of instance and static functions for other vector math operations.  Static methods may
	seem redundant, but they are useful when operating on buffers of numbers, which can be cast to
	Vec4 type.
*/
template <typename T = double>
class Vec4 {
public:
	// 4-component vector
	T x, y, z, w;

	Vec4()
	: x(0), y(0), z(0), w(0)
	{}

	Vec4(T *v) {set(v);}

	Vec4(T x, T y, T z, T w)
	: x(x), y(y), z(z), w(w)
	{}

	Vec4(const Vec4 &v)
	{ x = v.x; y = v.y; z = v.z; w = v.w; }

	void set(const Vec4 &v)
	{ x = v.x; y = v.y; z = v.z; w = v.w; }

	void set(T *v)
	{ T *vv = &x; *vv++ = *v++; *vv++ = *v++; *vv++ = *v++; *vv++ = *v++; }

	void set(T x, T y, T z, T w)
	{this->x = x; this->y = y; this->z = z; this->w = w;}

	T& operator[](int i)		{ return ((T *)&x)[i]; }
    T  operator[](int i) const	{ return ((T *)&x)[i]; }
    operator const T*()			{ return (T *)&x; }

	/*! Negation */
	const Vec4 operator- () const {
		return Vec4(-x, -y, -z, -w);
	}

	/*! Subtraction */
	const Vec4 operator- (const Vec4 &v) const {
		return Vec4(x-v.x, y-v.y, z-v.z, w-v.w);
	}

	/*! Addition */
	const Vec4 operator+ (Vec4 &v) const {
		return Vec4(x+v.x, y+v.y, z+v.z, w+v.w);
	}

	/*! Scale */
	const Vec4 operator* (T s) const {
		return Vec4(x*s, y*s, z*s, w*s);
	}

	/*! Scalar Divide */
	const Vec4 operator/ (T s) const {
		double sinv = 1./s;
		return Vec4(x*sinv, y*sinv, z*sinv, w*sinv);
	}

	/*! Multiply */
	const Vec4 operator* (Vec4 v) const {
		return Vec4(x*v.x, y*v.y, z*v.z, w*v.w);
	}

	/*! Divide */
	const Vec4 operator/ (Vec4 v) const {
		return Vec4(x/v.x, y/v.y, z/v.z, w/v.w);
	}

	/*! In-place Constant Addition */
	Vec4 &operator+= (T s) {
		x += s; y += s; z +=s; w +=s;
		return *this;
	}

	/*! In-place Constant Subtraction */
	Vec4 &operator-= (T s)
	{
		x -= s; y -= s; z -=s; w -=s;
		return *this;
	}

	/*! In-place Addition */
	Vec4 &operator+= (Vec4 v) {
		x += v.x; y += v.y; z += v.z; w += v.w;
		return *this;
	}

	/*! In-place Subtraction */
	Vec4 &operator-= (Vec4 v) {
		x -= v.x; y -= v.y; z -= v.z; w -= v.w;
		return *this;
	}

	/*! In-place Scale */
	Vec4 &operator*= (T s) {
		x *= s; y *= s; z *=s ; w *=s ;
		return *this;
	}

	/*! In-place Multiply */
	Vec4 &operator*= (Vec4 v) {
		x *= v.x; y *= v.y; z *= v.z; w *= v.w;
		return *this;
	}

	/*!
		Addition

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void add(Vec4 &v, Vec4 &v1, Vec4 &v2) {
		v.x = v1.x + v2.x;
		v.y = v1.y + v2.y;
		v.z = v1.z + v2.z;
		v.w = v1.w + v2.w;
	}

	/*!
		Subtraction

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void sub(Vec4 &v, Vec4 &v1, Vec4 &v2) {
		v.x = v1.x - v2.x;
		v.y = v1.y - v2.y;
		v.z = v1.z - v2.z;
		v.w = v1.w - v2.w;
	}

	/*!
		Component-wise Multiply

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void mul(Vec4 &v, Vec4 &v1, Vec4 &v2) {
		v.x = v1.x * v2.x;
		v.y = v1.y * v2.y;
		v.z = v1.z * v2.z;
		v.w = v1.w * v2.w;
	}

	/*!
		Component-wise Divide

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void div(Vec4 &v, Vec4 &v1, Vec4 &v2) {
		v.x = v1.x / v2.x;
		v.y = v1.y / v2.y;
		v.z = v1.z / v2.z;
		v.w = v1.w / v2.w;
	}

	/*!
		Scale

		@ret v	Result
		@param v1	First input
		@param s	Scale factor
	*/
	static void scale(Vec4 &v, Vec4 &v1, T s) {
		v.x = v1.x * s;
		v.y = v1.y * s;
		v.z = v1.z * s;
		v.w = v1.w * s;
	}

	/*!
		Dot product

		@ret dot product
		@param v1	First input
		@param v2	Second input
	*/
	static float dot(const Vec4 &v1, const Vec4 &v2) {
		float result = v1.x * v2.x;
		result += v1.y * v2.y;
		result += v1.z * v2.z;
		result += v1.w * v2.w;

		return result;
	}

	/*!
		Normalize

		@param v	Vector to normalize
	*/
	static void normalize(Vec4 &v) {
		float magnitude_sqr = Vec4::dot(v, v);

		if (magnitude_sqr > FLOAT_EPS) {
			float scale = sqrt(magnitude_sqr);
			scale = 1.0f / scale;
			v.x *= scale;
			v.y *= scale;
			v.z *= scale;
			v.w *= scale;
		}
		else {
			v.x = FLOAT_EPS;
			v.y = FLOAT_EPS;
			v.z = FLOAT_EPS;
			v.w = FLOAT_EPS;
		}
	}

	/*!
		Linear Interpolation

		@ret v			Result
		@param v1		First input
		@param v1		Second input
		@param interp	Interpolation factor [0, 1]
	*/
	static void lerp(Vec4 &v, Vec4 &v1, Vec4 &v2, T interp) {
		v.x = v1.x + interp*(v2.x - v1.x);
		v.y = v1.y + interp*(v2.y - v1.y);
		v.z = v1.z + interp*(v2.z - v1.z);
		v.w = v1.w + interp*(v2.w - v1.w);
	}
};

typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;

}	// space::

#endif	//SPACE_VEC4_H
