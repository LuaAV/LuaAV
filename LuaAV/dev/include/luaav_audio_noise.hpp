#ifndef INCLUDE_LUAAV_AUDIO_NOISE_H
#define INCLUDE_LUAAV_AUDIO_NOISE_H 1

#include "luaav_audio_scl.hpp"

/// Linear congruential uniform pseudo-random number generator.

///	This generator is very fast requiring only a single integer multiply and add per
/// iteration.  However, the least significant bits of the numbers are less 
/// random; the most extreme case being the LSB which at best flips between 0 and 1.
/// This generator also exhibits poor dimensional distribution, therefore it is
/// best to have a different generator for each dimension, rather than sharing one.
struct RNGLinCon {
	RNGLinCon(unsigned int seed)
	: val(seed), mul(1), add(0) {	
		type(0); 
	}
	
	/// Change the type of equation used.
	/// 0 - Knuth, Numerical Recipes in C\n
	/// 1 - BCPL
	void type(int v){
		switch(v){
			case 1:	mul = 2147001325; add =  715136305; break; // BCPL
			default:mul =    1664525; add = 1013904223;        // Knuth, Numerical Recipes in C
		}
	}
	
	inline double operator()() const { 
		val = val*mul+add; 
		return float(int(val)) / 2147483648.f;
	}	
	
	mutable unsigned int val;
	unsigned int mul, add;
};


/// White noise

/// White noise has a uniform power spectrum.
///
struct NoiseWhite {
	
	NoiseWhite(unsigned int seed) : rng(seed) {}
	
	inline double operator()() const { return rng(); }	///< Generate next value
	
	RNGLinCon rng;
};

/// Brownian noise

/// Brownian noise has a power spectrum of 1/f^2.
/// It is produced by integrating white (uniform) noise.
struct NoiseBrown {
	
	NoiseBrown(unsigned int seed) 
	: rng(seed), val(0), step(0.04) {}
	
	inline double operator()() const { 
		val = scl::clip(val + rng() * step, 1, -1);
		return val;
	}
	
	RNGLinCon rng;
	mutable double val, step;
};


inline unsigned int trailingZeroes(unsigned int v){ 
	static const unsigned int deBruijnBitPosition[32] = {
		 0, 1,28, 2,29,14,24, 3,30,22,20,15,25,17, 4, 8, 
		31,27,13,23,21,19,16, 7,26,12,18, 6,11, 5,10, 9
	};
	v = (v & -v);
	return deBruijnBitPosition[((unsigned int)(v * 0x077CB531UL)) >> 27];
}

/// Pink Noise

/// Pink noise has a power spectrum of 1/f.
/// It is produced by summing together 12 octaves of white noise.
struct NoisePink {
	
	NoisePink(unsigned int seed) 
	: rng(seed) {
		mRunningSum = 0.f;
		for(unsigned int i=0; i<11; ++i){	/* init octaves with uniform randoms */
			double r = rng();
			mOctave[i] = r;
			mRunningSum += r;
		}
		mPhasor = 0;
	}
	
	
	inline double operator()() const { 
		// phasor range:	[1, 2048]
		//					[0, 10]		trailing zeroes
		mPhasor++;
		if(mPhasor != 2048){
			unsigned int i = trailingZeroes(mPhasor);	// # trailing zeroes is octave
			float r = rng();						// uniform random
			mRunningSum += r - mOctave[i];			// add new & subtract stored
			mOctave[i] = r;							// store new
		}
		else{
			mPhasor = 0;
		}
		
		// add white noise every double
		return (mRunningSum + rng()) * 0.083333333f;
	}
	
	RNGLinCon rng;
		
	mutable double mOctave[11];
	mutable unsigned int mPhasor;
	mutable double mRunningSum;
};

#endif /* include guard */
