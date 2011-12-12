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
#include "c2ga.h"
namespace c2ga {

const int c2ga_spaceDim = 4;
const int c2ga_nbGroups = 5;
const bool c2ga_metricEuclidean = false;
const char *c2ga_basisVectorNames[4] = {
	"no", "e1", "e2", "ni"
};
const int c2ga_grades[] = {GRADE_0, GRADE_1, GRADE_2, GRADE_3, GRADE_4, 0, 0, 0, 0, 0};
const int c2ga_groups[] = {GROUP_0, GROUP_1, GROUP_2, GROUP_3, GROUP_4};
const int c2ga_groupSize[5] = {
	1, 4, 6, 4, 1
};
const int c2ga_mvSize[32] = {
	0, 1, 4, 5, 6, 7, 10, 11, 4, 5, 8, 9, 10, 11, 14, 15, 1, 2, 5, 6, 
	7, 8, 11, 12, 5, 6, 9, 10, 11, 12, 15, 16};
const int c2ga_basisElements[16][5] = {
	{-1},
	{0, -1},
	{1, -1},
	{2, -1},
	{3, -1},
	{0, 1, -1},
	{0, 2, -1},
	{1, 2, -1},
	{0, 3, -1},
	{1, 3, -1},
	{2, 3, -1},
	{0, 1, 2, -1},
	{0, 1, 3, -1},
	{0, 2, 3, -1},
	{1, 2, 3, -1},
	{0, 1, 2, 3, -1}
};
const double c2ga_basisElementSignByIndex[16] =
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const double c2ga_basisElementSignByBitmap[16] =
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const int c2ga_basisElementIndexByBitmap[16] =
	{0, 1, 2, 5, 3, 6, 7, 11, 4, 8, 9, 12, 10, 13, 14, 15};
const int c2ga_basisElementBitmapByIndex[16] =
	{0, 1, 2, 4, 8, 3, 5, 6, 9, 10, 12, 7, 11, 13, 14, 15};
const int c2ga_basisElementGradeByBitmap[16] =
	{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
const int c2ga_basisElementGroupByBitmap[16] =
	{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

const char *g_c2gaTypenames[] = 
{
	"mv",
	"double",
	"no_t",
	"e1_t",
	"e2_t",
	"ni_t",
	"noni_t",
	"I2_t",
	"I4_t",
	"I4i_t",
	"vectorE2GA",
	"bivectorE2GA",
	"rotorE2GA",
	"normalizedPoint",
	"dualCircle",
	"freeVector",
	"freeBivector",
	"circle",
	"line",
	"pseudoscalar",
	"normalizedTranslator",
	"translator",
	"evenVersor",
	"oddVersor"
};
no_t no;
e1_t e1;
e2_t e2;
ni_t ni;
noni_t noni;
I2_t I2;
I4_t I4;
I4i_t I4i;
vectorE2GA vectorE1(vectorE2GA::coord_e1_e2, 1.0, 0.0);
vectorE2GA vectorE2(vectorE2GA::coord_e1_e2, 0.0, 1.0);
normalizedPoint pointAtOrigin(normalizedPoint::coord_e1_e2_ni, 0.0, 0.0, 0.0);
dualCircle pointAtInfinity(dualCircle::coord_no_e1_e2_ni, 0.0, 0.0, 0.0, 1.0);


void ReportUsage::printReport(FILE *F /*= stdout*/, bool includeCount /* = true */) {
	fprintf(F, "Report usage is disabled");
}

/* 
These strings determine how the output of string() is formatted.
You can alter them at runtime using c2ga_setStringFormat().
*/
 
const char *c2ga_string_fp = "%2.2f"; 
const char *c2ga_string_start = ""; 
const char *c2ga_string_end = ""; 
const char *c2ga_string_mul = "*"; 
const char *c2ga_string_wedge = "^"; 
const char *c2ga_string_plus = " + "; 
const char *c2ga_string_minus = " - "; 

void c2ga_setStringFormat(const char *what, const char *format) {
 
	if (!strcmp(what, "fp")) 
		c2ga_string_fp = (format) ? format : "%2.2f"; 
	else if (!strcmp(what, "start")) 
		c2ga_string_start = (format) ? format : ""; 
	else if (!strcmp(what, "end")) 
		c2ga_string_end = (format) ? format : ""; 
	else if (!strcmp(what, "mul")) 
		c2ga_string_mul = (format) ? format : "*"; 
	else if (!strcmp(what, "wedge")) 
		c2ga_string_wedge = (format) ? format : "^"; 
	else if (!strcmp(what, "plus")) 
		c2ga_string_plus = (format) ? format : " + "; 
	else if (!strcmp(what, "minus")) 
		c2ga_string_minus = (format) ? format : " - ";
}



#ifdef WIN32
#define snprintf _snprintf
#pragma warning(disable:4996) /* quit your whining already */
#endif /* WIN32 */
const char *c_str(const mv &V, char *str, int maxLength, const char *fp) 
{
	int stdIdx = 0, l;
	char tmpBuf[256], tmpFloatBuf[256]; 
	int k = 0, bei, ia = 0, s = c2ga_mvSize[V.gu()], p = 0, cnt = 0;
	const double *c = V.getC();

	// set up the floating point precision
	if (fp == NULL) fp = c2ga_string_fp;

	// start the string
	l = snprintf(tmpBuf, 256, "%s", c2ga_string_start);
	if (stdIdx + l <= maxLength) {
		strcpy(str + stdIdx, tmpBuf);
		stdIdx += l;
	}
	else {
		snprintf(str, maxLength, "toString_mv: buffer too small");
		return str;
	}

	// print all coordinates
	for (int i = 0; i <= 5; i++) {
		if (V.gu() & (1 << i)) {
			for (int j = 0; j < c2ga_groupSize[i]; j++) {
				double coord = (double)c2ga_basisElementSignByIndex[ia] *c[k];
				/* goal: print [+|-]V.m_c[k][* basisVector1 ^ ... ^ basisVectorN] */			
				snprintf(tmpFloatBuf, 256, fp, (double)fabs(coord)); // cast to double to force custom float types to Plain Old Data
				if (atof(tmpFloatBuf) != 0.0) {
					l = 0;

					// print [+|-]
					l += snprintf(tmpBuf + l, 256-l, "%s", (coord >= 0.0) 
						? (cnt ? c2ga_string_plus : "")
						: c2ga_string_minus);
						
					// print obj.m_c[k]
					int dummyArg = 0; // prevents compiler warning on some platforms
					l += snprintf(tmpBuf + l, 256-l, tmpFloatBuf, dummyArg);

					if (i) { // if not grade 0, print [* basisVector1 ^ ... ^ basisVectorN]
						l += snprintf(tmpBuf + l, 256-l, "%s", c2ga_string_mul);

						// print all basis vectors
						bei = 0;
						while (c2ga_basisElements[ia][bei] >= 0) {
							l += snprintf(tmpBuf + l, 256-l, "%s%s", (bei) ? c2ga_string_wedge : "", 
							 c2ga_basisVectorNames[c2ga_basisElements[ia][bei]]);
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
		else ia += c2ga_groupSize[i];
	}

    // if no coordinates printed: 0
	l = 0;
	if (cnt == 0) {
		l += snprintf(tmpBuf + l, 256-l, "0");
	}

    // end the string
	l += snprintf(tmpBuf + l, 256-l, "%s", c2ga_string_end);
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
void gp_default_0_0_0(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
}
void gp_default_0_1_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
}
void gp_default_0_2_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
	C[4] += A[0]*B[4];
	C[5] += A[0]*B[5];
}
void gp_default_0_3_3(const double *A, const double *B, double *C) {
	gp_default_0_1_1(A, B, C);
}
void gp_default_0_4_4(const double *A, const double *B, double *C) {
	gp_default_0_0_0(A, B, C);
}
void gp_default_1_0_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
}
void gp_default_1_1_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[3]+A[1]*B[1]+A[2]*B[2]-A[3]*B[0]);
}
void gp_default_1_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]-A[1]*B[0]);
	C[1] += (A[0]*B[2]-A[2]*B[0]);
	C[2] += (A[1]*B[2]-A[2]*B[1]);
	C[3] += (A[0]*B[3]-A[3]*B[0]);
	C[4] += (A[1]*B[3]-A[3]*B[1]);
	C[5] += (A[2]*B[3]-A[3]*B[2]);
}
void gp_default_1_2_1(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[0]-A[2]*B[1]);
	C[1] += (A[0]*B[4]-A[2]*B[2]-A[3]*B[0]);
	C[2] += (A[0]*B[5]+A[1]*B[2]-A[3]*B[1]);
	C[3] += (A[1]*B[4]+A[2]*B[5]-A[3]*B[3]);
}
void gp_default_1_2_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[4]-A[1]*B[3]+A[3]*B[0]);
	C[2] += (A[0]*B[5]-A[2]*B[3]+A[3]*B[1]);
	C[3] += (A[1]*B[5]-A[2]*B[4]+A[3]*B[2]);
}
void gp_default_1_3_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[1]+A[2]*B[0]);
	C[1] += (-A[0]*B[2]-A[1]*B[0]);
	C[2] += (-A[0]*B[3]-A[3]*B[0]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]);
	C[4] += (-A[2]*B[3]-A[3]*B[1]);
	C[5] += (A[1]*B[3]-A[3]*B[2]);
}
void gp_default_1_3_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
}
void gp_default_1_4_3(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[2]*B[0];
	C[2] += -A[1]*B[0];
	C[3] += -A[3]*B[0];
}
void gp_default_2_0_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
	C[4] += A[4]*B[0];
	C[5] += A[5]*B[0];
}
void gp_default_2_1_1(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]+A[1]*B[2]-A[3]*B[0]);
	C[1] += (A[0]*B[3]+A[2]*B[2]-A[4]*B[0]);
	C[2] += (A[1]*B[3]-A[2]*B[1]-A[5]*B[0]);
	C[3] += (A[3]*B[3]-A[4]*B[1]-A[5]*B[2]);
}
void gp_default_2_1_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[3]-A[3]*B[1]+A[4]*B[0]);
	C[2] += (A[1]*B[3]-A[3]*B[2]+A[5]*B[0]);
	C[3] += (A[2]*B[3]-A[4]*B[2]+A[5]*B[1]);
}
void gp_default_2_2_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[4]-A[1]*B[5]-A[2]*B[2]+A[3]*B[3]-A[4]*B[0]-A[5]*B[1]);
}
void gp_default_2_2_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[2]+A[1]*B[3]-A[2]*B[0]-A[3]*B[1]);
	C[2] += (-A[0]*B[5]+A[1]*B[4]-A[4]*B[1]+A[5]*B[0]);
	C[3] += (A[0]*B[4]+A[1]*B[5]-A[4]*B[0]-A[5]*B[1]);
	C[4] += (A[2]*B[5]+A[3]*B[4]-A[4]*B[3]-A[5]*B[2]);
	C[5] += (-A[2]*B[4]+A[3]*B[5]+A[4]*B[2]-A[5]*B[3]);
}
void gp_default_2_2_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[5]-A[1]*B[4]+A[2]*B[3]+A[3]*B[2]-A[4]*B[1]+A[5]*B[0]);
}
void gp_default_2_3_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[1]-A[1]*B[2]-A[2]*B[0]);
	C[1] += (-A[1]*B[3]-A[3]*B[1]+A[5]*B[0]);
	C[2] += (A[0]*B[3]-A[3]*B[2]-A[4]*B[0]);
	C[3] += (-A[2]*B[3]-A[4]*B[1]-A[5]*B[2]);
}
void gp_default_2_3_3(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[2]+A[1]*B[1]-A[3]*B[0]);
	C[1] += (-A[1]*B[3]+A[2]*B[2]-A[5]*B[0]);
	C[2] += (A[0]*B[3]-A[2]*B[1]+A[4]*B[0]);
	C[3] += (A[3]*B[3]-A[4]*B[2]+A[5]*B[1]);
}
void gp_default_2_4_2(const double *A, const double *B, double *C) {
	C[0] += -A[1]*B[0];
	C[1] += A[0]*B[0];
	C[2] += A[3]*B[0];
	C[3] += -A[2]*B[0];
	C[4] += A[5]*B[0];
	C[5] += -A[4]*B[0];
}
void gp_default_3_0_3(const double *A, const double *B, double *C) {
	gp_default_1_0_1(A, B, C);
}
void gp_default_3_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[0]);
	C[1] += (-A[0]*B[1]-A[2]*B[0]);
	C[2] += (-A[0]*B[3]-A[3]*B[0]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]);
	C[4] += (-A[1]*B[3]-A[3]*B[2]);
	C[5] += (-A[2]*B[3]+A[3]*B[1]);
}
void gp_default_3_1_4(const double *A, const double *B, double *C) {
	gp_default_1_3_4(A, B, C);
}
void gp_default_3_2_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[2]-A[1]*B[0]-A[2]*B[1]);
	C[1] += (A[0]*B[5]-A[1]*B[3]-A[3]*B[1]);
	C[2] += (-A[0]*B[4]-A[2]*B[3]+A[3]*B[0]);
	C[3] += (-A[1]*B[4]-A[2]*B[5]-A[3]*B[2]);
}
void gp_default_3_2_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[5]-A[2]*B[2]+A[3]*B[1]);
	C[2] += (-A[0]*B[4]+A[1]*B[2]-A[3]*B[0]);
	C[3] += (-A[1]*B[5]+A[2]*B[4]-A[3]*B[3]);
}
void gp_default_3_3_0(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]+A[1]*B[1]+A[2]*B[2]+A[3]*B[0]);
}
void gp_default_3_3_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[2]+A[2]*B[0]);
	C[1] += (A[0]*B[1]-A[1]*B[0]);
	C[2] += (A[1]*B[2]-A[2]*B[1]);
	C[3] += (-A[0]*B[3]+A[3]*B[0]);
	C[4] += (A[2]*B[3]-A[3]*B[2]);
	C[5] += (-A[1]*B[3]+A[3]*B[1]);
}
void gp_default_3_4_1(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[0];
	C[1] += A[2]*B[0];
	C[2] += -A[1]*B[0];
	C[3] += A[3]*B[0];
}
void gp_default_4_0_4(const double *A, const double *B, double *C) {
	gp_default_0_0_0(A, B, C);
}
void gp_default_4_1_3(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[0];
	C[1] += -A[0]*B[2];
	C[2] += A[0]*B[1];
	C[3] += A[0]*B[3];
}
void gp_default_4_2_2(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[1];
	C[1] += A[0]*B[0];
	C[2] += A[0]*B[3];
	C[3] += -A[0]*B[2];
	C[4] += A[0]*B[5];
	C[5] += -A[0]*B[4];
}
void gp_default_4_3_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += -A[0]*B[2];
	C[2] += A[0]*B[1];
	C[3] += -A[0]*B[3];
}
void gp_default_4_4_0(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[0];
}
void gp_euclidean_0_0_0(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
}
void gp_euclidean_0_1_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
}
void gp_euclidean_0_2_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
	C[4] += A[0]*B[4];
	C[5] += A[0]*B[5];
}
void gp_euclidean_0_3_3(const double *A, const double *B, double *C) {
	gp_euclidean_0_1_1(A, B, C);
}
void gp_euclidean_0_4_4(const double *A, const double *B, double *C) {
	gp_euclidean_0_0_0(A, B, C);
}
void gp_euclidean_1_0_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
}
void gp_euclidean_1_1_0(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[0]+A[1]*B[1]+A[2]*B[2]+A[3]*B[3]);
}
void gp_euclidean_1_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]-A[1]*B[0]);
	C[1] += (A[0]*B[2]-A[2]*B[0]);
	C[2] += (A[1]*B[2]-A[2]*B[1]);
	C[3] += (A[0]*B[3]-A[3]*B[0]);
	C[4] += (A[1]*B[3]-A[3]*B[1]);
	C[5] += (A[2]*B[3]-A[3]*B[2]);
}
void gp_euclidean_1_2_1(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[0]-A[2]*B[1]-A[3]*B[3]);
	C[1] += (A[0]*B[0]-A[2]*B[2]-A[3]*B[4]);
	C[2] += (A[0]*B[1]+A[1]*B[2]-A[3]*B[5]);
	C[3] += (A[0]*B[3]+A[1]*B[4]+A[2]*B[5]);
}
void gp_euclidean_1_2_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[4]-A[1]*B[3]+A[3]*B[0]);
	C[2] += (A[0]*B[5]-A[2]*B[3]+A[3]*B[1]);
	C[3] += (A[1]*B[5]-A[2]*B[4]+A[3]*B[2]);
}
void gp_euclidean_1_3_2(const double *A, const double *B, double *C) {
	C[0] += (A[2]*B[0]+A[3]*B[1]);
	C[1] += (-A[1]*B[0]+A[3]*B[2]);
	C[2] += (A[0]*B[0]+A[3]*B[3]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]);
	C[4] += (A[0]*B[1]-A[2]*B[3]);
	C[5] += (A[0]*B[2]+A[1]*B[3]);
}
void gp_euclidean_1_3_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
}
void gp_euclidean_1_4_3(const double *A, const double *B, double *C) {
	C[0] += -A[3]*B[0];
	C[1] += A[2]*B[0];
	C[2] += -A[1]*B[0];
	C[3] += A[0]*B[0];
}
void gp_euclidean_2_0_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
	C[4] += A[4]*B[0];
	C[5] += A[5]*B[0];
}
void gp_euclidean_2_1_1(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]+A[1]*B[2]+A[3]*B[3]);
	C[1] += (-A[0]*B[0]+A[2]*B[2]+A[4]*B[3]);
	C[2] += (-A[1]*B[0]-A[2]*B[1]+A[5]*B[3]);
	C[3] += (-A[3]*B[0]-A[4]*B[1]-A[5]*B[2]);
}
void gp_euclidean_2_1_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[3]-A[3]*B[1]+A[4]*B[0]);
	C[2] += (A[1]*B[3]-A[3]*B[2]+A[5]*B[0]);
	C[3] += (A[2]*B[3]-A[4]*B[2]+A[5]*B[1]);
}
void gp_euclidean_2_2_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[0]-A[1]*B[1]-A[2]*B[2]-A[3]*B[3]-A[4]*B[4]-A[5]*B[5]);
}
void gp_euclidean_2_2_2(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[2]+A[2]*B[1]-A[3]*B[4]+A[4]*B[3]);
	C[1] += (A[0]*B[2]-A[2]*B[0]-A[3]*B[5]+A[5]*B[3]);
	C[2] += (-A[0]*B[1]+A[1]*B[0]-A[4]*B[5]+A[5]*B[4]);
	C[3] += (A[0]*B[4]+A[1]*B[5]-A[4]*B[0]-A[5]*B[1]);
	C[4] += (-A[0]*B[3]+A[2]*B[5]+A[3]*B[0]-A[5]*B[2]);
	C[5] += (-A[1]*B[3]-A[2]*B[4]+A[3]*B[1]+A[4]*B[2]);
}
void gp_euclidean_2_2_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[5]-A[1]*B[4]+A[2]*B[3]+A[3]*B[2]-A[4]*B[1]+A[5]*B[0]);
}
void gp_euclidean_2_3_1(const double *A, const double *B, double *C) {
	C[0] += (-A[2]*B[0]-A[4]*B[1]-A[5]*B[2]);
	C[1] += (A[1]*B[0]+A[3]*B[1]-A[5]*B[3]);
	C[2] += (-A[0]*B[0]+A[3]*B[2]+A[4]*B[3]);
	C[3] += (-A[0]*B[1]-A[1]*B[2]-A[2]*B[3]);
}
void gp_euclidean_2_3_3(const double *A, const double *B, double *C) {
	C[0] += (A[3]*B[3]-A[4]*B[2]+A[5]*B[1]);
	C[1] += (-A[1]*B[3]+A[2]*B[2]-A[5]*B[0]);
	C[2] += (A[0]*B[3]-A[2]*B[1]+A[4]*B[0]);
	C[3] += (-A[0]*B[2]+A[1]*B[1]-A[3]*B[0]);
}
void gp_euclidean_2_4_2(const double *A, const double *B, double *C) {
	C[0] += -A[5]*B[0];
	C[1] += A[4]*B[0];
	C[2] += -A[3]*B[0];
	C[3] += -A[2]*B[0];
	C[4] += A[1]*B[0];
	C[5] += -A[0]*B[0];
}
void gp_euclidean_3_0_3(const double *A, const double *B, double *C) {
	gp_euclidean_1_0_1(A, B, C);
}
void gp_euclidean_3_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]+A[1]*B[3]);
	C[1] += (-A[0]*B[1]+A[2]*B[3]);
	C[2] += (A[0]*B[0]+A[3]*B[3]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]);
	C[4] += (A[1]*B[0]-A[3]*B[2]);
	C[5] += (A[2]*B[0]+A[3]*B[1]);
}
void gp_euclidean_3_1_4(const double *A, const double *B, double *C) {
	gp_euclidean_1_3_4(A, B, C);
}
void gp_euclidean_3_2_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[2]-A[1]*B[4]-A[2]*B[5]);
	C[1] += (A[0]*B[1]+A[1]*B[3]-A[3]*B[5]);
	C[2] += (-A[0]*B[0]+A[2]*B[3]+A[3]*B[4]);
	C[3] += (-A[1]*B[0]-A[2]*B[1]-A[3]*B[2]);
}
void gp_euclidean_3_2_3(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[5]+A[2]*B[4]-A[3]*B[3]);
	C[1] += (A[0]*B[5]-A[2]*B[2]+A[3]*B[1]);
	C[2] += (-A[0]*B[4]+A[1]*B[2]-A[3]*B[0]);
	C[3] += (A[0]*B[3]-A[1]*B[1]+A[2]*B[0]);
}
void gp_euclidean_3_3_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[0]-A[1]*B[1]-A[2]*B[2]-A[3]*B[3]);
}
void gp_euclidean_3_3_2(const double *A, const double *B, double *C) {
	C[0] += (-A[2]*B[3]+A[3]*B[2]);
	C[1] += (A[1]*B[3]-A[3]*B[1]);
	C[2] += (-A[1]*B[2]+A[2]*B[1]);
	C[3] += (-A[0]*B[3]+A[3]*B[0]);
	C[4] += (A[0]*B[2]-A[2]*B[0]);
	C[5] += (-A[0]*B[1]+A[1]*B[0]);
}
void gp_euclidean_3_4_1(const double *A, const double *B, double *C) {
	C[0] += A[3]*B[0];
	C[1] += -A[2]*B[0];
	C[2] += A[1]*B[0];
	C[3] += -A[0]*B[0];
}
void gp_euclidean_4_0_4(const double *A, const double *B, double *C) {
	gp_euclidean_0_0_0(A, B, C);
}
void gp_euclidean_4_1_3(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[3];
	C[1] += -A[0]*B[2];
	C[2] += A[0]*B[1];
	C[3] += -A[0]*B[0];
}
void gp_euclidean_4_2_2(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[5];
	C[1] += A[0]*B[4];
	C[2] += -A[0]*B[3];
	C[3] += -A[0]*B[2];
	C[4] += A[0]*B[1];
	C[5] += -A[0]*B[0];
}
void gp_euclidean_4_3_1(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[3];
	C[1] += A[0]*B[2];
	C[2] += -A[0]*B[1];
	C[3] += A[0]*B[0];
}
void gp_euclidean_4_4_0(const double *A, const double *B, double *C) {
	gp_euclidean_0_0_0(A, B, C);
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
	C[2] = A[2];
	C[3] = A[3];
}
void copyMul_1(const double *A, double *C, double s) {
	C[0] = A[0]*s;
	C[1] = A[1]*s;
	C[2] = A[2]*s;
	C[3] = A[3]*s;
}
void copyDiv_1(const double *A, double *C, double s) {
	C[0] = A[0]/s;
	C[1] = A[1]/s;
	C[2] = A[2]/s;
	C[3] = A[3]/s;
}
void add_1(const double *A, double *C) {
	C[0] += A[0];
	C[1] += A[1];
	C[2] += A[2];
	C[3] += A[3];
}
void sub_1(const double *A, double *C) {
	C[0] -= A[0];
	C[1] -= A[1];
	C[2] -= A[2];
	C[3] -= A[3];
}
void neg_1(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[1];
	C[2] = -A[2];
	C[3] = -A[3];
}
void add2_1_1(const double *A, const double *B, double *C) {
	C[0] = (A[0]+B[0]);
	C[1] = (A[1]+B[1]);
	C[2] = (A[2]+B[2]);
	C[3] = (A[3]+B[3]);
}
void sub2_1_1(const double *A, const double *B, double *C) {
	C[0] = (A[0]-B[0]);
	C[1] = (A[1]-B[1]);
	C[2] = (A[2]-B[2]);
	C[3] = (A[3]-B[3]);
}
void hp_1_1(const double *A, const double *B, double *C) {
	C[0] = A[0]*B[0];
	C[1] = A[1]*B[1];
	C[2] = A[2]*B[2];
	C[3] = A[3]*B[3];
}
void ihp_1_1(const double *A, const double *B, double *C) {
	C[0] = A[0]/((B[0]));
	C[1] = A[1]/((B[1]));
	C[2] = A[2]/((B[2]));
	C[3] = A[3]/((B[3]));
}
bool equals_1_1(const double *A, const double *B, double eps) {
		if (((A[0] - B[0]) < -eps) || ((A[0] - B[0]) > eps)) return false;
		if (((A[1] - B[1]) < -eps) || ((A[1] - B[1]) > eps)) return false;
		if (((A[2] - B[2]) < -eps) || ((A[2] - B[2]) > eps)) return false;
		if (((A[3] - B[3]) < -eps) || ((A[3] - B[3]) > eps)) return false;
	return true;
}
bool zeroGroup_1(const double *A, double eps) {
		if ((A[0] < -eps) || (A[0] > eps)) return false;
		if ((A[1] < -eps) || (A[1] > eps)) return false;
		if ((A[2] < -eps) || (A[2] > eps)) return false;
		if ((A[3] < -eps) || (A[3] > eps)) return false;
		return true;
}
void copyGroup_2(const double *A, double *C) {
	C[0] = A[0];
	C[1] = A[1];
	C[2] = A[2];
	C[3] = A[3];
	C[4] = A[4];
	C[5] = A[5];
}
void copyMul_2(const double *A, double *C, double s) {
	C[0] = A[0]*s;
	C[1] = A[1]*s;
	C[2] = A[2]*s;
	C[3] = A[3]*s;
	C[4] = A[4]*s;
	C[5] = A[5]*s;
}
void copyDiv_2(const double *A, double *C, double s) {
	C[0] = A[0]/s;
	C[1] = A[1]/s;
	C[2] = A[2]/s;
	C[3] = A[3]/s;
	C[4] = A[4]/s;
	C[5] = A[5]/s;
}
void add_2(const double *A, double *C) {
	C[0] += A[0];
	C[1] += A[1];
	C[2] += A[2];
	C[3] += A[3];
	C[4] += A[4];
	C[5] += A[5];
}
void sub_2(const double *A, double *C) {
	C[0] -= A[0];
	C[1] -= A[1];
	C[2] -= A[2];
	C[3] -= A[3];
	C[4] -= A[4];
	C[5] -= A[5];
}
void neg_2(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[1];
	C[2] = -A[2];
	C[3] = -A[3];
	C[4] = -A[4];
	C[5] = -A[5];
}
void add2_2_2(const double *A, const double *B, double *C) {
	C[0] = (A[0]+B[0]);
	C[1] = (A[1]+B[1]);
	C[2] = (A[2]+B[2]);
	C[3] = (A[3]+B[3]);
	C[4] = (A[4]+B[4]);
	C[5] = (A[5]+B[5]);
}
void sub2_2_2(const double *A, const double *B, double *C) {
	C[0] = (A[0]-B[0]);
	C[1] = (A[1]-B[1]);
	C[2] = (A[2]-B[2]);
	C[3] = (A[3]-B[3]);
	C[4] = (A[4]-B[4]);
	C[5] = (A[5]-B[5]);
}
void hp_2_2(const double *A, const double *B, double *C) {
	C[0] = A[0]*B[0];
	C[1] = A[1]*B[1];
	C[2] = A[2]*B[2];
	C[3] = A[3]*B[3];
	C[4] = A[4]*B[4];
	C[5] = A[5]*B[5];
}
void ihp_2_2(const double *A, const double *B, double *C) {
	C[0] = A[0]/((B[0]));
	C[1] = A[1]/((B[1]));
	C[2] = A[2]/((B[2]));
	C[3] = A[3]/((B[3]));
	C[4] = A[4]/((B[4]));
	C[5] = A[5]/((B[5]));
}
bool equals_2_2(const double *A, const double *B, double eps) {
		if (((A[0] - B[0]) < -eps) || ((A[0] - B[0]) > eps)) return false;
		if (((A[1] - B[1]) < -eps) || ((A[1] - B[1]) > eps)) return false;
		if (((A[2] - B[2]) < -eps) || ((A[2] - B[2]) > eps)) return false;
		if (((A[3] - B[3]) < -eps) || ((A[3] - B[3]) > eps)) return false;
		if (((A[4] - B[4]) < -eps) || ((A[4] - B[4]) > eps)) return false;
		if (((A[5] - B[5]) < -eps) || ((A[5] - B[5]) > eps)) return false;
	return true;
}
bool zeroGroup_2(const double *A, double eps) {
		if ((A[0] < -eps) || (A[0] > eps)) return false;
		if ((A[1] < -eps) || (A[1] > eps)) return false;
		if ((A[2] < -eps) || (A[2] > eps)) return false;
		if ((A[3] < -eps) || (A[3] > eps)) return false;
		if ((A[4] < -eps) || (A[4] > eps)) return false;
		if ((A[5] < -eps) || (A[5] > eps)) return false;
		return true;
}
void copyGroup_3(const double *A, double *C) {
	copyGroup_1(A, C);
}
void copyMul_3(const double *A, double *C, double s) {
	copyMul_1(A, C, s);
}
void copyDiv_3(const double *A, double *C, double s) {
	copyDiv_1(A, C, s);
}
void add_3(const double *A, double *C) {
	add_1(A, C);
}
void sub_3(const double *A, double *C) {
	sub_1(A, C);
}
void neg_3(const double *A, double *C) {
	neg_1(A, C);
}
void add2_3_3(const double *A, const double *B, double *C) {
	add2_1_1(A, B, C);
}
void sub2_3_3(const double *A, const double *B, double *C) {
	sub2_1_1(A, B, C);
}
void hp_3_3(const double *A, const double *B, double *C) {
	hp_1_1(A, B, C);
}
void ihp_3_3(const double *A, const double *B, double *C) {
	ihp_1_1(A, B, C);
}
bool equals_3_3(const double *A, const double *B, double eps) {
	return equals_1_1(A, B, eps);
}
bool zeroGroup_3(const double *A, double eps) {
	return zeroGroup_1(A, eps);
}
void copyGroup_4(const double *A, double *C) {
	copyGroup_0(A, C);
}
void copyMul_4(const double *A, double *C, double s) {
	copyMul_0(A, C, s);
}
void copyDiv_4(const double *A, double *C, double s) {
	copyDiv_0(A, C, s);
}
void add_4(const double *A, double *C) {
	add_0(A, C);
}
void sub_4(const double *A, double *C) {
	sub_0(A, C);
}
void neg_4(const double *A, double *C) {
	neg_0(A, C);
}
void add2_4_4(const double *A, const double *B, double *C) {
	add2_0_0(A, B, C);
}
void sub2_4_4(const double *A, const double *B, double *C) {
	sub2_0_0(A, B, C);
}
void hp_4_4(const double *A, const double *B, double *C) {
	hp_0_0(A, B, C);
}
void ihp_4_4(const double *A, const double *B, double *C) {
	ihp_0_0(A, B, C);
}
bool equals_4_4(const double *A, const double *B, double eps) {
	return equals_0_0(A, B, eps);
}
bool zeroGroup_4(const double *A, double eps) {
	return zeroGroup_0(A, eps);
}
void dual_default_0_4(const double *A, double *C) {
	C[0] = -A[0];
}
void undual_default_0_4(const double *A, double *C) {
	C[0] = A[0];
}
void dual_default_1_3(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[2];
	C[2] = A[1];
	C[3] = A[3];
}
void undual_default_1_3(const double *A, double *C) {
	C[0] = A[0];
	C[1] = A[2];
	C[2] = -A[1];
	C[3] = -A[3];
}
void dual_default_2_2(const double *A, double *C) {
	C[0] = A[1];
	C[1] = -A[0];
	C[2] = -A[3];
	C[3] = A[2];
	C[4] = -A[5];
	C[5] = A[4];
}
void undual_default_2_2(const double *A, double *C) {
	C[0] = -A[1];
	C[1] = A[0];
	C[2] = A[3];
	C[3] = -A[2];
	C[4] = A[5];
	C[5] = -A[4];
}
void dual_default_3_1(const double *A, double *C) {
	C[0] = A[0];
	C[1] = -A[2];
	C[2] = A[1];
	C[3] = -A[3];
}
void undual_default_3_1(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = A[2];
	C[2] = -A[1];
	C[3] = A[3];
}
void dual_default_4_0(const double *A, double *C) {
	undual_default_0_4(A, C);
}
void undual_default_4_0(const double *A, double *C) {
	dual_default_0_4(A, C);
}
void dual_euclidean_0_4(const double *A, double *C) {
	undual_default_0_4(A, C);
}
void undual_euclidean_0_4(const double *A, double *C) {
	undual_default_0_4(A, C);
}
void dual_euclidean_1_3(const double *A, double *C) {
	C[0] = -A[3];
	C[1] = A[2];
	C[2] = -A[1];
	C[3] = A[0];
}
void undual_euclidean_1_3(const double *A, double *C) {
	dual_euclidean_1_3(A, C);
}
void dual_euclidean_2_2(const double *A, double *C) {
	C[0] = -A[5];
	C[1] = A[4];
	C[2] = -A[3];
	C[3] = -A[2];
	C[4] = A[1];
	C[5] = -A[0];
}
void undual_euclidean_2_2(const double *A, double *C) {
	dual_euclidean_2_2(A, C);
}
void dual_euclidean_3_1(const double *A, double *C) {
	C[0] = A[3];
	C[1] = -A[2];
	C[2] = A[1];
	C[3] = -A[0];
}
void undual_euclidean_3_1(const double *A, double *C) {
	dual_euclidean_3_1(A, C);
}
void dual_euclidean_4_0(const double *A, double *C) {
	undual_default_0_4(A, C);
}
void undual_euclidean_4_0(const double *A, double *C) {
	undual_default_0_4(A, C);
}
void applyGomGmv_1_1(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m1[0]+A[1]*O.m_m1[1]+A[2]*O.m_m1[2]+A[3]*O.m_m1[3]);
	C[1] = (A[0]*O.m_m1[4]+A[1]*O.m_m1[5]+A[2]*O.m_m1[6]+A[3]*O.m_m1[7]);
	C[2] = (A[0]*O.m_m1[8]+A[1]*O.m_m1[9]+A[2]*O.m_m1[10]+A[3]*O.m_m1[11]);
	C[3] = (A[0]*O.m_m1[12]+A[1]*O.m_m1[13]+A[2]*O.m_m1[14]+A[3]*O.m_m1[15]);
}
void applyGomGmv_2_2(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m2[0]+A[1]*O.m_m2[1]+A[2]*O.m_m2[2]+A[3]*O.m_m2[3]+A[4]*O.m_m2[4]+A[5]*O.m_m2[5]);
	C[1] = (A[0]*O.m_m2[6]+A[1]*O.m_m2[7]+A[2]*O.m_m2[8]+A[3]*O.m_m2[9]+A[4]*O.m_m2[10]+A[5]*O.m_m2[11]);
	C[2] = (A[0]*O.m_m2[12]+A[1]*O.m_m2[13]+A[2]*O.m_m2[14]+A[3]*O.m_m2[15]+A[4]*O.m_m2[16]+A[5]*O.m_m2[17]);
	C[3] = (A[0]*O.m_m2[18]+A[1]*O.m_m2[19]+A[2]*O.m_m2[20]+A[3]*O.m_m2[21]+A[4]*O.m_m2[22]+A[5]*O.m_m2[23]);
	C[4] = (A[0]*O.m_m2[24]+A[1]*O.m_m2[25]+A[2]*O.m_m2[26]+A[3]*O.m_m2[27]+A[4]*O.m_m2[28]+A[5]*O.m_m2[29]);
	C[5] = (A[0]*O.m_m2[30]+A[1]*O.m_m2[31]+A[2]*O.m_m2[32]+A[3]*O.m_m2[33]+A[4]*O.m_m2[34]+A[5]*O.m_m2[35]);
}
void applyGomGmv_3_3(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m3[0]+A[1]*O.m_m3[1]+A[2]*O.m_m3[2]+A[3]*O.m_m3[3]);
	C[1] = (A[0]*O.m_m3[4]+A[1]*O.m_m3[5]+A[2]*O.m_m3[6]+A[3]*O.m_m3[7]);
	C[2] = (A[0]*O.m_m3[8]+A[1]*O.m_m3[9]+A[2]*O.m_m3[10]+A[3]*O.m_m3[11]);
	C[3] = (A[0]*O.m_m3[12]+A[1]*O.m_m3[13]+A[2]*O.m_m3[14]+A[3]*O.m_m3[15]);
}
void applyGomGmv_4_4(const om &O, const double *A, double *C) {
	C[0] = A[0]*O.m_m4[0];
}

