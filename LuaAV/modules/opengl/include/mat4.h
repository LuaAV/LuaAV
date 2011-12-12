#ifndef SPACE_MAT4_H
#define SPACE_MAT4_H 1

#include "vec4.h"

/*
	This class is not optimized for SIMD as it needs to be
*/

namespace space {

#ifndef PI
#define PI       3.14159265358979323846
#endif

#define DEG_TO_RAD (PI/180.0)

template<typename T = double>
class Mat4 {
public:
	Vec4<T> col[4];

    Mat4() {
		col[0].set(1., 0., 0., 0.);
		col[1].set(0., 1., 0., 0.);
		col[2].set(0., 0., 1., 0.);
		col[3].set(0., 0., 0., 1.);
	}
	
	Mat4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	) {
		col[0].set(m00, m01, m02, m03);
		col[1].set(m10, m11, m12, m13);
		col[2].set(m20, m21, m22, m23);
		col[3].set(m30, m31, m32, m33);
	}
	
	Mat4(const Mat4 &m) {
		col[0].set(m.col[0]);
		col[1].set(m.col[1]);
		col[2].set(m.col[2]);
		col[3].set(m.col[3]);
	}
	
	Mat4& operator= (const Mat4& m) {
		col[0].set(m.col[0]);
		col[1].set(m.col[1]);
		col[2].set(m.col[2]);
		col[3].set(m.col[3]);
		return *this;
	}

	void identity() {
		col[0].set(1., 0., 0., 0.);
		col[1].set(0., 1., 0., 0.);
		col[2].set(0., 0., 1., 0.);
		col[3].set(0., 0., 0., 1.);
	}
	
	void setall(T v) {
		col[0].set(v, v, v, v);
		col[1].set(v, v, v, v);
		col[2].set(v, v, v, v);
		col[3].set(v, v, v, v);
	}
	
	void set(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	) {
		col[0].set(m00, m01, m02, m03);
		col[1].set(m10, m11, m12, m13);
		col[2].set(m20, m21, m22, m23);
		col[3].set(m30, m31, m32, m33);
	}
	
	T * ptr() {return &(col[0].x);}

	void mul(Mat4<T> &m2) {
		Mat4 m1(*this);
		col[0].x = m1.col[0].x*m2.col[0].x + m1.col[1].x*m2.col[0].y + m1.col[2].x*m2.col[0].z + m1.col[3].x*m2.col[0].w;
		col[0].y = m1.col[0].y*m2.col[0].x + m1.col[1].y*m2.col[0].y + m1.col[2].y*m2.col[0].z + m1.col[3].y*m2.col[0].w;
		col[0].z = m1.col[0].z*m2.col[0].x + m1.col[1].z*m2.col[0].y + m1.col[2].z*m2.col[0].z + m1.col[3].z*m2.col[0].w;
		col[0].w = m1.col[0].w*m2.col[0].x + m1.col[1].w*m2.col[0].y + m1.col[2].w*m2.col[0].z + m1.col[3].w*m2.col[0].w;
		
		col[1].x = m1.col[0].x*m2.col[1].x + m1.col[1].x*m2.col[1].y + m1.col[2].x*m2.col[1].z + m1.col[3].x*m2.col[1].w;
		col[1].y = m1.col[0].y*m2.col[1].x + m1.col[1].y*m2.col[1].y + m1.col[2].y*m2.col[1].z + m1.col[3].y*m2.col[1].w;
		col[1].z = m1.col[0].z*m2.col[1].x + m1.col[1].z*m2.col[1].y + m1.col[2].z*m2.col[1].z + m1.col[3].z*m2.col[1].w;
		col[1].w = m1.col[0].w*m2.col[1].x + m1.col[1].w*m2.col[1].y + m1.col[2].w*m2.col[1].z + m1.col[3].w*m2.col[1].w;
		
		col[2].x = m1.col[0].x*m2.col[2].x + m1.col[1].x*m2.col[2].y + m1.col[2].x*m2.col[2].z + m1.col[3].x*m2.col[2].w;
		col[2].y = m1.col[0].y*m2.col[2].x + m1.col[1].y*m2.col[2].y + m1.col[2].y*m2.col[2].z + m1.col[3].y*m2.col[2].w;
		col[2].z = m1.col[0].z*m2.col[2].x + m1.col[1].z*m2.col[2].y + m1.col[2].z*m2.col[2].z + m1.col[3].z*m2.col[2].w;
		col[2].w = m1.col[0].w*m2.col[2].x + m1.col[1].w*m2.col[2].y + m1.col[2].w*m2.col[2].z + m1.col[3].w*m2.col[2].w;
		
		col[3].x = m1.col[0].x*m2.col[3].x + m1.col[1].x*m2.col[3].y + m1.col[2].x*m2.col[3].z + m1.col[3].x*m2.col[3].w;
		col[3].y = m1.col[0].y*m2.col[3].x + m1.col[1].y*m2.col[3].y + m1.col[2].y*m2.col[3].z + m1.col[3].y*m2.col[3].w;
		col[3].z = m1.col[0].z*m2.col[3].x + m1.col[1].z*m2.col[3].y + m1.col[2].z*m2.col[3].z + m1.col[3].z*m2.col[3].w;
		col[3].w = m1.col[0].w*m2.col[3].x + m1.col[1].w*m2.col[3].y + m1.col[2].w*m2.col[3].z + m1.col[3].w*m2.col[3].w;
	}

