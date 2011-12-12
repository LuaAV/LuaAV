#ifndef INCLUDE_AL_MATRIX4_HPP
#define INCLUDE_AL_MATRIX4_HPP

#include "allocore/math/al_Constants.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/math/al_Vec.hpp"

namespace al {

template<class T> class Matrix4;

/*
	The constructor will map into memory locations as follows:

	Matrix4(arg1, arg2, arg3, ...)
		
	arg1 ->m[0]	arg2 ->m[4]	arg3 ->m[8]		arg4 ->m[12]
	arg5 ->m[1]	arg6 ->m[5]	arg7 ->m[9]		arg8 ->m[13]
	arg9 ->m[2]	arg10->m[6]	arg11->m[10]	arg12->m[14]
	arg13->m[3]	arg14->m[7]	arg15->m[11]	arg16->m[15]	
	
	Matrix4(r1c1, r1c2, r1c3, r1c4, 
			r2c1, r2c2, r2c3, r2c4, 
			r3c1, r3c2, r3c3, r3c4, 
			r4c1, r4c2, r4c3, r4c4)
*/

/// 4x4 Matrix (Homogenous Transform)
template<typename T=double>
class Matrix4 : public Mat<4, T> {	
public:
	typedef Mat<4, T> Base;

	Matrix4()
	: Base(
		1, 0, 0, 0, 
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	)
	{}

	Matrix4(
		const T& r1c1, const T& r1c2, const T& r1c3, const T& r1c4,
		const T& r2c1, const T& r2c2, const T& r2c3, const T& r2c4,
		const T& r3c1, const T& r3c2, const T& r3c3, const T& r3c4,
		const T& r4c1, const T& r4c2, const T& r4c3, const T& r4c4
	)
	:	Base(
			r1c1, r1c2, r1c3, r1c4,
			r2c1, r2c2, r2c3, r2c4,
			r3c1, r3c2, r3c3, r3c4,
			r4c1, r4c2, r4c3, r4c4
		)
	{}
	
	Matrix4(
		const Vec<3,T>& xaxis,
		const Vec<3,T>& yaxis,
		const Vec<3,T>& zaxis,
		const Vec<3,T>& position
	)
	:	Base(
			xaxis[0], yaxis[0], zaxis[0], position[0],
			xaxis[1], yaxis[1], zaxis[1], position[1],
			xaxis[2], yaxis[2], zaxis[2], position[2],
			0, 0, 0, 1
		)
	{}
	
	Matrix4(const T * src)
	:	Base(src)
	{}
	
	Matrix4(const Base & src)
	:	Base(src)
	{}
	
	Matrix4& set(const Base & src) { Base::set(src.elems); return *this; }
	
	Quat<T>& toQuat() {
		Quat<T> q;
		q.fromMatrix(Base::elems);
		return q;
	}
	Matrix4& fromQuat(Quat<T>& q) { q.toMatrix(Base::elems); return *this; }
	Matrix4& fromQuatTransposed(Quat<T>& q) { q.toMatrixTransposed(Base::elems); return *this; }
		
	static const Matrix4 identity() {
		return Matrix4(
			1,	0,	0,	0, 
			0,	1,	0,	0, 
			0,	0,	1,	0, 
			0,	0,	0,	1
		);
	}
	
	static const Matrix4 translate(T x, T y, T z) {
		return Matrix4(
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1
		);
	}	
	
	static const Matrix4 scale(T x, T y, T z) {
		return Matrix4(
			x,	0,	0,	0,
			0,	y,	0,	0, 
			0,	0,	z,	0, 
			0,	0,	0,	1
		);
	}
	
	static const Matrix4 rotateYZ(T theta) {
		const T C = cos(theta); 
		const T S = sin(theta);
		return Matrix4(	1, 0, 0, 0, 
						0, C,-S, 0, 
						0, S, C, 0, 
						0, 0, 0, 1);
	}
	static const Matrix4 rotateZX(T theta) {
		const T C = cos(theta); 
		const T S = sin(theta);
		return Matrix4(	C, 0, S, 0, 
						0, 1, 0, 0, 
						-S,0, C, 0, 
						0, 0, 0, 1);
	}
	static const Matrix4 rotateXY(T theta) {
		const T C = cos(theta); 
		const T S = sin(theta);
		return Matrix4(	C,-S, 0, 0, 
						S, C, 0, 0, 
						0, 0, 1, 0, 
						0, 0, 0, 1);
	}

	static const Matrix4 rotate(float angle, const Vec<3, T> &v) {
		Vec<3, T> axis(v);
		axis.normalize();
		
		float c = cos(angle*M_DEG2RAD);
		float s = sin(angle*M_DEG2RAD);
			
		Matrix4 m(
			axis[0]*axis[0]*(1-c)+c,
			axis[1]*axis[0]*(1-c)+axis[2]*s,
			axis[0]*axis[2]*(1-c)-axis[1]*s,
			0,
			
			axis[0]*axis[1]*(1-c)-axis[2]*s,
			axis[1]*axis[1]*(1-c)+c,
			axis[1]*axis[2]*(1-c)+axis[0]*s,
			0,

			axis[0]*axis[2]*(1-c)+axis[1]*s,
			axis[1]*axis[2]*(1-c)-axis[0]*s,
			axis[2]*axis[2]*(1-c)+c,
			0,
			
			0, 0, 0, 1
		);
		
		return m;
	}
	
