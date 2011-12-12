/*
Gaigen 2.5 Test Suite
*/
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#include <stdio.h>
#include <utility> // for std::swap
#include <time.h> /* used to seed random generator */
#include "e2ga.h"
namespace e2ga {

const int e2ga_spaceDim = 2;
const int e2ga_nbGroups = 3;
const bool e2ga_metricEuclidean = true;
const char *e2ga_basisVectorNames[2] = {
	"e1", "e2"
};
const int e2ga_grades[] = {GRADE_0, GRADE_1, GRADE_2, 0, 0, 0};
const int e2ga_groups[] = {GROUP_0, GROUP_1, GROUP_2};
const int e2ga_groupSize[3] = {
	1, 2, 1
};
const int e2ga_mvSize[8] = {
	0, 1, 2, 3, 1, 2, 3, 4};
const int e2ga_basisElements[4][3] = {
	{-1},
	{0, -1},
	{1, -1},
	{0, 1, -1}
};
const double e2ga_basisElementSignByIndex[4] =
	{1, 1, 1, 1};
const double e2ga_basisElementSignByBitmap[4] =
	{1, 1, 1, 1};
const int e2ga_basisElementIndexByBitmap[4] =
	{0, 1, 2, 3};
const int e2ga_basisElementBitmapByIndex[4] =
	{0, 1, 2, 3};
const int e2ga_basisElementGradeByBitmap[4] =
	{0, 1, 1, 2};
const int e2ga_basisElementGroupByBitmap[4] =
	{0, 1, 1, 2};

const char *g_e2gaTypenames[] = 
{
	"mv",
	"double",
	"e1_t",
	"e2_t",
	"I2_t",
	"I2i_t",
	"vector",
	"bivector",
	"rotor"
};
e1_t e1;
e2_t e2;
I2_t I2;
I2i_t I2i;
vector vectorE1(vector::coord_e1_e2, 1.0, 0.0);
vector vectorE2(vector::coord_e1_e2, 0.0, 2.0);
rotor rotor90(rotor::coord_scalar_e1e2, 0.707111, 0.707111);


void ReportUsage::printReport(FILE *F /*= stdout*/, bool includeCount /* = true */) {
	fprintf(F, "Report usage is disabled");
}

/* 
These strings determine how the output of string() is formatted.
You can alter them at runtime using e2ga_setStringFormat().
*/
 
const char *e2ga_string_fp = "%2.2f"; 
const char *e2ga_string_start = ""; 
const char *e2ga_string_end = ""; 
const char *e2ga_string_mul = "*"; 
const char *e2ga_string_wedge = "^"; 
const char *e2ga_string_plus = " + "; 
const char *e2ga_string_minus = " - "; 

void e2ga_setStringFormat(const char *what, const char *format) {
 
	if (!strcmp(what, "fp")) 
		e2ga_string_fp = (format) ? format : "%2.2f"; 
	else if (!strcmp(what, "start")) 
		e2ga_string_start = (format) ? format : ""; 
	else if (!strcmp(what, "end")) 
		e2ga_string_end = (format) ? format : ""; 
	else if (!strcmp(what, "mul")) 
		e2ga_string_mul = (format) ? format : "*"; 
	else if (!strcmp(what, "wedge")) 
		e2ga_string_wedge = (format) ? format : "^"; 
	else if (!strcmp(what, "plus")) 
		e2ga_string_plus = (format) ? format : " + "; 
	else if (!strcmp(what, "minus")) 
		e2ga_string_minus = (format) ? format : " - ";
}



#ifdef WIN32
#define snprintf _snprintf
#pragma warning(disable:4996) /* quit your whining already */
#endif /* WIN32 */
const char *c_str(const mv &V, char *str, int maxLength, const char *fp) 
{
	int stdIdx = 0, l;
	char tmpBuf[256], tmpFloatBuf[256]; 
	int k = 0, bei, ia = 0, s = e2ga_mvSize[V.gu()], p = 0, cnt = 0;
	const double *c = V.getC();

	// set up the floating point precision
	if (fp == NULL) fp = e2ga_string_fp;

	// start the string
	l = snprintf(tmpBuf, 256, "%s", e2ga_string_start);
	if (stdIdx + l <= maxLength) {
		strcpy(str + stdIdx, tmpBuf);
		stdIdx += l;
	}
	else {
		snprintf(str, maxLength, "toString_mv: buffer too small");
		return str;
	}

	// print all coordinates
	for (int i = 0; i <= 3; i++) {
		if (V.gu() & (1 << i)) {
			for (int j = 0; j < e2ga_groupSize[i]; j++) {
				double coord = (double)e2ga_basisElementSignByIndex[ia] *c[k];
				/* goal: print [+|-]V.m_c[k][* basisVector1 ^ ... ^ basisVectorN] */			
				snprintf(tmpFloatBuf, 256, fp, (double)fabs(coord)); // cast to double to force custom float types to Plain Old Data
				if (atof(tmpFloatBuf) != 0.0) {
					l = 0;

					// print [+|-]
					l += snprintf(tmpBuf + l, 256-l, "%s", (coord >= 0.0) 
						? (cnt ? e2ga_string_plus : "")
						: e2ga_string_minus);
						
					// print obj.m_c[k]
					int dummyArg = 0; // prevents compiler warning on some platforms
					l += snprintf(tmpBuf + l, 256-l, tmpFloatBuf, dummyArg);

					if (i) { // if not grade 0, print [* basisVector1 ^ ... ^ basisVectorN]
						l += snprintf(tmpBuf + l, 256-l, "%s", e2ga_string_mul);

						// print all basis vectors
						bei = 0;
						while (e2ga_basisElements[ia][bei] >= 0) {
							l += snprintf(tmpBuf + l, 256-l, "%s%s", (bei) ? e2ga_string_wedge : "", 
							 e2ga_basisVectorNames[e2ga_basisElements[ia][bei]]);
							 bei++;
						}
					}

					// copy all to 'str'
					if (stdIdx + l <= maxLength) {
						strcpy(str + stdIdx, tmpBuf);
						stdIdx += l;
					}
					else {
						snprintf(str, maxLength, "toString_mv: buffer too small");
						return str;
					}
					cnt++;
				}
				k++; ia++;
			}
		}
		else ia += e2ga_groupSize[i];
	}

    // if no coordinates printed: 0
	l = 0;
	if (cnt == 0) {
		l += snprintf(tmpBuf + l, 256-l, "0");
	}

    // end the string
	l += snprintf(tmpBuf + l, 256-l, "%s", e2ga_string_end);
	if (stdIdx + l <= maxLength) {
		strcpy(str + stdIdx, tmpBuf);
		stdIdx += l;
		return str;
	}
	else {
		snprintf(str, maxLength, "toString(): buffer too small\n");
		return str;
	}
}

std::string toString(const mv & obj, const char *fp /* = NULL */) {
	std::string str;
	int strSize = 2048;
	while (strSize <= 1024 * 1024) {
		str.resize(strSize);
		c_str(obj, &(str[0]), strSize-1, fp);
		if (str.find("buffer too small") != std::string::npos) {
			strSize *= 2; // need larger buffer
			continue;
		}
		else break; // done
	}
	str.resize(strlen(&(str[0])));
	return str;
}

// def SB:
/// Computes the partial geometric product of two multivectors (group 0  x  group 0 -> group 0)
void gp_default_0_0_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 1 -> group 1)
void gp_default_0_1_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 0  x  group 2 -> group 2)
void gp_default_0_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 0 -> group 1)
void gp_default_1_0_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 1 -> group 0)
void gp_default_1_1_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 1 -> group 2)
void gp_default_1_1_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 2 -> group 1)
void gp_default_1_2_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 0 -> group 2)
void gp_default_2_0_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 1 -> group 1)
void gp_default_2_1_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 2 -> group 0)
void gp_default_2_2_0(const double *A, const double *B, double *C);
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
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_0_2(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_0_2(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_1_1(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_1_1(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_2_0(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_2_0(const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_1_1(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_2_2(const om &O, const double *A, double *C);
void gp_default_0_0_0(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
}
void gp_default_0_1_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
}
void gp_default_0_2_2(const double *A, const double *B, double *C) {
	gp_default_0_0_0(A, B, C);
}
void gp_default_1_0_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
}
void gp_default_1_1_0(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[0]+A[1]*B[1]);
}
void gp_default_1_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]-A[1]*B[0]);
}
void gp_default_1_2_1(const double *A, const double *B, double *C) {
	C[0] += -A[1]*B[0];
	C[1] += A[0]*B[0];
}
void gp_default_2_0_2(const double *A, const double *B, double *C) {
	gp_default_0_0_0(A, B, C);
}
void gp_default_2_1_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[1];
	C[1] += -A[0]*B[0];
}
void gp_default_2_2_0(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[0];
}
void copyGroup_0(const double *A, double *C) {
	C[0] = A[0];
}
void copyMul_0(const double *A, double *C, double s) {
	C[0] = A[0]*s;
}
void copyDiv_0(const double *A, double *C, double s) {
	C[0] = A[0]/s;
}
void add_0(const double *A, double *C) {
	C[0] += A[0];
}
void sub_0(const double *A, double *C) {
	C[0] -= A[0];
}
void neg_0(const double *A, double *C) {
	C[0] = -A[0];
}
void add2_0_0(const double *A, const double *B, double *C) {
	C[0] = (A[0]+B[0]);
}
void sub2_0_0(const double *A, const double *B, double *C) {
	C[0] = (A[0]-B[0]);
}
void hp_0_0(const double *A, const double *B, double *C) {
	C[0] = A[0]*B[0];
}
void ihp_0_0(const double *A, const double *B, double *C) {
	C[0] = A[0]/((B[0]));
}
bool equals_0_0(const double *A, const double *B, double eps) {
		if (((A[0] - B[0]) < -eps) || ((A[0] - B[0]) > eps)) return false;
	return true;
}
bool zeroGroup_0(const double *A, double eps) {
		if ((A[0] < -eps) || (A[0] > eps)) return false;
		return true;
}
void copyGroup_1(const double *A, double *C) {
	C[0] = A[0];
	C[1] = A[1];
}
void copyMul_1(const double *A, double *C, double s) {
	C[0] = A[0]*s;
	C[1] = A[1]*s;
}
void copyDiv_1(const double *A, double *C, double s) {
	C[0] = A[0]/s;
	C[1] = A[1]/s;
}
void add_1(const double *A, double *C) {
	C[0] += A[0];
	C[1] += A[1];
}
void sub_1(const double *A, double *C) {
	C[0] -= A[0];
	C[1] -= A[1];
}
void neg_1(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[1];
}
void add2_1_1(const double *A, const double *B, double *C) {
	C[0] = (A[0]+B[0]);
	C[1] = (A[1]+B[1]);
}
void sub2_1_1(const double *A, const double *B, double *C) {
	C[0] = (A[0]-B[0]);
	C[1] = (A[1]-B[1]);
}
void hp_1_1(const double *A, const double *B, double *C) {
	C[0] = A[0]*B[0];
	C[1] = A[1]*B[1];
}
void ihp_1_1(const double *A, const double *B, double *C) {
	C[0] = A[0]/((B[0]));
	C[1] = A[1]/((B[1]));
}
bool equals_1_1(const double *A, const double *B, double eps) {
		if (((A[0] - B[0]) < -eps) || ((A[0] - B[0]) > eps)) return false;
		if (((A[1] - B[1]) < -eps) || ((A[1] - B[1]) > eps)) return false;
	return true;
}
bool zeroGroup_1(const double *A, double eps) {
		if ((A[0] < -eps) || (A[0] > eps)) return false;
		if ((A[1] < -eps) || (A[1] > eps)) return false;
		return true;
}
void copyGroup_2(const double *A, double *C) {
	copyGroup_0(A, C);
}
void copyMul_2(const double *A, double *C, double s) {
	copyMul_0(A, C, s);
}
void copyDiv_2(const double *A, double *C, double s) {
	copyDiv_0(A, C, s);
}
void add_2(const double *A, double *C) {
	add_0(A, C);
}
void sub_2(const double *A, double *C) {
	sub_0(A, C);
}
void neg_2(const double *A, double *C) {
	neg_0(A, C);
}
void add2_2_2(const double *A, const double *B, double *C) {
	add2_0_0(A, B, C);
}
void sub2_2_2(const double *A, const double *B, double *C) {
	sub2_0_0(A, B, C);
}
void hp_2_2(const double *A, const double *B, double *C) {
	hp_0_0(A, B, C);
}
void ihp_2_2(const double *A, const double *B, double *C) {
	ihp_0_0(A, B, C);
}
bool equals_2_2(const double *A, const double *B, double eps) {
	return equals_0_0(A, B, eps);
}
bool zeroGroup_2(const double *A, double eps) {
	return zeroGroup_0(A, eps);
}
void dual_default_0_2(const double *A, double *C) {
	C[0] = -A[0];
}
void undual_default_0_2(const double *A, double *C) {
	C[0] = A[0];
}
void dual_default_1_1(const double *A, double *C) {
	C[0] = A[1];
	C[1] = -A[0];
}
void undual_default_1_1(const double *A, double *C) {
	C[0] = -A[1];
	C[1] = A[0];
}
void dual_default_2_0(const double *A, double *C) {
	undual_default_0_2(A, C);
}
void undual_default_2_0(const double *A, double *C) {
	dual_default_0_2(A, C);
}
void applyGomGmv_1_1(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m1[0]+A[1]*O.m_m1[1]);
	C[1] = (A[0]*O.m_m1[2]+A[1]*O.m_m1[3]);
}
void applyGomGmv_2_2(const om &O, const double *A, double *C) {
	C[0] = A[0]*O.m_m2[0];
}

