#ifndef INCLUDE_AL_MATH_FUNCTIONS_HPP
#define INCLUDE_AL_MATH_FUNCTIONS_HPP

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
#include <math.h>
#include <stdlib.h>
#include "system/al_Config.h"
#include "math/al_Constants.hpp"


// Undefine these macros if found (in windows.h) in favor of proper functions
// defined in this file.
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#ifdef sinc
#undef sinc
#endif


namespace al {

/// Returns absolute value
template<class T> T abs(T v);

/// Returns value clipped ouside of range [-eps, eps]
template<class T> T atLeast(T v, T eps);

/// Fast approximation to atan2().

// Author: Jim Shima, http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm.
// |error| < 0.01 rad
template<class T> T atan2Fast(T y, T x);

/// Returns number of bits set to 1.

/// From "Bit Twiddling Hacks", 
/// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
uint32_t bitsSet(uint32_t v);

/// Returns floating point value rounded to next highest integer.
template<class T> T ceil(T val);
template<class T> T ceil(T val, T step);
template<class T> T ceil(T val, T step, T recStep);

/// Returns power of two ceiling of value

/// This uses an algorithm devised by Sean Anderson, Sep. 2001.
/// From "Bit Twiddling Hacks", http://graphics.stanford.edu/~seander/bithacks.html.
uint32_t ceilPow2(uint32_t value);

/// Returns value clipped to [lo, hi]
template<class T> T clip(T value, T hi=T(1), T lo=T(0));

/// Returns value clipped to [lo, hi] and signifies clipping behavior

/// clipFlag signifies if and where clipping occured.  0 means no clipping
/// occured, -1 means clipping occured at the lower bound, and 1 means
/// clipping at the upper bound.
template<class T> T clip(T v, int& clipFlag, T hi, T lo);

/// Returns value clipped to [-hi, hi].
template<class T> T clipS(T value, T hi=T(1));

/// Returns whether or not an integer value is even.
template<class T> bool even(T v);

/// Returns factorial. Argument must be less than or equal to 12.
uint32_t factorial(uint32_t v);

/// Returns floor of floating point value.
template<class T> T floor(T val);
template<class T> T floor(T val, T step);
template<class T> T floor(T val, T step, T recStep);

/// Returns power of two floor of value

/// This uses an algorithm devised by Sean Anderson, Sep. 2001.
/// From "Bit Twiddling Hacks", http://graphics.stanford.edu/~seander/bithacks.html.
uint32_t floorPow2(uint32_t value);

/// Returns value folded into [lo, hi]

/// For out-of-range values, the boundaries act like mirrors reflecting
/// the value into the range. For an even number of periods out of the range
/// this is identical to a wrap().
template<class T> T fold(T value, T hi=T(1), T lo=T(0));

/// Returns value folded into [lo, hi] one time.
template<class T> T foldOnce(T value, T hi=T(1), T lo=T(0));

/// Returns e^(-v*v)
template<class T> T gaussian(T v);

/// Return greatest common divisor of two arguments
template<class T> T gcd(const T& x, const T& y);

/// Generalized Laguerre polynomial L{n,k}
///
/// http://en.wikipedia.org/wiki/Laguerre_polynomials
template<class T> T laguerre(int n, int k, T x);

/// Returns least common multiple
template<class T> T lcm(const T& x, const T& y);

/// Associated Legendre polynomial
///
/// P_l^m(cos(t)) = (-1)^{l+m} / (2^l l!) sin^m(t) (d/d cos(t))^{l+m} sin^{2l}(t)
///
/// @param[in]	l	degree s.t. l>=0
/// @param[in]	m	order s.t. -l<=m <=l
/// @param[in]	t	variable
///
/// http://comp.cs.ehime-u.ac.jp/~ogata/nac/index.html
template<class T> T legendre(int l, int m, T t);
template<class T> T legendre(int l, int m, T ct, T st);

/// Returns whether the absolute value is less than an epsilon.
template<class T> bool lessAbs(T v, T eps=T(0.000001));

/// Returns base 2 logarithm of value.

/// If the value is not an exact power of two, the logarithm of the next
/// highest power of two will taken.
/// This uses an algorithm devised by Eric Cole, Jan. 2006.
/// From "Bit Twiddling Hacks", http://graphics.stanford.edu/~seander/bithacks.html.
uint32_t log2(uint32_t v);

/// Returns maximum of two values
template<class T> T max(T v1, T v2);

/// Returns maximum of three values
template<class T> T max(T v1, T v2, T v3);

/// Returns mean of two values
template<class T> T mean(T v1, T v2);

/// Returns minimum of two values
template<class T> T min(T v1, T v2);

/// Returns minimum of three values
template<class T> T min(T v1, T v2, T v3);

/// Returns the next representable floating-point or integer value following x in the direction of y
template<class T> T nextAfter(T x, T y);

/// Returns the number of digits in the integer portion
template<class T> T numInt(const T& v);

/// Returns whether or not an integer value is odd.
template<class T> bool odd(T v);

/// Evaluates polynomial a0 + a1 x + a2 x^2
template<class T> T poly(T x, T a0, T a1, T a2);

/// Evaluates polynomial a0 + a1 x + a2 x^2 + a3 x^3
template<class T> T poly(T x, T a0, T a1, T a2, T a3);

template<class T> T pow2(T v);			///< Returns value to the 2nd power.
template<class T> T pow2S(T v);			///< Returns value to the 2nd power preserving sign.
template<class T> T pow3(T v);			///< Returns value to the 3rd power.
template<class T> T pow3Abs(T v);		///< Returns absolute value to the 3rd power.
template<class T> T pow4(T v);			///< Returns value to the 4th power.
template<class T> T pow5(T v);			///< Returns value to the 5th power.
template<class T> T pow6(T v);			///< Returns value to the 6th power.
template<class T> T pow8(T v);			///< Returns value to the 8th power.
template<class T> T pow16(T v);			///< Returns value to the 16th power.
template<class T> T pow64(T v);			///< Returns value to the 64th power.

/// Returns (n+1)th prime number up to n=53.
unsigned char prime(uint32_t n);

/// Returns the value r such that r = x - n*y.
template<class T> T remainder(const T& x, const T& y);

/// Returns value rounded to nearest integer towards zero.
template<class T> T round(T v);

/// Returns value rounded to nearest integer multiple of 'step' towards zero.
template<class T> T round(T v, T step);

/// Returns value rounded to nearest integer multiple of 'step' towards zero. Faster version to avoid 1/step divide.
template<class T> T round(T v, T step, T recStep);

/// Returns value rounded to nearest integer away from zero.
template<class T> T roundAway(T v);

/// Returns value rounded to nearest to nearest integer multiple of 'step' away from zero.
template<class T> T roundAway(T v, T step);

/// Signum function for real numbers
template<class T> T sgn(const T& v, const T& norm=T(1));

/// Unnormalized sinc function
template<class T> T sinc(T radians, T eps=T(0.0001));

/// Returns slope of line passing through two points.
template<class T> T slope(T x1, T y1, T x2, T y2);

/// Sort values so that value1 <= value2.
template<class T> void sort(T& value1, T& value2);

/// Sum of integers squared from 1 to n.
template<class T> T sumOfSquares(T n);

/// Returns number of trailing zeros in 32-bit int

/// This implements an algorithm from the paper 
/// "Using de Bruijn Sequences to Index 1 in a Computer Word"
/// by Charles E. Leiserson, Harald Prokof, and Keith H. Randall.
uint32_t trailingZeroes(uint32_t v);

/// Truncates floating point value at decimal.
template<class T> T trunc(T v);

/// Truncates floating point value to step.
template<class T> T trunc(T v, T step);

/// Truncates floating point value to step. Faster version to avoid 1/step divide.
template<class T> T trunc(T v, T step, T recStep);

/// Returns whether value is in interval [lo, hi].
template<class T> bool within(T v, T lo, T hi);

/// Returns whether 3 values are in interval [lo, hi].
template<class T> bool within3(T v1, T v2, T v3, T lo, T hi);

/// Returns whether value is in interval [lo, hi).
template<class T> bool withinIE(T v, T lo, T hi);

/// Returns value wrapped in [lo, hi).
template<class T> T wrap(T value, T hi=T(1), T lo=T(0));

/// Returns value wrapped in [lo, hi).

/// 'numWraps' reports how many wrappings occured where the sign, + or -,
/// signifies above 'hi' or below 'lo', respectively.
template<class T> T wrap(T value, long& numWraps, T hi=T(1), T lo=T(0));

/// Returns value incremented by 1 and wrapped into interval [0, max).
template<class T> T wrapAdd1(T v, T max){ ++v; return v == max ? 0 : v; }

/// Like wrap(), but only adds or subtracts 'hi' once from value.
template<class T> T wrapOnce(T value, T hi=T(1));

template<class T> T wrapOnce(T value, T hi, T lo);

/// Returns value wrapped in [-pi, pi)
template<class T> T wrapPhase(T radians);			

/// Like wrapPhase(), but only wraps once
template<class T> T wrapPhaseOnce(T radians);




// Implementation
//------------------------------------------------------------------------------

#define TEM template<class T>

namespace{
	template<class T> const T roundEps();
	template<> inline const float  roundEps<float >(){ return 0.499999925f; }
	template<> inline const double roundEps<double>(){ return 0.499999985; }

