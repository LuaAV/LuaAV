#ifndef INCLUDE_AL_VEC_HPP
#define INCLUDE_AL_VEC_HPP

/*
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <cmath>

namespace al {


template <int N, class T> class Vec;
template <class T> class Vec3;
template <class T> class Vec4;
template <int N, class T> class Mat;

typedef Vec<2,float>	Vec2f;
typedef Vec<2,double>	Vec2d;
typedef Vec<2,int>		Vec2i;
typedef Vec3<float>		Vec3f;
typedef Vec3<double>	Vec3d;
typedef Vec3<int>		Vec3i;
typedef Vec4<float>		Vec4f;
typedef Vec4<double>	Vec4d;
typedef Vec4<int>		Vec4i;
typedef Mat<3,float>	Mat3f;
typedef Mat<3,double>	Mat3d;
typedef Mat<4,float>	Mat4f;
typedef Mat<4,double>	Mat4d;
typedef Mat<4,int>		Mat4i;

// Forward iterates from 0 to n-1. Current index is 'i'.
#define IT(n) for(int i=0; i<n; ++i)



///< N-vector

/// This is a fixed size array to enable better loop unrolling optimizations
/// by the compiler and to avoid an extra 'size' data member for small-sized
/// arrays.
template <int N, class T>
class Vec{
public:

	typedef T value_type; 

	/// Element values
	T elems[N];

	/// @param[in] v		value to initialize all elements to
	Vec(const T& v=T()){ set(v); }
	
	/// @param[in] v1		value to initialize first element
	/// @param[in] v2		value to initialize second element
	Vec(const T& v1, const T& v2){ set(v1, v2); }
	
	/// @param[in] v1		value to initialize first element
	/// @param[in] v2		value to initialize second element
	/// @param[in] v3		value to initialize third element
	Vec(const T& v1, const T& v2, const T& v3){ set(v1, v2, v3); }
	
	/// @param[in] v1		value to initialize first element
	/// @param[in] v2		value to initialize second element
	/// @param[in] v3		value to initialize third element
	/// @param[in] v4		value to initialize fourth element
	Vec(const T& v1, const T& v2, const T& v3, const T& v4){ set(v1, v2, v3, v4); }

	/// @param[in] v		vector to initialize all elements to
	template <class T2>
	Vec(const Vec<N, T2>& v){ set(v); }

	/// @param[in] v		pointer to array to initialize all elements to
	/// @param[in] stride	stride factor through array
	template <class T2>
	Vec(const T2 * v, int stride=1){ set(v,stride); }

	
	/// Set element at index with no bounds checking
	T& operator[](int i){ return elems[i];}
	
	/// Get element at index with no bounds checking
	const T& operator[](int i) const { return elems[i]; }

	/// Return true if objects are element-wise equal, false otherwise
	bool operator ==(const Vec& v) const { IT(N){ if((*this)[i] != v[i]) return false; } return true; }
	
	/// Return true if all elements are equal to value, false otherwise
	bool operator ==(const T& v) const { IT(N){ if((*this)[i] != v   ) return false; } return true; }

	/// Return true if objects are not element-wise equal, false otherwise
	bool operator !=(const Vec& v) const { return !(*this == v); }
	
	/// Return true if all elements are not equal to value, false otherwise
	bool operator !=(const T& v) const { return !(*this == v); }
	
	Vec& operator  =(const Vec& v){ return set(v); }
	Vec& operator  =(const   T& v){ return set(v); }
	Vec& operator +=(const Vec& v){ IT(N) (*this)[i] += v[i]; return *this; }
	Vec& operator +=(const   T& v){ IT(N) (*this)[i] += v;    return *this; }
	Vec& operator -=(const Vec& v){ IT(N) (*this)[i] -= v[i]; return *this; }
	Vec& operator -=(const   T& v){ IT(N) (*this)[i] -= v;    return *this; }
	Vec& operator *=(const Vec& v){ IT(N) (*this)[i] *= v[i]; return *this; }
	Vec& operator *=(const   T& v){ IT(N) (*this)[i] *= v;    return *this; }
	Vec& operator /=(const Vec& v){ IT(N) (*this)[i] /= v[i]; return *this; }
	Vec& operator /=(const   T& v){ IT(N) (*this)[i] /= v;    return *this; }

	Vec operator + (const Vec& v) const { return Vec(*this) += v; }
	Vec operator + (const   T& v) const { return Vec(*this) += v; }
	Vec operator - (const Vec& v) const { return Vec(*this) -= v; }
	Vec operator - (const   T& v) const { return Vec(*this) -= v; }
	Vec operator * (const Vec& v) const { return Vec(*this) *= v; }
	Vec operator * (const   T& v) const { return Vec(*this) *= v; }
	Vec operator / (const Vec& v) const { return Vec(*this) /= v; }
	Vec operator / (const   T& v) const { return Vec(*this) /= v; }
	Vec operator - () const { return Vec(*this).negate(); }

	/// Returns dot (inner) product between vectors
	T dot(const Vec& v) const {
		T r = (*this)[0] * v[0];
		for(int i=1; i<N; ++i){ r += (*this)[i] * v[i]; }
		return r;
	}
	
	/// Returns magnitude
	T mag() const { return std::sqrt(magSqr()); }
	
	/// Returns magnitude squared
	T magSqr() const { return dot(*this); }
	
	/// Returns p-norm of elements
	
	/// The p-norm is pth root of the sum of the absolute value of the elements 
	/// raised to the pth, (sum |x_n|^p) ^ (1/p).
	T norm(const T& p) const {
		using namespace std;
		T r = pow(abs((*this)[0]), p);
		for(int i=1; i<N; ++i){ r += pow(abs((*this)[i]), p); }
		return pow(r, T(1)/p);		
	}

	/// Return 1-norm (sum) of elements
	T norm1() const { return sumAbs(); }
	
	/// Return 2-norm of elements
	T norm2() const { return mag(); }

	/// Returns product of elements
	T product() const {
		T r = (*this)[0];
		for(int i=1; i<N; ++i){ r *= (*this)[i]; }
		return r;
	}

	/// Returns closest vector on unit N-sphere
	Vec sgn() const { return Vec(*this).normalize(); }

	/// Get a subvector
	template <int M>
	Vec<M,T> sub(int begin=0){
		return Vec<M,T>(ptr()+begin);
	}

	/// Returns sum of elements
	T sum() const {
		T r = (*this)[0];
		for(int i=1; i<N; ++i){ r += (*this)[i]; }
		return r;
	}

	/// Returns sum of absolute value (1-norm) of elements
	T sumAbs() const {
		T r = abs((*this)[0]);
		for(int i=1; i<N; ++i){ r += abs((*this)[i]); }
		return r;
	}

	/// Negates all elements
	Vec& negate(){ IT(N){ (*this)[i] = -(*this)[i]; } return *this; }

	/// Scales elements evenly so magnitude is one
	Vec& normalize();
	
	/// linear interpolation
	void lerp(const Vec& target, T amt) { set(lerp(*this, target, amt)); }
	static Vec lerp(const Vec& input, const Vec& target, T amt) {
		return input+amt*(target-input);
	}

	/// Set elements from another vector
	template <class T2>
	Vec& set(const Vec<N, T2> &v){ IT(N){ (*this)[i] = T(v[i]); } return *this; }
	
	/// Set all elements to the same value
	Vec& set(const T& v){ IT(N){ (*this)[i] = v; } return *this; }

	/// Set elements from raw C-pointer
	template <class T2>
	Vec& set(const T2 * v){
		IT(N){ (*this)[i] = T(v[i]); }
		return *this;
	}
	
	/// Set elements from strided raw C-pointer
	template <class T2>
	Vec& set(const T2 * v, int stride){
		IT(N){ (*this)[i] = T(v[i*stride]); }
		return *this;
	}

	Vec& set(const T& v1, const T& v2){
		return set(v1,v2,v1,v1,v1,v1); }

	Vec& set(const T& v1, const T& v2, const T& v3){
		return set(v1,v2,v3,v1,v1,v1); }

	Vec& set(const T& v1, const T& v2, const T& v3, const T& v4){
		return set(v1,v2,v3,v4,v1,v1); }

	Vec& set(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5){
		return set(v1,v2,v3,v4,v5,v1); }

	Vec& set(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6){		
		switch(N){
		default:(*this)[5] = v6;
		case 5: (*this)[4] = v5;
		case 4: (*this)[3] = v4;
		case 3: (*this)[2] = v3;
		case 2: (*this)[1] = v2;
		case 1: (*this)[0] = v1;
		}
		return *this;
	}

	/// Set all elements to zero
	Vec& zero(){ return set(T(0)); }

	/// Get read-only pointer to elements
	const T* ptr() const { return elems; }
	
	/// Get read-write pointer to elements
	T* ptr(){ return elems; }
	

	/// Returns number of elements
	static int size(){ return N; }
	
};



// N x N matrix
template <int N, class T>
class Mat{
public:

	/// column-major array
	T elems[N*N];

	Mat(){ set(T(0)); }

	Mat(const T * arr){ set(arr); }

	Mat(
		const T& v11, const T& v21, const T& v31,
		const T& v12, const T& v22, const T& v32,
		const T& v13, const T& v23, const T& v33
	){
		set(v11,v21,v31, v12,v22,v32, v13,v23,v33);
	}

	Mat(
		const T& v11, const T& v21, const T& v31, const T& v41,
		const T& v12, const T& v22, const T& v32, const T& v42,
		const T& v13, const T& v23, const T& v33, const T& v43,
		const T& v14, const T& v24, const T& v34, const T& v44
	){
		set(v11,v21,v31,v41, v12,v22,v32,v42, v13,v23,v33,v43, v14,v24,v34,v44);
	}

	/// Set element at index with no bounds checking
	T& operator[](int i){ return elems[i];}
	
	/// Get element at index with no bounds checking
	const T& operator[](int i) const { return elems[i]; }

	/// Set element at row i, column j
	T& operator()(int i, int j){ return elems[j*N+i]; }
	
	/// Get element at row i, column j
	const T& operator()(int i, int j) const { return elems[j*N+i]; }

	/// Return column i as vector
	Vec<N,T> col(int i) const { return Vec<N,T>(elems + i*N); }

	/// Return row i as vector
	Vec<N,T> row(int i) const { return Vec<N,T>(elems+i, N); }
	
	/// Return diagonal
	Vec<N,T> diagonal() const { return Vec<N,T>(elems, N+1); }

	Mat& operator *= (const Mat& v){ return multiply(*this, Mat(*this),v); }
	Mat& operator += (const Mat& v){ IT(size()){ (*this)[i] += v[i]; } return *this; }
	Mat& operator -= (const Mat& v){ IT(size()){ (*this)[i] -= v[i]; } return *this; }
	Mat& operator += (const T& v){ IT(size()){ (*this)[i] += v; } return *this; }
	Mat& operator -= (const T& v){ IT(size()){ (*this)[i] -= v; } return *this; }
	Mat& operator *= (const T& v){ IT(size()){ (*this)[i] *= v; } return *this; }
	Mat& operator /= (const T& v){ IT(size()){ (*this)[i] /= v; } return *this; }

	Mat operator - () const { Mat r; IT(size()){ r[i]=-(*this)[i]; } return r; }
	Mat operator + (const Mat& v) const { return Mat(*this) += v; }
	Mat operator - (const Mat& v) const { return Mat(*this) -= v; }
	Mat operator * (const Mat& v) const { return Mat(*this) *= v; }
	Mat operator + (const T& v) const { return Mat(*this) += v; }
	Mat operator - (const T& v) const { return Mat(*this) -= v; }
	Mat operator * (const T& v) const { return Mat(*this) *= v; }
	Mat operator / (const T& v) const { return Mat(*this) /= v; }


	/// Set elements on diagonal to one and all others to zero
	Mat& identity(){
		IT(size()){ (*this)[i] = (i%(N+1)) ? T(0) : T(1); }
		return *this;
	}
	
	/// Set all elements to value
	Mat& set(const T& v){ IT(size()){ (*this)[i]=v; } return *this; }
	
	/// Set elements in column-major order from C array
	Mat& set(const T * arr){ IT(size()){ (*this)[i]=arr[i]; } return *this; }
	
	Mat& set(
		const T& v11, const T& v21, const T& v31,
		const T& v12, const T& v22, const T& v32,
		const T& v13, const T& v23, const T& v33
	){
		elems[0] = v11; elems[3] = v12; elems[6] = v13;
		elems[1] = v21; elems[4] = v22; elems[7] = v23;
		elems[2] = v31; elems[5] = v32; elems[8] = v33;
		return *this;
	}
	
	Mat& set(
		const T& v11, const T& v21, const T& v31, const T& v41,
		const T& v12, const T& v22, const T& v32, const T& v42,
		const T& v13, const T& v23, const T& v33, const T& v43,
		const T& v14, const T& v24, const T& v34, const T& v44
	){
		elems[0] = v11; elems[4] = v12; elems[8] = v13; elems[12] = v14;
		elems[1] = v21; elems[5] = v22; elems[9] = v23; elems[13] = v24;
		elems[2] = v31; elems[6] = v32; elems[10] = v33; elems[14] = v34;
		elems[3] = v41; elems[7] = v42; elems[11] = v43; elems[15] = v44;
		return *this;
	}
	
	Mat& transpose(){
		for(int j=0; j<N-1; ++j){		// row and column
		for(int i=j+1; i<N; ++i){	// offset into row or column
			int i1 = j*N + i;
			int i2 = j + i*N;
			T t = elems[i1];
			elems[i1] = elems[i2];
			elems[i2] = t;
		}} return *this;
	}


	/// Get trace (sum of diagonal elements)
	T trace() const { return diagonal().sum(); }

	/// Computes matrix product r = a * b
	
	/// Returns reference to result
	///
	static Mat& multiply(Mat& r, const Mat& a, const Mat& b){
		for(int j=0; j<N; ++j){
			const Vec<N,T>& bcol = b.col(j);
			for(int i=0; i<N; ++i){
				r(i,j) = a.row(i).dot(bcol);
			}
		}
		return r;
	}
	
	/// Computes product of matrix multiplied by column vector, r = m * vCol
	static Vec<N,T>& multiply(Vec<N,T>& r, const Mat& m, const Vec<N,T>& vCol){
		IT(N){ r[i] = m.row(i).dot(vCol); }
		return r;
	}

	/// Computes product of row vector multiplied by matrix, r = vRow * m
	static Vec<N,T>& multiply(Vec<N,T>& r, const Vec<N,T>& vRow, const Mat& m){
		IT(N){ r[i] = vRow.dot(m.col(i)); }
		return r;
	}
	
	/// Get read-only pointer to elements
	const T* ptr() const { return elems; }
	
	/// Get read-write pointer to elements
	T* ptr(){ return elems; }

	/// Returns total number of elements
	static int size(){ return N*N; }

};




// Non-member binary arithmetic operations

// Vec
template <int N, class T>
inline Vec<N,T> operator + (T s, const Vec<N,T>& v){ return  v+s; }

template <int N, class T>
inline Vec<N,T> operator - (T s, const Vec<N,T>& v){ return -v+s; }

template <int N, class T>
inline Vec<N,T> operator * (T s, const Vec<N,T>& v){ return  v*s; }

template <int N, class T>
inline Vec<N,T> operator / (T s, const Vec<N,T>& v){
	Vec<N,T> r; IT(N){ r[i] = s/v[i]; } return r;
}

//template <int N, class T, class F>
//inline Vec<N,T> binaryOp(const Vec<N,T>& a, const Vec<N,T>& b, const F& func){
//	Vec<N,T> r;
//	IT(N){ r[i] = func(a[0], b[0]); }
//	return r;
//}

//template <int N, class T>
//inline Vec<N,T> binaryOp(const Vec<N,T>& a, const Vec<N,T>& b, T (* const func)(const T&, const T&)){
//	Vec<N,T> r;
//	IT(N){ r[i] = func(a[0], b[0]); }
//	return r;
//}

//template <int N, class T>
//inline Mat<N,T> operator* (const Mat<N,T>& a, const Mat<N,T>& b){
//	Mat<N,T> r; return Mat<N,T>::multiply(r, a,b);
//}

// Mat
template <int N, class T>
inline Mat<N,T> operator + (T s, const Mat<N,T>& v){ return  v+s; }

template <int N, class T>
inline Mat<N,T> operator - (T s, const Mat<N,T>& v){ return -v+s; }

template <int N, class T>
inline Mat<N,T> operator * (T s, const Mat<N,T>& v){ return  v*s; }


// Vec/Mat
template <int N, class T>
inline Vec<N,T> operator* (const Mat<N,T>& m, const Vec<N,T>& vCol){
	Vec<N,T> r; return Mat<N,T>::multiply(r, m,vCol);
}

template <int N, class T>
inline Vec<N,T> operator* (const Vec<N,T>& vRow, const Mat<N,T>& m){
	Vec<N,T> r; return Mat<N,T>::multiply(r, vRow,m);
}




// Specialized vector functions

/// Returns concatenation of two vectors
template <int N1, class T1, int N2, class T2>
inline Vec<N1+N2, T1> concat(const Vec<N1,T1>& a, const Vec<N2,T2>& b){
	Vec<N1+N2, T1> r;
	r.set(a.ptr());
	for(int i=0; i<N2; ++i) r[i+N1] = T1(b[i]);
	return r;
}


/// Sets r to cross product, a x b
template <class T>
inline void cross(Vec<3,T>& r, const Vec<3,T>& a, const Vec<3,T>& b){
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
}


/// Returns cross product, a x b
template <class T>
inline Vec<3,T> cross(const Vec<3,T>& a, const Vec<3,T>& b){
	Vec<3,T> r;	cross(r,a,b); return r;
}


/// Returns wedge product a ^ b

/// Since the ^ operator has lower precedence than other arithmetic operators
/// (-, +, *, /, ==) being a bitwise XOR, use parenthesis around this operation 
/// to ensure correct results.
template <int N, class T>
Vec<N,T> operator^ (const Vec<N,T>& a, const Vec<N,T>& b);


/// Returns wedge (cross) product of two 3-vectors
template <class T>
inline Vec<3,T> operator^ (const Vec<3,T>& a, const Vec<3,T>& b){
	return cross(a,b);
}


/// Returns angle, in radians, between two vectors
template <int N, class T>
static T angle(const Vec<N,T>& a, const Vec<N,T>& b){
	using namespace std;
	return acos(a.sgn().dot(b.sgn()));
}


/*! Centroid of a triangle defined by three points
	@param p1	Point1
	@param p2	Point2
	@param p3	Point3
	@ret c		Centroid
*/
template <int N, class T>
inline void centroid3(Vec<N,T>& c, const Vec<N,T>& p1, const Vec<N,T>& p2, const Vec<N,T>& p3){
	static const T _1_3 = T(1)/T(3);
	c = (p1+p2+p3)*_1_3;
}