double mv::largestCoordinate() const {
	double maxValue = 0.0;
	int nbC = e2ga_mvSize[m_gu], i;
	for (i = 0; i < nbC; i++)
		if (fabs(m_c[i]) > maxValue) maxValue = fabs(m_c[i]);
	return maxValue;
}

double mv::largestBasisBlade(unsigned int &bm) const {
	int nc = e2ga_mvSize[gu()];
	double maxC = -1.0, C;

	int idx = 0;
	int group = 0;
	int i = 0;
	
	bm = 0;

	while (i < nc) {
		if (gu() & (1 << group)) {
			for (int j = 0; j < e2ga_groupSize[group]; j++) {
				C = fabs(m_c[i]);
				if (C > maxC) {
					maxC = C;
					bm = e2ga_basisElementBitmapByIndex[idx];
				}
				idx++;
				i++;
			}
		}
		else idx += e2ga_groupSize[group];
		group++;
	}

	return maxC;
} // end of mv::largestBasisBlade()




void mv::compress(double epsilon /*= 0.0*/) {
	set(mv_compress(m_c, epsilon, m_gu));
}

mv mv_compress(const double *c, double epsilon /*= 0.0*/, int gu /*= 7*/) {
	int i, j, ia = 0, ib = 0, f, s;
	int cgu = 0;
	double cc[4];

	// for all grade parts...
	for (i = 0; i < 3; i++) {
		// check if grade part has memory use:
		if (!(gu & (1 << i))) continue;

		// check if abs coordinates of grade part are all < epsilon
		s = e2ga_groupSize[i];
		j = ia + s;
		f = 0;
		for (; ia < j; ia++)
			if (fabs(c[ia]) > epsilon) {f = 1; break;}
		ia = j;
		if (f) {
			e2ga::copy_N(cc + ib, c + ia - s, s);
			ib += s;
			cgu |= (1 << i);
		}
	}
	
	return mv(cgu, cc);
}

