#ifndef INCLUDE_AL_MATH_RANDOM_HPP
#define INCLUDE_AL_MATH_RANDOM_HPP

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

#include <time.h>					/* req'd for time() */
#include <cmath>
#include "allocore/types/al_Conversion.hpp"	/* req'd for int to float conversion */
#include "allocore/math/al_Constants.hpp"
#include "allocore/math/al_Generators.hpp"

namespace al {
namespace rnd{

class LinCon;
class MulLinCon;
class Tausworthe;
template<class RNG> class Random;


/// Get a random seed in interval [0, 4294967296)
inline static uint32_t seed(){
	static gen::RMulAdd<uint32_t> seedGen(1664525, 1013904223, time(NULL));
	return seedGen();
}


/// Random distribution generator
template <class RNG=al::rnd::Tausworthe>
class Random{
public:

	/// Returns uniform random in [0,1)
	float uniform(){ return al::uintToUnit<float>(mRNG()); }
	
	/// Returns uniform random in [-1,1)
	float uniformS(){ return al::uintToUnitS<float>(mRNG()); }

	/// Returns uniform random in [0, hi)
	template <class T>
	T uniform(const T& hi){ return hi*uniform();  }
	
	/// Returns uniform random in [lo, hi)
	template <class T>
	T uniform(const T& hi, const T& lo){ return (hi-lo)*uniform() + lo;  }

	/// Returns uniform random in [-lim, lim)
	template <class T>
	T uniformS(const T& lim){ return lim*uniformS(); }

	/// Returns random Gaussian
	float gaussian(){ float r; gaussian(r,r); return r; }

	/// Returns two random Gaussians
	template <class T>
	void gaussian(T& y1, T& y2);

	/// Returns true with a probability of p.
	bool prob(float p=0.5f){ return uniform() < p; }

	/// Randomly shuffles elements in array.
	template <class T>
	void shuffle(T * arr, uint32_t len);

protected:
	RNG mRNG;
};



/// Linear congruential uniform pseudo-random number generator.

///	This generator is very fast requiring only a single integer multiply and add per
/// iteration.  However, the least significant bits of the numbers are less 
/// random; the most extreme case being the LSB which at best flips between 0 and 1.
/// This generator also exhibits poor dimensional distribution, therefore it is
/// best to have a different generator for each dimension, rather than sharing one.
class LinCon : public gen::RMulAdd<uint32_t>{
public:

	LinCon(){ val=al::rnd::seed(); type(0); }

	/// @param[in] seed	Initial seed value
	LinCon(uint32_t seed): gen::RMulAdd<uint32_t>(1,0,seed){ type(0); }

	/// Change the type of equation used.
	
	/// 0 - Knuth, Numerical Recipes in C\n
	/// 1 - BCPL
	void type(int v){
		switch(v){
		case 1:	mul = 2147001325; add =  715136305; break; // BCPL
		default:mul =    1664525; add = 1013904223;        // Knuth, Numerical Recipes in C
		}
	}
};



/// Multiplicative linear congruential uniform pseudo-random number generator.

///	This generator is a faster LCG requiring only a single integer multiply.
///
class MulLinCon : public gen::RMul<uint32_t>{
public:

	MulLinCon(){ val=al::rnd::seed(); type(0); }
	
	/// @param[in] seed	Initial seed value
	MulLinCon(uint32_t seed): gen::RMul<uint32_t>(1,seed){ type(0); }
	
	/// Change the type of equation used.

	/// 0 - Marsaglia, Super-Duper\n
	///
	void type(int v){
		switch(v){
		default: mul = 69069;	// Super-duper
		}
	}
};



/// Combined Tausworthe uniform pseudo-random number generator.

/// This generator produces highly random numbers, but is more expensive than
/// than a linear congruential RNG.
/// It is based on the paper 
/// P. L'Ecuyer, "Maximally Equidistributed Combined Tausworthe Generators", 
/// Mathematics of Computation, 65, 213 (1996), 203--213.
/// http://www.iro.umontreal.ca/~lecuyer/papers.html
class Tausworthe{
public:

	Tausworthe();

	/// @param[in] seed		Initial seed value
	Tausworthe(uint32_t seed);
	
	uint32_t operator()();				///< Generates uniform random unsigned integer in [0, 2^32).
	void operator = (uint32_t seed);	///< Set seed
	void seed(uint32_t s);				///< Set seed
	void seed(uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4); ///< Set seed

private:
	uint32_t s1, s2, s3, s4;
	void iterate();
};


/// Get global random number generator
inline Random<>& globalRNG(){
	static Random<> r; return r;
}

/// Returns true with probability p
inline bool prob(float p=0.5){ return globalRNG().prob(p); }

/// Returns uniform random in [0,1)
inline float uniform(){ return globalRNG().uniform(); }

/// Returns uniform random in [0, hi)
template <class T>
inline T uniform(const T& hi){ return globalRNG().uniform(hi); }

/// Returns uniform random in [lo, hi)
template <class T>
inline T uniform(const T& hi, const T& lo){ return globalRNG().uniform(hi,lo); }

/// Returns signed uniform random in [0,1)
inline float uniformS(){ return globalRNG().uniformS(); }

/// Returns signed uniform random in [-lim, lim)
template <class T>
inline T uniformS(const T& lim){ return globalRNG().uniformS(lim); }


// Implementation_______________________________________________________________

//---- Tausworthe

inline Tausworthe::Tausworthe(){ seed(al::rnd::seed()); }
inline Tausworthe::Tausworthe(uint32_t sd){ seed(sd); }

inline uint32_t Tausworthe::operator()(){
	iterate();
	return s1 ^ s2 ^ s3 ^ s4;
}

inline void Tausworthe::operator=(uint32_t s){ seed(s); }

inline void Tausworthe::seed(uint32_t s){
	al::rnd::LinCon g(s);
	g();
	seed(g(), g(), g(), g());
}

inline void Tausworthe::seed(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4){
	//printf("%u %u %u %u\n", v1, v2, v3, v4);
	v1 & 0xffffffe ? s1 = v1 : s1 = ~v1;
	v2 & 0xffffff8 ? s2 = v2 : s2 = ~v2;
	v3 & 0xffffff0 ? s3 = v3 : s3 = ~v3;
	v4 & 0xfffff80 ? s4 = v4 : s4 = ~v4;
}
	
inline void Tausworthe::iterate(){
	s1 = ((s1 & 0xfffffffe) << 18) ^ (((s1 <<  6) ^ s1) >> 13);
	s2 = ((s2 & 0xfffffff8) <<  2) ^ (((s2 <<  2) ^ s2) >> 27);
	s3 = ((s3 & 0xfffffff0) <<  7) ^ (((s3 << 13) ^ s3) >> 21);
	s4 = ((s4 & 0xffffff80) << 13) ^ (((s4 <<  3) ^ s4) >> 12);
}



template <class RNG>
template <class T> void Random<RNG>::gaussian(T& y1, T& y2){
	float x1, x2, w;

	do{
		x1 = uniformS();
		x2 = uniformS();
		w = x1 * x1 + x2 * x2;
	} while(w >= 1.f);

	w = std::sqrt((-2.f * std::log(w)) / w);
	y1 = T(x1 * w);
	y2 = T(x2 * w);
}


// Fisher-Yates shuffle
template <class RNG>
template <class T>
void Random<RNG>::shuffle(T * arr, uint32_t len){
	for(uint32_t i=len-1; i>0; --i){
		uint32_t j = uniform(i+1);
		T t = arr[i];
		arr[i] = arr[j];
		arr[j] = t;
	}
}



} // al::rnd::
} // al::

#endif
