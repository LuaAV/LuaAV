#ifndef SPACE_VEC3_H
#define SPACE_VEC3_H

#include <math.h>

/*
IEEE 754 Specification:

Size(bits):						32			64
Usual C implementation:			float		double
Sign(bits):						1			1
Exponent(bits):					8			11
Significand/Mantissa(bits):		23			52
Range:							+/-10^38	+/-10^308
Smallest absolute value:		10^-38		10^-308
Precision						1 in 10^6	1 in 10^15

*/

#define FLOAT_EPS		(10e-6f)

#ifndef ABS
	#define ABS(x) ((x)<0?-(x):(x))
#endif

#define VEC3_RAD2DEG (57.29577951308)

namespace space {

/*!
	\class Vec3

	A 3D vector class.  Contains overloaded operators for the usual arithmetic operations as well
	as a range of instance and static functions for other vector math operations.  Static methods may
	seem redundant, but they are useful when operating on buffers of numbers, which can be cast to
	Vec3 type.
*/
template <typename T = double>
class Vec3 {
public:
	// 3-component vector
	T x, y, z;

	Vec3()
	: x(0), y(0), z(0)
	{}

	Vec3(T *v) {set(v);}

	Vec3(T x, T y, T z)
	: x(x), y(y), z(z)
	{}

	template <typename T2>
	Vec3(Vec3<T2> &v) {
		x = (T)v.x;
		y = (T)v.y;
		z = (T)v.z;
	}

	Vec3(const Vec3 &v)
	{ x = v.x; y = v.y; z = v.z; }

	T& operator[](int i)		{ return ((T *)&x)[i]; }
	T  operator[](int i) const	{ return ((T *)&x)[i]; }
	operator const T*()			{ return (T *)&x; }
	
	T* ptr() { return (T *)&x; }

	void set(const Vec3 &v)
	{ x = v.x; y = v.y; z = v.z; }

	void set(T *v)
	{ T *vv = &x; *vv++ = *v++; *vv++ = *v++; *vv++ = *v++; }

	void set(T x, T y, T z)
	{this->x = x; this->y = y; this->z = z;}

	/*! Negation */
	const Vec3 operator- () const {
		return Vec3(-x, -y, -z);
	}

	/*! Subtraction */
	const Vec3 operator- (const Vec3 &v) const {
		return Vec3(x-v.x, y-v.y, z-v.z);
	}

	/*! Addition */
	const Vec3 operator+ (const Vec3 &v) const {
		return Vec3(x+v.x, y+v.y, z+v.z);
	}

	/*! Scale */
	const Vec3 operator* (T s) const {
		return Vec3(x*s, y*s, z*s);
	}

	/*! Scalar Divide */
	const Vec3 operator/ (T s) const {
		double sinv = 1./s;
		return Vec3(x*sinv, y*sinv, z*sinv);
	}

	/*! Multiply */
	const Vec3 operator* (const Vec3 v) const {
		return Vec3(x*v.x, y*v.y, z*v.z);
	}

	/*! Divide */
	const Vec3 operator/ (const Vec3 &v) const {
		return Vec3(x/v.x, y/v.y, z/v.z);
	}

	/*! Cross */
	const Vec3 operator^ (Vec3 &v) const {
		Vec3 n; Vec3::cross(n, *this, v);
		return n;
	}

	/*! In-place Constant Addition */
	Vec3 &operator+= (T s) {
		x += s; y += s; z +=s ;
		return *this;
	}

	/*! In-place Constant Subtraction */
	Vec3 &operator-= (T s) {
		x -= s; y -= s; z -=s ;
		return *this;
	}