mv mv_compress(int nbBlades, const unsigned int *bitmaps, const double *coords) {
	// convert basis blade compression to regular coordinate array:
	double A[4];
	for (int i = 0; i < 4; i++) A[i] = 0.0;

	for (int i = 0; i < nbBlades; i++) {
		A[e2ga_basisElementIndexByBitmap[bitmaps[i]]] = coords[i] * (double)e2ga_basisElementSignByBitmap[bitmaps[i]];		
	}

	return mv_compress(A);
}


void mv::expand(const double *ptrs[3], bool nulls /* = true */) const {
	const double *c = m_c; // this pointer gets incremented below
	
	if (m_gu & 1) {
		ptrs[0] =  c;
		c += 1;
	}
	else ptrs[0] = (nulls) ? NULL : nullFloats();
	if (m_gu & 2) {
		ptrs[1] =  c;
		c += 2;
	}
	else ptrs[1] = (nulls) ? NULL : nullFloats();
	if (m_gu & 4) {
		ptrs[2] =  c;
	}
	else ptrs[2] = (nulls) ? NULL : nullFloats();
}

void om::set_grade_2_0()
{
	// Set image of e1^e2
	m_m2[0] = (m_m1[0]*m_m1[3]-m_m1[1]*m_m1[2]);

}
void om::set(const vector &ie1, const vector &ie2)
{
	// Set image of e1
	m_m1[0] = ie1.m_e1;
	m_m1[2] = ie1.m_e2;

	// Set image of e2
	m_m1[1] = ie2.m_e1;
	m_m1[3] = ie2.m_e2;

	om::set_grade_2_0();
}
void om::set(const double *M)
{
	// Set image of e1
	m_m1[0] = M[0];
	m_m1[2] = M[2];

	// Set image of e2
	m_m1[1] = M[1];
	m_m1[3] = M[3];

	om::set_grade_2_0();
}
mv add(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() | b.gu();
	double c[4];
	
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
			bidx += 2;
		}
		else copyGroup_1(a.getC() + aidx, c + cidx);
		aidx += 2;
		cidx += 2;
	}
	else if (b.gu() & 2) {
		copyGroup_1(b.getC() + bidx, c + cidx);
		bidx += 2;
		cidx += 2;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			add2_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
		else copyGroup_2(a.getC() + aidx, c + cidx);
		cidx += 1;
	}
	else if (b.gu() & 4) {
		copyGroup_2(b.getC() + bidx, c + cidx);
		cidx += 1;
	}
	return mv(gu, c);
}
mv subtract(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() | b.gu();
	double c[4];
	
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
			bidx += 2;
		}
		else copyGroup_1(a.getC() + aidx, c + cidx);
		aidx += 2;
		cidx += 2;
	}
	else if (b.gu() & 2) {
		neg_1(b.getC() + bidx, c + cidx);
		bidx += 2;
		cidx += 2;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			sub2_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
		else copyGroup_2(a.getC() + aidx, c + cidx);
		cidx += 1;
	}
	else if (b.gu() & 4) {
		neg_2(b.getC() + bidx, c + cidx);
		cidx += 1;
	}
	return mv(gu, c);
}
mv applyVersor(const mv &a, const mv &b)
{
	return extractGrade(gp(gp(a, b), versorInverse(a)), b.gu());
}
mv applyUnitVersor(const mv &a, const mv &b)
{
	return extractGrade(gp(gp(a, b), reverse(a)), b.gu());
}
mv applyVersorWI(const mv &a, const mv &b, const mv &c)
{
	return extractGrade(gp(gp(a, b), c), b.gu());
}
mv div(const mv &a, const double b)
{
	int idx = 0;
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, b);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, b);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, b);
	}
	return mv(gu, c);
}
mv dual(const mv &a)
{
	int idx = 0;
	double c[4];
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		dual_default_0_2(a.getC() + idx, c + 3);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		dual_default_1_1(a.getC() + idx, c + 1);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		dual_default_2_0(a.getC() + idx, c + 0);
	}
	
	return mv_compress(c, 0.0, 7);
}
mv undual(const mv &a)
{
	int idx = 0;
	double c[4];
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		undual_default_0_2(a.getC() + idx, c + 3);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		undual_default_1_1(a.getC() + idx, c + 1);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		undual_default_2_0(a.getC() + idx, c + 0);
	}
	
	return mv_compress(c, 0.0, 7);
}
bool equals(const mv &a, const mv &b, const double c)
{
	int aidx = 0, bidx = 0;
	
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			if (!equals_0_0(a.getC() + aidx, b.getC() + bidx, c)) return false;
			bidx += 1;
		}
		else if (!zeroGroup_0(a.getC() + bidx, c)) return false;
		aidx += 1;
	}
	else if (b.gu() & 1) {
		if (!zeroGroup_0(b.getC() + bidx, c)) return false;
		bidx += 1;
	}
	
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			if (!equals_1_1(a.getC() + aidx, b.getC() + bidx, c)) return false;
			bidx += 2;
		}
		else if (!zeroGroup_1(a.getC() + bidx, c)) return false;
		aidx += 2;
	}
	else if (b.gu() & 2) {
		if (!zeroGroup_1(b.getC() + bidx, c)) return false;
		bidx += 2;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			if (!equals_2_2(a.getC() + aidx, b.getC() + bidx, c)) return false;
		}
		else if (!zeroGroup_2(a.getC() + bidx, c)) return false;
	}
	else if (b.gu() & 4) {
		if (!zeroGroup_2(b.getC() + bidx, c)) return false;
	}
	return true;
}
mv extractGrade(const mv &a, int groupBitmap)
{
	int aidx = 0, cidx = 0;
	int gu =  a.gu() & groupBitmap;
	double c[4];
	
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
			cidx += 2;
		}
		aidx += 2;
	}
	
	if (a.gu() & 4) {
		if (groupBitmap & 4) {
			copyGroup_2(a.getC() + aidx, c + cidx);
		}
	}
	return mv(gu, c);
}
mv extractGrade2(const mv &a)
{
	return extractGrade(a, 4);
}
mv gp(const mv &a, const mv &b)
{
	double c[4];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 3);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
			gp_default_1_1_2(_a[1], _b[1], c + 3);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 3);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	return mv_compress(c, 0.0, 7);
}
int gradeBitmap(const mv &a, const double b)
{
	int idx = 0;
	int bitmap = 0;
	
	if (a.gu() & 1) {
		if (!zeroGroup_0(a.getC() + idx, b)) bitmap |= 1;
		idx += 1;
	}
	
	if (a.gu() & 2) {
		if (!zeroGroup_1(a.getC() + idx, b)) bitmap |= 2;
		idx += 2;
	}
	
	if (a.gu() & 4) {
		if (!zeroGroup_2(a.getC() + idx, b)) bitmap |= 4;
	}
	return bitmap;
}
mv hp(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() & b.gu();
	double c[4];
	
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			hp_0_0(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 1;
			cidx += 1;
		}
		aidx += 1;
	}
	else if (b.gu() & 1) {
		bidx += 1;
	}
	
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			hp_1_1(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 2;
			cidx += 2;
		}
		aidx += 2;
	}
	else if (b.gu() & 2) {
		bidx += 2;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			hp_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
	}
	else if (b.gu() & 4) {
	}
	return mv(gu, c);
}
mv ihp(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() & b.gu();
	double c[4];
	
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			ihp_0_0(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 1;
			cidx += 1;
		}
		aidx += 1;
	}
	else if (b.gu() & 1) {
		bidx += 1;
	}
	
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			ihp_1_1(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 2;
			cidx += 2;
		}
		aidx += 2;
	}
	else if (b.gu() & 2) {
		bidx += 2;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			ihp_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
	}
	else if (b.gu() & 4) {
	}
	return mv(gu, c);
}
mv increment(const mv &a)
{
	mv _dst(a);
	double val = _dst.get_scalar() + 1.0;
	_dst.set_scalar(val);
	return _dst;
}
mv decrement(const mv &a)
{
	mv _dst(a);
	double val = _dst.get_scalar() - 1.0;
	_dst.set_scalar(val);
	return _dst;
}
double sp(const mv &a, const mv &b)
{
	double c[1];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_1(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	return c[0];
}
mv lc(const mv &a, const mv &b)
{
	double c[4];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 3);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	return mv_compress(c, 0.0, 7);
}
mv rc(const mv &a, const mv &b)
{
	double c[4];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 3);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	return mv_compress(c, 0.0, 7);
}
mv hip(const mv &a, const mv &b)
{
	double c[4];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_4(c);
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	return mv_compress(c, 0.0, 7);
}
mv mhip(const mv &a, const mv &b)
{
	double c[4];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 3);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 3);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	return mv_compress(c, 0.0, 7);
}
double norm(const mv &a)
{
	double c[1];
	double n2 = 0.0;
	int idx = 0;
	
	if (a.gu() & 1) { /* group 0 (grade 0) */
		c[0] = 0.0;
		gp_default_0_0_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 1;
	}
	
	if (a.gu() & 2) { /* group 1 (grade 1) */
		c[0] = 0.0;
		gp_default_1_1_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 2;
	}
	
	if (a.gu() & 4) { /* group 2 (grade 2) */
		c[0] = 0.0;
		gp_default_2_2_0(a.getC() + idx, a.getC() + idx, c);
		n2 -= c[0];
	}
	return ::sqrt(n2);
}
double norm2(const mv &a)
{
	double c[1];
	double n2 = 0.0;
	int idx = 0;
	
	if (a.gu() & 1) { /* group 0 (grade 0) */
		c[0] = 0.0;
		gp_default_0_0_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 1;
	}
	
	if (a.gu() & 2) { /* group 1 (grade 1) */
		c[0] = 0.0;
		gp_default_1_1_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 2;
	}
	
	if (a.gu() & 4) { /* group 2 (grade 2) */
		c[0] = 0.0;
		gp_default_2_2_0(a.getC() + idx, a.getC() + idx, c);
		n2 -= c[0];
	}
	return n2;
}
mv op(const mv &a, const mv &b)
{
	double c[4];
	const double* _a[3];
	const double* _b[3];
	a.expand(_a);
	b.expand(_b);
	e2ga::zero_4(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 3);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_2(_a[1], _b[1], c + 3);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 3);
		}
	}
	return mv_compress(c, 0.0, 7);
}
mv gp(const mv &a, const double b)
{
	double c[4];
	const double* _a[3];
	const double* _b[1] = {&b};
	a.expand(_a);
	e2ga::zero_4(c);
	if (a.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
	}
	if (a.gu() & 2) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
	}
	if (a.gu() & 4) {
			gp_default_2_0_2(_a[2], _b[0], c + 3);
	}
	return mv_compress(c, 0.0, 7);
}
mv sas(const mv &a, const double b, const double c)
{
	int idxa = 0, idxc = 0;
	int gu = a.gu() | ((c != 0.0) ? GRADE_0 : 0); // '| GRADE_0' to make sure the scalar part is included
	double C[4];
	
	if (a.gu() & 1) {
		copyMul_0(a.getC() + idxa, C + idxc, b);
		if (c != 0.0) C[idxc] += c;
		idxa += 1;
		idxc += 1;
	}
	else if (c != 0.0) {
		C[idxc] = c;
		idxc += 1;
	}
	
	if (a.gu() & 2) {
		copyMul_1(a.getC() + idxa, C + idxc, b);
		idxa += 2;
		idxc += 2;
	}
	
	if (a.gu() & 4) {
		copyMul_2(a.getC() + idxa, C + idxc, b);
	}
	return mv(gu, C);
}


