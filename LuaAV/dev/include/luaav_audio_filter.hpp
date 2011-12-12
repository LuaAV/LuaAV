#ifndef INCLUDE_LUAAV_AUDIO_FILTER_H
#define INCLUDE_LUAAV_AUDIO_FILTER_H 1

#include "luaav_audio_scl.hpp"



/// One-pole filter
class OnePole{
public:

	OnePole(const double& prev=0., const double& smooth=1., const double& gain=1.) {
		mo[1] = prev;
		set(smooth, gain);
	}

	inline OnePole& set(const double& smooth, const double& gain=1.){
		ma[0] = gain;
		mb[0] = smooth;
		return *this;
	}

	inline double operator()(const double& v) const {
		return mo[0] = mo[0]*mb[0] + v*ma[0];
	}

	mutable double mo[1];
	double ma[1], mb[1];
};


/// 2-pole/2-zero IIR filter.
/// The biquadratic filter contains 2 zeroes and 2 poles in its transfer
/// function. The zeroes provide a better response near the DC and Nyquist
/// frequencies than an all-pole filter would. Second-order IIRs have a 12 
/// dB/octave cutoff slope and are normally cascaded (run in series) to obtain
/// a sharper response. This particular implementation utilizes the Butterworth
/// design.
class Biquad {
public:
	
	enum Filter { LP, HP, BP, BPC, BR, AP };

	/// @param[in]	frq		Initial center frequency in Hz.
	/// @param[in]	res		Initial resonance.
	/// @param[in]	frq		Initial type of filter.
	Biquad(double srate, int filterType = LP)
	:	d1(0), d2(0), mISR(1./srate) {
		mFrqToRad = M_2PI * mISR;
		set(1000., 1., filterType);
	}
	
	void set(double frq, double res, int type) {	///< Set all filter params.
		mRes = res;
		mType = type;
		freq(frq);
	}
	
	inline void freq(double v) {					///< Set center frequency. 
		mFreq = v;
		float phs = scl::clip(mFreq * mFrqToRad, 3.11, 0.);
		mReal = scl::cosT8(phs);
		mImag = scl::sinT9(phs);
		res(mRes);
	}
	
	inline void res(double v){
		mRes = fabs(v);
		mAlpha = mImag / mRes;
		mB0 = 1. / (1. + mAlpha);	// reciprocal of mB0
		mB1 = -2. * mReal * mB0;
		mB2 = (1. - mAlpha) * mB0;
		
		type(mType);
	}
	
	inline void type(int typeA) {
		mType = typeA;
		switch(mType) {
			case LP:
				mA1 = (1. - mReal) * mB0;
				mA0 = mA1 * 0.5;
				mA2 = mA0;
				break;
			case HP:
				mA1 = (-1. + mReal) * mB0;
				mA0 = -mA1 * 0.5;
				mA2 = mA0;
				break;
			case BP:
				mA0 = mImag * 0.5 * mB0;
				mA1 = 0.;
				mA2 = -mA0;
				break;
			case BPC:
				mA0 = mAlpha * mB0;
				mA1 = 0.;
				mA2 = -mA0;
				break;
			case BR:
				mA0 = mB0;	// 1.f * a0
				mA1 = mB1;
				mA2 = mB0;	// 1.f * a0
				break;
			case AP:
				mA0 = mB2;
				mA1 = mB1;
				mA2 = mB0;
				break;
			default:;
		}
	}

//	/// Set input (a's) and output (b's) coefficients directly
//	void coef(double a0, double a1, double a2, double b0, double b1, double b2) {
//		mA0=a0; mA1=a1; mA2=a2; mB0=b0; mB1=b1; mB2=b2;
//	}
//	void zero() { d1=d2=0.; }						///< Zero internal delays.

	double operator()(double i0) {				///< Return next filter output.
		// Direct form II
		i0 = i0 - d1*mB1 - d2*mB2;
		double o0 = i0*mA0 + d1*mA1 + d2*mA2;
		d2 = d1; 
		d1 = i0;
		return o0;
	}
//	double nextBP(double i0) {					///< Optimized for band-pass (BP, BPC) types.
//		i0 = i0 - d1*mB1 - d2*mB2;	
//		double o0 = (i0 - d2)*mA0;
//		d2 = d1; d1 = i0;
//		return o0;
//	}
	
	double mA0, mA1, mA2, mB0, mB1, mB2;	// ffd and fbk coefficients
	double d1, d2;		// inner double delays
	double mFreq, mRes;	// center frequency, resonance
	int mType;
	double mReal, mImag;	// real, imag components of center frequency
	double mAlpha;
	double mFrqToRad;
	double mISR;
};


#endif /* include guard */
