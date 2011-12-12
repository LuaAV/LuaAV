#ifndef INCLUDE_LUAAV_AUDIO_SPATIAL_H
#define INCLUDE_LUAAV_AUDIO_SPATIAL_H 1

#include "luaav_audio_scl.hpp"

class Pan2 {
public:
	Pan2(double pos=0){ w1=w2=0.707106781186548; }
	
	double left(double in){
		return in * w1; 
	}
	double right(double in){
		return in * w2; 
	}
	double left(double in1, double in2){
		return in1 * w1 + in2 * w2;
	}
	double right(double in1, double in2){
		return in1 * w2 + in2 * w1;
	}
	
	/// Set position using a quadratic approximation.
	void pos(double v){
		// gives correct result at -1, 0, and 1
		static const double c0 = 0.707106781186548;		//1./sqrt(2);
		static const double c1 = -0.207106781186548;	//0.5 - c0;
		static const double c2 = 2.41421356237309;		//-0.5/c1;
		
		v = (v < -1.) ? -1. : (v > 1.) ? 1. : v;
		w1 = c1 * v * (v + c2) + c0;
		w2 = w1 + v;
	}
	
	double w1, w2; // channel weights
};


#endif /* include guard */