mv exp(const mv &x, int order /*  = 12 */) {
	unsigned long maxC;
	int scale = 1;
	mv xScaled;
	mv tmp1, tmp2; // temp mv used for various purposes
	mv xPow1, xPow2;
	mv *result1 = &tmp1, *result2 = &tmp2;
	double s_x2, a;
	int i;
   
	// First try special cases: check if (x * x) is scalar
	tmp1 = gp(x, x);
	s_x2 = _double(tmp1);
	if ((norm2_returns_scalar(tmp1) - s_x2 * s_x2) < 1E-14) {
		// OK (x * x == ~scalar), so use special cases:
		if (s_x2 < 0.0) {
			a = ::sqrt(-s_x2);
			return sas(x, ::sin(a) / a, ::cos(a));
		}
		else if (s_x2 > 0.0) {
			a = ::sqrt(s_x2);
			return sas(x, ::sinh(a) / a, ::cosh(a));
		}
		else {
			return sas(x, 1.0, 1.0);
		}
	}

	// else do general series eval . . .

	// result = 1 + ....	
	*result1 = 1.0;
	if (order == 0) return *result1;

	// find scale (power of 2) such that its norm is < 1
	maxC = (unsigned long)x.largestCoordinate(); // unsigned type is fine, because largest coordinate is absolute
	scale = 1;
	if (maxC > 1) scale <<= 1;
	while (maxC)
	{
		maxC >>= 1;
		scale <<= 1;
	}

	// scale
	xScaled = gp(x, 1.0 / (double)scale); 

	// taylor series approximation
	xPow1 = 1.0; 
	for (i = 1; i <= order; i++) {
		xPow2 = gp(xPow1, xScaled);
		xPow1 = gp(xPow2, 1.0 / (double)i);
		
		*result2 = add(*result1, xPow1); // result2 = result1 + xPow1
		std::swap(result1, result2); // result is always in 'result1' at end of loop
    }

	// undo scaling
	while (scale > 1)
	{
		*result2 = gp(*result1, *result1);
		std::swap(result1, result2); // result is always in 'result1' at end of loop
		scale >>= 1;
	}
    
    return *result1;
} // end of exp()



