#ifndef INCLUDE_AL_COMPLEX_HPP
#define INCLUDE_AL_COMPLEX_HPP

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

template <class T> class Complex;
template <class T> class Polar;

typedef Polar<float>	Polarf;
typedef Polar<double>	Polard;
typedef Complex<float>	Complexf;
typedef Complex<double>	Complexd;



/// Polar number
template <class T>
class Polar{
public:
	/// @param[in] p	phase, in radians
	Polar(const T& p): m(1.), p(p){}
	
	/// @param[in] m	magnitude
	/// @param[in] p	phase, in radians
	Polar(const T& m, const T& p): m(m), p(p){}
	
	/// @param[in] v	rectangular complex number to convert from
	Polar(const Complex<T>& v){ *this = v; }

	Polar& operator = (const Complex<T>& v){ m=v.norm(); p=v.arg(); return *this; }

	T m;	///< Magnitude
	T p;	///< Phase, in radians
};


/// Complex number
template <class T>
class Complex{
public:
	typedef Complex<T> C;

	union{
		struct{ T r, i; };
		T elems[2];
	};
	
	Complex(const Complex& v): r(v.r), i(v.i){}
	Complex(const Polar<T>& v){ *this = v; }
	Complex(const T& r=(T)1, const T& i=(T)0): r(r), i(i){}
	Complex(const T& m, const T& p, int fromPolar){ (*this) = Polar<T>(m,p); }

	
	C& arg(const T& v){ return fromPolar(norm(), v); }					///< Set phase leaving magnitude the same
	C& fromPhase(const T& v){ r=::cos(v); i=::sin(v); return *this; }	///< Set phase and normalize
	C& fromPolar(const T& m, const T& p){ return (*this)(Polar<T>(m,p)); }	///< Set magnitude and phase
	C& norm(const T& v){ return fromPolar(v, arg()); }					///< Set magnitude leaving phase the same

	C& operator()(const T& vr, const T& vi){ r=vr; i=vi; return *this; }
	C& operator()(const Polar<T>& p){ return *this = p; }
	T& operator[](uint32_t i){ return elems[i];}
	const T& operator[](uint32_t i) const { return elems[i]; }

	bool operator ==(const C& v) const { return (r==v.r) && (i==v.i); }		///< Returns true if all components are equal
	bool operator ==(const T& v) const { return (r==v  ) && (i==T(0));}		///< Returns true if real and equals value
	bool operator !=(const C& v) const { return (r!=v.r) || (i!=v.i); }		///< Returns true if any components are not equal
	bool operator > (const C& v) const { return norm2() > v.norm2(); }		///< Returns true if norm is greater than argument's norm
	bool operator < (const C& c) const { return norm2() < c.norm2(); }		///< Returns true if norm is less than argument's norm

	C& operator = (const Polar<T>& v){ r=v.m*::cos(v.p); i=v.m*::sin(v.p); return *this; }
	C& operator = (const C& v){ r=v.r; i=v.i; return *this; }
	C& operator = (const T& v){ r=v;   i=T(0); return *this; }
	C& operator -=(const C& v){ r-=v.r; i-=v.i; return *this; }
	C& operator -=(const T& v){ r-=v; return *this; }
	C& operator +=(const C& v){ r+=v.r; i+=v.i; return *this; }
	C& operator +=(const T& v){ r+=v; return *this; }
	C& operator *=(const C& v){ return (*this)(r*v.r - i*v.i, i*v.r + r*v.i); }
	C& operator *=(const T& v){ r*=v; i*=v; return *this; }
	C& operator /=(const C& v){ return (*this) *= v.recip(); }
	C& operator /=(const T& v){ r/=v; i/=v; return *this; }

	C operator - () const { return C(-r, -i); }
	C operator - (const C& v) const { return C(*this) -= v; }
	C operator - (const T& v) const { return C(*this) -= v; }
	C operator + (const C& v) const { return C(*this) += v; }
	C operator + (const T& v) const { return C(*this) += v; }
	C operator * (const C& v) const { return C(*this) *= v; }
	C operator * (const T& v) const { return C(*this) *= v; }
	C operator / (const C& v) const { return C(*this) /= v; }
	C operator / (const T& v) const { return C(*this) /= v; }
	
	T arg() const { return atan2(i, r); }					///< Returns argument (angle)
	C conj() const { return C(r,-i); }						///< Returns conjugate, z*
	T dot(const C& v) const { return r*v.r + i*v.i; }		///< Returns vector dot product
	C exp() const { return Polar<T>(::exp(r), i); }			///< Returns e^z
	C log() const { return Complex<T>(T(0.5)*::log(norm2()), arg()); } ///< Returns log(z)
	T norm() const { return ::sqrt(norm2()); }				///< Returns norm (radius), |z|
	T norm2() const { return dot(*this); }					///< Returns square of norm, |z|^2
	C& normalize(){ return *this /= norm(); }				///< Sets norm (radius) to 1, |z|=1
	C pow(const C& v) const { return ((*this).log()*v).exp(); }	///< Returns z^v
	C pow(const T& v) const { return ((*this).log()*v).exp(); }	///< Returns z^v
	C recip() const { return conj()/norm2(); }				///< Return multiplicative inverse, 1/z
	C sgn() const { return C(*this).normalize(); }			///< Returns signum, z/|z|, the closest point on unit circle
	C sqr() const { return C(r*r-i*i, T(2)*r*i); }			///< Returns square

	/// Returns square root
	C sqrt() const {
		static const T c = T(1)/::sqrt(T(2));
		T n = norm();
		T a = ::sqrt(n+r) * c;
		T b = ::sqrt(n-r) * (i<T(0) ? -c : c);		
		return C(a,b);
	}

	C cos() const { return C(::cos(r)*::cosh(i),-::sin(r)*::sinh(i)); } ///< Returns cos(z)
	C sin() const { return C(::sin(r)*::cosh(i), ::cos(r)*::sinh(i)); } ///< Returns sin(z)

	T abs() const { return norm(); }						///< Returns norm (radius), |z|
	T mag() const { return norm(); }						///< Returns norm (radius), |z|
	T magSqr() const { return norm2(); }					///< Returns square of norm, |z|^2
	T phase() const { return arg(); }						///< Returns argument (angle)
};

#define TEM template <class T>
TEM Complex<T> exp(const Complex<T>& c){ return c.exp(); }
TEM Complex<T> log(const Complex<T>& c){ return c.log(); }
TEM Complex<T> pow(const Complex<T>& b, const Complex<T>& e){ return b.pow(e); }
TEM Complex<T> operator + (T r, const Complex<T>& c){ return  c+r; }
TEM Complex<T> operator - (T r, const Complex<T>& c){ return -c+r; }
TEM Complex<T> operator * (T r, const Complex<T>& c){ return  c*r; }
TEM Complex<T> operator / (T r, const Complex<T>& c){ return  c.conj()*(r/c.norm()); }
#undef TEM


template <class VecN, class T>
VecN rotate(const VecN& v, const VecN& p, const Complex<T>& a){
	return v*a.r + p*a.i;
}

/// Rotates two vectors by angle in plane formed from bivector v1 ^ v2
template <class VecN, class T>
void rotatePlane(VecN& v1, VecN& v2, const Complex<T>& a){
	VecN t = al::rotate(v1, v2, a);
	v2 = al::rotate(v2, VecN(-v1), a);
	v1 = t;
}

} // ::al::

#endif /* include guard */