/*! Get the normal to a triangle defined by three points
	@param p1	Point1
	@param p2	Point2
	@param p3	Point3
	@ret n		Normal
*/
template <class T>
inline void normal(Vec<3,T>& n, const Vec<3,T>& p1, const Vec<3,T>& p2, const Vec<3,T>& p3){
	cross(n, p2-p1, p3-p1);	
	n.normalize();
}

/*! Returns vector containing element-wise minimum between two vectors
*/
template <int N, class T>
inline Vec<N,T> vmin(const Vec<N,T>& a, const Vec<N,T>& b){
	Vec<N,T> r;
	IT(N){ r[i] = a[i] > b[i] ? b[i] : a[i]; }
	return r;
}

/*! Returns vector containing element-wise maximum between two vectors
*/
template <int N, class T>
inline Vec<N,T> vmax(const Vec<N,T>& a, const Vec<N,T>& b){	
	Vec<N,T> r;
	IT(N){ r[i] = a[i] < b[i] ? b[i] : a[i]; }
	return r;
}


// Specialized vector classes

/// 2-vector
template <class T>
struct Vec2 : public Vec<2,T> {

	typedef Vec<2,T> Base;

	Vec2(const Base& v=Base()){ *this = v; }
	Vec2(const T& x, const T& y){ Base::set(x,y); }

	Vec2& operator= (const Base& v){ Base::set(v); return *this; }
};


/// 3-vector
template <class T>
struct Vec3 : public Vec<3,T> {

	typedef Vec<3,T> Base;

	Vec3(const Base& v=Base()){ *this = v; }
	Vec3(const T& x, const T& y, const T& z){ Base::set(x,y,z); }

	Vec3& operator= (const Base& v){ Base::set(v); return *this; }
	
	
};


/// 4-vector
template <class T>
struct Vec4 : public Vec<4,T> {

	typedef Vec<4,T> Base;
	
	Vec4(const Base& v=Base()){ *this = v; }
	Vec4(const T& x, const T& y, const T& z, const T& w){ Base::set(x,y,z,w); }
	
	Vec4& operator= (const Base& v){ Base::set(v); return *this; }

};



// Implementation

template <int N, class T>
Vec<N,T>& Vec<N,T>::normalize(){
	float m = mag();
	if(m > T(1e-20)){
		(*this) /= m;
	}
	else{
		set(T(0));
		elems[0] = T(1);
	}
	return *this;
}


#undef IT

} // ::al::

#endif /* include guard */