mv cosh(const mv &x, int order) {
	mv tmp1, tmp2; // temp mv used for various purposes
	mv xPow1, xPow2; // xScaled^... goes here
	mv *result1 = &tmp1, *result2 = &tmp2;
	double s_x2;
	int i;
   
	// First try special cases: check if (x * x) is scalar
	tmp1 = gp(x, x);
	s_x2 = _double(tmp1);
	if ((norm2_returns_scalar(tmp1) - s_x2 * s_x2) < 1E-14) {
		// OK (x * x == ~scalar), so use special cases:
		if (s_x2 > 0.0) {
			return mv(::cosh(::sqrt(s_x2)));
		}
		else if (s_x2 < 0.0) {
			return mv(::cos(::sqrt(-s_x2)));
		}
		else {
			return mv(1.0);
		}
	}

	// else do general series eval . . .


	*result1 = 1.0;
	if (order == 0) return *result1;

	// taylor series approximation
	xPow1 = 1.0;
	for (i = 1; i <= order; i++) {
		xPow2 = gp(xPow1, x);
		xPow1 = gp(xPow2, 1.0 / (double)i); // xPow1 = xScaled^i / i! 
		
		if ((i % 2) == 0) {
			*result2 = add(*result1, xPow1); 
			std::swap(result1, result2); // result is always in 'result1' at end of loop		
		}
    }

    return *result1;
} // end of cosh()

