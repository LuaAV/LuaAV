#include <time.h> /* used to seed random generator */
#include "c2ga.h"
namespace c2ga {
// Missing dependencies declarations:
/// Generates a random double in [0.0 1.0) interval using the c library rand() function
double random_double_dont_mangle_1_returns_double();
/// Seeds the random number generator for  double
void random_double_dont_mangle_1_returns_double_seed(unsigned int seed);
/// Seeds the random number generator for  double with the current time
void random_double_dont_mangle_1_returns_double_timeSeed();
/// Returns mv - mv.
mv subtract_dont_mangle_2_returns_mv(const mv &a, const mv &b);

/**
Generates a random versor.
The scale is uniformly distributed over the interval [0 1).
The maximum non-zero grade-part is 'grade'.
Only the basis vectors marked in 'basisVectorBitmap' will be used to generate the
versor/blade. Use 'basisVectorBitmap = -1' (the default) to use all basisvectors.

Returns random_versor_dont_mangle_3_returns_mv_ex(arg1, scale, grade, basisVectorBitmap, 0.01, scale * 4.0);
*/
mv random_versor_dont_mangle_3_returns_mv(double scale, int grade, int basisVectorBitmap = -1);

/**
This version of random_versor_dont_mangle_3_returns_mv() has extra arguments which help to avoid 
near-singular blades / versors.

Near-singular blades / versors are avoid by testing the norm and largest coordinate
of the random blade / versor. If the test does not pass, the function recursively
tries to generate another random blade / versor.

'minimumNorm' is the minimum allowed norm of the blade/versor before scaling. 
'minimumNorm' must be > 0.0 for versors.

'largestCoordinate' is the largest coordinate allowed after scaling.
*/
mv random_versor_dont_mangle_3_returns_mv_ex(double scale, int grade, int basisVectorBitmap, double minimumNorm, double largestCoordinate);

/// Returns mv + mv.
mv add_dont_mangle_4_returns_mv(const mv &a, const mv &b);
/// Returns random vectorE2GA with a scale in the interval [0, scale)
vectorE2GA random_vectorE2GA_dont_mangle_7_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random vectorE2GA with a scale in the interval [0, scale)
vectorE2GA random_vectorE2GA_dont_mangle_7(const double scale);
/// Returns random rotorE2GA with a scale in the interval [0, scale)
rotorE2GA random_rotorE2GA_dont_mangle_11_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random rotorE2GA with a scale in the interval [0, scale)
rotorE2GA random_rotorE2GA_dont_mangle_11(const double scale);
/// Returns random line with a scale in the interval [0, scale)
line random_line_dont_mangle_24_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random line with a scale in the interval [0, scale)
line random_line_dont_mangle_24(const double scale);
/// Returns random circle with a scale in the interval [0, scale)
circle random_circle_dont_mangle_27_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random circle with a scale in the interval [0, scale)
circle random_circle_dont_mangle_27(const double scale);
/// Returns random bivectorE2GA with a scale in the interval [0, scale)
bivectorE2GA random_bivectorE2GA_dont_mangle_31_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random bivectorE2GA with a scale in the interval [0, scale)
bivectorE2GA random_bivectorE2GA_dont_mangle_31(const double scale);
/// Returns random normalizedPoint with a scale in the interval [0, scale)
normalizedPoint random_normalizedPoint_dont_mangle_35_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random normalizedPoint with a scale in the interval [0, scale)
normalizedPoint random_normalizedPoint_dont_mangle_35(const double scale);
/// Returns random e1_t with a scale in the interval [0, scale)
e1_t random_e1_t_dont_mangle_39_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random e1_t with a scale in the interval [0, scale)
e1_t random_e1_t_dont_mangle_39(const double scale);
/// Returns random e2_t with a scale in the interval [0, scale)
e2_t random_e2_t_dont_mangle_40_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random e2_t with a scale in the interval [0, scale)
e2_t random_e2_t_dont_mangle_40(const double scale);
/// Returns random no_t with a scale in the interval [0, scale)
no_t random_no_t_dont_mangle_43_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random no_t with a scale in the interval [0, scale)
no_t random_no_t_dont_mangle_43(const double scale);
/// Returns random ni_t with a scale in the interval [0, scale)
ni_t random_ni_t_dont_mangle_44_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random ni_t with a scale in the interval [0, scale)
ni_t random_ni_t_dont_mangle_44(const double scale);

/**
Generates a random blade.
The scale is uniformly distributed over the interval [0 1).
The maximum non-zero grade-part is 'grade'.
Only the basis vectors marked in 'basisVectorBitmap' will be used to generate the
versor/blade. Use 'basisVectorBitmap = -1' (the default) to use all basisvectors.

Returns random_blade_dont_mangle_47_returns_mv_ex(arg1, scale, grade, basisVectorBitmap, 0.01, scale * 4.0);
*/
mv random_blade_dont_mangle_47_returns_mv(double scale, int grade, int basisVectorBitmap = -1);

/**
This version of random_blade_dont_mangle_47_returns_mv() has extra arguments which help to avoid 
near-singular blades / versors.

Near-singular blades / versors are avoid by testing the norm and largest coordinate
of the random blade / versor. If the test does not pass, the function recursively
tries to generate another random blade / versor.

'minimumNorm' is the minimum allowed norm of the blade/versor before scaling. 
'minimumNorm' must be > 0.0 for versors.

'largestCoordinate' is the largest coordinate allowed after scaling.
*/
mv random_blade_dont_mangle_47_returns_mv_ex(double scale, int grade, int basisVectorBitmap, double minimumNorm, double largestCoordinate);

/// Returns grade groupBitmap of  mv.
mv extractGrade_dont_mangle_48_returns_mv(const mv &a, int groupBitmap);
/// Returns random pseudoscalar with a scale in the interval [0, scale)
pseudoscalar random_pseudoscalar_dont_mangle_79_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random pseudoscalar with a scale in the interval [0, scale)
pseudoscalar random_pseudoscalar_dont_mangle_79(const double scale);
/// Returns random oddVersor with a scale in the interval [0, scale)
oddVersor random_oddVersor_dont_mangle_80_ex(const double scale, const double minimumNorm, const double largestCoordinate);
/// Returns random oddVersor with a scale in the interval [0, scale)
oddVersor random_oddVersor_dont_mangle_80(const double scale);
/// Generates a random double in [0.0 1.0) interval using the c library rand() function
double random_double_dont_mangle_89();
/// Seeds the random number generator for  double
void random_double_dont_mangle_89_seed(unsigned int seed);
/// Seeds the random number generator for  double with the current time
void random_double_dont_mangle_89_timeSeed();
/// Returns geometric product of mv and mv.
mv gp_dont_mangle_92_returns_mv(const mv &a, const mv &b);
/// Returns norm of vectorE2GA using default metric.
double norm_dont_mangle_94(const vectorE2GA &a);
/// internal conversion function
double norm_dont_mangle_94_returns_scalar(const vectorE2GA &a);
/// Returns norm of mv using default metric.
double norm_dont_mangle_96(const mv &a);
/// internal conversion function
double norm_dont_mangle_96_returns_scalar(const mv &a);
/// Returns norm of rotorE2GA using default metric.
double norm_dont_mangle_98(const rotorE2GA &a);
/// internal conversion function
double norm_dont_mangle_98_returns_scalar(const rotorE2GA &a);
/// Returns geometric product of mv and double.
mv gp_dont_mangle_99(const mv &a, const double b);
/// Returns norm of line using default metric.
double norm_dont_mangle_101(const line &a);
/// internal conversion function
double norm_dont_mangle_101_returns_scalar(const line &a);
/// Returns norm of circle using default metric.
double norm_dont_mangle_103(const circle &a);
/// internal conversion function
double norm_dont_mangle_103_returns_scalar(const circle &a);
/// Returns norm of bivectorE2GA using default metric.
double norm_dont_mangle_105(const bivectorE2GA &a);
/// internal conversion function
double norm_dont_mangle_105_returns_scalar(const bivectorE2GA &a);
/// Returns norm of normalizedPoint using default metric.
double norm_dont_mangle_108(const normalizedPoint &a);
/// internal conversion function
double norm_dont_mangle_108_returns_scalar(const normalizedPoint &a);
/// Returns norm of e1_t using default metric.
double norm_dont_mangle_109(const e1_t &a);
/// internal conversion function
double norm_dont_mangle_109_returns_scalar(const e1_t &a);
/// Returns norm of e2_t using default metric.
double norm_dont_mangle_111(const e2_t &a);
/// internal conversion function
double norm_dont_mangle_111_returns_scalar(const e2_t &a);
/// Returns norm of no_t using default metric.
double norm_dont_mangle_114(const no_t &a);
/// internal conversion function
double norm_dont_mangle_114_returns_scalar(const no_t &a);
/// Returns norm of ni_t using default metric.
double norm_dont_mangle_116(const ni_t &a);
/// internal conversion function
double norm_dont_mangle_116_returns_scalar(const ni_t &a);
/// Returns norm of pseudoscalar using default metric.
double norm_dont_mangle_118(const pseudoscalar &a);
/// internal conversion function
double norm_dont_mangle_118_returns_scalar(const pseudoscalar &a);
/// Returns norm of oddVersor using default metric.
double norm_dont_mangle_119(const oddVersor &a);
/// internal conversion function
double norm_dont_mangle_119_returns_scalar(const oddVersor &a);
// Missing dependencies inline definitions:
inline double random_double_dont_mangle_1_returns_double() {
return (double)((double)(rand() & 0x7FFF) / 32768.0) + 
	(double)((double)(rand() & 0x7FFF) / (32768.0 * 32768.0)) + 
	(double)((double)(rand() & 0x7FFF) / (32768.0 * 32768.0 * 32768.0)) + 
	(double)((double)(rand() & 0x7FFF) / (32768.0 * 32768.0 * 32768.0 * 32768.0)); 
}
inline void random_double_dont_mangle_1_returns_double_seed(unsigned int seed) {
	srand(seed);
}

inline void random_double_dont_mangle_1_returns_double_timeSeed() {
	random_double_dont_mangle_1_returns_double_seed((unsigned int)time(NULL));
}

inline vectorE2GA random_vectorE2GA_dont_mangle_7_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	vectorE2GA tmp;
	double n, mul, lc;
	bool nullBlade;
	double re1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), re2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_e1 = re1;
	tmp.m_e2 = re2;

	n = norm_dont_mangle_94_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = ((n == 0.0) && (lc != 0.0));
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_vectorE2GA_dont_mangle_7_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_vectorE2GA_dont_mangle_7_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return vectorE2GA(vectorE2GA::coord_e1_e2,
			mul*tmp.m_e1, // e1
			mul*tmp.m_e2 // e2
		);
}
inline vectorE2GA random_vectorE2GA_dont_mangle_7(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_vectorE2GA_dont_mangle_7_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline rotorE2GA random_rotorE2GA_dont_mangle_11_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	rotorE2GA tmp;
	double n, mul, lc;
	bool nullBlade;
	double r0_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), 
			r1_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_scalar = (r0_e1*r1_e1+r0_e2*r1_e2);
	tmp.m_e1_e2 = (r0_e1*r1_e2-r0_e2*r1_e1);

	n = norm_dont_mangle_98_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = false;
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_rotorE2GA_dont_mangle_11_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_rotorE2GA_dont_mangle_11_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return rotorE2GA(rotorE2GA::coord_scalar_e1e2,
			mul*tmp.m_scalar, // scalar
			mul*tmp.m_e1_e2 // e1_e2
		);
}
inline rotorE2GA random_rotorE2GA_dont_mangle_11(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_rotorE2GA_dont_mangle_11_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline line random_line_dont_mangle_24_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	line tmp;
	double n, mul, lc;
	bool nullBlade;
	double re1_e2_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), re1_no_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), re2_no_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_e1_e2_ni = re1_e2_ni;
	tmp.m_e1_no_ni = re1_no_ni;
	tmp.m_e2_no_ni = re2_no_ni;

	n = norm_dont_mangle_101_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = ((n == 0.0) && (lc != 0.0));
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_line_dont_mangle_24_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_line_dont_mangle_24_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return line(line::coord_e1e2ni_e1noni_e2noni,
			mul*tmp.m_e1_e2_ni, // e1_e2_ni
			mul*tmp.m_e1_no_ni, // e1_no_ni
			mul*tmp.m_e2_no_ni // e2_no_ni
		);
}
inline line random_line_dont_mangle_24(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_line_dont_mangle_24_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline circle random_circle_dont_mangle_27_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	circle tmp;
	double n, mul, lc;
	bool nullBlade;
	double rno_e1_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), rno_e1_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), rno_e2_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), re1_e2_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_no_e1_e2 = rno_e1_e2;
	tmp.m_no_e1_ni = rno_e1_ni;
	tmp.m_no_e2_ni = rno_e2_ni;
	tmp.m_e1_e2_ni = re1_e2_ni;

	n = norm_dont_mangle_103_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = ((n == 0.0) && (lc != 0.0));
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_circle_dont_mangle_27_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_circle_dont_mangle_27_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return circle(circle::coord_noe1e2_noe1ni_noe2ni_e1e2ni,
			mul*tmp.m_no_e1_e2, // no_e1_e2
			mul*tmp.m_no_e1_ni, // no_e1_ni
			mul*tmp.m_no_e2_ni, // no_e2_ni
			mul*tmp.m_e1_e2_ni // e1_e2_ni
		);
}
inline circle random_circle_dont_mangle_27(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_circle_dont_mangle_27_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline bivectorE2GA random_bivectorE2GA_dont_mangle_31_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	bivectorE2GA tmp;
	double n, mul, lc;
	bool nullBlade;
	double r0_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), 
			r1_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_e1_e2 = (r0_e1*r1_e2-r0_e2*r1_e1);

	n = norm_dont_mangle_105_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = ((n == 0.0) && (lc != 0.0));
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_bivectorE2GA_dont_mangle_31_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_bivectorE2GA_dont_mangle_31_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return bivectorE2GA(bivectorE2GA::coord_e1e2,
			mul*tmp.m_e1_e2 // e1_e2
		);
}
inline bivectorE2GA random_bivectorE2GA_dont_mangle_31(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_bivectorE2GA_dont_mangle_31_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline normalizedPoint random_normalizedPoint_dont_mangle_35_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	normalizedPoint tmp;
	double n, mul, lc;
	bool nullBlade;
	double re1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), re2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), rni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_e1 = re1;
	tmp.m_e2 = re2;
	tmp.m_ni = rni;

	n = norm_dont_mangle_108_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = ((n == 0.0) && (lc != 0.0));
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_normalizedPoint_dont_mangle_35_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_normalizedPoint_dont_mangle_35_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return normalizedPoint(normalizedPoint::coord_e1_e2_ni,
			mul*tmp.m_e1, // e1
			mul*tmp.m_e2, // e2
			mul*tmp.m_ni // ni
		);
}
inline normalizedPoint random_normalizedPoint_dont_mangle_35(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_normalizedPoint_dont_mangle_35_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline e1_t random_e1_t_dont_mangle_39_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	return e1_t(		);
}
inline e1_t random_e1_t_dont_mangle_39(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_e1_t_dont_mangle_39_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline e2_t random_e2_t_dont_mangle_40_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	return e2_t(		);
}
inline e2_t random_e2_t_dont_mangle_40(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_e2_t_dont_mangle_40_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline no_t random_no_t_dont_mangle_43_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	return no_t(		);
}
inline no_t random_no_t_dont_mangle_43(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_no_t_dont_mangle_43_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline ni_t random_ni_t_dont_mangle_44_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	return ni_t(		);
}
inline ni_t random_ni_t_dont_mangle_44(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_ni_t_dont_mangle_44_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline pseudoscalar random_pseudoscalar_dont_mangle_79_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	pseudoscalar tmp;
	double n, mul, lc;
	bool nullBlade;
	double rno_e1_e2_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_no_e1_e2_ni = rno_e1_e2_ni;

	n = norm_dont_mangle_118_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = ((n == 0.0) && (lc != 0.0));
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_pseudoscalar_dont_mangle_79_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_pseudoscalar_dont_mangle_79_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return pseudoscalar(pseudoscalar::coord_noe1e2ni,
			mul*tmp.m_no_e1_e2_ni // no_e1_e2_ni
		);
}
inline pseudoscalar random_pseudoscalar_dont_mangle_79(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_pseudoscalar_dont_mangle_79_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline oddVersor random_oddVersor_dont_mangle_80_ex(const double scale, const double minimumNorm, const double largestCoordinate)
{
	oddVersor tmp;
	double n, mul, lc;
	bool nullBlade;
	double r0_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r0_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), 
			r1_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r1_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), 
			r2_no = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r2_e1 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r2_e2 = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double(), r2_ni = -1.0 + 2.0 * random_double_dont_mangle_1_returns_double();
	tmp.m_no = (r0_e1*r1_e1*r2_no-r0_e1*r1_no*r2_e1+r0_e2*r1_e2*r2_no-r0_e2*r1_no*r2_e2+r0_no*r1_e1*r2_e1+r0_no*r1_e2*r2_e2+-2.0*r0_no*r1_ni*r2_no);
	tmp.m_e1 = (r0_e1*r1_e1*r2_e1+r0_e1*r1_e2*r2_e2-r0_e1*r1_ni*r2_no-r0_e1*r1_no*r2_ni-r0_e2*r1_e1*r2_e2+r0_e2*r1_e2*r2_e1+r0_ni*r1_e1*r2_no-r0_ni*r1_no*r2_e1+r0_no*r1_e1*r2_ni-r0_no*r1_ni*r2_e1);
	tmp.m_e2 = (r0_e1*r1_e1*r2_e2-r0_e1*r1_e2*r2_e1+r0_e2*r1_e1*r2_e1+r0_e2*r1_e2*r2_e2-r0_e2*r1_ni*r2_no-r0_e2*r1_no*r2_ni+r0_ni*r1_e2*r2_no-r0_ni*r1_no*r2_e2+r0_no*r1_e2*r2_ni-r0_no*r1_ni*r2_e2);
	tmp.m_ni = (r0_e1*r1_e1*r2_ni-r0_e1*r1_ni*r2_e1+r0_e2*r1_e2*r2_ni-r0_e2*r1_ni*r2_e2+r0_ni*r1_e1*r2_e1+r0_ni*r1_e2*r2_e2+-2.0*r0_ni*r1_no*r2_ni);
	tmp.m_no_e1_e2 = (r0_e1*r1_e2*r2_no-r0_e1*r1_no*r2_e2-r0_e2*r1_e1*r2_no+r0_e2*r1_no*r2_e1+r0_no*r1_e1*r2_e2-r0_no*r1_e2*r2_e1);
	tmp.m_no_e1_ni = (r0_e1*r1_ni*r2_no-r0_e1*r1_no*r2_ni-r0_ni*r1_e1*r2_no+r0_ni*r1_no*r2_e1+r0_no*r1_e1*r2_ni-r0_no*r1_ni*r2_e1);
	tmp.m_no_e2_ni = (r0_e2*r1_ni*r2_no-r0_e2*r1_no*r2_ni-r0_ni*r1_e2*r2_no+r0_ni*r1_no*r2_e2+r0_no*r1_e2*r2_ni-r0_no*r1_ni*r2_e2);
	tmp.m_e1_e2_ni = (r0_e1*r1_e2*r2_ni-r0_e1*r1_ni*r2_e2-r0_e2*r1_e1*r2_ni+r0_e2*r1_ni*r2_e1+r0_ni*r1_e1*r2_e2-r0_ni*r1_e2*r2_e1);

	n = norm_dont_mangle_119_returns_scalar(tmp);
	lc = tmp.largestCoordinate();
	nullBlade = false;
	// Recurse if generated random value has a norm below user-supplied limit, unless this is a null blade
	if ((n < minimumNorm) && (!nullBlade)) {
		return random_oddVersor_dont_mangle_80_ex(scale, minimumNorm, largestCoordinate);
	}
	// Compute multiplier
	if (n < 0.0001) {
		mul = 1.0;
	}
	else {
		mul = scale * (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double()) / n;
		// Test largest coordinate
		if ((lc * ::fabs(mul)) > largestCoordinate ) {
			return random_oddVersor_dont_mangle_80_ex(scale, minimumNorm, largestCoordinate);
		}
	}
	// Apply multiplier, return
	return oddVersor(oddVersor::coord_no_e1_e2_ni_noe1e2_noe1ni_noe2ni_e1e2ni,
			mul*tmp.m_no, // no
			mul*tmp.m_e1, // e1
			mul*tmp.m_e2, // e2
			mul*tmp.m_ni, // ni
			mul*tmp.m_no_e1_e2, // no_e1_e2
			mul*tmp.m_no_e1_ni, // no_e1_ni
			mul*tmp.m_no_e2_ni, // no_e2_ni
			mul*tmp.m_e1_e2_ni // e1_e2_ni
		);
}
inline oddVersor random_oddVersor_dont_mangle_80(const double scale)
{
	double minimumNorm = 0.0001;
	double largestCoordinate = 4.0;
	return random_oddVersor_dont_mangle_80_ex(scale, minimumNorm, scale * largestCoordinate);
}
inline double random_double_dont_mangle_89() {
return (double)((double)(rand() & 0x7FFF) / 32768.0) + 
	(double)((double)(rand() & 0x7FFF) / (32768.0 * 32768.0)) + 
	(double)((double)(rand() & 0x7FFF) / (32768.0 * 32768.0 * 32768.0)) + 
	(double)((double)(rand() & 0x7FFF) / (32768.0 * 32768.0 * 32768.0 * 32768.0)); 
}
inline void random_double_dont_mangle_89_seed(unsigned int seed) {
	srand(seed);
}

