#ifndef INCLUDE_AL_MATRIX4_HPP
#define INCLUDE_AL_MATRIX4_HPP

#include "math/al_Constants.hpp"
#include "math/al_Quat.hpp"
#include "math/al_Vec.hpp"

namespace al {

template<class T> class Matrix4;

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
		const T& v11, const T& v21, const T& v31, const T& v41,
		const T& v12, const T& v22, const T& v32, const T& v42,
		const T& v13, const T& v23, const T& v33, const T& v43,
		const T& v14, const T& v24, const T& v34, const T& v44
	)
	:	Base(
			v11, v21, v31, v41,
			v12, v22, v32, v42,
			v13, v23, v33, v43,
			v14, v24, v34, v44
		)
	{}
	
	Matrix4(
		const Vec3<T>& xaxis,
		const Vec3<T>& yaxis,
		const Vec3<T>& zaxis,
		const Vec3<T>& position
	)
	:	Base(
			xaxis[0], xaxis[1], xaxis[2], 0,
			yaxis[0], yaxis[1], yaxis[2], 0,
			zaxis[0], zaxis[1], zaxis[2], 0,
			position[0], position[1], position[2], 1
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
	void fromQuat(Quat<T>& q) { q.toMatrix(Base::elems); }
		
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
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1
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
		return Matrix4(	1,	0,	0,	0, 
						0,	C,	S,	0, 
						0,	-S,	C,	0, 
						0,	0,	0,	1	);
	}
	static const Matrix4 rotateZX(T theta) {
		const T C = cos(theta); 
		const T S = sin(theta);
		return Matrix4(	C,	0,	-S,	0, 
						0,	1,	0,	0, 
						S,	0,	C,	0, 
						0,	0,	0,	1 );
	}
	static const Matrix4 rotateXY(T theta) {
		const T C = cos(theta); 
		const T S = sin(theta);
		return Matrix4(	C,	S,	0,	0, 
						-S,	C,	0,	0, 
						0,	0,	1,	0, 
						0,	0,	0,	1	);
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
		return Matrix4(	1,	y,	z,	0,
						0,	1,	0,	0,
						0,	0,	1,	0,
						0,	0,	0,	1	);
	}
	static const Matrix4 shearZX(T z, T x) {
		return Matrix4(	1,	0,	0,	0,
						x,	1,	z,	0,
						0,	0,	1,	0,
						0,	0,	0,	1	);
	}
	static const Matrix4 shearXY(T x, T y) {
		return Matrix4(	1,	0,	0,	0,
						0,	1,	0,	0,
						x,	y,	1,	0,
						0,	0,	0,	1	);
	}
		
	static const Matrix4 perspective(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		const T n2 = n*2;
		const T fn2 = f*n2;
		return Matrix4(	n2/W,	0,		0,		0, 
						0,		n2/H,	0,		0, 
						W2/W,	H2/H,	-D2/D,	-1,
						0,		0,		-fn2/D,	0 );
	}
	
	static const Matrix4 perspective(T fovy, T aspect, T near, T far) {
		float f = 1/tan(fovy*M_DEG2RAD/2.);
		return Matrix4(
			f/aspect, 0, 0, 0,
			0, f, 0, 0,
			0, 0, (far+near)/(near-far), -1,
			0, 0, (2*far*near)/(near-far), 0
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
		return Matrix4(	W/n2,	0,		0,		0,
						0,		H/n2,	0,		0,
						0,		0,		0,		-D/fn2,
						W2/n2,	H2/n2,	-1,		D2/fn2	);
	}
	
	static const Matrix4 ortho(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		return Matrix4(	2/W,	0,		0,		0,
						0,		2/H,	0,		0,
						0,		0,		-2/D,	0,
						-W2/W,	-H2/H,	-D2/D,	1	);
	}
	
	static const Matrix4 unOrtho(T l, T r, T b, T t, T n, T f) {
		const T W = r-l;	const T W2 = r+l;
		const T H = t-b;	const T H2 = t+b;
		const T D = f-n;	const T D2 = f+n;
		return Matrix4(	W/2,	0,		0,		0,
						0,		H/2,	0,		0,
						0,		0,		D/-2,	0,
						W2/2,	H2/2,	D2/2,	1	);
	}
	
	static const Matrix4 lookAt(const Vec3<T>& ux, const Vec3<T>& uy, const Vec3<T>& uz, const Vec3<T>& pos) {
		return Matrix4(
			ux[0], uy[0], -uz[0], 0,
			ux[1], uy[1], -uz[1], 0,
			ux[2], uy[2], -uz[2], 0,
			-(ux.dot(pos)), -(uy.dot(pos)), (uz.dot(pos)), 1
		);
	}
	
	// for stereographics:
	static const Matrix4 lookAtLeft(const Vec3<T>& ux, const Vec3<T>& uy, const Vec3<T>& uz, const Vec3<T>& pos, double eyeSep) {
		return lookAtOffAxis(ux,uy,uz, pos,-eyeSep);
	}
	static const Matrix4 lookAtRight(const Vec3<T>& ux, const Vec3<T>& uy, const Vec3<T>& uz, const Vec3<T>& pos, double eyeSep) {
		return lookAtOffAxis(ux,uy,uz, pos, eyeSep);
	}
	static const Matrix4 lookAtOffAxis(const Vec3<T>& ux, const Vec3<T>& uy, const Vec3<T>& uz, const Vec3<T>& pos, double eyeShift){
		return lookAt(ux, uy, uz, pos + (ux * eyeShift));
	}
};

typedef Matrix4<double>	Matrix4d;	///< Double-precision quaternion
typedef Matrix4<float>	Matrix4f;	///< Single-precision quaternion

} // al::

#endif /* include guard */