double mv::largestCoordinate() const {
	double maxValue = 0.0;
	int nbC = c2ga_mvSize[m_gu], i;
	for (i = 0; i < nbC; i++)
		if (fabs(m_c[i]) > maxValue) maxValue = fabs(m_c[i]);
	return maxValue;
}

double mv::largestBasisBlade(unsigned int &bm) const {
	int nc = c2ga_mvSize[gu()];
	double maxC = -1.0, C;

	int idx = 0;
	int group = 0;
	int i = 0;
	
	bm = 0;

	while (i < nc) {
		if (gu() & (1 << group)) {
			for (int j = 0; j < c2ga_groupSize[group]; j++) {
				C = fabs(m_c[i]);
				if (C > maxC) {
					maxC = C;
					bm = c2ga_basisElementBitmapByIndex[idx];
				}
				idx++;
				i++;
			}
		}
		else idx += c2ga_groupSize[group];
		group++;
	}

	return maxC;
} // end of mv::largestBasisBlade()




void mv::compress(double epsilon /*= 0.0*/) {
	set(mv_compress(m_c, epsilon, m_gu));
}

mv mv_compress(const double *c, double epsilon /*= 0.0*/, int gu /*= 31*/) {
	int i, j, ia = 0, ib = 0, f, s;
	int cgu = 0;
	double cc[16];

	// for all grade parts...
	for (i = 0; i < 5; i++) {
		// check if grade part has memory use:
		if (!(gu & (1 << i))) continue;

		// check if abs coordinates of grade part are all < epsilon
		s = c2ga_groupSize[i];
		j = ia + s;
		f = 0;
		for (; ia < j; ia++)
			if (fabs(c[ia]) > epsilon) {f = 1; break;}
		ia = j;
		if (f) {
			c2ga::copy_N(cc + ib, c + ia - s, s);
			ib += s;
			cgu |= (1 << i);
		}
	}
	
	return mv(cgu, cc);
}