mv sinh(const mv &x, int order /*  = 12 */) {
	mv tmp1, tmp2; // temp mv used for various purposes
	mv xPow1, xPow2; // xScaled^... goes here
	mv *result1 = &tmp1, *result2 = &tmp2;
	double s_x2, a;
	int i;
   
	// First try special cases: check if (x * x) is scalar
	tmp1 = gp(x, x);
	s_x2 = _double(tmp1);
	if ((norm2_returns_scalar(tmp1) - s_x2 * s_x2) < 1E-14) {
		// OK (x * x == ~scalar), so use special cases:
		if (s_x2 < 0.0) {
			a = ::sqrt(-s_x2);
			return sas(x, ::sin(a) / a, 0.0);
		}
		else if (s_x2 > 0.0) {
			a = ::sqrt(s_x2);
			return sas(x, ::sinh(a) / a, 0.0);
		}
		else {
			return x;
		}
	}

	// else do general series eval . . .

	// result = A +  A^3/3! + A^5/5!
	result1->set(); // result = 0
    if (order == 0) return *result1;
    	
	// taylor series approximation
	xPow1 = 1.0;
	for (i = 1; i <= order; i++) {
		xPow2 = gp(xPow1, x); // xPow2 = xPow1 * x
		xPow1 = gp(xPow2, 1.0 / (double)i); // xPow1 = xScaled^i / i! 
		
		if ((i % 2) == 1) {
			*result2 = add(*result1, xPow1); 
			std::swap(result1, result2); 
		}
	}

    return *result1;
} // end of sinh()