	void translate(Vec3<T> &v) {
//		identity();
		col[3].x += v.x;
		col[3].y += v.y;
		col[3].z += v.z;
	}
	
	void rotate(T angle, Vec3<T> &axis) {
//		identity();
	
		Vec3<T>::normalize(axis);
		T c = cos(angle*DEG_TO_RAD);
		T s = sin(angle*DEG_TO_RAD);
		
		Mat4 rot;
		rot.col[0].x = axis.x*axis.x*(1-c)+c;
		rot.col[0].y = axis.y*axis.x*(1-c)+axis.z*s;
		rot.col[0].z = axis.x*axis.z*(1-c)-axis.y*s;
		
		rot.col[1].x = axis.x*axis.y*(1-c)-axis.z*s;
		rot.col[1].y = axis.y*axis.y*(1-c)+c;
		rot.col[1].z = axis.y*axis.z*(1-c)+axis.x*s;

		rot.col[2].x = axis.x*axis.z*(1-c)+axis.y*s;
		rot.col[2].y = axis.y*axis.z*(1-c)-axis.x*s;
		rot.col[2].z = axis.z*axis.z*(1-c)+c;
		
		mul(rot);
	}
	
	void scale(Vec3<T> &v) {
//		identity();
	
		col[0].x = v.x;
		col[1].y = v.y;
		col[2].z = v.z;
	}
	
	void perspective(T fovy, T aspect, T nearr, T farr) {
		setall(0.);
	
		T f = 1/tan(fovy*DEG_TO_RAD/2.);
		col[0].x = f/aspect;
		col[1].y = f;
		col[2].z = (farr+nearr)/(nearr-farr);
		col[2].w = -1.;
		col[3].z = (2*farr*nearr)/(nearr-farr);
	}
	
	void lookat(Vec3<T> &eye, Vec3<T> &look, Vec3<T> &up) {
		Vec3<T> f;
		Vec3<T>::sub(f, look, eye);
		Vec3<T>::normalize(f);
		
		Vec3<T>::normalize(up);
		
		Vec3<T> s;
		Vec3<T>::cross(s, f, up);
		
		Vec3<T> u;
		Vec3<T>::cross(u, s, f);
		
		col[0].x = s.x;
		col[0].y = u.x;
		col[0].z = -f.x;
		col[0].w = 0.;
		
		col[1].x = s.y;
		col[1].y = u.y;
		col[1].z = -f.y;
		col[1].w = 0.;
		
		col[2].x = s.z;
		col[2].y = u.z;
		col[2].z = -f.z;
		col[2].w = 0.;
		
		col[3].x = 0.;
		col[3].y = 0.;
		col[3].z = 0.;
		col[3].w = 1.;
	}
	
	void ortho(T left, T right, T bot, T top, T nearr, T farr) {
		setall(0.);
		
		col[0].x = 2./(right-left);
		col[1].y = 2./(top-bot);
		col[2].z = -2./(farr-nearr);

		col[3].x = -(right+left)/(right-left);
		col[3].y = -(top+bot)/(top-bot);
		col[3].z = -(farr+nearr)/(farr-nearr);
		col[3].w = 1.;
	}
};

}	// space::

#endif	// SPACE_MAT4_H
