#include "types/al_Color.hpp"

namespace al{

Color& Color::operator= (const HSV& hsv){
	
	float h=hsv.h*6.f, s=hsv.s, v=hsv.v;
	
	if(s == 0.f){	// achromatic (gray)
		return set(v);
	}
										// sector 0 to 5
	unsigned int i = (unsigned int)(h);	// integer part of h
	float f = h - float(i);				// fractional part of h
	float p = v * (1.f - s);

	// depends on hue section being even or odd
	float q = v * (1.f - s*( (i & 1U) ? f : (1.f - f) ));

	switch( i ) {
		case 0: r=v; g=q; b=p; break;
		case 1:	r=q; g=v; b=p; break;
		case 2:	r=p; g=v; b=q; break;
		case 3:	r=p; g=q; b=v; break;
		case 4: r=q; g=p; b=v; break;
		default:r=v; g=p; b=q; break;
	}
	
	return *this;
}


HSV& HSV::operator= (const Color& c){

	float r=c.r, g=c.g, b=c.b;

	float min = r<g ? (r<b ? r:b) : (g<b ? g:b);
	float max = r>g ? (r>b ? r:b) : (g>b ? g:b);

	v = max;							// set value
	float rng = max - min;			// range of RGB components

	if(rng != 0.f && max != 0.f){		// chromatic data...
		s = rng / max;					// set saturation
		
		float hl;
		if     (r == max)	hl =       (g - b)/rng;	// between yellow & magenta
		else if(g == max)	hl = 2.f + (b - r)/rng;	// between cyan & yellow
		else				hl = 4.f + (r - g)/rng;	// between magenta & cyan

		if(hl < 0.f) hl += 6.f;

		static const float _1_6 = 1.f/6.f;
		h = hl * _1_6;
	}
	else{ h=s=0.f; }					// this is a gray, no chroma...
	
	return *this;
}


} // al::