inline void random_double_dont_mangle_89_timeSeed() {
	random_double_dont_mangle_89_seed((unsigned int)time(NULL));
}

inline double norm_dont_mangle_94(const vectorE2GA &a)
{
	return ::fabs(::sqrt(::fabs((a.m_e1*a.m_e1+a.m_e2*a.m_e2))));

}
inline double norm_dont_mangle_94_returns_scalar(const vectorE2GA &a) {
	return norm_dont_mangle_94(a);
}
inline double norm_dont_mangle_96_returns_scalar(const mv &a) {
	return norm_dont_mangle_96(a);
}
inline double norm_dont_mangle_98(const rotorE2GA &a)
{
	return ::fabs(::sqrt(::fabs((a.m_e1_e2*a.m_e1_e2+a.m_scalar*a.m_scalar))));

}
inline double norm_dont_mangle_98_returns_scalar(const rotorE2GA &a) {
	return norm_dont_mangle_98(a);
}
inline double norm_dont_mangle_101(const line &a)
{
	return ::fabs(::sqrt(::fabs((-a.m_e1_no_ni*a.m_e1_no_ni-a.m_e2_no_ni*a.m_e2_no_ni))));

}
inline double norm_dont_mangle_101_returns_scalar(const line &a) {
	return norm_dont_mangle_101(a);
}
inline double norm_dont_mangle_103(const circle &a)
{
	return ::fabs(::sqrt(::fabs((-2.0*a.m_e1_e2_ni*a.m_no_e1_e2-a.m_no_e1_ni*a.m_no_e1_ni-a.m_no_e2_ni*a.m_no_e2_ni))));

}
inline double norm_dont_mangle_103_returns_scalar(const circle &a) {
	return norm_dont_mangle_103(a);
}
inline double norm_dont_mangle_105(const bivectorE2GA &a)
{
	return ::fabs(::sqrt(::fabs(a.m_e1_e2*a.m_e1_e2)));

}
inline double norm_dont_mangle_105_returns_scalar(const bivectorE2GA &a) {
	return norm_dont_mangle_105(a);
}
inline double norm_dont_mangle_108(const normalizedPoint &a)
{
	return ::fabs(::sqrt(::fabs((a.m_e1*a.m_e1+a.m_e2*a.m_e2+-2.0*a.m_ni))));

}
inline double norm_dont_mangle_108_returns_scalar(const normalizedPoint &a) {
	return norm_dont_mangle_108(a);
}
inline double norm_dont_mangle_109(const e1_t &a)
{
	return ::fabs(1.0);

}
inline double norm_dont_mangle_109_returns_scalar(const e1_t &a) {
	return norm_dont_mangle_109(a);
}
inline double norm_dont_mangle_111(const e2_t &a)
{
	return ::fabs(1.0);

}
inline double norm_dont_mangle_111_returns_scalar(const e2_t &a) {
	return norm_dont_mangle_111(a);
}
inline double norm_dont_mangle_114(const no_t &a)
{
	return ::fabs(0.0);

}
inline double norm_dont_mangle_114_returns_scalar(const no_t &a) {
	return norm_dont_mangle_114(a);
}
inline double norm_dont_mangle_116(const ni_t &a)
{
	return ::fabs(0.0);

}
inline double norm_dont_mangle_116_returns_scalar(const ni_t &a) {
	return norm_dont_mangle_116(a);
}
inline double norm_dont_mangle_118(const pseudoscalar &a)
{
	return ::fabs(::sqrt(::fabs(-a.m_no_e1_e2_ni*a.m_no_e1_e2_ni)));

}
inline double norm_dont_mangle_118_returns_scalar(const pseudoscalar &a) {
	return norm_dont_mangle_118(a);
}
inline double norm_dont_mangle_119(const oddVersor &a)
{
	return ::fabs(::sqrt(::fabs((a.m_e1*a.m_e1+-2.0*a.m_e1_e2_ni*a.m_no_e1_e2+a.m_e2*a.m_e2+-2.0*a.m_ni*a.m_no-a.m_no_e1_ni*a.m_no_e1_ni-a.m_no_e2_ni*a.m_no_e2_ni))));

}
inline double norm_dont_mangle_119_returns_scalar(const oddVersor &a) {
	return norm_dont_mangle_119(a);
}
// Missing dependencies definitions:
/// Computes the partial geometric product of two multivectors (group 0  x  group 0 -> group 0)
void gp_default_0_0_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 1 -> group 1)
void gp_default_0_1_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 2 -> group 2)
void gp_default_0_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 3 -> group 3)
void gp_default_0_3_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 4 -> group 4)
void gp_default_0_4_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 0 -> group 1)
void gp_default_1_0_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 1 -> group 0)
void gp_default_1_1_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 1 -> group 2)
void gp_default_1_1_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 2 -> group 1)
void gp_default_1_2_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 2 -> group 3)
void gp_default_1_2_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 3 -> group 2)
void gp_default_1_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 3 -> group 4)
void gp_default_1_3_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 4 -> group 3)
void gp_default_1_4_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 0 -> group 2)
void gp_default_2_0_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 1 -> group 1)
void gp_default_2_1_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 1 -> group 3)
void gp_default_2_1_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 0)
void gp_default_2_2_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 2)
void gp_default_2_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 4)
void gp_default_2_2_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 3 -> group 1)
void gp_default_2_3_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 3 -> group 3)
void gp_default_2_3_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 4 -> group 2)
void gp_default_2_4_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 0 -> group 3)
void gp_default_3_0_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 1 -> group 2)
void gp_default_3_1_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 1 -> group 4)
void gp_default_3_1_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 2 -> group 1)
void gp_default_3_2_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 2 -> group 3)
void gp_default_3_2_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 0)
void gp_default_3_3_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 2)
void gp_default_3_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 4 -> group 1)
void gp_default_3_4_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 0 -> group 4)
void gp_default_4_0_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 1 -> group 3)
void gp_default_4_1_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 2 -> group 2)
void gp_default_4_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 3 -> group 1)
void gp_default_4_3_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 4 -> group 0)
void gp_default_4_4_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 0 -> group 0)
void gp_euclidean_0_0_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 1 -> group 1)
void gp_euclidean_0_1_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 2 -> group 2)
void gp_euclidean_0_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 3 -> group 3)
void gp_euclidean_0_3_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 4 -> group 4)
void gp_euclidean_0_4_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 0 -> group 1)
void gp_euclidean_1_0_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 1 -> group 0)
void gp_euclidean_1_1_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 1 -> group 2)
void gp_euclidean_1_1_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 2 -> group 1)
void gp_euclidean_1_2_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 2 -> group 3)
void gp_euclidean_1_2_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 3 -> group 2)
void gp_euclidean_1_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 3 -> group 4)
void gp_euclidean_1_3_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 4 -> group 3)
void gp_euclidean_1_4_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 0 -> group 2)
void gp_euclidean_2_0_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 1 -> group 1)
void gp_euclidean_2_1_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 1 -> group 3)
void gp_euclidean_2_1_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 0)
void gp_euclidean_2_2_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 2)
void gp_euclidean_2_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 4)
void gp_euclidean_2_2_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 3 -> group 1)
void gp_euclidean_2_3_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 3 -> group 3)
void gp_euclidean_2_3_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 4 -> group 2)
void gp_euclidean_2_4_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 0 -> group 3)
void gp_euclidean_3_0_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 1 -> group 2)
void gp_euclidean_3_1_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 1 -> group 4)
void gp_euclidean_3_1_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 2 -> group 1)
void gp_euclidean_3_2_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 2 -> group 3)
void gp_euclidean_3_2_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 0)
void gp_euclidean_3_3_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 2)
void gp_euclidean_3_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 4 -> group 1)
void gp_euclidean_3_4_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 0 -> group 4)
void gp_euclidean_4_0_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 1 -> group 3)
void gp_euclidean_4_1_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 2 -> group 2)
void gp_euclidean_4_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 3 -> group 1)
void gp_euclidean_4_3_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 4 -> group 0)
void gp_euclidean_4_4_0(const double *A, const double *B, double *C);
/// copies coordinates of group 0
void copyGroup_0(const double *A, double *C);
/// copies and multiplies (by s) coordinates of group 0
void copyMul_0(const double *A, double *C, double s);
/// copies and divides (by s) coordinates of group 0
void copyDiv_0(const double *A, double *C, double s);
/// adds coordinates of group 0 from variable A to C
void add_0(const double *A, double *C);
/// subtracts coordinates of group 0 in variable A from C
void sub_0(const double *A, double *C);
/// negate coordinates of group 0 of variable A
void neg_0(const double *A, double *C);
/// adds coordinates of group 0 of variables A and B
void add2_0_0(const double *A, const double *B, double *C);
/// subtracts coordinates of group 0 of variables A from B
void sub2_0_0(const double *A, const double *B, double *C);
/// performs coordinate-wise multiplication of coordinates of group 0 of variables A and B
void hp_0_0(const double *A, const double *B, double *C);
/// performs coordinate-wise division of coordinates of group 0 of variables A and B
/// (no checks for divide by zero are made)
void ihp_0_0(const double *A, const double *B, double *C);
/// check for equality up to eps of coordinates of group 0 of variables A and B
bool equals_0_0(const double *A, const double *B, double eps);
/// checks if coordinates of group 0 of variable A are zero up to eps
bool zeroGroup_0(const double *A, double eps);
/// copies coordinates of group 1
void copyGroup_1(const double *A, double *C);
/// copies and multiplies (by s) coordinates of group 1
void copyMul_1(const double *A, double *C, double s);
/// copies and divides (by s) coordinates of group 1
void copyDiv_1(const double *A, double *C, double s);
/// adds coordinates of group 1 from variable A to C
void add_1(const double *A, double *C);
/// subtracts coordinates of group 1 in variable A from C
void sub_1(const double *A, double *C);
/// negate coordinates of group 1 of variable A
void neg_1(const double *A, double *C);
/// adds coordinates of group 1 of variables A and B
void add2_1_1(const double *A, const double *B, double *C);
/// subtracts coordinates of group 1 of variables A from B
void sub2_1_1(const double *A, const double *B, double *C);
/// performs coordinate-wise multiplication of coordinates of group 1 of variables A and B
void hp_1_1(const double *A, const double *B, double *C);
/// performs coordinate-wise division of coordinates of group 1 of variables A and B
/// (no checks for divide by zero are made)
void ihp_1_1(const double *A, const double *B, double *C);
/// check for equality up to eps of coordinates of group 1 of variables A and B
bool equals_1_1(const double *A, const double *B, double eps);
/// checks if coordinates of group 1 of variable A are zero up to eps
bool zeroGroup_1(const double *A, double eps);
/// copies coordinates of group 2
void copyGroup_2(const double *A, double *C);
/// copies and multiplies (by s) coordinates of group 2
void copyMul_2(const double *A, double *C, double s);
/// copies and divides (by s) coordinates of group 2
void copyDiv_2(const double *A, double *C, double s);
/// adds coordinates of group 2 from variable A to C
void add_2(const double *A, double *C);
/// subtracts coordinates of group 2 in variable A from C
void sub_2(const double *A, double *C);
/// negate coordinates of group 2 of variable A
void neg_2(const double *A, double *C);
/// adds coordinates of group 2 of variables A and B
void add2_2_2(const double *A, const double *B, double *C);
/// subtracts coordinates of group 2 of variables A from B
void sub2_2_2(const double *A, const double *B, double *C);
/// performs coordinate-wise multiplication of coordinates of group 2 of variables A and B
void hp_2_2(const double *A, const double *B, double *C);
/// performs coordinate-wise division of coordinates of group 2 of variables A and B
/// (no checks for divide by zero are made)
void ihp_2_2(const double *A, const double *B, double *C);
/// check for equality up to eps of coordinates of group 2 of variables A and B
bool equals_2_2(const double *A, const double *B, double eps);
/// checks if coordinates of group 2 of variable A are zero up to eps
bool zeroGroup_2(const double *A, double eps);
/// copies coordinates of group 3
void copyGroup_3(const double *A, double *C);
/// copies and multiplies (by s) coordinates of group 3
void copyMul_3(const double *A, double *C, double s);
/// copies and divides (by s) coordinates of group 3
void copyDiv_3(const double *A, double *C, double s);
/// adds coordinates of group 3 from variable A to C
void add_3(const double *A, double *C);
/// subtracts coordinates of group 3 in variable A from C
void sub_3(const double *A, double *C);
/// negate coordinates of group 3 of variable A
void neg_3(const double *A, double *C);
/// adds coordinates of group 3 of variables A and B
void add2_3_3(const double *A, const double *B, double *C);
/// subtracts coordinates of group 3 of variables A from B
void sub2_3_3(const double *A, const double *B, double *C);
/// performs coordinate-wise multiplication of coordinates of group 3 of variables A and B
void hp_3_3(const double *A, const double *B, double *C);
/// performs coordinate-wise division of coordinates of group 3 of variables A and B
/// (no checks for divide by zero are made)
void ihp_3_3(const double *A, const double *B, double *C);
/// check for equality up to eps of coordinates of group 3 of variables A and B
bool equals_3_3(const double *A, const double *B, double eps);
/// checks if coordinates of group 3 of variable A are zero up to eps
bool zeroGroup_3(const double *A, double eps);
/// copies coordinates of group 4
void copyGroup_4(const double *A, double *C);
/// copies and multiplies (by s) coordinates of group 4
void copyMul_4(const double *A, double *C, double s);
/// copies and divides (by s) coordinates of group 4
void copyDiv_4(const double *A, double *C, double s);
/// adds coordinates of group 4 from variable A to C
void add_4(const double *A, double *C);
/// subtracts coordinates of group 4 in variable A from C
void sub_4(const double *A, double *C);
/// negate coordinates of group 4 of variable A
void neg_4(const double *A, double *C);
/// adds coordinates of group 4 of variables A and B
void add2_4_4(const double *A, const double *B, double *C);
/// subtracts coordinates of group 4 of variables A from B
void sub2_4_4(const double *A, const double *B, double *C);
/// performs coordinate-wise multiplication of coordinates of group 4 of variables A and B
void hp_4_4(const double *A, const double *B, double *C);
/// performs coordinate-wise division of coordinates of group 4 of variables A and B
/// (no checks for divide by zero are made)
void ihp_4_4(const double *A, const double *B, double *C);
/// check for equality up to eps of coordinates of group 4 of variables A and B
bool equals_4_4(const double *A, const double *B, double eps);
/// checks if coordinates of group 4 of variable A are zero up to eps
bool zeroGroup_4(const double *A, double eps);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_0_4(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_0_4(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_1_3(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_1_3(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_2_2(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_2_2(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_3_1(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_3_1(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_4_0(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_4_0(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_0_4(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_0_4(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_1_3(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_1_3(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_2_2(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_2_2(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_3_1(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_3_1(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_4_0(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_4_0(const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_1_1(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_2_2(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_3_3(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_4_4(const om &O, const double *A, double *C);
mv subtract_dont_mangle_2_returns_mv(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() | b.gu();
	double c[16];
	
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			sub2_0_0(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 1;
		}
		else copyGroup_0(a.getC() + aidx, c + cidx);
		aidx += 1;
		cidx += 1;
	}
	else if (b.gu() & 1) {
		neg_0(b.getC() + bidx, c + cidx);
		bidx += 1;
		cidx += 1;
	}
	
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			sub2_1_1(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 4;
		}
		else copyGroup_1(a.getC() + aidx, c + cidx);
		aidx += 4;
		cidx += 4;
	}
	else if (b.gu() & 2) {
		neg_1(b.getC() + bidx, c + cidx);
		bidx += 4;
		cidx += 4;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			sub2_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 6;
		}
		else copyGroup_2(a.getC() + aidx, c + cidx);
		aidx += 6;
		cidx += 6;
	}
	else if (b.gu() & 4) {
		neg_2(b.getC() + bidx, c + cidx);
		bidx += 6;
		cidx += 6;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			sub2_3_3(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 4;
		}
		else copyGroup_3(a.getC() + aidx, c + cidx);
		aidx += 4;
		cidx += 4;
	}
	else if (b.gu() & 8) {
		neg_3(b.getC() + bidx, c + cidx);
		bidx += 4;
		cidx += 4;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			sub2_4_4(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
		else copyGroup_4(a.getC() + aidx, c + cidx);
		cidx += 1;
	}
	else if (b.gu() & 16) {
		neg_4(b.getC() + bidx, c + cidx);
		cidx += 1;
	}
	return mv(gu, c);
}


mv random_versor_dont_mangle_3_returns_mv(double scale, int grade, int basisVectorBitmap) {
	double minimumNorm = 0.01;
	double largestCoordinate = 4.0;
	return random_versor_dont_mangle_3_returns_mv_ex(scale, grade, basisVectorBitmap, minimumNorm, scale * largestCoordinate);
}

mv random_versor_dont_mangle_3_returns_mv_ex(double scale, int _grade, int basisVectorBitmap, 
		double minimumNorm, double largestCoordinate) 
{
	mv randomVector, tmp1, tmp2;
	mv *IR1 = &tmp1, *IR2 = &tmp2; // IR = intermediate result
	double randomValues[4];
	double n2, mul;
	int i;
	int grade = _grade;
	
	// set IR1 (intermediate result) to 1
	IR1->set(1.0);

	while (grade > 0) {	// loop until grade == 0
		// fill array with random values
		for (i = 0; i < 4; i++) 
			randomValues[i] = (basisVectorBitmap & (1 << i))
				? (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double())
				: 0.0;
		
		// make it a multivector:
		randomVector.set(GRADE_1, randomValues);
		
		// multiply 
		(*IR2) = gp(*IR1, randomVector);
		
		// swap IR pointers: 
		std::swap(IR1, IR2);
		
		// lower grade
		grade--;
	}
	
	// compute norm/multiplier, apply it, or recurse if we happened to create a near-null versor
	n2 = norm_dont_mangle_96_returns_scalar(*IR1);
	if ((double)fabs(n2) > minimumNorm * minimumNorm) {
		if (n2 != 0.0) {
			mul = scale * random_double_dont_mangle_1_returns_double() / n2;
			if (IR1->largestCoordinate() * mul < largestCoordinate)
				return gp_dont_mangle_99(*IR1, mul);
		}
		else if (IR1->largestCoordinate() < largestCoordinate)
			return *IR1;
	}
	
	// try again:
	return random_versor_dont_mangle_3_returns_mv_ex(scale, _grade, basisVectorBitmap, minimumNorm, largestCoordinate); 
}
mv add_dont_mangle_4_returns_mv(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() | b.gu();
	double c[16];
	
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			add2_0_0(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 1;
		}
		else copyGroup_0(a.getC() + aidx, c + cidx);
		aidx += 1;
		cidx += 1;
	}
	else if (b.gu() & 1) {
		copyGroup_0(b.getC() + bidx, c + cidx);
		bidx += 1;
		cidx += 1;
	}
	
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			add2_1_1(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 4;
		}
		else copyGroup_1(a.getC() + aidx, c + cidx);
		aidx += 4;
		cidx += 4;
	}
	else if (b.gu() & 2) {
		copyGroup_1(b.getC() + bidx, c + cidx);
		bidx += 4;
		cidx += 4;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			add2_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 6;
		}
		else copyGroup_2(a.getC() + aidx, c + cidx);
		aidx += 6;
		cidx += 6;
	}
	else if (b.gu() & 4) {
		copyGroup_2(b.getC() + bidx, c + cidx);
		bidx += 6;
		cidx += 6;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			add2_3_3(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 4;
		}
		else copyGroup_3(a.getC() + aidx, c + cidx);
		aidx += 4;
		cidx += 4;
	}
	else if (b.gu() & 8) {
		copyGroup_3(b.getC() + bidx, c + cidx);
		bidx += 4;
		cidx += 4;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			add2_4_4(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
		else copyGroup_4(a.getC() + aidx, c + cidx);
		cidx += 1;
	}
	else if (b.gu() & 16) {
		copyGroup_4(b.getC() + bidx, c + cidx);
		cidx += 1;
	}
	return mv(gu, c);
}


mv random_blade_dont_mangle_47_returns_mv(double scale, int grade, int basisVectorBitmap) {
	double minimumNorm = 0.01;
	double largestCoordinate = 4.0;
	return random_blade_dont_mangle_47_returns_mv_ex(scale, grade, basisVectorBitmap, minimumNorm, scale * largestCoordinate);
}

mv random_blade_dont_mangle_47_returns_mv_ex(double scale, int _grade, int basisVectorBitmap, 
		double minimumNorm, double largestCoordinate) 
{
	mv randomVector, tmp1, tmp2;
	mv *IR1 = &tmp1, *IR2 = &tmp2; // IR = intermediate result
	double randomValues[4];
	double n2, mul;
	int i;
	int grade = _grade;
	
	// set IR1 (intermediate result) to 1
	IR1->set(1.0);

	while (grade > 0) {	// loop until grade == 0
		// fill array with random values
		for (i = 0; i < 4; i++) 
			randomValues[i] = (basisVectorBitmap & (1 << i))
				? (-1.0 + 2.0 * random_double_dont_mangle_1_returns_double())
				: 0.0;
		
		// make it a multivector:
		randomVector.set(GRADE_1, randomValues);
		
		// multiply 
		(*IR2) = op(*IR1, randomVector);
		
		// swap IR pointers: 
		std::swap(IR1, IR2);
		
		// lower grade
		grade--;
	}
	
	// compute norm/multiplier, apply it, or recurse if we happened to create a near-null versor
	n2 = norm_dont_mangle_96_returns_scalar(*IR1);
	if ((double)fabs(n2) > minimumNorm * minimumNorm) {
		if (n2 != 0.0) {
			mul = scale * random_double_dont_mangle_1_returns_double() / n2;
			if (IR1->largestCoordinate() * mul < largestCoordinate)
				return gp_dont_mangle_99(*IR1, mul);
		}
		else if (IR1->largestCoordinate() < largestCoordinate)
			return *IR1;
	}
	
	// try again:
	return random_blade_dont_mangle_47_returns_mv_ex(scale, _grade, basisVectorBitmap, minimumNorm, largestCoordinate); 
}
mv extractGrade_dont_mangle_48_returns_mv(const mv &a, int groupBitmap)
{
	int aidx = 0, cidx = 0;
	int gu =  a.gu() & groupBitmap;
	double c[16];
	
	if (a.gu() & 1) {
		if (groupBitmap & 1) {
			copyGroup_0(a.getC() + aidx, c + cidx);
			cidx += 1;
		}
		aidx += 1;
	}
	
	if (a.gu() & 2) {
		if (groupBitmap & 2) {
			copyGroup_1(a.getC() + aidx, c + cidx);
			cidx += 4;
		}
		aidx += 4;
	}
	
	if (a.gu() & 4) {
		if (groupBitmap & 4) {
			copyGroup_2(a.getC() + aidx, c + cidx);
			cidx += 6;
		}
		aidx += 6;
	}
	
	if (a.gu() & 8) {
		if (groupBitmap & 8) {
			copyGroup_3(a.getC() + aidx, c + cidx);
			cidx += 4;
		}
		aidx += 4;
	}
	
	if (a.gu() & 16) {
		if (groupBitmap & 16) {
			copyGroup_4(a.getC() + aidx, c + cidx);
		}
	}
	return mv(gu, c);
}
mv gp_dont_mangle_92_returns_mv(const mv &a, const mv &b)
{
	double c[16];
	const double* _a[5];
	const double* _b[5];
	a.expand(_a);
	b.expand(_b);
	c2ga::zero_16(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_euclidean_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_euclidean_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_euclidean_0_2_2(_a[0], _b[2], c + 5);
		}
		if (b.gu() & 8) {
			gp_euclidean_0_3_3(_a[0], _b[3], c + 11);
		}
		if (b.gu() & 16) {
			gp_euclidean_0_4_4(_a[0], _b[4], c + 15);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_euclidean_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_euclidean_1_1_0(_a[1], _b[1], c + 0);
			gp_euclidean_1_1_2(_a[1], _b[1], c + 5);
		}
		if (b.gu() & 4) {
			gp_euclidean_1_2_1(_a[1], _b[2], c + 1);
			gp_euclidean_1_2_3(_a[1], _b[2], c + 11);
		}
		if (b.gu() & 8) {
			gp_euclidean_1_3_2(_a[1], _b[3], c + 5);
			gp_euclidean_1_3_4(_a[1], _b[3], c + 15);
		}
		if (b.gu() & 16) {
			gp_euclidean_1_4_3(_a[1], _b[4], c + 11);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_euclidean_2_0_2(_a[2], _b[0], c + 5);
		}
		if (b.gu() & 2) {
			gp_euclidean_2_1_1(_a[2], _b[1], c + 1);
			gp_euclidean_2_1_3(_a[2], _b[1], c + 11);
		}
		if (b.gu() & 4) {
			gp_euclidean_2_2_0(_a[2], _b[2], c + 0);
			gp_euclidean_2_2_2(_a[2], _b[2], c + 5);
			gp_euclidean_2_2_4(_a[2], _b[2], c + 15);
		}
		if (b.gu() & 8) {
			gp_euclidean_2_3_1(_a[2], _b[3], c + 1);
			gp_euclidean_2_3_3(_a[2], _b[3], c + 11);
		}
		if (b.gu() & 16) {
			gp_euclidean_2_4_2(_a[2], _b[4], c + 5);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_euclidean_3_0_3(_a[3], _b[0], c + 11);
		}
		if (b.gu() & 2) {
			gp_euclidean_3_1_2(_a[3], _b[1], c + 5);
			gp_euclidean_3_1_4(_a[3], _b[1], c + 15);
		}
		if (b.gu() & 4) {
			gp_euclidean_3_2_1(_a[3], _b[2], c + 1);
			gp_euclidean_3_2_3(_a[3], _b[2], c + 11);
		}
		if (b.gu() & 8) {
			gp_euclidean_3_3_0(_a[3], _b[3], c + 0);
			gp_euclidean_3_3_2(_a[3], _b[3], c + 5);
		}
		if (b.gu() & 16) {
			gp_euclidean_3_4_1(_a[3], _b[4], c + 1);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_euclidean_4_0_4(_a[4], _b[0], c + 15);
		}
		if (b.gu() & 2) {
			gp_euclidean_4_1_3(_a[4], _b[1], c + 11);
		}
		if (b.gu() & 4) {
			gp_euclidean_4_2_2(_a[4], _b[2], c + 5);
		}
		if (b.gu() & 8) {
			gp_euclidean_4_3_1(_a[4], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_euclidean_4_4_0(_a[4], _b[4], c + 0);
		}
	}
	return mv_compress(c, 0.0, 31);
}
double norm_dont_mangle_96(const mv &a)
{
	double n2 = 0.0;
	double c[1];
	const double* _a[5];
	a.expand(_a);
	c2ga::zero_1(c);
	if (_a[0] != NULL) {  /* group 0 (grade 0) */
		c[0] = 0.0;
			gp_default_0_0_0(_a[0], _a[0], c);
		n2 += c[0];
	}
	if (_a[1] != NULL) {  /* group 1 (grade 1) */
		c[0] = 0.0;
			gp_default_1_1_0(_a[1], _a[1], c);
		n2 += c[0];
	}
	if (_a[2] != NULL) {  /* group 2 (grade 2) */
		c[0] = 0.0;
			gp_default_2_2_0(_a[2], _a[2], c);
		n2 -= c[0];
	}
	if (_a[3] != NULL) {  /* group 3 (grade 3) */
		c[0] = 0.0;
			gp_default_3_3_0(_a[3], _a[3], c);
		n2 -= c[0];
	}
	if (_a[4] != NULL) {  /* group 4 (grade 4) */
		c[0] = 0.0;
			gp_default_4_4_0(_a[4], _a[4], c);
		n2 += c[0];
	}
	return ((n2 < 0.0) ? ::sqrt(-n2) : ::sqrt(n2));
}
mv gp_dont_mangle_99(const mv &a, const double b)
{
	double c[16];
	const double* _a[5];
	const double* _b[1] = {&b};
	a.expand(_a);
	c2ga::zero_16(c);
	if (a.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
	}
	if (a.gu() & 2) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
	}
	if (a.gu() & 4) {
			gp_default_2_0_2(_a[2], _b[0], c + 5);
	}
	if (a.gu() & 8) {
			gp_default_3_0_3(_a[3], _b[0], c + 11);
	}
	if (a.gu() & 16) {
			gp_default_4_0_4(_a[4], _b[0], c + 15);
	}
	return mv_compress(c, 0.0, 31);
}
// Testing code declarations:
// Testing code inline definitions:

int test_gp_dont_mangle_123(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 64;
	mv A, B, C, D, E, V1, V2;
	int i;
	int o;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		o = (random_double_dont_mangle_1_returns_double() < 0.5) ? 0 : 1; // even or odd?
		A = random_versor_dont_mangle_3_returns_mv(random_double_dont_mangle_1_returns_double(), ((int)(random_double_dont_mangle_1_returns_double() * 4.5) & 0xFFFE) + o, basisVectorBitmap);
		B = random_versor_dont_mangle_3_returns_mv(random_double_dont_mangle_1_returns_double(), ((int)(random_double_dont_mangle_1_returns_double() * 4.5) & 0xFFFE) + o, basisVectorBitmap);
		C = random_versor_dont_mangle_3_returns_mv(random_double_dont_mangle_1_returns_double(), ((int)(random_double_dont_mangle_1_returns_double() * 4.5) & 0xFFFE) + o, basisVectorBitmap);
		
		{ // test (A+B) C = A C + B C
			// D = A + B
			D = add_dont_mangle_4_returns_mv(A, B);
			// V1 = D C
			V1 = gp(D, C);
			// D = A C
			D = gp(A, C);
			// E = B C
			E = gp(B, C);
			// V2 = D + E
			V2 = add_dont_mangle_4_returns_mv(D, E);
			// test equality
			D = subtract_dont_mangle_2_returns_mv(V1, V2);
			// use mv_largestCoordinate() to verify
			if (D.largestCoordinate() > 1E-11) {
				printf("gp() test failed on '(A+B) C = A C + B C' (dif=%e)\n", (double)D.largestCoordinate());
				return 0; // failure
			}
		}
		
		{ // test A (B+C) = A B + A C
			// D = B + C
			D = add_dont_mangle_4_returns_mv(B, C);
			// V1 = A D
			V1 = gp(A, D);
			// D = A B
			D = gp(A, B);
			// E = A C
			E = gp(A, C);
			// V2 = D + E
			V2 = add_dont_mangle_4_returns_mv(D, E);
			// test equality
			D = subtract_dont_mangle_2_returns_mv(V1, V2);
			// use largestCoordinate() to verify
			if (D.largestCoordinate() > 1E-12) {
				printf("gp() test failed on 'A (B+C) = A B + A C' (dif=%e)\n", (double)D.largestCoordinate());
				return 0; // failure
			}
		}
		
		{ // test A (B C) = (A B) C
			// D = B C
			D = gp(B, C);
			// V1 = A D
			V1 = gp(A, D);
			// D = A B
			D = gp(A, B);
			// V2 = D C
			V2 = gp(D, C);
			// test equality
			D = subtract_dont_mangle_2_returns_mv(V1, V2);
			// use largestCoordinate() to verify
			if (D.largestCoordinate() > 1E-12) {
				printf("gp() test failed on 'A (B C) = (A B) C' (dif=%e)\n", (double)D.largestCoordinate());
				return 0; // failure
			}
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_127(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 4;
	vectorE2GA A;
	vectorE2GA B;
	rotorE2GA C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_vectorE2GA_dont_mangle_7(s);
		B = random_vectorE2GA_dont_mangle_7(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_128(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 4;
	rotorE2GA A;
	vectorE2GA B;
	vectorE2GA C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_rotorE2GA_dont_mangle_11(s);
		B = random_vectorE2GA_dont_mangle_7(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_131(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 4;
	vectorE2GA A;
	rotorE2GA B;
	vectorE2GA C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_vectorE2GA_dont_mangle_7(s);
		B = random_rotorE2GA_dont_mangle_11(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_124(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 4;
	rotorE2GA A;
	rotorE2GA B;
	rotorE2GA C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_rotorE2GA_dont_mangle_11(s);
		B = random_rotorE2GA_dont_mangle_11(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_125(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 5;
	rotorE2GA A;
	line B;
	oddVersor C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_rotorE2GA_dont_mangle_11(s);
		B = random_line_dont_mangle_24(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_126(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 7;
	circle A;
	line B;
	evenVersor C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_circle_dont_mangle_27(s);
		B = random_line_dont_mangle_24(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_129(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	bivectorE2GA A;
	bivectorE2GA B;
	double C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_bivectorE2GA_dont_mangle_31(s);
		B = random_bivectorE2GA_dont_mangle_31(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_130(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 8;
	normalizedPoint A;
	normalizedPoint B;
	evenVersor C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_normalizedPoint_dont_mangle_35(s);
		B = random_normalizedPoint_dont_mangle_35(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_132(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	e1_t A;
	e2_t B;
	I2_t C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_e1_t_dont_mangle_39(s);
		B = random_e2_t_dont_mangle_40(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_gp_dont_mangle_133(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	no_t A;
	ni_t B;
	evenVersor C;
	mv gA, gB, gC1, gC2;
	
	double s;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		A = random_no_t_dont_mangle_43(s);
		B = random_ni_t_dont_mangle_44(s);
		
		// A gp B
		C = gp(A, B);
		gC1 = C;
		
		// convert all A and B to gmv and add/subtract as GMVs
		gA = A;
		gB = B;
		gC2 = gp(gA, gB);
		
		// see if result is equal up to precision:
		gA = subtract_dont_mangle_2_returns_mv(gC1, gC2);
		if (gA.largestCoordinate() > 1E-13) {
			printf("gp() test failed (largestCoordinate = %e)\n", (double)gA.largestCoordinate());
			return 0; // failure
		}		
	}
	return 1; // success
}

int test_op_dont_mangle_134_1(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 16;
	mv A, B, C, D, E, dif;
	int i, ga, gb, gd;
	double s;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		ga = (int)(random_double_dont_mangle_1_returns_double() * 4.5);
		A = random_blade_dont_mangle_47_returns_mv(s, ga, basisVectorBitmap);
		
		s = random_double_dont_mangle_1_returns_double();
		gb = (int)(random_double_dont_mangle_1_returns_double() * 4.5);
		B = random_blade_dont_mangle_47_returns_mv(s, gb, basisVectorBitmap);
		
		// compute product using op()
		C = op(A, B);
		
		// simulate product using geometric product & grade part selection
		D = gp(A, B);
		gd = (ga > gb) ? ga - gb : gb - ga;
		E = extractGrade_dont_mangle_48_returns_mv(D,  c2ga_grades[ga + gb]);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(C, E);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_134_2(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 16;
	mv A, B;
	int i, ga;
	double s;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		s = random_double_dont_mangle_1_returns_double();
		ga = (int)(random_double_dont_mangle_1_returns_double() * 4.5);
		if (ga == 0) continue; // do not perform this test for grade 0
		A = random_blade_dont_mangle_47_returns_mv(s, ga, basisVectorBitmap);
		
		// compute A ^ A (should be zero)
		B = op(A, A);
		
		// check if B is zero:
		if (B.largestCoordinate() > 1E-13) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)B.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_137(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	vectorE2GA A;
	vectorE2GA B;
	bivectorE2GA C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_vectorE2GA_dont_mangle_7(1.0);
		B = random_vectorE2GA_dont_mangle_7(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_135(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	rotorE2GA A;
	vectorE2GA B;
	vectorE2GA C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_rotorE2GA_dont_mangle_11(1.0);
		B = random_vectorE2GA_dont_mangle_7(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_138(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	vectorE2GA A;
	rotorE2GA B;
	vectorE2GA C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_vectorE2GA_dont_mangle_7(1.0);
		B = random_rotorE2GA_dont_mangle_11(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_136(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	rotorE2GA A;
	rotorE2GA B;
	rotorE2GA C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_rotorE2GA_dont_mangle_11(1.0);
		B = random_rotorE2GA_dont_mangle_11(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_140(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	vectorE2GA A;
	line B;
	pseudoscalar C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_vectorE2GA_dont_mangle_7(1.0);
		B = random_line_dont_mangle_24(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_139(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 2;
	rotorE2GA A;
	circle B;
	circle C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_rotorE2GA_dont_mangle_11(1.0);
		B = random_circle_dont_mangle_27(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_142(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 1;
	bivectorE2GA A;
	bivectorE2GA B;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_bivectorE2GA_dont_mangle_31(1.0);
		B = random_bivectorE2GA_dont_mangle_31(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_141(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 1;
	pseudoscalar A;
	oddVersor B;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_pseudoscalar_dont_mangle_79(1.0);
		B = random_oddVersor_dont_mangle_80(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_143(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 1;
	no_t A;
	ni_t B;
	noni_t C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_no_t_dont_mangle_43(1.0);
		B = random_ni_t_dont_mangle_44(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}

int test_op_dont_mangle_144(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 1;
	ni_t A;
	no_t B;
	evenVersor C;
	

	mv gmvA, gmvB, gmvC, gmvD, dif;
	int i;
	int basisVectorBitmap = -1;
	
	for (i = 0; i < NB_LOOPS; i++) {
		A = random_ni_t_dont_mangle_44(1.0);
		B = random_no_t_dont_mangle_43(1.0);
		
		// convert smv to GMV
		gmvA = A;
		gmvB = B;
				
		// compute product using op()
		gmvC = op(A, B);
		
		// perform GMV version 
		gmvD = op(gmvA, gmvB);

		// check if equal:
		dif = subtract_dont_mangle_2_returns_mv(gmvC, gmvD);
		if (dif.largestCoordinate() > 1E-12) {
			printf("op() test failed (largestCoordinate = %e)\n", (double)dif.largestCoordinate());
			return 0; // failure
		}
	}
	return 1; // success
}
// Testing code definitions:

int test_metric_default_mv(int NB_TESTS_SCALER) 
{
	int i, j;
	double arr[4], dif;
	mv A, bv[4];
	double M[16+1] = {
		0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0	}; // metric matrix

	// get all basis vectors

	c2ga::zero_4(arr);
	arr[0] = 1.0;
	bv[0].set(2, arr);

	c2ga::zero_4(arr);
	arr[1] = 1.0;
	bv[1].set(2, arr);

	c2ga::zero_4(arr);
	arr[2] = 1.0;
	bv[2].set(2, arr);

	c2ga::zero_4(arr);
	arr[3] = 1.0;
	bv[3].set(2, arr);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			A = gp(bv[i], bv[j]);
			dif = M[i * 4 + j] - A.get_scalar();
			if ((dif < -1E-14) || (dif > 1E-14)) {
				printf("test_metric_default_mv() test failed for %s %s\n", c2ga_basisVectorNames[i], c2ga_basisVectorNames[j]);
				return 0;
			}
		}
	}
	
	return 1;
}

int test_metric_euclidean_mv(int NB_TESTS_SCALER) 
{
	int i, j;
	double arr[4], dif;
	mv A, bv[4];
	double M[16+1] = {
		1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0	}; // metric matrix

	// get all basis vectors

	c2ga::zero_4(arr);
	arr[0] = 1.0;
	bv[0].set(2, arr);

	c2ga::zero_4(arr);
	arr[1] = 1.0;
	bv[1].set(2, arr);

	c2ga::zero_4(arr);
	arr[2] = 1.0;
	bv[2].set(2, arr);

	c2ga::zero_4(arr);
	arr[3] = 1.0;
	bv[3].set(2, arr);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			A = gp_dont_mangle_92_returns_mv(bv[i], bv[j]);
			dif = M[i * 4 + j] - A.get_scalar();
			if ((dif < -1E-14) || (dif > 1E-14)) {
				printf("test_metric_euclidean_mv() test failed for %s %s\n", c2ga_basisVectorNames[i], c2ga_basisVectorNames[j]);
				return 0;
			}
		}
	}
	
	return 1;
}

int test_parse_mv(int NB_TESTS_SCALER) 
{
	const int NB_LOOPS = 100 + NB_TESTS_SCALER / 64;
	mv A, B, C;
	std::string str;
	
	int i, basisVectorBitmap = -1;

	for (i = 0; i < NB_LOOPS; i++) {
		A = random_versor_dont_mangle_3_returns_mv(random_double_dont_mangle_89(), (int)(random_double_dont_mangle_89() * 4.5), basisVectorBitmap);
		
		str = A.toString("%2.20e");
		
		try {
			B = parse(str);
		} catch (const std::string &exStr) {
			printf("parse() test failed (%s)\n", exStr.c_str());
			return 0; // failure
		}
		
		C = subtract_dont_mangle_2_returns_mv(A, B);

		if (C.largestCoordinate() > 1E-14) {
			printf("parse() test failed (%s)\n", str.c_str());
			return 0; // failure
		}
	}
	
	return 1; // success
}
} // end of namespace c2ga

int main(int argc, char *argv[]) {
	int retVal = 0;
	// The number of tests will be proportional to this value.
	// This should become a command-line option
	const int NB_TESTS_SCALER = 16384;
	
	// seed random number generators with current time
	c2ga::random_double_dont_mangle_89_timeSeed();

	// run all test functions
	if (!c2ga::test_metric_default_mv(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_metric_euclidean_mv(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_parse_mv(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_123(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_127(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_128(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_131(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_124(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_125(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_126(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_129(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_130(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_132(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_gp_dont_mangle_133(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_134_1(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_134_2(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_137(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_135(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_138(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_136(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_140(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_139(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_142(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_141(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_143(NB_TESTS_SCALER)) retVal = -1;
	if (!c2ga::test_op_dont_mangle_144(NB_TESTS_SCALER)) retVal = -1;

	if (retVal != 0) printf("Test failed.\n");
	else printf("Done.\n");	

	return retVal;
}