	inline uint32_t deBruijn(uint32_t v){
		static const uint32_t deBruijnBitPosition[32] = {
			 0, 1,28, 2,29,14,24, 3,30,22,20,15,25,17, 4, 8, 
			31,27,13,23,21,19,16, 7,26,12,18, 6,11, 5,10, 9
		};
		return deBruijnBitPosition[(uint32_t(v * 0x077CB531UL)) >> 27];
	}
	
	const uint32_t mFactorial12u[13] = {
		1, 1, 2, 6, 24, 120, 720, 5040, 40320, 
		362880, 3628800, 39916800, 479001600
	};

	const uint8_t mPrimes54[54] = {
	/*	  0    1    2    3    4    5    6    7    8    9   */
		  2,   3,   5,   7,  11,  13,  17,  19,  23,  29, // 0
		 31,  37,  41,  43,  47,  53,  59,  61,	 67,  71, // 1
		 73,  79,  83,  89,  97, 101, 103, 107, 109, 113, // 2
		127, 131, 137, 139, 149, 151, 157, 163, 167, 173, // 3
		179, 181, 191, 193, 197, 199, 211, 223, 227, 229, // 4
		233, 239, 241, 251								  // 5
	};
}



/// Returns absolute value
//TEM inline T abs(T v){ return std::fabs(v); }
template<> inline float abs(float v){ return fabsf(v); }
template<> inline double abs(double v){ return fabs(v); }
template<> inline char abs(char v){ return labs(v); }
template<> inline short abs(short v){ return labs(v); }
template<> inline int abs(int v){ return labs(v); }
template<> inline long abs(long v){ return labs(v); }
template<> inline long long abs(long long v){ return ::llabs(v); }

TEM inline T atLeast(T v, T e){	return (v >= T(0)) ? max(v, e) : min(v, -e); }

TEM T atan2Fast(T y, T x){
	
	T r, angle;
	T ay = al::abs(y) + T(1e-10);      // kludge to prevent 0/0 condition
	
	if(x < T(0)){
		r = (x + ay) / (ay - x);
		angle = T(M_3PI_4);
	}
	else{
		r = (x - ay) / (x + ay);
		angle = T(M_PI_4);
	}
	
	angle += (T(0.1963)*r*r - T(0.9817))*r;
	return y < T(0) ? -angle : angle;
}

inline uint32_t bitsSet(uint32_t v){
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	return ((v + ((v >> 4) & 0xF0F0F0F)) * 0x1010101) >> 24; // count
}

TEM inline T ceil(T v){ return round(v + roundEps<T>()); }
TEM inline T ceil(T v, T s){ return ceil(v/s)*s; }
TEM inline T ceil(T v, T s, T r){ return ceil(v*r)*s; }

inline uint32_t ceilPow2(uint32_t v){
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return ++v;
}

TEM inline T clip(T v, T hi, T lo){
	     if(v < lo) return lo;
	else if(v > hi)	return hi;
	return v;
}

TEM inline T clip(T v, int & clipFlag, T hi, T lo){
	clipFlag = 0;
	     if(v < lo){ clipFlag = -1; return lo; }
	else if(v > hi){ clipFlag =  1; return hi; }
	return v;
}

TEM inline T clipS(T v, T hi){ return al::clip(v, hi, -hi); }

TEM inline bool even(T v){ return 0 == al::odd(v); }

inline uint32_t factorial(uint32_t v){ return mFactorial12u[v]; }

TEM inline T floor(T v){ return al::round(v - roundEps<T>()); }
TEM inline T floor(T v, T s){ return al::floor(v/s)*s; }
TEM inline T floor(T v, T s, T r){ return al::floor(v*r)*s; }

inline uint32_t floorPow2(uint32_t v){
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return (v >> 1) + 1;
}

TEM inline T fold(T v, T hi, T lo){
	long numWraps;
	v = al::wrap(v, numWraps, hi, lo);
	if(numWraps & 1) v = hi + lo - v;
	return v;
}

TEM inline T foldOnce(T v, T hi, T lo){
	if(v > hi) return hi + (hi - v);
	if(v < lo) return lo + (lo - v);
	return v;
}

TEM inline T gaussian(T v){ return ::exp(-v*v); }

TEM T gcd(const T& x, const T& y){
	if(y==T(0)) return x;
	return al::gcd(y, al::remainder(x,y));
}

TEM T laguerre(int n, int k, T x) {
	T res = 1, bin = 1;
	
	for(int i=n; i>=1; --i){
		bin = bin * (k+i) / (n + 1 - i);
		res = bin - x * res / i;
	}
	return res;
}

TEM inline T lcm(const T& x, const T& y){ return (x*y)/al::gcd(x,y); }

TEM T legendre(int l, int m, T ct, T st){

	if(l<0){ /*printf("l=%d. l must be non-negative.\n");*/ return 0; }
	if(m<-l || m>l){ /*printf("m=%d. m must be -l <= m <= l.\n");*/ return 0; }

	// compute P_l^m(x) by the recurrence relation
	//		(l-m)P_l^m(x) = x(2l-1)P_{l-1}^m(x) - (l+m-1)P_{l-2}^m(x)
	// with 
	//		P_m^m(x) = (-1)^m (2m-1)!! (1-x)^{m/2}, 
	//		P_{m+1}^m(x) = x(2m+1) P_m^m(x).

	T P = 0;
	int mm = al::abs(m);			/*   mm = |m|   */
	T y1 = 1.;
	
	for(int i=1; i<=mm; ++i)
		y1 *= -((i<<1) - 1) * st;
	
	if(l==mm) P = y1;

	else{
		T y = ((mm<<1) + 1) * ct * y1;
		if(l==(mm+1)) P = y;

		else{
			T c = (mm<<1) - 1;
			for(int k=mm+2; k<=l; ++k){
				T y2 = y1;
				y1 = y;
				T d = c / (k - mm);
				y = (2. + d) * ct * y1 - (1. + d) * y2;
			}
			P = y;
		}
	}

	// In the case that m<0, 
	// compute P_n^{-|m|}(x) by the formula 
	//		P_l^{-|m|}(x) = (-1)^{|m|}((l-|m|)!/(l+|m|)!)^{1/2} P_l^{|m|}(x). 
	if(m<0){
		for(int i=l-mm+1; i<=l+mm; ++i) P *= 1. / i;
		if(al::odd(mm)) P = -P;
	}

	return P;
}


TEM T legendre(int l, int m, T t){
	return al::legendre(l,m, std::cos(t), std::sin(t));
}

TEM inline bool lessAbs(T v, T eps){ return al::abs(v) < eps; }

inline uint32_t log2(uint32_t v){ return deBruijn(al::ceilPow2(v)); }

TEM inline T max(T v1, T v2){ return v1<v2?v2:v1; }
TEM inline T max(T v1, T v2, T v3){ return al::max(al::max(v1,v2),v3); }
TEM inline T mean(T v1, T v2){ return (v1 + v2) * T(0.5); }
TEM inline T min(T v1, T v2){ return v1<v2?v1:v2; }
TEM inline T min(T v1, T v2, T v3){ return al::min(al::min(v1,v2),v3); }

TEM inline T nextAfter(T x, T y){ return x<y ? x+1 : x-1; }
template<> inline float nextAfter(float x, float y){ return nextafterf(x,y); }
template<> inline double nextAfter(double x, double y){ return nextafter(x,y); }
template<> inline long double nextAfter(long double x, long double y){ return nextafterl(x,y); }

TEM inline T numInt(const T& v){ return al::floor(::log10(v)) + 1; }

TEM inline bool odd(T v){ return v & T(1); }

TEM inline T poly(T v, T a0, T a1, T a2){ return a0 + v*(a1 + v*a2); }
TEM inline T poly(T v, T a0, T a1, T a2, T a3){ return a0 + v*(a1 + v*(a2 + v*a3)); }

TEM inline T pow2 (T v){ return v*v; }
TEM inline T pow2S(T v){ return v*al::abs(v); }
TEM inline T pow3 (T v){ return v*v*v; }
TEM inline T pow3Abs(T v){ return al::abs(pow3(v)); }
TEM inline T pow4 (T v){ return pow2(pow2(v)); }
TEM inline T pow5 (T v){ return v * pow4(v); }
TEM inline T pow6 (T v){ return pow3(pow2(v)); }
TEM inline T pow8 (T v){ return pow4(pow2(v)); }
TEM inline T pow16(T v){ return pow4(pow4(v)); }
TEM inline T pow64(T v){ return pow8(pow8(v)); }

inline uint8_t prime(uint32_t n){ return mPrimes54[n]; }

template<> inline float remainder<float>(const float& x, const float& y){ return ::remainderf(x,y); }
template<> inline double remainder<double>(const double& x, const double& y){ return ::remainder(x,y); }
template<> inline long double remainder<long double>(const long double& x, const long double& y){ return ::remainderl(x,y); }
TEM inline T remainder(const T& x, const T& y){ return x-(x/y)*y; }

TEM inline T round(T v){
	static const double roundMagic = 6755399441055744.; // 2^52 * 1.5
	double r=v;
	return (r + roundMagic) - roundMagic;
}
TEM inline T round(T v, T s){ return round<double>(v/s) * s; }
TEM inline T round(T v, T s, T r){ return round<T>(v * r) * s; }
TEM inline T roundAway(T v){ return v<T(0) ? al::floor(v) : al::ceil(v); }
TEM inline T roundAway(T v, T s){ return v<T(0) ? al::floor(v,s) : al::ceil(v,s); }

TEM inline T sgn(const T& v, const T& norm){ return v<T(0) ? -norm : norm; }

TEM inline T sinc(T r, T eps){ return (al::abs(r) > eps) ? std::sin(r)/r : std::cos(r); }

TEM inline T slope(T x1, T y1, T x2, T y2){ return (y2 - y1) / (x2 - x1); }

TEM inline void sort(T& v1, T& v2){ if(v1>v2){ T t=v1; v1=v2; v2=t; } }

TEM inline T sumOfSquares(T n){
	static const T c1_6 = 1/T(6);
	static const T c2_6 = c1_6*T(2);
	return n*(n+1)*(c2_6*n+c1_6);
}

inline uint32_t trailingZeroes(uint32_t v){ return deBruijn(v & -v); }

TEM inline T trunc(T v){ return al::round( (v > (T)0) ? v-roundEps<T>() : v+roundEps<T>() ); }
TEM inline T trunc(T v, T s){ return al::trunc(v/s)*s; }
TEM inline T trunc(T v, T s, T r){ return al::trunc(v*r)*s; }

TEM inline bool within  (T v, T lo, T hi){ return !((v < lo) || (v > hi)); }
TEM inline bool withinIE(T v, T lo, T hi){ return (!(v < lo)) && (v < hi); }

TEM inline bool within3(T v1, T v2, T v3, T lo, T hi){
	return al::within(v1,lo,hi) && al::within(v2,lo,hi) && al::within(v3,lo,hi);
}

TEM inline T wrap(T v, T hi, T lo){
	if(lo == hi) return lo;
	
	//if(v >= hi){
	if(!(v < hi)){
		T diff = hi - lo;
		v -= diff;
		if(!(v < hi)) v -= diff * (T)(uint32_t)((v - lo)/diff);
	}
	else if(v < lo){
		T diff = hi - lo;
		v += diff;	// this might give diff if range is too large, so check at end of block...
		if(v < lo) v += diff * (T)(uint32_t)(((lo - v)/diff) + 1);
		if(v==diff) return al::nextAfter(v, lo);
	}
	return v;
}

TEM inline T wrap(T v, long& numWraps, T hi, T lo){
	if(lo == hi){ numWraps = 0xFFFFFFFF; return lo; }
	
	T diff = hi - lo;
	numWraps = 0;
	
	if(v >= hi){
		v -= diff;
		if(v >= hi){
			numWraps = (long)((v - lo)/diff);
			v -= diff * (T)numWraps;
		}
		numWraps++;
	}
	else if(v < lo){
		v += diff;
		if(v < lo){
			numWraps = (long)((v - lo)/diff) - 1;
			v -= diff * (T)numWraps;
		}
		numWraps--;
	}
	return v;
}

TEM inline T wrapOnce(T v, T hi){
	     if(v >= hi ) return v - hi;
	else if(v < T(0)) return v + hi;
	return v;
}

TEM inline T wrapOnce(T v, T hi, T lo){
	     if(v >= hi) return v - hi + lo;
	else if(v <  lo) return v + hi - lo;
	return v;
}

TEM inline T wrapPhase(T r){
	if(r >= T(M_PI)){
		r -= T(M_2PI);
		if(r < T(M_PI)) return r;
	}
	else if (r < T(-M_PI)){
		r += T(M_2PI);
		if(r >= T(-M_PI)) return r;
	}
	else return r;
	
	return r - T(M_2PI) * (long)((r + T(M_PI)) * T(M_1_2PI));
}

TEM inline T wrapPhaseOnce(T r){
	if(r >= T(M_PI))		return r - T(M_2PI);
	else if(r < T(-M_PI))	return r + T(M_2PI);
	return r;
}


#undef TEM

} // ::al::

#endif
