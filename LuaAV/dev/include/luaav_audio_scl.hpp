#ifndef INCLUDE_LUAAV_AUDIO_SCL_H
#define INCLUDE_LUAAV_AUDIO_SCL_H 1

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288 
#define M_PI_2      1.57079632679489661923132169163975144   /* pi/2 */
#define M_PI_4      0.785398163397448309615660845819875721  /* pi/4 */
#endif
#ifndef M_2PI
#define M_2PI		6.283185307179586231941716828464095101		// 2pi
#endif
#ifndef M_1_2PI
#define M_1_2PI		0.159154943091895345554011992339482617		// 1/(2pi)
#endif

extern "C" {
	double sin ( double );
	double cos ( double );
	double fmod ( double, double );
	double floor ( double );
	double fabs(double);
}

namespace scl {

	// wrap phase at [0, lim)
	inline double wrap1(double v, double lim=1.0f) {
		if (v >= 0.0f) {
			return fmod(v, lim);
		} else {
			return lim - fmod(-v, lim);
		}
	}	

	inline double wrapPhase(double r){
		if(r >= M_PI){
			r -= M_2PI;
			if(r < M_PI) return r;
		}
		else if (r < -M_PI){
			r += M_2PI;
			if(r >= -M_PI) return r;
		}
		else return r;
		return r - M_2PI * floor((r + M_PI) * M_1_2PI);
	}

	inline double clip(double v, double hi, double lo){
			 if(v < lo) return lo;
		else if(v > hi)	return hi;
		return v;
	}

	inline double round(double v){
		static const double roundMagic = 6755399441055744.; // 2^52 * 1.5
		return (v + roundMagic) - roundMagic;
	}

	inline double ceil(double v){ return round(v + 0.499999985); }
	
	

	inline double sinP9(double n) {
		double nn = n*n;
		return n * ((double)3.1415191 + nn * ((double)-5.1662729 + nn * ((double)2.5422065 + nn * ((double)-0.5811243 + nn * (double)0.0636716))));
	} 

	inline double sinT9(double r){
		if(r < M_PI_4 && r > -M_PI_4){
			double rr = r*r;
			return r * (1. - 2.7557319224e-06 * rr * (60480. - rr * (3024. - rr * (72. - rr))));
		} else if(r > 0.){
			r -= M_PI_2;
			double rr = r*r;
			return 1. - rr * 2.7557319224e-07 * (1814400. - rr * (151200. - rr * (5040. - rr * (90. - rr))));
		} else {
			r += M_PI_2;
			double rr = r*r;
			return -1. + rr * 2.7557319224e-07 * (1814400. - rr * (151200. - rr * (5040. - rr * (90. - rr))));
		}
	}

	inline double cosT8(double r) {
		if(r < M_PI_4 && r > -M_PI_4){
			double rr = r*r;
			return 1. - rr * 2.4801587302e-05 * (20160. - rr * (1680. - rr * (56. - rr)));
		} else if(r > 0.) {
			r -= M_PI_2;
			double rr = r*r;
			return -r * (1. - 1.9841269841e-04 * rr * (840. - rr * (42. - rr)));
		} else {
			r += M_PI_2;
			double rr = r*r;
			return r * (1. - 1.9841269841e-04 * rr * (840. - rr * (42. - rr)));
		}
	}

} // scl::

#endif /* include guard */
