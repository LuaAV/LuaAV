#ifndef INCLUDE_LUAAV_AUDIO_OSCILLATOR_H
#define INCLUDE_LUAAV_AUDIO_OSCILLATOR_H 1

#include "luaav_audio_scl.hpp"

/// accumulating phasor wrapped in range [0, 1)
class Phasor {
public:
	Phasor(double srate=44100.0, double phase=0) : mPhase(phase), mISR(1. / srate) {
		freq(440);
	}
	
	inline void freq(double v) {		///< Set frequency.
		mFreq = v;
		mPhaseInc = v * mISR;
	}
	
	inline void phase(double v) {
		mPhase = v;
	}
	
	inline double operator()(){ 
		double r = mPhase;
		mPhase = scl::wrap1(mPhase + mPhaseInc);
		return r;
	}
	
	double mFreq;
	double mPhase, mISR, mPhaseInc;
};



/// This oscillator uses a Taylor series approximation to compute sine 
/// values. Computation time is about as much as a linearly-interpolating table 
/// lookup. In most cases, Taylor series are also more spectrally pure than 
/// table lookup methods since the distortion arises as harmonics.
class SinOsc : public Phasor {
public:
	SinOsc(double srate=44100.0, double phase=0) : Phasor(srate, phase) {}
	
	inline double operator()(){ 
		double r = Phasor::operator()();
				
		// convert [0, 1) phase to [-1, 1)
		return scl::sinP9(r*2.-1.);
	}
};


/// Variable harmonic impulse wave
class Buzz : public Phasor {
public:

	Buzz(double srate=44100.0, double harmonics=8, double phase=0) : Phasor(srate, phase), mAmp(0), mPrev(0.) {
		mSPU_2 = srate * 0.5;
		this->harmonics(harmonics);
	}
	
	void harmonics(double value){
		mN = mNDesired = floor(value);
		setAmp();
		mNFrac = value - mN;
	}
	
	inline void antialias() {
		float maxN = floor(maxHarmonics());
		mN = mNDesired > maxN ? maxN : mNDesired;
		setAmp();
	}
	
	void setAmp() { 
		mAmp = (mN != 0.) ? (0.5 / mN) : 0.0; 
	}
	
	double maxHarmonics(){ return mSPU_2 / mFreq; }

	inline double operator()() {		///< Returns next double of all harmonic impulse
		double theta = M_PI * (Phasor::operator()()*2.-1.);

		double result;
		double denom = scl::sinT9(theta * 0.5);
		if( fabs(denom) < 0.000001 ) { 
			result = 1.; /*printf("Impulse::(): oops\n");*/ 
		} else {
			double nphase = scl::wrapPhase(theta * (mN + 0.5));
			result = ((scl::sinT9(nphase) - denom) / denom) * mAmp;
		}
		
		return result;
	}
	
	inline double odd() {
		double theta = M_PI * (Phasor::operator()()*2.-1.);

		double result;

		double n = scl::ceil(mN*0.5)*2.;	// get next highest even for formula
								// wave has odd harmonics 1,3, ..., n-1 with peak amp n

		double denom = scl::cosT8(scl::wrapPhase(theta - M_PI_2));	// this is more precise near zero-crossings
		if( fabs(denom) < 0.000001 ){
			if( theta > M_PI ) theta -= M_2PI;
			result = (theta > -M_PI_2 && theta < M_PI_2) ? 1 : -1;
		}
		else result = scl::sinT9(scl::wrapPhase(n * theta)) / (denom * n);
		
		return result;
	}
	
	inline double saw(double intg=0.993){ return mPrev=(*this)()*0.125 + intg*mPrev; }
	inline double square(double intg=0.993){ return mPrev=odd()*0.125 + intg*mPrev; }
	
	double mAmp;			// amplitude normalization factor
	double mN;				// # harmonics
	double mNDesired;		// desired number of harmonics
	double mNFrac;		
	double mSPU_2;			// cached locals
	double mPrev;			// previous output for integration
};



/// Sine oscillator based on an efficient recursion equation.
/// This oscillator is based on a recursion equation requiring only one
/// multiply and add per double computation. While calculation is fast, frequency
/// and phase updates are rather expensive and 64-bit precision is required 
/// to prevent growing or decaying in amplitude over time.  This generator is 
/// ideal in situations where a stationary sinusoid is all that is required, 
/// e.g. a grain or modulator.
class SineR {
public:

	/// @param[in]	frq		Frequency
	/// @param[in]	amp		Amplitude
	/// @param[in]	phs		Unit phase in [0, 1)
	SineR(double srate=44100, double frq=440, double phs=0) 
	: mISR(1. / srate) { 
		double f=frq*M_2PI;
		double p=phs*M_2PI;
		mul  = 2 * cos(f);
		val2 = sin(p - f * 2.);
		val  = sin(p - f     );
	}

	/// Generate next value.
	inline double operator()() const {
		double v0 = mul * val - val2;
		val2 = val;
		return val = v0;
	}

	double mISR;
	mutable double val, val2;
	double mul;	
};

#endif /* include guard */