	/*! In-place Addition */
	Vec3 &operator+= (Vec3 v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	/*! In-place Subtraction */
	Vec3 &operator-= (Vec3 v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	/*! In-place Scale */
	Vec3 &operator*= (T s) {
		x *= s; y *= s; z *=s ;
		return *this;
	}

	/*! In-place Multiply */
	Vec3 &operator*= (Vec3 v) {
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}

	/*! is Equal
		@param v1	First input
		@param v2	Second input
	*/
	static bool isEqual(Vec3 &v1, Vec3 &v2) {
		if(v1.x == v2.x && v1.y == v2.y && v1.z == v2.z){
			return true;
		}
		return false;
	}

	/*! Addition

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void add(Vec3 &v, Vec3 &v1, Vec3 &v2) {
		v.x = v1.x + v2.x;
		v.y = v1.y + v2.y;
		v.z = v1.z + v2.z;
	}

	/*! Subtraction

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void sub(Vec3 &v, Vec3 &v1, Vec3 &v2) {
		v.x = v1.x - v2.x;
		v.y = v1.y - v2.y;
		v.z = v1.z - v2.z;
	}

	/*! Component-wise Multiply

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void mul(Vec3 &v, Vec3 &v1, Vec3 &v2) {
		v.x = v1.x * v2.x;
		v.y = v1.y * v2.y;
		v.z = v1.z * v2.z;
	}

	/*! Component-wise Divide

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void div(Vec3 &v, Vec3 &v1, Vec3 &v2) {
		v.x = v1.x / v2.x;
		v.y = v1.y / v2.y;
		v.z = v1.z / v2.z;
	}

	/*! Scale

		@ret v	Result
		@param v1	First input
		@param s	Scale factor
	*/
	static void scale(Vec3 &v, Vec3 &v1, T s) {
		v.x = v1.x * s;
		v.y = v1.y * s;
		v.z = v1.z * s;
	}

	/*! Dot product

		@ret dot product
		@param v1	First input
		@param v2	Second input
	*/
	static float dot(const Vec3 &v1, const Vec3 &v2) {
		float result = v1.x * v2.x;
		result += v1.y * v2.y;
		result += v1.z * v2.z;

		return result;
	}

	/*! Magnitude squared

		@ret magnitude squared
		@param v	Input vector
	*/
	static float mag_sqr(const Vec3 &v) {
		return dot(v, v);
	}

	/*! Magnitude

		@ret magnitude
		@param v	Input vector
	*/
	static float mag(const Vec3 &v) {
		return sqrt(dot(v, v));
	}

	/*! Cross product

		@ret v	Result
		@param v1	First input
		@param v2	Second input
	*/
	static void cross(Vec3 &v, const Vec3 &v1, const Vec3 &v2) {
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
	}

	/*! Normalize

		@param v	Vector to normalize
	*/
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

	/*! Linear Interpolation

		@ret v		Result
		@param v1		First input
		@param v1		Second input
		@param interp	Interpolation factor [0, 1]
	*/
	static void lerp(Vec3 &v, Vec3 &v1, Vec3 &v2, T interp) {
		v.x = v1.x + interp*(v2.x - v1.x);
		v.y = v1.y + interp*(v2.y - v1.y);
		v.z = v1.z + interp*(v2.z - v1.z);
	}
	
	
	static Vec3 easein2(Vec3 &v1, Vec3 &v2, T t) {
		return (v2-v1)*t*t+v1;
	}
	
	static Vec3 easeout2(Vec3 &v1, Vec3 &v2, T t) {
		return -(v2-v1)*t*(t-2)+v1;
	}
	
	static Vec3 easeinout2(Vec3 &v1, Vec3 &v2, T t) {
		t *= 2.;
		if(t < 1.) {
			return (v2-v1)*0.5*t*t+v1;
		}
		else {
			T amt = t-2;
			return -(v2-v1)*0.5*(amt*amt-2.)+v1;
		}
	}

/*
function cubic_in(t, b, c, d)
	local amt = t/d
	return c*amt*amt*amt+b
end

function cubic_out(t, b, c, d)
	local amt = t/d-1
	return c*(amt*amt*amt+1)+b
end

function cubic_inout(t, b, c, d)
	t = t*2/d
	if(t < 1) then
		return c/2*t*t*t+b
	else
		local amt = t-2
		return c/2*(amt*amt*amt+2)+b
	end
end

function quartic_in(t, b, c, d)
	return c*pow(t/d, 4)+b
end

function quartic_out(t, b, c, d)
	return -c*(pow(t/d-1, 4)-1)+b
end

function quartic_inout(t, b, c, d)
	t = t*2
	if(t < 1.) then
		return c/2*pow(t/d, 4)+b
	else
	 	return -c/2*(pow(t-2, 4)-2)+b
	 end
end
*/

	static void bezier(Vec3 &v, Vec3 &p1, Vec3 &p2, Vec3 &p3, Vec3 &p4, float mu) {
		T mum1 = 1. - mu;
		T mum13 = mum1 * mum1 * mum1;
		T mu3 = mu * mu * mu;

		v.x = mum13*p1.x + 3*mu*mum1*mum1*p2.x + 3*mu*mu*mum1*p3.x + mu3*p4.x;
		v.y = mum13*p1.y + 3*mu*mum1*mum1*p2.y + 3*mu*mu*mum1*p3.y + mu3*p4.y;
		v.z = mum13*p1.z + 3*mu*mum1*mum1*p2.z + 3*mu*mu*mum1*p3.z + mu3*p4.z;
	}



	/*
		de casteljau algorithm for four point interpolation 
		@ret v result
		@param a		First point
		@param b		Second point
		@param c		Third point
		@param d		Fourth point
		@param interp	Interpolation factor [0, 1]

	*/
	static void casteljau(Vec3 &v, Vec3 &a, Vec3 &b, Vec3 &c, Vec3 &d, T interp){
		Vec3 ab,bc,cd,abbc,bccd;
		lerp (ab, a,b,interp);           // point between a and b 
		lerp (bc, b,c,interp);           // point between b and c 
		lerp (cd, c,d,interp);           // point between c and d 
		lerp (abbc, ab,bc,interp);       // point between ab and bc 
		lerp (bccd, bc,cd,interp);       // point between bc and cd 
		lerp (v, abbc,bccd,interp);		 // point on the curve 

	}

	/*
		hermite curve interpolation 
		@ret v result
		@start point p1
		@start tangent t1 (direction and velocity at the start point)
		@end point p2
		@end tangent t2,
		@param interp	Interpolation factor [0, 1]

	*/
	static void hermite(Vec3 &v, Vec3 &p1, Vec3 &t1, Vec3 &p2, Vec3 &t2, T interp){
		T interpSq = interp*interp;
		T interpCb = interpSq*interp;
		
		// calculate basis functions
		T h1 = 2*interpCb - 3*interpSq + 1;
		T h2 = -2*interpCb + 3*interpSq;
		T h3 = interpCb - 2*interpSq + interp;
		T h4 = interpCb - interpSq;

		// multiply and sum all funtions together to build the interpolated point along the curve.
		v = p1*h1 + p2*h2 + t1*h3 + t2*h4;
	}
	
	static void hermite2(Vec3 &v, Vec3 &p0, Vec3 &p1, Vec3 &p2, Vec3 &p3, T mu/*interp*/, T tension=0, T bias=0) {
		T mu2 = mu * mu;
		T mu3 = mu2 * mu;
		
		T a0 =  2*mu3 - 3*mu2 + 1;
		T a1 =    mu3 - 2*mu2 + mu;
		T a2 =    mu3 -   mu2;
		T a3 = -2*mu3 + 3*mu2;
		
		T k1 = (1+bias)*(1-tension)*0.5;
		T k2 = (1-bias)*(1-tension)*0.5;
			
		Vec3 m0  = (p1-p0)*k1;
		m0 += (p2-p1)*k2;

		Vec3 m1  = (p2-p1)*k1;
		m1 += (p3-p2)*k2;
		
		v = p1*a0 + m0*a1 + m1*a2 + p2*a3;
	}
	

	/*! Get the normal to a triangle defined by three points
		@param p1	Point1
		@param p2	Point2
		@param p3	Point3
		@ret n	Normal
	*/
	static void normal(Vec3 &n, Vec3 &p1, Vec3 &p2, Vec3 &p3) {
		Vec3 v1, v2;
		sub(v1, p2, p1);
		sub(v2, p3, p1);
		normalize(v1);
		normalize(v2);
		cross(n, v1, v2);
	}

	/*! Centroid of a triangle defined by three points
		@param p1	Point1
		@param p2	Point2
		@param p3	Point3
		@ret c	Centroid
	*/
	static void centroid3(Vec3 &c, const Vec3 &p1, const Vec3 &p2, const Vec3 &p3) {
		c.x = 0.333333*(p1.x+p2.x+p3.x);
		c.y = 0.333333*(p1.y+p2.y+p3.y);
		c.z = 0.333333*(p1.z+p2.z+p3.z);
	}
	
	/*! Tangent to a normal
		
		@ret t		Tangent plane vector
		@param n	Normal vector
	*/
	static Vec3 tangent(const Vec3 &n) {
		static T eps = 0.0001;
		static Vec3 arbitrary_axis1(1, 0, 0);
		
		Vec3 axis1, tangent;
		cross(axis1, n, arbitrary_axis1);
		cross(tangent, n, axis1);
		
		if(mag_sqr(tangent) < eps) {
			static Vec3 arbitrary_axis2(0, 1, 0);
			cross(axis1, n, arbitrary_axis2);
			cross(tangent, n, axis1);
		}
		
		return tangent;
	}
	
	/*! Calculate the rotation axis between two points
		@param v1	current direction
		@param v2	desired direction
		@ret axis	rotation axis
	*/
	static Vec3 rotaxis(const Vec3 &v1, const Vec3 &v2) {
		Vec3 vv1(v1), vv2(v2);
		normalize(vv1); normalize(vv2);
		return (vv2-vv1)*0.5 + vv1;
	}
	

	/*! Minimum
	*/
	static Vec3 vmin(const Vec3 &v1, const Vec3 &v2) {
		Vec3 rv;
		rv.x = (v1.x > v2.x) ? v2.x : v1.x;
		rv.y = (v1.y > v2.y) ? v2.y : v1.y;
		rv.z = (v1.z > v2.z) ? v2.z : v1.z;
		return rv;
	}

	/*! Maximum
	*/
	static Vec3 vmax(const Vec3 &v1, const Vec3 &v2) {
		Vec3 rv;
		rv.x = (v1.x < v2.x) ? v2.x : v1.x;
		rv.y = (v1.y < v2.y) ? v2.y : v1.y;
		rv.z = (v1.z < v2.z) ? v2.z : v1.z;
		return rv;
	}
	
	/*! Modulo
	*/
	static Vec3 mod(const Vec3 &v, T m) {
		Vec3 rv;
		rv.x = fmod(v.x, m);
		rv.y = fmod(v.y, m);
		rv.z = fmod(v.z, m);
		return rv;
	}
	
	/*! Quantize
	 */
	static Vec3 quantize(const Vec3 &v, T q) {
		Vec3 rv;
		rv.x = (v.x < 0.) ? (v.x - (q + fmod(v.x, q))) : (v.x - fmod(v.x, q));
		rv.y = (v.y < 0.) ? (v.y - (q + fmod(v.y, q))) : (v.y - fmod(v.y, q));
		rv.z = (v.z < 0.) ? (v.z - (q + fmod(v.z, q))) : (v.z - fmod(v.z, q));
		return rv;
	}
	
	static T angle(const Vec3 &v1, const Vec3 &v2) {
		return acos(dot(v1, v2));
	}

	static Vec3 slerp(const Vec3 &v1, const Vec3 &v2, T amt) {
		T theta = Vec3::angle(v1, v2);
		T c1 = sin((1-amt)*theta)/sin(theta);
		T c2 = sin(amt*theta)/sin(theta);
		
		return v1*c1 + v2*c2;
	}
	
	static Vec3 line_intersect_2d(Vec3 &p1, Vec3 &p2, Vec3 &p3, Vec3 &p4) {
		Vec3 v;
		
		float p1p2 = p1.x*p2.y - p1.y*p2.x;
		float p3p4 = p3.x*p4.y - p3.y*p4.x;
		float denom = (p1.x - p2.x)*(p3.y - p4.y) - (p1.y - p2.y)*(p3.x - p4.x);
		
		v.x =	p1p2*(p3.x - p4.x) - (p1.x - p2.x)*p3p4;		
		v.y =	p1p2*(p3.y - p4.y) - (p1.y - p2.y)*p3p4;
	
		v.x /= denom;
		v.y /= denom;
		
		v.z = 0.;
		
		return v;
	}
	
	static void axis_angle(T &angle, Vec3 &axis, const Vec3 &v1, const Vec3 &v2) {
		// get the normal to the plane (i.e. the unit bivector containing the v1 and v2)
		Vec3::cross(axis, v1, v2);
		Vec3::normalize(axis);

		// calculate the angle between v1 and v2
		T dotmag = Vec3::dot(v1, v2);
		angle = VEC3_RAD2DEG*acos(dotmag);
	}
};

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;

}	// muro::

#endif	// SPACE_VEC3_H