	static const Matrix4 shearYZ(T y, T z) {
		return Matrix4(	1,	0,	0,	0,
						y,	1,	0,	0,
						z,	0,	1,	0,
						0,	0,	0,	1	);
	}
	static const Matrix4 shearZX(T z, T x) {
		return Matrix4(	1,	x,	0,	0,
						0,	1,	0,	0,
						0,	z,	1,	0,
						0,	0,	0,	1	);
	}
	static const Matrix4 shearXY(T x, T y) {
		return Matrix4(	1,	0,	x,	0,
						0,	1,	y,	0,
						0,	0,	1,	0,
						0,	0,	0,	1	);
	}
		
	static const Matrix4 perspective(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		const T n2 = n*2;
		const T fn2 = f*n2;
		return Matrix4(	n2/W,	0,		W2/W,		0, 
						0,		n2/H,	H2/H,		0, 
						0,		0,		-D2/D,	-fn2/D,
						0,		0,		-1,			0 );
	}
	
	static const Matrix4 perspective(T fovy, T aspect, T near, T far) {
		float f = 1/tan(fovy*M_DEG2RAD/2.);
		return Matrix4(
			f/aspect,	0, 0,						0,
			0,			f, 0,						0,
			0,			0, (far+near)/(near-far),	(2*far*near)/(near-far),
			0,			0, -1,						0
		);
	}
	
	// for stereographics:
	static const Matrix4 perspectiveLeft(T fovy, T aspect, T near, T far, T eyeSep, T focal) {
		return perspectiveOffAxis(fovy, aspect, near, far,-eyeSep, focal);
	}
	static const Matrix4 perspectiveRight(T fovy, T aspect, T near, T far, T eyeSep, T focal) {
		return perspectiveOffAxis(fovy, aspect, near, far, eyeSep, focal);
	}
	static const Matrix4 perspectiveOffAxis(T fovy, T aspect, T near, T far, T eyeShift, T focal) {
		T shift = -0.5*eyeShift*near/focal;
		T top = near * tan(fovy*M_DEG2RAD*0.5);	// height of view at distance = near
		T bottom = -top;
		T left = -aspect*top + shift;
		T right = aspect*top + shift;
		return perspective(left, right, bottom, top, near, far);
	}
	
	static const Matrix4 unPerspective(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		const T n2 = n*2;
		const T fn2 = f*n2;
		return Matrix4(	W/n2,	0,		0,		W2/n2,
						0,		H/n2,	0,		H2/n2,
						0,		0,		0,		-1,
						0,		0,		-D/fn2,	D2/fn2	);
	}
	
	static const Matrix4 ortho(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		return Matrix4(	2/W,	0,		0,		-W2/W,
						0,		2/H,	0,		-H2/H,
						0,		0,		-2/D,	-D2/D,
						0,		0,		0,		1	);
	}
	
	static const Matrix4 unOrtho(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		return Matrix4(	W/2,	0,		0,		W2/2,
						0,		H/2,	0,		H2/2,
						0,		0,		D/-2,	D2/2,
						0,		0,		0,		1	);
	}
	
	static const Matrix4 lookAt(const Vec<3,T>& ux, const Vec<3,T>& uy, const Vec<3,T>& uz, const Vec<3,T>& pos) {
		return Matrix4(
			 ux[0], ux[1], ux[2], -(ux.dot(pos)),
			 uy[0], uy[1], uy[2], -(uy.dot(pos)),
			-uz[0],-uz[1],-uz[2],  (uz.dot(pos)),
			0, 0, 0, 1
		);
	}
	
	static const Matrix4 lookAt(const Vec<3,T>& eye, const Vec<3,T>& at, const Vec<3,T>& up) {
		Vec<3,T> z = (at - eye).normalize();
		Vec<3,T> x = cross(up, z);
		Vec<3,T> y = cross(z, x);
		return lookAt(x, y, z, eye);
	}
	
	// for stereographics:
	static const Matrix4 lookAtLeft(const Vec<3,T>& ux, const Vec<3,T>& uy, const Vec<3,T>& uz, const Vec<3,T>& pos, double eyeSep) {
		return lookAtOffAxis(ux,uy,uz, pos,-eyeSep);
	}
	static const Matrix4 lookAtRight(const Vec<3,T>& ux, const Vec<3,T>& uy, const Vec<3,T>& uz, const Vec<3,T>& pos, double eyeSep) {
		return lookAtOffAxis(ux,uy,uz, pos, eyeSep);
	}
	static const Matrix4 lookAtOffAxis(const Vec<3,T>& ux, const Vec<3,T>& uy, const Vec<3,T>& uz, const Vec<3,T>& pos, double eyeShift){
		return lookAt(ux, uy, uz, pos + (ux * eyeShift));
	}
};

typedef Matrix4<double>	Matrix4d;	///< Double-precision quaternion
typedef Matrix4<float>	Matrix4f;	///< Single-precision quaternion

} // al::

#endif /* include guard */