mv mv_compress(int nbBlades, const unsigned int *bitmaps, const double *coords) {
	// convert basis blade compression to regular coordinate array:
	double A[16];
	for (int i = 0; i < 16; i++) A[i] = 0.0;

	for (int i = 0; i < nbBlades; i++) {
		A[c2ga_basisElementIndexByBitmap[bitmaps[i]]] = coords[i] * (double)c2ga_basisElementSignByBitmap[bitmaps[i]];		
	}

	return mv_compress(A);
}


void mv::expand(const double *ptrs[5], bool nulls /* = true */) const {
	const double *c = m_c; // this pointer gets incremented below
	
	if (m_gu & 1) {
		ptrs[0] =  c;
		c += 1;
	}
	else ptrs[0] = (nulls) ? NULL : nullFloats();
	if (m_gu & 2) {
		ptrs[1] =  c;
		c += 4;
	}
	else ptrs[1] = (nulls) ? NULL : nullFloats();
	if (m_gu & 4) {
		ptrs[2] =  c;
		c += 6;
	}
	else ptrs[2] = (nulls) ? NULL : nullFloats();
	if (m_gu & 8) {
		ptrs[3] =  c;
		c += 4;
	}
	else ptrs[3] = (nulls) ? NULL : nullFloats();
	if (m_gu & 16) {
		ptrs[4] =  c;
	}
	else ptrs[4] = (nulls) ? NULL : nullFloats();
}

