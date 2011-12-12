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
template <int N, class T> class Mat;

typedef Vec<2,float>	Vec2f;
typedef Vec<2,double>	Vec2d;
typedef Vec<2,int>		Vec2i;
typedef Vec<3,float>	Vec3f;
typedef Vec<3,double>	Vec3d;
typedef Vec<3,int>		Vec3i;
typedef Vec<4,float>	Vec4f;
typedef Vec<4,double>	Vec4d;
typedef Vec<4,int>		Vec4i;
typedef Mat<3,float>	Mat3f;
typedef Mat<3,double>	Mat3d;
typedef Mat<4,float>	Mat4f;
typedef Mat<4,double>	Mat4d;
typedef Mat<4,int>		Mat4i;

// Forward iterates from 0 to n-1. Current index is 'i'.
#define IT(n) for(int i=0; i<n; ++i)



/// N-vector

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
	
	Vec reflect(const Vec& normal) {
		return this - (2 * dot(normal) * normal);
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



/// N x N square matrix
template <int N, class T>
class Mat{
public:

	/// Column-major array of elements
	T elems[N*N];

	Mat(){ set(T(0)); }

	/// param[in] arr	one dimensional array in column-major
	Mat(const T * arr){ set(arr); }

	Mat(
		const T& r1c1, const T& r1c2, const T& r1c3,
		const T& r2c1, const T& r2c2, const T& r2c3,
		const T& r3c1, const T& r3c2, const T& r3c3
	){
		set(r1c1, r1c2, r1c3,
			r2c1, r2c2, r2c3,
			r3c1, r3c2, r3c3
		);
	}

	Mat(
		const T& r1c1, const T& r1c2, const T& r1c3, const T& r1c4,
		const T& r2c1, const T& r2c2, const T& r2c3, const T& r2c4,
		const T& r3c1, const T& r3c2, const T& r3c3, const T& r3c4,
		const T& r4c1, const T& r4c2, const T& r4c3, const T& r4c4
	){
		set(r1c1, r1c2, r1c3, r1c4,
			r2c1, r2c2, r2c3, r2c4,
			r3c1, r3c2, r3c3, r3c4,
			r4c1, r4c2, r4c3, r4c4
		);
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

	
	/// Set all elements to value
	Mat& set(const T& v){ IT(size()){ (*this)[i]=v; } return *this; }
	
	/// Set elements in column-major order from C array
	template <class U>
	Mat& set(const U * arr){ IT(size()){ (*this)[i]=arr[i]; } return *this; }

	/// Set elements in column-major order from C array
	
	/// @param[in] arr			1D array from which to copy (stride=1)
	/// @param[in] numElements	number of elements to copy
	/// @param[in] matOffset	index offset into matrix
	/// @param[in] matStride	amount to stride through matrix
	template <class U>
	Mat& set(const U * arr, int numElements, int matOffset, int matStride=1){
		IT(numElements){ (*this)[i*matStride+matOffset]=arr[i]; } return *this;
	}
	
	Mat& set(
		const T& r1c1, const T& r1c2, const T& r1c3,
		const T& r2c1, const T& r2c2, const T& r2c3,
		const T& r3c1, const T& r3c2, const T& r3c3
	){
		elems[0] = r1c1; elems[3] = r1c2; elems[6] = r1c3;
		elems[1] = r2c1; elems[4] = r2c2; elems[7] = r2c3;
		elems[2] = r3c1; elems[5] = r3c2; elems[8] = r3c3;
		return *this;
	}
	
	Mat& set(
		const T& r1c1, const T& r1c2, const T& r1c3, const T& r1c4,
		const T& r2c1, const T& r2c2, const T& r2c3, const T& r2c4,
		const T& r3c1, const T& r3c2, const T& r3c3, const T& r3c4,
		const T& r4c1, const T& r4c2, const T& r4c3, const T& r4c4
	){
		elems[0] = r1c1; elems[4] = r1c2; elems[ 8] = r1c3; elems[12] = r1c4;
		elems[1] = r2c1; elems[5] = r2c2; elems[ 9] = r2c3; elems[13] = r2c4;
		elems[2] = r3c1; elems[6] = r3c2; elems[10] = r3c3; elems[14] = r3c4;
		elems[3] = r4c1; elems[7] = r4c2; elems[11] = r4c3; elems[15] = r4c4;
		return *this;
	}

	/// Set elements on diagonal to one and all others to zero
	Mat& setIdentity(){ return ((*this) = identity()); }
	
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

	/// Returns identity matrix
	static Mat identity(){
		Mat m;
		IT(size()){ m[i] = (i%(N+1)) ? T(0) : T(1); }
		return m;
	}

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
inline Vec<N,T> min(const Vec<N,T>& a, const Vec<N,T>& b){
	Vec<N,T> r;
	IT(N){ r[i] = a[i] > b[i] ? b[i] : a[i]; }
	return r;
}

/*! Returns vector containing element-wise maximum between two vectors
*/
template <int N, class T>
inline Vec<N,T> max(const Vec<N,T>& a, const Vec<N,T>& b){	
	Vec<N,T> r;
	IT(N){ r[i] = a[i] < b[i] ? b[i] : a[i]; }
	return r;
}



// Implementation --------------------------------------------------------------

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