mv cos(const mv &x, int order) {
	mv tmp1, tmp2; // temp mv used for various purposes
	mv xPow1, xPow2; // xScaled^... goes here
	mv *result1 = &tmp1, *result2 = &tmp2; // accumulated result goes here (note: 'result1' = 'R')
	double s_x2;
	int i;
   
	// First try special cases: check if (x * x) is scalar
	tmp1 = gp(x, x);
	s_x2 = _double(tmp1);
	if ((norm2_returns_scalar(tmp1) - s_x2 * s_x2) < 1E-14) {
		// OK (x * x == ~scalar), so use special cases:
		if (s_x2 > 0.0) {
			return mv(::cos(::sqrt(s_x2)));
		}
		else if (s_x2 < 0.0) {
			return mv(::cosh(::sqrt(-s_x2)));
		}
		else {
			return mv(1.0);
		}
	}

	// else do general series eval . . .


	*result1 = 1.0;
	if (order == 0) return *result1;

	// taylor series approximation
	xPow1 = 1.0; // xPow1 = 1.0
	for (i = 1; i <= order; i++) {
		xPow2 = gp(xPow1, x); // xPow2 = xPow1 * x
		xPow1 = gp(xPow2, 1.0 / (double)i); // xPow1 = xScaled^i / i! 
		
		if ((i % 4) == 2)
		{
			*result2 = subtract(*result1, xPow1); // result2 = result1 - xPow1
			std::swap(result1, result2); // result is always in 'result1' at end of loop
		}
		else if ((i % 4) == 0) 
		{
			*result2 = add(*result1, xPow1); // result2 = result1 + xPow1
			std::swap(result1, result2); // result is always in 'result1' at end of loop
		}		
    }

	return *result1;
} // end of cos()