void om::set_grade_2_0()
{
	// Set image of no^e1
	m_m2[0] = (m_m1[0]*m_m1[5]-m_m1[1]*m_m1[4]);
	m_m2[6] = (m_m1[0]*m_m1[9]-m_m1[1]*m_m1[8]);
	m_m2[12] = (m_m1[4]*m_m1[9]-m_m1[5]*m_m1[8]);
	m_m2[18] = (m_m1[0]*m_m1[13]-m_m1[1]*m_m1[12]);
	m_m2[24] = (-m_m1[12]*m_m1[5]+m_m1[13]*m_m1[4]);
	m_m2[30] = (-m_m1[12]*m_m1[9]+m_m1[13]*m_m1[8]);

}
void om::set_grade_2_1()
{
	// Set image of no^e2
	m_m2[1] = (m_m1[0]*m_m1[6]-m_m1[2]*m_m1[4]);
	m_m2[7] = (m_m1[0]*m_m1[10]-m_m1[2]*m_m1[8]);
	m_m2[13] = (m_m1[10]*m_m1[4]-m_m1[6]*m_m1[8]);
	m_m2[19] = (m_m1[0]*m_m1[14]-m_m1[12]*m_m1[2]);
	m_m2[25] = (-m_m1[12]*m_m1[6]+m_m1[14]*m_m1[4]);
	m_m2[31] = (-m_m1[10]*m_m1[12]+m_m1[14]*m_m1[8]);

}
void om::set_grade_2_2()
{
	// Set image of e1^e2
	m_m2[2] = (m_m1[1]*m_m1[6]-m_m1[2]*m_m1[5]);
	m_m2[8] = (m_m1[1]*m_m1[10]-m_m1[2]*m_m1[9]);
	m_m2[14] = (m_m1[10]*m_m1[5]-m_m1[6]*m_m1[9]);
	m_m2[20] = (m_m1[1]*m_m1[14]-m_m1[13]*m_m1[2]);
	m_m2[26] = (-m_m1[13]*m_m1[6]+m_m1[14]*m_m1[5]);
	m_m2[32] = (-m_m1[10]*m_m1[13]+m_m1[14]*m_m1[9]);

}
void om::set_grade_2_3()
{
	// Set image of no^ni
	m_m2[3] = (m_m1[0]*m_m1[7]-m_m1[3]*m_m1[4]);
	m_m2[9] = (m_m1[0]*m_m1[11]-m_m1[3]*m_m1[8]);
	m_m2[15] = (m_m1[11]*m_m1[4]-m_m1[7]*m_m1[8]);
	m_m2[21] = (m_m1[0]*m_m1[15]-m_m1[12]*m_m1[3]);
	m_m2[27] = (-m_m1[12]*m_m1[7]+m_m1[15]*m_m1[4]);
	m_m2[33] = (-m_m1[11]*m_m1[12]+m_m1[15]*m_m1[8]);

}
void om::set_grade_2_4()
{
	// Set image of e1^ni
	m_m2[4] = (m_m1[1]*m_m1[7]-m_m1[3]*m_m1[5]);
	m_m2[10] = (m_m1[1]*m_m1[11]-m_m1[3]*m_m1[9]);
	m_m2[16] = (m_m1[11]*m_m1[5]-m_m1[7]*m_m1[9]);
	m_m2[22] = (m_m1[1]*m_m1[15]-m_m1[13]*m_m1[3]);
	m_m2[28] = (-m_m1[13]*m_m1[7]+m_m1[15]*m_m1[5]);
	m_m2[34] = (-m_m1[11]*m_m1[13]+m_m1[15]*m_m1[9]);

}
void om::set_grade_2_5()
{
	// Set image of e2^ni
	m_m2[5] = (m_m1[2]*m_m1[7]-m_m1[3]*m_m1[6]);
	m_m2[11] = (-m_m1[10]*m_m1[3]+m_m1[11]*m_m1[2]);
	m_m2[17] = (-m_m1[10]*m_m1[7]+m_m1[11]*m_m1[6]);
	m_m2[23] = (-m_m1[14]*m_m1[3]+m_m1[15]*m_m1[2]);
	m_m2[29] = (-m_m1[14]*m_m1[7]+m_m1[15]*m_m1[6]);
	m_m2[35] = (m_m1[10]*m_m1[15]-m_m1[11]*m_m1[14]);

}
void om::set_grade_3_0()
{
	// Set image of no^e1^e2
	m_m3[0] = (m_m1[0]*m_m2[14]-m_m1[4]*m_m2[8]+m_m1[8]*m_m2[2]);
	m_m3[4] = (m_m1[0]*m_m2[26]+m_m1[12]*m_m2[2]-m_m1[4]*m_m2[20]);
	m_m3[8] = (m_m1[0]*m_m2[32]+m_m1[12]*m_m2[8]-m_m1[8]*m_m2[20]);
	m_m3[12] = (m_m1[12]*m_m2[14]+m_m1[4]*m_m2[32]-m_m1[8]*m_m2[26]);

}
void om::set_grade_3_1()
{
	// Set image of no^e1^ni
	m_m3[1] = (m_m1[0]*m_m2[16]-m_m1[4]*m_m2[10]+m_m1[8]*m_m2[4]);
	m_m3[5] = (m_m1[0]*m_m2[28]+m_m1[12]*m_m2[4]-m_m1[4]*m_m2[22]);
	m_m3[9] = (m_m1[0]*m_m2[34]+m_m1[12]*m_m2[10]-m_m1[8]*m_m2[22]);
	m_m3[13] = (m_m1[12]*m_m2[16]+m_m1[4]*m_m2[34]-m_m1[8]*m_m2[28]);

}
void om::set_grade_3_2()
{
	// Set image of no^e2^ni
	m_m3[2] = (m_m1[0]*m_m2[17]-m_m1[4]*m_m2[11]+m_m1[8]*m_m2[5]);
	m_m3[6] = (m_m1[0]*m_m2[29]+m_m1[12]*m_m2[5]-m_m1[4]*m_m2[23]);
	m_m3[10] = (m_m1[0]*m_m2[35]+m_m1[12]*m_m2[11]-m_m1[8]*m_m2[23]);
	m_m3[14] = (m_m1[12]*m_m2[17]+m_m1[4]*m_m2[35]-m_m1[8]*m_m2[29]);

}
void om::set_grade_3_3()
{
	// Set image of e1^e2^ni
	m_m3[3] = (m_m1[1]*m_m2[17]-m_m1[5]*m_m2[11]+m_m1[9]*m_m2[5]);
	m_m3[7] = (m_m1[1]*m_m2[29]+m_m1[13]*m_m2[5]-m_m1[5]*m_m2[23]);
	m_m3[11] = (m_m1[1]*m_m2[35]+m_m1[13]*m_m2[11]-m_m1[9]*m_m2[23]);
	m_m3[15] = (m_m1[13]*m_m2[17]+m_m1[5]*m_m2[35]-m_m1[9]*m_m2[29]);

}
void om::set_grade_4_0()
{
	// Set image of no^e1^e2^ni
	m_m4[0] = (m_m1[0]*m_m3[15]-m_m1[12]*m_m3[3]-m_m1[4]*m_m3[11]+m_m1[8]*m_m3[7]);

}
void om::set(const dualCircle &ino, const dualCircle &ie1, const dualCircle &ie2, const dualCircle &ini)
{
	// Set image of no
	m_m1[0] = ino.m_no;
	m_m1[4] = ino.m_e1;
	m_m1[8] = ino.m_e2;
	m_m1[12] = ino.m_ni;

	// Set image of e1
	m_m1[1] = ie1.m_no;
	m_m1[5] = ie1.m_e1;
	m_m1[9] = ie1.m_e2;
	m_m1[13] = ie1.m_ni;

	// Set image of e2
	m_m1[2] = ie2.m_no;
	m_m1[6] = ie2.m_e1;
	m_m1[10] = ie2.m_e2;
	m_m1[14] = ie2.m_ni;

	// Set image of ni
	m_m1[3] = ini.m_no;
	m_m1[7] = ini.m_e1;
	m_m1[11] = ini.m_e2;
	m_m1[15] = ini.m_ni;

	om::set_grade_2_0();
	om::set_grade_2_1();
	om::set_grade_2_2();
	om::set_grade_2_3();
	om::set_grade_2_4();
	om::set_grade_2_5();
	om::set_grade_3_0();
	om::set_grade_3_1();
	om::set_grade_3_2();
	om::set_grade_3_3();
	om::set_grade_4_0();
}
void om::set(const double *M)
{
	// Set image of no
	m_m1[0] = M[0];
	m_m1[4] = M[4];
	m_m1[8] = M[8];
	m_m1[12] = M[12];

	// Set image of e1
	m_m1[1] = M[1];
	m_m1[5] = M[5];
	m_m1[9] = M[9];
	m_m1[13] = M[13];

	// Set image of e2
	m_m1[2] = M[2];
	m_m1[6] = M[6];
	m_m1[10] = M[10];
	m_m1[14] = M[14];

	// Set image of ni
	m_m1[3] = M[3];
	m_m1[7] = M[7];
	m_m1[11] = M[11];
	m_m1[15] = M[15];

	om::set_grade_2_0();
	om::set_grade_2_1();
	om::set_grade_2_2();
	om::set_grade_2_3();
	om::set_grade_2_4();
	om::set_grade_2_5();
	om::set_grade_3_0();
	om::set_grade_3_1();
	om::set_grade_3_2();
	om::set_grade_3_3();
	om::set_grade_4_0();
}
mv gp(const mv &a, const mv &b)
{
	double c[16];
	const double* _a[5];
	const double* _b[5];
	a.expand(_a);
	b.expand(_b);
	c2ga::zero_16(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 5);
		}
		if (b.gu() & 8) {
			gp_default_0_3_3(_a[0], _b[3], c + 11);
		}
		if (b.gu() & 16) {
			gp_default_0_4_4(_a[0], _b[4], c + 15);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
			gp_default_1_1_2(_a[1], _b[1], c + 5);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
			gp_default_1_2_3(_a[1], _b[2], c + 11);
		}
		if (b.gu() & 8) {
			gp_default_1_3_2(_a[1], _b[3], c + 5);
			gp_default_1_3_4(_a[1], _b[3], c + 15);
		}
		if (b.gu() & 16) {
			gp_default_1_4_3(_a[1], _b[4], c + 11);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 5);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
			gp_default_2_1_3(_a[2], _b[1], c + 11);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
			gp_default_2_2_2(_a[2], _b[2], c + 5);
			gp_default_2_2_4(_a[2], _b[2], c + 15);
		}
		if (b.gu() & 8) {
			gp_default_2_3_1(_a[2], _b[3], c + 1);
			gp_default_2_3_3(_a[2], _b[3], c + 11);
		}
		if (b.gu() & 16) {
			gp_default_2_4_2(_a[2], _b[4], c + 5);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_default_3_0_3(_a[3], _b[0], c + 11);
		}
		if (b.gu() & 2) {
			gp_default_3_1_2(_a[3], _b[1], c + 5);
			gp_default_3_1_4(_a[3], _b[1], c + 15);
		}
		if (b.gu() & 4) {
			gp_default_3_2_1(_a[3], _b[2], c + 1);
			gp_default_3_2_3(_a[3], _b[2], c + 11);
		}
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
			gp_default_3_3_2(_a[3], _b[3], c + 5);
		}
		if (b.gu() & 16) {
			gp_default_3_4_1(_a[3], _b[4], c + 1);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_default_4_0_4(_a[4], _b[0], c + 15);
		}
		if (b.gu() & 2) {
			gp_default_4_1_3(_a[4], _b[1], c + 11);
		}
		if (b.gu() & 4) {
			gp_default_4_2_2(_a[4], _b[2], c + 5);
		}
		if (b.gu() & 8) {
			gp_default_4_3_1(_a[4], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
		}
	}
	return mv_compress(c, 0.0, 31);
}
mv op(const mv &a, const mv &b)
{
	double c[16];
	const double* _a[5];
	const double* _b[5];
	a.expand(_a);
	b.expand(_b);
	c2ga::zero_16(c);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 5);
		}
		if (b.gu() & 8) {
			gp_default_0_3_3(_a[0], _b[3], c + 11);
		}
		if (b.gu() & 16) {
			gp_default_0_4_4(_a[0], _b[4], c + 15);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_2(_a[1], _b[1], c + 5);
		}
		if (b.gu() & 4) {
			gp_default_1_2_3(_a[1], _b[2], c + 11);
		}
		if (b.gu() & 8) {
			gp_default_1_3_4(_a[1], _b[3], c + 15);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 5);
		}
		if (b.gu() & 2) {
			gp_default_2_1_3(_a[2], _b[1], c + 11);
		}
		if (b.gu() & 4) {
			gp_default_2_2_4(_a[2], _b[2], c + 15);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_default_3_0_3(_a[3], _b[0], c + 11);
		}
		if (b.gu() & 2) {
			gp_default_3_1_4(_a[3], _b[1], c + 15);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_default_4_0_4(_a[4], _b[0], c + 15);
		}
	}
	return mv_compress(c, 0.0, 31);
}
} // end of namespace c2ga
