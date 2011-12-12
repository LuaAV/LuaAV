#ifndef INCLUDE_AL_CONVERSION_HPP
#define INCLUDE_AL_CONVERSION_HPP

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


#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <sstream>		/* string conversion */
#include "allocore/system/al_Config.h"

#define CONST(N, vf, vd)\
	template <class T> struct N;\
	template<> struct N< float>{ operator uint32_t() const { return UINT32_C(vf); } };\
	template<> struct N<double>{ operator uint64_t() const { return UINT64_C(vd); } };

	CONST(MaskExpo, 0x7F800000, 0x7FF0000000000000)	// IEEE-754 floating-point exponent bit mask
	CONST(MaskFrac, 0x007FFFFF, 0x000FFFFFFFFFFFFF) // IEEE-754 floating-point fraction bit mask
	CONST(MaskSign, 0x80000000, 0x8000000000000000) // IEEE-754 floating-point sign bit mask
	CONST(Expo1   , 0x3F800000, 0x3FF0000000000000) // IEEE-754 floating-point [1-2) exponent interval
#undef CONST

namespace al{

/// Union for twiddling bits of floats
template<class T> struct Twiddle;

template<> struct Twiddle<float>{
	Twiddle(const float& v): f(v){}
	Twiddle(const uint32_t& v): u(v){}
	Twiddle(const int32_t& v): i(v){}
	union{ int32_t i; uint32_t u; float f; };
};

template<> struct Twiddle<double>{
	Twiddle(const double& v): f(v){}
	Twiddle(const uint64_t& v): u(v){}
	Twiddle(const int64_t& v): i(v){}
	union{ int64_t i; uint64_t u; double f; };
};

/// Convert decimal integer to ascii base-36 character
char base10To36(int dec10);

/// Convert ascii base-36 character to decimal integer 
int base36To10(char ascii36);

/// Convert a string of 1s and 0s to an integer.
uint32_t bitsToUInt(const char * string);

/// Sets argument to zero if subnormal
float blockSubnormal(float v);

/// Sets argument to zero if subnormal
double blockSubnormal(double v);

/// Returns 1 if little endian
int endian();

/// Returns biased decimal value of 32-bit float exponent field.

/// The true exponent is the return value minus 127.
/// For example, values in [0.5, 1) return 126 (01111110), so the true
///	exponent is 126 - 127 = -1.
uint32_t floatExponent(float v);

/// Returns mantissa field as float between [0, 1).
float floatMantissa(float v);

/// Converts linear integer phase to fraction

///	2^bits is the effective size of the lookup table. \n
///	Note: the fraction only has 24-bits of precision.
float fraction(uint32_t bits, uint32_t phase);

/// Convert 16-bit signed integer to floating point in [-1, 1)
float intToUnit(int16_t v);

/// Type-pun 32-bit unsigned int to 32-bit float

/// This function uses a union to avoid problems with direct pointer casting
/// when the fstrict-aliasing compiler flag is on.
inline float punUF(uint32_t v){ Twiddle<float> u(v); return u.f; }

/// Type-pun 32-bit float to 32-bit unsigned int

/// This function uses a union to avoid problems with direct pointer casting
/// when the fstrict-aliasing compiler flag is on.
inline uint32_t punFU( float v){ Twiddle< float> u(v); return u.u; }
inline  int32_t punFI( float v){ Twiddle< float> u(v); return u.i; }

inline  int64_t punFI(  double v){ Twiddle<double> u(v); return u.i; }
inline uint64_t punFU(  double v){ Twiddle<double> u(v); return u.u; }
inline   double punUF(uint64_t v){ Twiddle<double> u(v); return u.f; }
inline   double punIF( int64_t v){ Twiddle<double> u(v); return u.f; }

/// Convert numerical type to a string
template <class T> std::string toString(const T& v);

/// Convert array of numerical types to a comma separated string
template <class T>
std::string toString(const T * v, int num, int stride=1);

/// Convert 32-bit unsigned integer to unit float in [0, 1)
template<class T> T uintToUnit (uint32_t v);

/// Convert 32-bit unsigned integer to unit float in [-1, 1)
template<class T> T uintToUnitS(uint32_t v);

/// Convert floating point in [0, 1) to unsigned long in [0, 2^32)

/// This conversion is most accurate on a linear scale.
/// Input values outside [0, 1) result in undefined behavior.
uint32_t unitToUInt(float u);

/// Convert floating point in [0, 1) to unsigned long in [0, 2^32)

/// This conversion is most accurate on an exponential scale.
///	Input values outside [-1, 1) return 0.
///	Values in [-1, 0] behave as positive values in [0, 1).
uint32_t unitToUInt2(float u);

/// Convert unit float in [0,1) to 8-bit unsigned int in [0, 256).
uint8_t unitToUInt8(float u);




// Implementation
//------------------------------------------------------------------------------

inline char base10To36(int v){
	static const char * c = "0123456789abcdefghijklmnopqrstuvwxyz";
	if(v>=0 && v<=35) return c[v];
	return '0';
}

inline int base36To10(char v){
	v = tolower(v);
	if(v>='0' && v<='9') return v - '0';
	if(v>='a' && v<='z') return v - 'a' + 10;
	return 0;	// non-alphanumeric
}

inline uint32_t bitsToUInt(const char * string){
	uint32_t v=0; int n = strlen(string);
	for(int i=0; i<n; ++i) if(string[i] == '1') v |= 1<<(n-1-i);
	return v;
}

// alternate version...
//inline uint32_t bitsToUInt(const char * bits){
//	uint32_t i=0, r=0;
//	for(; bits[i] && i<32; ++i) r |= ((bits[i]=='1'?1:0) << (31-i));
//	return r>>(32-i);
//}

/// Sets argument to zero if subnormal
inline float blockSubnormal(float v){
	const uint32_t i = punFU(v);
	const uint32_t frac = i & MaskFrac<float>(); 
	const uint32_t expo = i & MaskExpo<float>(); 
	if(expo == 0 && frac != 0) v = 0.f;
	return v;
}

/// Sets argument to zero if subnormal
inline double blockSubnormal(double v){
	const uint64_t i = punFU(v);
	const uint64_t frac = i & MaskFrac<double>(); 
	const uint64_t expo = i & MaskExpo<double>(); 
	if(expo == 0 && frac != 0) v = 0.;
	return v;
}

inline int endian(){
	static int x=1;
	return *(char *)&x;
}

inline uint32_t floatExponent(float v){
	return punFU(v) >> 23 & 0xff;
}

inline float floatMantissa(float v){
	uint32_t frac = punFU(v);
	frac = (frac & MaskFrac<float>()) | Expo1<float>();
	return punUF(frac) - 1.f;
}

inline float fraction(uint32_t bits, uint32_t phase){	
	phase = phase << bits >> 9 | Expo1<float>();
	return punUF(phase) - 1.f;
}

inline float intToUnit(int16_t v){
	uint32_t vu = (((uint32_t)v) + 0x808000) << 7; // set fraction in float [2, 4)
	return punUF(vu) - 3.f;
}

template <class T>
std::string toString(const T * v, int n, int s){
	std::string r;
	for(int i=0; i<n; ++i){
		r += toString(v[i*s]);
		if(i<(n-1)) r += ", ";
	}
	return r;
}

template <class T>
std::string toString(const T& v){
	using namespace std;
	stringstream ss(stringstream::in | stringstream::out);
	ss << v;
	string r;
	ss >> r;
	return r;
}

template<> inline float uintToUnit<float>(uint32_t v){
	v = v >> 9 | Expo1<float>(); 
	return punUF(v) - 1.f;
}

template<> inline float uintToUnitS<float>(uint32_t v){
	v = v >> 9 | 0x40000000;
	return punUF(v) - 3.f;
}

inline uint32_t unitToUInt(float v){
	++v;	// go into [1,2] range, FP fraction is now result
	return punFU(v) << 9;
}

// TODO: make 64-bit ready
inline uint32_t unitToUInt2(float v){
	uint32_t normalU = punFU(v);
	uint32_t rbs = 126UL - (normalU >> 23UL);
//	printf("%x %lu\n", (normalU | 0x800000) << 8, rbs);
//	printf("%x\n", 0x80000000UL >> rbs);
	return (((normalU | 0x800000UL) << 8UL) & (~ULONG_MAX | 0xffffffffUL)) >> rbs;

//	uint32_t normalU = punFU(v);
//	uint32_t rbs = 118UL - ((normalU >> 23UL) & (~ULONG_MAX | 0x7fffffUL));
////	printf("%x %lu\n", (normalU | 0x800000) << 8, rbs);
////	printf("%x\n", 0x80000000UL >> rbs);
//	return ((normalU & (~ULONG_MAX | 0xffffffUL)) | 0x800000UL) >> rbs;
////	return (((normalU | 0x800000UL) << 8UL) & (~ULONG_MAX | 0xffffffffUL)) >> rbs;

//Her00	
//float y = v + 1.f; 
//return ((unsigned long&)v) & 0x7FFFFF;      // last 23 bits 
}

inline uint8_t unitToUInt8(float u){
	++u;
	return (punFU(u) >> 15) & MaskFrac<float>();
}

} // al::

#endif