mv sin(const mv &x, int order) {
	mv tmp1, tmp2; // temp mv used for various purposes
	mv xPow1, xPow2; // xScaled^... goes here
	mv *result1 = &tmp1, *result2 = &tmp2;
	double s_x2, a;
	int i;
   
	// First try special cases: check if (x * x) is scalar
	tmp1 = gp(x, x);
	s_x2 = _double(tmp1);
	if ((norm2_returns_scalar(tmp1) - s_x2 * s_x2) < 1E-14) {
		// OK (x * x == ~scalar), so use special cases:
		if (s_x2 < 0.0) {
			a = ::sqrt(-s_x2);
			return sas(x, ::sinh(a) / a, 0.0);
		}
		else if (s_x2 > 0.0) {
			a = ::sqrt(s_x2);
			return sas(x, ::sin(a) / a, 0.0);
		}
		else {
			return x;
		}
	}

	// else do general series eval . . .

	// result = A -  ....	+ ... - ...
	result1->set(); // result1 = 0;
    if (order == 0) return *result1;
    	
	// taylor series approximation
	xPow1 = 1.0; // xPow1 = 1.0
	for (i = 1; i <= order; i++) {
		xPow2 = gp(xPow1, x); // xPow2 = xPow1 * x
		
		xPow1 = gp(xPow2, 1.0 / (double)i); // xPow1 = xScaled^i / i! 
		
		if ((i % 4) == 3)
		{
			*result2 = subtract(*result1, xPow1); // result2 = result1 - xPow1
			std::swap(result1, result2); // result is always in 'result1' at end of loop
		}
		else if ((i % 4) == 1) 
		{
			*result2 = add(*result1, xPow1); // result2 = result1 + xPow1
			std::swap(result1, result2); // result is always in 'result1' at end of loop
		}
	}

	return *result1;
} // end of sin()

mv negate(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		neg_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		neg_1(a.getC() + idx, c + idx);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		neg_2(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv cliffordConjugate(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		copyGroup_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		neg_1(a.getC() + idx, c + idx);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		neg_2(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv gradeInvolution(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		copyGroup_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		neg_1(a.getC() + idx, c + idx);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		copyGroup_2(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv reverse(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		copyGroup_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyGroup_1(a.getC() + idx, c + idx);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		neg_2(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv unit(const mv &a)
{
	int idx = 0;
	double n = norm_returns_scalar(a);
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, n);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, n);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, n);
	}
	return mv(gu, c);
}
mv versorInverse(const mv &a)
{
	int idx = 0;
	double n2 = norm2_returns_scalar(a);
	int gu = a.gu();
	double c[4];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, n2);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, n2);
		idx += 2;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, -n2);
	}
	return mv(gu, c);
}
bool zero(const mv &a, const double b)
{
	int idx = 0;
	
	if (a.gu() & 1) {
		if (!zeroGroup_0(a.getC() + idx, b)) return false;
		idx += 1;
	}
	
	if (a.gu() & 2) {
		if (!zeroGroup_1(a.getC() + idx, b)) return false;
		idx += 2;
	}
	
	if (a.gu() & 4) {
		if (!zeroGroup_2(a.getC() + idx, b)) return false;
	}
	return true;
}
} // end of namespace e2ga
