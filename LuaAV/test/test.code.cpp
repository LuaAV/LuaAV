extern "C" {

#include "lua.h"
#include "lauxlib.h"

}

#include <stdio.h>

template <typename T>
struct Vec3{
public:
	T x;
	T y;
	T z;
	
	Vec3(T x=0, T y=0, T z=0)
	:	x(x), y(y), z(z)
	{}
		
	/*! Negation */
	const Vec3 operator- () const {
		return Vec3(-x, -y, -z);
	}

	/*! Addition */
	const Vec3 operator+ (const Vec3 &v) const {
		return Vec3(x+v.x, y+v.y, z+v.z);
	}
	
	const Vec3 operator+ (char s) const {
		return Vec3(x+s, y+s, z+s);
	}
	
	const Vec3 operator+ (long s) const {
		return Vec3(x+s, y+s, z+s);
	}
	
	const Vec3 operator+ (float s) const {
		return Vec3(x+s, y+s, z+s);
	}
	
	const Vec3 operator+ (double s) const {
		return Vec3(x+s, y+s, z+s);
	}
	
	
	

	/*! Subtraction */
	const Vec3 operator- (const Vec3 &v) const {
		return Vec3(x-v.x, y-v.y, z-v.z);
	}
	
	const Vec3 operator- (char s) const {
		return Vec3(x-s, y-s, z-s);
	}
	
	const Vec3 operator- (long s) const {
		return Vec3(x-s, y-s, z-s);
	}
	
	const Vec3 operator- (float s) const {
		return Vec3(x-s, y-s, z-s);
	}
	
	const Vec3 operator- (double s) const {
		return Vec3(x-s, y-s, z-s);
	}
	
	
	
	

	/*! Multiplication */
	const Vec3 operator* (const Vec3 &v) const {
		return Vec3(x*v.x, y*v.y, z*v.z);
	}
	
	const Vec3 operator* (char s) const {
		return Vec3(x*s, y*s, z*s);
	}
	
	const Vec3 operator* (long s) const {
		return Vec3(x*s, y*s, z*s);
	}
	
	const Vec3 operator* (float s) const {
		return Vec3(x*s, y*s, z*s);
	}
	
	const Vec3 operator* (double s) const {
		return Vec3(x*s, y*s, z*s);
	}



	/*! Divide */
	const Vec3 operator/ (const Vec3 &v) const {
		return Vec3(x/v.x, y/v.y, z/v.z);
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

	/*! In-place Multiply */
	Vec3 &operator*= (Vec3 v) {
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}
	
	/*! In-place Divide */
	const Vec3 operator/= (Vec3 v) const {
		x = x/v.x; y = y/v.y; z = z/v.z;
		return *this;
	}
};

template <typename T>
const Vec3<T> operator+ (float s, const Vec3<T> &v) {
	return Vec3<T>(v.x+s, v.y+s, v.z+s);
}

template <typename T>
const Vec3<T> operator- (float s, const Vec3<T> &v) {
	return Vec3<T>(s-v.x, s-v.y, s-v.z);
}

template <typename T>
const Vec3<T> operator* (float s, const Vec3<T> &v) {
	return Vec3<T>(v.x*s, v.y*s, v.z*s);
}

template <typename T>
const Vec3<T> operator/ (float s, const Vec3<T> &v) {
	return Vec3<T>(s/v.x, s/v.y, s/v.z);
}

template <typename T>
void swizzle(Vec3<T> &vout, const Vec3<T> &vin, int n, int *outmask, int *inmask) {
	for(int i=0; i < n; i++) {
		(&vout.x)[ outmask[i] ] = (&vin.x)[ inmask[i] ];
	}
}

template <typename T, unsigned N>
struct VecN{
public:
	T v[N];
	
	VecN() {}
	
	VecN(T *vv) {
		for(unsigned int i=0; i < N; i++) {
			v[i] = vv[i];
		}
	}
	
	VecN(const VecN &vn) {
		for(unsigned int i=0; i < N; i++) {
			v[i] = vn.v[i];
		}
	}
	
	/*! Negation */
	const VecN operator- () const {
		VecN res;
		for(unsigned int i=0; i < N; i++) {
			res.v[i] = -v[i];
		}
		return res;
	}

	/*! Addition */
	const VecN operator+ (const VecN &vn) const {
		VecN res;
		for(unsigned int i=0; i < N; i++) {
			res.v[i] = v[i]+vn.v[i];
		}
		return res;
	}
	
	/*! Subtraction */
	const VecN operator- (const VecN &vn) const {
		VecN res;
		for(unsigned int i=0; i < N; i++) {
			res.v[i] = v[i]-vn.v[i];
		}
		return res;
	}
	
	/*! Multiplication */
	const VecN operator* (const VecN &vn) const {
		VecN res;
		for(unsigned int i=0; i < N; i++) {
			res.v[i] = v[i]*vn.v[i];
		}
		return res;
	}
	
	/*! Division */
	const VecN operator/ (const VecN &vn) const {
		VecN res;
		for(unsigned int i=0; i < N; i++) {
			res.v[i] = v[i]/vn.v[i];
		}
		return res;
	}
};

template <typename T, unsigned N>
void swizzle(VecN<T, N> &vout, const VecN<T, N> &vin, int n, int *outmask, int *inmask) {
	for(int i=0; i < n; i++) {
		vout.v[ outmask[i] ] = vin.v[ inmask[i] ];
	}
}

extern "C" {

/* out0 = 1.4*in0*in0+2; */
#define MAT(x)	(*(x))

int test(lua_State *L) {
	float data[12] = {0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11.};
	float out_data[12];
	
	VecN<float, 6> *v_in = (VecN<float, 6> *)data;
	VecN<float, 6> *v_out = (VecN<float, 6> *)out_data;
	for(int i=0; i < 2; i++) {
		int oo[] = {1, 0};
		int ii[] = {0, 1};
		MAT(v_out) = MAT(v_in)+MAT(v_in);
		swizzle(MAT(v_out), MAT(v_in), 2, oo, ii);
		v_in++;
		v_out++;
	}
	
	for(int j=0; j < 12; j++) {
		printf("j: %f\n", out_data[j]);
	}
	
	/*
	Vec3<char> v1c(64, 123, 5);
	v1c = v1c*0.5;
	printf("t: %d %d %d\n", v1c.x, v1c.y, v1c.z);
	*/
	return 0;
}

}