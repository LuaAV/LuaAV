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
#include "c3ga.h"
namespace c3ga {

const int c3ga_spaceDim = 5;
const int c3ga_nbGroups = 6;
const bool c3ga_metricEuclidean = false;
const char *c3ga_basisVectorNames[5] = {
	"no", "e1", "e2", "e3", "ni"
};
const int c3ga_grades[] = {GRADE_0, GRADE_1, GRADE_2, GRADE_3, GRADE_4, GRADE_5, 0, 0, 0, 0, 0, 0};
const int c3ga_groups[] = {GROUP_0, GROUP_1, GROUP_2, GROUP_3, GROUP_4, GROUP_5};
const int c3ga_groupSize[6] = {
	1, 5, 10, 10, 5, 1
};
const int c3ga_mvSize[64] = {
	0, 1, 5, 6, 10, 11, 15, 16, 10, 11, 15, 16, 20, 21, 25, 26, 5, 6, 10, 11, 
	15, 16, 20, 21, 15, 16, 20, 21, 25, 26, 30, 31, 1, 2, 6, 7, 11, 12, 16, 17, 
	11, 12, 16, 17, 21, 22, 26, 27, 6, 7, 11, 12, 16, 17, 21, 22, 16, 17, 21, 22, 
	26, 27, 31, 32};
const int c3ga_basisElements[32][6] = {
	{-1},
	{0, -1},
	{1, -1},
	{2, -1},
	{3, -1},
	{4, -1},
	{0, 1, -1},
	{0, 2, -1},
	{1, 2, -1},
	{0, 3, -1},
	{1, 3, -1},
	{2, 3, -1},
	{0, 4, -1},
	{1, 4, -1},
	{2, 4, -1},
	{3, 4, -1},
	{0, 1, 2, -1},
	{0, 1, 3, -1},
	{0, 2, 3, -1},
	{1, 2, 3, -1},
	{0, 1, 4, -1},
	{0, 2, 4, -1},
	{1, 2, 4, -1},
	{0, 3, 4, -1},
	{1, 3, 4, -1},
	{2, 3, 4, -1},
	{0, 1, 2, 3, -1},
	{0, 1, 2, 4, -1},
	{0, 1, 3, 4, -1},
	{0, 2, 3, 4, -1},
	{1, 2, 3, 4, -1},
	{0, 1, 2, 3, 4, -1}
};
const double c3ga_basisElementSignByIndex[32] =
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const double c3ga_basisElementSignByBitmap[32] =
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const int c3ga_basisElementIndexByBitmap[32] =
	{0, 1, 2, 6, 3, 7, 8, 16, 4, 9, 10, 17, 11, 18, 19, 26, 5, 12, 13, 20, 14, 21, 22, 27, 15, 23, 24, 28, 25, 29, 30, 31};
const int c3ga_basisElementBitmapByIndex[32] =
	{0, 1, 2, 4, 8, 16, 3, 5, 6, 9, 10, 12, 17, 18, 20, 24, 7, 11, 13, 14, 19, 21, 22, 25, 26, 28, 15, 23, 27, 29, 30, 31};
const int c3ga_basisElementGradeByBitmap[32] =
	{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5};
const int c3ga_basisElementGroupByBitmap[32] =
	{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5};

const char *g_c3gaTypenames[] = 
{
	"mv",
	"double",
	"no_t",
	"e1_t",
	"e2_t",
	"e3_t",
	"ni_t",
	"noni_t",
	"I3_t",
	"I5_t",
	"I5i_t",
	"vectorE3GA",
	"bivectorE3GA",
	"rotorE3GA",
	"normalizedPoint",
	"dualSphere",
	"dualPlane",
	"freeVector",
	"freeBivector",
	"flatPoint",
	"pointPair",
	"line",
	"circle",
	"sphere",
	"plane",
	"pseudoscalar",
	"normalizedTranslator",
	"translator",
	"RBM",
	"evenVersor",
	"oddVersor"
};
no_t no;
e1_t e1;
e2_t e2;
e3_t e3;
ni_t ni;
noni_t noni;
I3_t I3;
I5_t I5;
I5i_t I5i;
vectorE3GA vectorE1(vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0);
vectorE3GA vectorE2(vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0);
vectorE3GA vectorE3(vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0);
normalizedPoint pointAtOrigin(normalizedPoint::coord_e1_e2_e3_ni, 0.0, 0.0, 0.0, 0.0);
dualSphere pointAtInfinity(dualSphere::coord_no_e1_e2_e3_ni, 0.0, 0.0, 0.0, 0.0, 1.0);


void ReportUsage::printReport(FILE *F /*= stdout*/, bool includeCount /* = true */) {
	fprintf(F, "Report usage is disabled");
}

/* 
These strings determine how the output of string() is formatted.
You can alter them at runtime using c3ga_setStringFormat().
*/
 
const char *c3ga_string_fp = "%2.2f"; 
const char *c3ga_string_start = ""; 
const char *c3ga_string_end = ""; 
const char *c3ga_string_mul = "*"; 
const char *c3ga_string_wedge = "^"; 
const char *c3ga_string_plus = " + "; 
const char *c3ga_string_minus = " - "; 

void c3ga_setStringFormat(const char *what, const char *format) {
 
	if (!strcmp(what, "fp")) 
		c3ga_string_fp = (format) ? format : "%2.2f"; 
	else if (!strcmp(what, "start")) 
		c3ga_string_start = (format) ? format : ""; 
	else if (!strcmp(what, "end")) 
		c3ga_string_end = (format) ? format : ""; 
	else if (!strcmp(what, "mul")) 
		c3ga_string_mul = (format) ? format : "*"; 
	else if (!strcmp(what, "wedge")) 
		c3ga_string_wedge = (format) ? format : "^"; 
	else if (!strcmp(what, "plus")) 
		c3ga_string_plus = (format) ? format : " + "; 
	else if (!strcmp(what, "minus")) 
		c3ga_string_minus = (format) ? format : " - ";
}



#ifdef WIN32
#define snprintf _snprintf
#pragma warning(disable:4996) /* quit your whining already */
#endif /* WIN32 */
const char *c_str(const mv &V, char *str, int maxLength, const char *fp) 
{
	int stdIdx = 0, l;
	char tmpBuf[256], tmpFloatBuf[256]; 
	int k = 0, bei, ia = 0, s = c3ga_mvSize[V.gu()], p = 0, cnt = 0;
	const double *c = V.getC();

	// set up the floating point precision
	if (fp == NULL) fp = c3ga_string_fp;

	// start the string
	l = snprintf(tmpBuf, 256, "%s", c3ga_string_start);
	if (stdIdx + l <= maxLength) {
		strcpy(str + stdIdx, tmpBuf);
		stdIdx += l;
	}
	else {
		snprintf(str, maxLength, "toString_mv: buffer too small");
		return str;
	}

	// print all coordinates
	for (int i = 0; i <= 6; i++) {
		if (V.gu() & (1 << i)) {
			for (int j = 0; j < c3ga_groupSize[i]; j++) {
				double coord = (double)c3ga_basisElementSignByIndex[ia] *c[k];
				/* goal: print [+|-]V.m_c[k][* basisVector1 ^ ... ^ basisVectorN] */			
				snprintf(tmpFloatBuf, 256, fp, (double)fabs(coord)); // cast to double to force custom float types to Plain Old Data
				if (atof(tmpFloatBuf) != 0.0) {
					l = 0;

					// print [+|-]
					l += snprintf(tmpBuf + l, 256-l, "%s", (coord >= 0.0) 
						? (cnt ? c3ga_string_plus : "")
						: c3ga_string_minus);
						
					// print obj.m_c[k]
					int dummyArg = 0; // prevents compiler warning on some platforms
					l += snprintf(tmpBuf + l, 256-l, tmpFloatBuf, dummyArg);

					if (i) { // if not grade 0, print [* basisVector1 ^ ... ^ basisVectorN]
						l += snprintf(tmpBuf + l, 256-l, "%s", c3ga_string_mul);

						// print all basis vectors
						bei = 0;
						while (c3ga_basisElements[ia][bei] >= 0) {
							l += snprintf(tmpBuf + l, 256-l, "%s%s", (bei) ? c3ga_string_wedge : "", 
							 c3ga_basisVectorNames[c3ga_basisElements[ia][bei]]);
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
		else ia += c3ga_groupSize[i];
	}

    // if no coordinates printed: 0
	l = 0;
	if (cnt == 0) {
		l += snprintf(tmpBuf + l, 256-l, "0");
	}

    // end the string
	l += snprintf(tmpBuf + l, 256-l, "%s", c3ga_string_end);
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
/// Computes the partial geometric product of two multivectors (group 0  x  group 5 -> group 5)
void gp_default_0_5_5(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 1  x  group 4 -> group 5)
void gp_default_1_4_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 5 -> group 4)
void gp_default_1_5_4(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 2  x  group 3 -> group 5)
void gp_default_2_3_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 4 -> group 2)
void gp_default_2_4_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 4 -> group 4)
void gp_default_2_4_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 5 -> group 3)
void gp_default_2_5_3(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 3  x  group 2 -> group 5)
void gp_default_3_2_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 0)
void gp_default_3_3_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 2)
void gp_default_3_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 4)
void gp_default_3_3_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 4 -> group 1)
void gp_default_3_4_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 4 -> group 3)
void gp_default_3_4_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 5 -> group 2)
void gp_default_3_5_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 0 -> group 4)
void gp_default_4_0_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 1 -> group 3)
void gp_default_4_1_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 1 -> group 5)
void gp_default_4_1_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 2 -> group 2)
void gp_default_4_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 2 -> group 4)
void gp_default_4_2_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 3 -> group 1)
void gp_default_4_3_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 3 -> group 3)
void gp_default_4_3_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 4 -> group 0)
void gp_default_4_4_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 4 -> group 2)
void gp_default_4_4_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 5 -> group 1)
void gp_default_4_5_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 0 -> group 5)
void gp_default_5_0_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 1 -> group 4)
void gp_default_5_1_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 2 -> group 3)
void gp_default_5_2_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 3 -> group 2)
void gp_default_5_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 4 -> group 1)
void gp_default_5_4_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 5 -> group 0)
void gp_default_5_5_0(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 0  x  group 5 -> group 5)
void gp_euclidean_0_5_5(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 1  x  group 4 -> group 5)
void gp_euclidean_1_4_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 1  x  group 5 -> group 4)
void gp_euclidean_1_5_4(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 2  x  group 3 -> group 5)
void gp_euclidean_2_3_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 4 -> group 2)
void gp_euclidean_2_4_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 4 -> group 4)
void gp_euclidean_2_4_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 2  x  group 5 -> group 3)
void gp_euclidean_2_5_3(const double *A, const double *B, double *C);
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
/// Computes the partial geometric product of two multivectors (group 3  x  group 2 -> group 5)
void gp_euclidean_3_2_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 0)
void gp_euclidean_3_3_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 2)
void gp_euclidean_3_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 3 -> group 4)
void gp_euclidean_3_3_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 4 -> group 1)
void gp_euclidean_3_4_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 4 -> group 3)
void gp_euclidean_3_4_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 3  x  group 5 -> group 2)
void gp_euclidean_3_5_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 0 -> group 4)
void gp_euclidean_4_0_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 1 -> group 3)
void gp_euclidean_4_1_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 1 -> group 5)
void gp_euclidean_4_1_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 2 -> group 2)
void gp_euclidean_4_2_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 2 -> group 4)
void gp_euclidean_4_2_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 3 -> group 1)
void gp_euclidean_4_3_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 3 -> group 3)
void gp_euclidean_4_3_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 4 -> group 0)
void gp_euclidean_4_4_0(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 4 -> group 2)
void gp_euclidean_4_4_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 4  x  group 5 -> group 1)
void gp_euclidean_4_5_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 0 -> group 5)
void gp_euclidean_5_0_5(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 1 -> group 4)
void gp_euclidean_5_1_4(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 2 -> group 3)
void gp_euclidean_5_2_3(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 3 -> group 2)
void gp_euclidean_5_3_2(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 4 -> group 1)
void gp_euclidean_5_4_1(const double *A, const double *B, double *C);
/// Computes the partial geometric product of two multivectors (group 5  x  group 5 -> group 0)
void gp_euclidean_5_5_0(const double *A, const double *B, double *C);
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
/// copies coordinates of group 5
void copyGroup_5(const double *A, double *C);
/// copies and multiplies (by s) coordinates of group 5
void copyMul_5(const double *A, double *C, double s);
/// copies and divides (by s) coordinates of group 5
void copyDiv_5(const double *A, double *C, double s);
/// adds coordinates of group 5 from variable A to C
void add_5(const double *A, double *C);
/// subtracts coordinates of group 5 in variable A from C
void sub_5(const double *A, double *C);
/// negate coordinates of group 5 of variable A
void neg_5(const double *A, double *C);
/// adds coordinates of group 5 of variables A and B
void add2_5_5(const double *A, const double *B, double *C);
/// subtracts coordinates of group 5 of variables A from B
void sub2_5_5(const double *A, const double *B, double *C);
/// performs coordinate-wise multiplication of coordinates of group 5 of variables A and B
void hp_5_5(const double *A, const double *B, double *C);
/// performs coordinate-wise division of coordinates of group 5 of variables A and B
/// (no checks for divide by zero are made)
void ihp_5_5(const double *A, const double *B, double *C);
/// check for equality up to eps of coordinates of group 5 of variables A and B
bool equals_5_5(const double *A, const double *B, double eps);
/// checks if coordinates of group 5 of variable A are zero up to eps
bool zeroGroup_5(const double *A, double eps);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_0_5(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_0_5(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_1_4(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_1_4(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_2_3(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_2_3(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_3_2(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_3_2(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_4_1(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_4_1(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_default_5_0(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_default_5_0(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_0_5(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_0_5(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_1_4(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_1_4(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_2_3(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_2_3(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_3_2(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_3_2(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_4_1(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_4_1(const double *A, double *C);
/// Computes the partial dual (w.r.t. full space) of a multivector.
void dual_euclidean_5_0(const double *A, double *C);
/// Computes the partial undual (w.r.t. full space) of a multivector.
void undual_euclidean_5_0(const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_1_1(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_2_2(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_3_3(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_4_4(const om &O, const double *A, double *C);
/// Computes the partial application of a general outermorphism to a general multivector
void applyGomGmv_5_5(const om &O, const double *A, double *C);
void gp_default_0_0_0(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
}
void gp_default_0_1_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
	C[4] += A[0]*B[4];
}
void gp_default_0_2_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
	C[4] += A[0]*B[4];
	C[5] += A[0]*B[5];
	C[6] += A[0]*B[6];
	C[7] += A[0]*B[7];
	C[8] += A[0]*B[8];
	C[9] += A[0]*B[9];
}
void gp_default_0_3_3(const double *A, const double *B, double *C) {
	gp_default_0_2_2(A, B, C);
}
void gp_default_0_4_4(const double *A, const double *B, double *C) {
	gp_default_0_1_1(A, B, C);
}
void gp_default_0_5_5(const double *A, const double *B, double *C) {
	gp_default_0_0_0(A, B, C);
}
void gp_default_1_0_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
	C[4] += A[4]*B[0];
}
void gp_default_1_1_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[4]+A[1]*B[1]+A[2]*B[2]+A[3]*B[3]-A[4]*B[0]);
}
void gp_default_1_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]-A[1]*B[0]);
	C[1] += (A[0]*B[2]-A[2]*B[0]);
	C[2] += (A[1]*B[2]-A[2]*B[1]);
	C[3] += (A[0]*B[3]-A[3]*B[0]);
	C[4] += (A[1]*B[3]-A[3]*B[1]);
	C[5] += (A[2]*B[3]-A[3]*B[2]);
	C[6] += (A[0]*B[4]-A[4]*B[0]);
	C[7] += (A[1]*B[4]-A[4]*B[1]);
	C[8] += (A[2]*B[4]-A[4]*B[2]);
	C[9] += (A[3]*B[4]-A[4]*B[3]);
}
void gp_default_1_2_1(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[6]-A[1]*B[0]-A[2]*B[1]-A[3]*B[3]);
	C[1] += (A[0]*B[7]-A[2]*B[2]-A[3]*B[4]-A[4]*B[0]);
	C[2] += (A[0]*B[8]+A[1]*B[2]-A[3]*B[5]-A[4]*B[1]);
	C[3] += (A[0]*B[9]+A[1]*B[4]+A[2]*B[5]-A[4]*B[3]);
	C[4] += (A[1]*B[7]+A[2]*B[8]+A[3]*B[9]-A[4]*B[6]);
}
void gp_default_1_2_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[4]-A[1]*B[3]+A[3]*B[0]);
	C[2] += (A[0]*B[5]-A[2]*B[3]+A[3]*B[1]);
	C[3] += (A[1]*B[5]-A[2]*B[4]+A[3]*B[2]);
	C[4] += (A[0]*B[7]-A[1]*B[6]+A[4]*B[0]);
	C[5] += (A[0]*B[8]-A[2]*B[6]+A[4]*B[1]);
	C[6] += (A[1]*B[8]-A[2]*B[7]+A[4]*B[2]);
	C[7] += (A[0]*B[9]-A[3]*B[6]+A[4]*B[3]);
	C[8] += (A[1]*B[9]-A[3]*B[7]+A[4]*B[4]);
	C[9] += (A[2]*B[9]-A[3]*B[8]+A[4]*B[5]);
}
void gp_default_1_3_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[4]+A[2]*B[0]+A[3]*B[1]);
	C[1] += (-A[0]*B[5]-A[1]*B[0]+A[3]*B[2]);
	C[2] += (-A[0]*B[6]+A[3]*B[3]-A[4]*B[0]);
	C[3] += (-A[0]*B[7]-A[1]*B[1]-A[2]*B[2]);
	C[4] += (-A[0]*B[8]-A[2]*B[3]-A[4]*B[1]);
	C[5] += (-A[0]*B[9]+A[1]*B[3]-A[4]*B[2]);
	C[6] += (-A[1]*B[4]-A[2]*B[5]-A[3]*B[7]);
	C[7] += (-A[2]*B[6]-A[3]*B[8]-A[4]*B[4]);
	C[8] += (A[1]*B[6]-A[3]*B[9]-A[4]*B[5]);
	C[9] += (A[1]*B[8]+A[2]*B[9]-A[4]*B[7]);
}
void gp_default_1_3_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[6]-A[1]*B[5]+A[2]*B[4]-A[4]*B[0]);
	C[2] += (A[0]*B[8]-A[1]*B[7]+A[3]*B[4]-A[4]*B[1]);
	C[3] += (A[0]*B[9]-A[2]*B[7]+A[3]*B[5]-A[4]*B[2]);
	C[4] += (A[1]*B[9]-A[2]*B[8]+A[3]*B[6]-A[4]*B[3]);
}
void gp_default_1_4_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[2]+A[2]*B[0]);
	C[2] += (A[0]*B[3]-A[1]*B[0]);
	C[3] += (A[0]*B[4]-A[4]*B[0]);
	C[4] += (A[2]*B[1]+A[3]*B[2]);
	C[5] += (-A[1]*B[1]+A[3]*B[3]);
	C[6] += (A[3]*B[4]-A[4]*B[1]);
	C[7] += (-A[1]*B[2]-A[2]*B[3]);
	C[8] += (-A[2]*B[4]-A[4]*B[2]);
	C[9] += (A[1]*B[4]-A[4]*B[3]);
}
void gp_default_1_4_5(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[4]-A[1]*B[3]+A[2]*B[2]-A[3]*B[1]+A[4]*B[0]);
}
void gp_default_1_5_4(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[0];
	C[1] += -A[3]*B[0];
	C[2] += A[2]*B[0];
	C[3] += -A[1]*B[0];
	C[4] += -A[4]*B[0];
}
void gp_default_2_0_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
	C[4] += A[4]*B[0];
	C[5] += A[5]*B[0];
	C[6] += A[6]*B[0];
	C[7] += A[7]*B[0];
	C[8] += A[8]*B[0];
	C[9] += A[9]*B[0];
}
void gp_default_2_1_1(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]+A[1]*B[2]+A[3]*B[3]-A[6]*B[0]);
	C[1] += (A[0]*B[4]+A[2]*B[2]+A[4]*B[3]-A[7]*B[0]);
	C[2] += (A[1]*B[4]-A[2]*B[1]+A[5]*B[3]-A[8]*B[0]);
	C[3] += (A[3]*B[4]-A[4]*B[1]-A[5]*B[2]-A[9]*B[0]);
	C[4] += (A[6]*B[4]-A[7]*B[1]-A[8]*B[2]-A[9]*B[3]);
}
void gp_default_2_1_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[3]-A[3]*B[1]+A[4]*B[0]);
	C[2] += (A[1]*B[3]-A[3]*B[2]+A[5]*B[0]);
	C[3] += (A[2]*B[3]-A[4]*B[2]+A[5]*B[1]);
	C[4] += (A[0]*B[4]-A[6]*B[1]+A[7]*B[0]);
	C[5] += (A[1]*B[4]-A[6]*B[2]+A[8]*B[0]);
	C[6] += (A[2]*B[4]-A[7]*B[2]+A[8]*B[1]);
	C[7] += (A[3]*B[4]-A[6]*B[3]+A[9]*B[0]);
	C[8] += (A[4]*B[4]-A[7]*B[3]+A[9]*B[1]);
	C[9] += (A[5]*B[4]-A[8]*B[3]+A[9]*B[2]);
}
void gp_default_2_2_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[7]-A[1]*B[8]-A[2]*B[2]-A[3]*B[9]-A[4]*B[4]-A[5]*B[5]+A[6]*B[6]-A[7]*B[0]-A[8]*B[1]-A[9]*B[3]);
}
void gp_default_2_2_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[6]-A[1]*B[2]+A[2]*B[1]-A[3]*B[4]+A[4]*B[3]-A[6]*B[0]);
	C[1] += (A[0]*B[2]+A[1]*B[6]-A[2]*B[0]-A[3]*B[5]+A[5]*B[3]-A[6]*B[1]);
	C[2] += (-A[0]*B[8]+A[1]*B[7]-A[4]*B[5]+A[5]*B[4]-A[7]*B[1]+A[8]*B[0]);
	C[3] += (A[0]*B[4]+A[1]*B[5]+A[3]*B[6]-A[4]*B[0]-A[5]*B[1]-A[6]*B[3]);
	C[4] += (-A[0]*B[9]+A[2]*B[5]+A[3]*B[7]-A[5]*B[2]-A[7]*B[3]+A[9]*B[0]);
	C[5] += (-A[1]*B[9]-A[2]*B[4]+A[3]*B[8]+A[4]*B[2]-A[8]*B[3]+A[9]*B[1]);
	C[6] += (A[0]*B[7]+A[1]*B[8]+A[3]*B[9]-A[7]*B[0]-A[8]*B[1]-A[9]*B[3]);
	C[7] += (A[2]*B[8]+A[4]*B[9]+A[6]*B[7]-A[7]*B[6]-A[8]*B[2]-A[9]*B[4]);
	C[8] += (-A[2]*B[7]+A[5]*B[9]+A[6]*B[8]+A[7]*B[2]-A[8]*B[6]-A[9]*B[5]);
	C[9] += (-A[4]*B[7]-A[5]*B[8]+A[6]*B[9]+A[7]*B[4]+A[8]*B[5]-A[9]*B[6]);
}
void gp_default_2_2_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[5]-A[1]*B[4]+A[2]*B[3]+A[3]*B[2]-A[4]*B[1]+A[5]*B[0]);
	C[1] += (A[0]*B[8]-A[1]*B[7]+A[2]*B[6]+A[6]*B[2]-A[7]*B[1]+A[8]*B[0]);
	C[2] += (A[0]*B[9]-A[3]*B[7]+A[4]*B[6]+A[6]*B[4]-A[7]*B[3]+A[9]*B[0]);
	C[3] += (A[1]*B[9]-A[3]*B[8]+A[5]*B[6]+A[6]*B[5]-A[8]*B[3]+A[9]*B[1]);
	C[4] += (A[2]*B[9]-A[4]*B[8]+A[5]*B[7]+A[7]*B[5]-A[8]*B[4]+A[9]*B[2]);
}
void gp_default_2_3_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[4]-A[1]*B[5]-A[2]*B[0]-A[3]*B[7]-A[4]*B[1]-A[5]*B[2]);
	C[1] += (-A[1]*B[6]-A[3]*B[8]-A[5]*B[3]-A[6]*B[4]+A[8]*B[0]+A[9]*B[1]);
	C[2] += (A[0]*B[6]-A[3]*B[9]+A[4]*B[3]-A[6]*B[5]-A[7]*B[0]+A[9]*B[2]);
	C[3] += (A[0]*B[8]+A[1]*B[9]-A[2]*B[3]-A[6]*B[7]-A[7]*B[1]-A[8]*B[2]);
	C[4] += (-A[2]*B[6]-A[4]*B[8]-A[5]*B[9]-A[7]*B[4]-A[8]*B[5]-A[9]*B[7]);
}
void gp_default_2_3_3(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[5]+A[1]*B[4]+A[3]*B[3]-A[4]*B[2]+A[5]*B[1]-A[6]*B[0]);
	C[1] += (-A[0]*B[7]-A[1]*B[3]+A[2]*B[2]+A[3]*B[4]-A[5]*B[0]-A[6]*B[1]);
	C[2] += (A[0]*B[3]-A[1]*B[7]-A[2]*B[1]+A[3]*B[5]+A[4]*B[0]-A[6]*B[2]);
	C[3] += (A[0]*B[9]-A[1]*B[8]+A[3]*B[6]-A[7]*B[2]+A[8]*B[1]-A[9]*B[0]);
	C[4] += (-A[1]*B[6]+A[2]*B[5]-A[3]*B[8]+A[4]*B[7]-A[8]*B[0]-A[9]*B[1]);
	C[5] += (A[0]*B[6]-A[2]*B[4]-A[3]*B[9]+A[5]*B[7]+A[7]*B[0]-A[9]*B[2]);
	C[6] += (-A[4]*B[9]+A[5]*B[8]+A[6]*B[6]-A[7]*B[5]+A[8]*B[4]-A[9]*B[3]);
	C[7] += (A[0]*B[8]+A[1]*B[9]-A[4]*B[4]-A[5]*B[5]+A[7]*B[1]+A[8]*B[2]);
	C[8] += (A[2]*B[9]-A[5]*B[6]+A[6]*B[8]-A[7]*B[7]+A[8]*B[3]+A[9]*B[4]);
	C[9] += (-A[2]*B[8]+A[4]*B[6]+A[6]*B[9]-A[7]*B[3]-A[8]*B[7]+A[9]*B[5]);
}
void gp_default_2_3_5(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[9]-A[1]*B[8]+A[2]*B[7]+A[3]*B[6]-A[4]*B[5]+A[5]*B[4]-A[6]*B[3]+A[7]*B[2]-A[8]*B[1]+A[9]*B[0]);
}
void gp_default_2_4_2(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[1]-A[3]*B[2]-A[5]*B[0]);
	C[1] += (A[0]*B[1]-A[3]*B[3]+A[4]*B[0]);
	C[2] += (-A[3]*B[4]+A[6]*B[1]-A[9]*B[0]);
	C[3] += (A[0]*B[2]+A[1]*B[3]-A[2]*B[0]);
	C[4] += (A[1]*B[4]+A[6]*B[2]+A[8]*B[0]);
	C[5] += (-A[0]*B[4]+A[6]*B[3]-A[7]*B[0]);
	C[6] += (-A[2]*B[1]-A[4]*B[2]-A[5]*B[3]);
	C[7] += (-A[5]*B[4]+A[8]*B[1]+A[9]*B[2]);
	C[8] += (A[4]*B[4]-A[7]*B[1]+A[9]*B[3]);
	C[9] += (-A[2]*B[4]-A[7]*B[2]-A[8]*B[3]);
}
void gp_default_2_4_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[3]*B[1]-A[6]*B[0]);
	C[1] += (A[3]*B[4]-A[4]*B[3]+A[5]*B[2]-A[9]*B[0]);
	C[2] += (-A[1]*B[4]+A[2]*B[3]-A[5]*B[1]+A[8]*B[0]);
	C[3] += (A[0]*B[4]-A[2]*B[2]+A[4]*B[1]-A[7]*B[0]);
	C[4] += (A[6]*B[4]-A[7]*B[3]+A[8]*B[2]-A[9]*B[1]);
}
void gp_default_2_5_3(const double *A, const double *B, double *C) {
	C[0] += -A[3]*B[0];
	C[1] += A[1]*B[0];
	C[2] += -A[0]*B[0];
	C[3] += -A[6]*B[0];
	C[4] += -A[5]*B[0];
	C[5] += A[4]*B[0];
	C[6] += -A[9]*B[0];
	C[7] += -A[2]*B[0];
	C[8] += A[8]*B[0];
	C[9] += -A[7]*B[0];
}
void gp_default_3_0_3(const double *A, const double *B, double *C) {
	gp_default_2_0_2(A, B, C);
}
void gp_default_3_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]+A[1]*B[3]-A[4]*B[0]);
	C[1] += (-A[0]*B[1]+A[2]*B[3]-A[5]*B[0]);
	C[2] += (-A[0]*B[4]+A[3]*B[3]-A[6]*B[0]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]-A[7]*B[0]);
	C[4] += (-A[1]*B[4]-A[3]*B[2]-A[8]*B[0]);
	C[5] += (-A[2]*B[4]+A[3]*B[1]-A[9]*B[0]);
	C[6] += (-A[4]*B[1]-A[5]*B[2]-A[7]*B[3]);
	C[7] += (-A[4]*B[4]-A[6]*B[2]-A[8]*B[3]);
	C[8] += (-A[5]*B[4]+A[6]*B[1]-A[9]*B[3]);
	C[9] += (-A[7]*B[4]+A[8]*B[1]+A[9]*B[2]);
}
void gp_default_3_1_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[4]-A[4]*B[2]+A[5]*B[1]-A[6]*B[0]);
	C[2] += (A[1]*B[4]-A[4]*B[3]+A[7]*B[1]-A[8]*B[0]);
	C[3] += (A[2]*B[4]-A[5]*B[3]+A[7]*B[2]-A[9]*B[0]);
	C[4] += (A[3]*B[4]-A[6]*B[3]+A[8]*B[2]-A[9]*B[1]);
}
void gp_default_3_2_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[2]-A[1]*B[4]-A[2]*B[5]-A[4]*B[0]-A[5]*B[1]-A[7]*B[3]);
	C[1] += (A[0]*B[8]+A[1]*B[9]-A[3]*B[5]-A[4]*B[6]-A[6]*B[1]-A[8]*B[3]);
	C[2] += (-A[0]*B[7]+A[2]*B[9]+A[3]*B[4]-A[5]*B[6]+A[6]*B[0]-A[9]*B[3]);
	C[3] += (-A[1]*B[7]-A[2]*B[8]-A[3]*B[2]-A[7]*B[6]+A[8]*B[0]+A[9]*B[1]);
	C[4] += (-A[4]*B[7]-A[5]*B[8]-A[6]*B[2]-A[7]*B[9]-A[8]*B[4]-A[9]*B[5]);
}
void gp_default_3_2_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[6]-A[1]*B[5]+A[2]*B[4]-A[3]*B[3]-A[4]*B[1]+A[5]*B[0]);
	C[1] += (A[0]*B[5]+A[1]*B[6]-A[2]*B[2]+A[3]*B[1]-A[4]*B[3]+A[7]*B[0]);
	C[2] += (-A[0]*B[4]+A[1]*B[2]+A[2]*B[6]-A[3]*B[0]-A[5]*B[3]+A[7]*B[1]);
	C[3] += (A[0]*B[9]-A[1]*B[8]+A[2]*B[7]-A[6]*B[3]+A[8]*B[1]-A[9]*B[0]);
	C[4] += (A[0]*B[8]+A[1]*B[9]-A[5]*B[2]+A[6]*B[1]-A[7]*B[4]+A[8]*B[3]);
	C[5] += (-A[0]*B[7]+A[2]*B[9]+A[4]*B[2]-A[6]*B[0]-A[7]*B[5]+A[9]*B[3]);
	C[6] += (A[3]*B[9]-A[4]*B[8]+A[5]*B[7]-A[6]*B[6]-A[8]*B[5]+A[9]*B[4]);
	C[7] += (-A[1]*B[7]-A[2]*B[8]+A[4]*B[4]+A[5]*B[5]-A[8]*B[0]-A[9]*B[1]);
	C[8] += (-A[3]*B[8]-A[4]*B[9]+A[6]*B[5]+A[7]*B[7]-A[8]*B[6]-A[9]*B[2]);
	C[9] += (A[3]*B[7]-A[5]*B[9]-A[6]*B[4]+A[7]*B[8]+A[8]*B[2]-A[9]*B[6]);
}
void gp_default_3_2_5(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[9]-A[1]*B[8]+A[2]*B[7]-A[3]*B[6]+A[4]*B[5]-A[5]*B[4]+A[6]*B[3]+A[7]*B[2]-A[8]*B[1]+A[9]*B[0]);
}
void gp_default_3_3_0(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[6]+A[1]*B[8]+A[2]*B[9]-A[3]*B[3]+A[4]*B[4]+A[5]*B[5]+A[6]*B[0]+A[7]*B[7]+A[8]*B[1]+A[9]*B[2]);
}
void gp_default_3_3_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[5]-A[1]*B[7]-A[2]*B[3]+A[3]*B[2]+A[5]*B[0]+A[7]*B[1]);
	C[1] += (A[0]*B[4]+A[1]*B[3]-A[2]*B[7]-A[3]*B[1]-A[4]*B[0]+A[7]*B[2]);
	C[2] += (A[1]*B[9]-A[2]*B[8]+A[4]*B[5]-A[5]*B[4]+A[8]*B[2]-A[9]*B[1]);
	C[3] += (-A[0]*B[3]+A[1]*B[4]+A[2]*B[5]+A[3]*B[0]-A[4]*B[1]-A[5]*B[2]);
	C[4] += (-A[0]*B[9]+A[2]*B[6]+A[4]*B[7]-A[6]*B[2]-A[7]*B[4]+A[9]*B[0]);
	C[5] += (A[0]*B[8]-A[1]*B[6]+A[5]*B[7]+A[6]*B[1]-A[7]*B[5]-A[8]*B[0]);
	C[6] += (-A[0]*B[6]-A[1]*B[8]-A[2]*B[9]+A[6]*B[0]+A[8]*B[1]+A[9]*B[2]);
	C[7] += (-A[3]*B[9]+A[5]*B[6]-A[6]*B[5]+A[7]*B[8]-A[8]*B[7]+A[9]*B[3]);
	C[8] += (A[3]*B[8]-A[4]*B[6]+A[6]*B[4]+A[7]*B[9]-A[8]*B[3]-A[9]*B[7]);
	C[9] += (-A[3]*B[6]-A[4]*B[8]-A[5]*B[9]+A[6]*B[3]+A[8]*B[4]+A[9]*B[5]);
}
void gp_default_3_3_4(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[7]+A[1]*B[5]-A[2]*B[4]-A[4]*B[2]+A[5]*B[1]-A[7]*B[0]);
	C[1] += (-A[1]*B[9]+A[2]*B[8]-A[3]*B[7]-A[7]*B[3]+A[8]*B[2]-A[9]*B[1]);
	C[2] += (A[0]*B[9]-A[2]*B[6]+A[3]*B[5]+A[5]*B[3]-A[6]*B[2]+A[9]*B[0]);
	C[3] += (-A[0]*B[8]+A[1]*B[6]-A[3]*B[4]-A[4]*B[3]+A[6]*B[1]-A[8]*B[0]);
	C[4] += (-A[4]*B[9]+A[5]*B[8]-A[6]*B[7]-A[7]*B[6]+A[8]*B[5]-A[9]*B[4]);
}
void gp_default_3_4_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[1]-A[1]*B[2]-A[2]*B[3]+A[3]*B[0]);
	C[1] += (-A[2]*B[4]+A[5]*B[1]+A[7]*B[2]+A[9]*B[0]);
	C[2] += (A[1]*B[4]-A[4]*B[1]+A[7]*B[3]-A[8]*B[0]);
	C[3] += (-A[0]*B[4]-A[4]*B[2]-A[5]*B[3]+A[6]*B[0]);
	C[4] += (-A[3]*B[4]+A[6]*B[1]+A[8]*B[2]+A[9]*B[3]);
}
void gp_default_3_4_3(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[3]+A[2]*B[2]-A[7]*B[0]);
	C[1] += (A[0]*B[3]-A[2]*B[1]+A[5]*B[0]);
	C[2] += (-A[0]*B[2]+A[1]*B[1]-A[4]*B[0]);
	C[3] += (-A[4]*B[3]+A[5]*B[2]-A[7]*B[1]);
	C[4] += (-A[2]*B[4]+A[3]*B[3]-A[9]*B[0]);
	C[5] += (A[1]*B[4]-A[3]*B[2]+A[8]*B[0]);
	C[6] += (-A[7]*B[4]+A[8]*B[3]-A[9]*B[2]);
	C[7] += (-A[0]*B[4]+A[3]*B[1]-A[6]*B[0]);
	C[8] += (A[5]*B[4]-A[6]*B[3]+A[9]*B[1]);
	C[9] += (-A[4]*B[4]+A[6]*B[2]-A[8]*B[1]);
}
void gp_default_3_5_2(const double *A, const double *B, double *C) {
	C[0] += A[2]*B[0];
	C[1] += -A[1]*B[0];
	C[2] += A[7]*B[0];
	C[3] += A[0]*B[0];
	C[4] += -A[5]*B[0];
	C[5] += A[4]*B[0];
	C[6] += A[3]*B[0];
	C[7] += A[9]*B[0];
	C[8] += -A[8]*B[0];
	C[9] += A[6]*B[0];
}
void gp_default_4_0_4(const double *A, const double *B, double *C) {
	gp_default_1_0_1(A, B, C);
}
void gp_default_4_1_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[0]);
	C[1] += (-A[0]*B[2]-A[2]*B[0]);
	C[2] += (A[0]*B[1]-A[3]*B[0]);
	C[3] += (A[0]*B[4]-A[4]*B[0]);
	C[4] += (-A[1]*B[2]-A[2]*B[3]);
	C[5] += (A[1]*B[1]-A[3]*B[3]);
	C[6] += (A[1]*B[4]-A[4]*B[3]);
	C[7] += (A[2]*B[1]+A[3]*B[2]);
	C[8] += (A[2]*B[4]+A[4]*B[2]);
	C[9] += (A[3]*B[4]-A[4]*B[1]);
}
void gp_default_4_1_5(const double *A, const double *B, double *C) {
	gp_default_1_4_5(A, B, C);
}
void gp_default_4_2_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[5]-A[1]*B[1]-A[2]*B[3]);
	C[1] += (A[0]*B[4]+A[1]*B[0]-A[3]*B[3]);
	C[2] += (-A[0]*B[9]+A[1]*B[6]-A[4]*B[3]);
	C[3] += (-A[0]*B[2]+A[2]*B[0]+A[3]*B[1]);
	C[4] += (A[0]*B[8]+A[2]*B[6]+A[4]*B[1]);
	C[5] += (-A[0]*B[7]+A[3]*B[6]-A[4]*B[0]);
	C[6] += (-A[1]*B[2]-A[2]*B[4]-A[3]*B[5]);
	C[7] += (A[1]*B[8]+A[2]*B[9]-A[4]*B[5]);
	C[8] += (-A[1]*B[7]+A[3]*B[9]+A[4]*B[4]);
	C[9] += (-A[2]*B[7]-A[3]*B[8]-A[4]*B[2]);
}
void gp_default_4_2_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[6]-A[1]*B[3]+A[2]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[9]-A[2]*B[5]+A[3]*B[4]-A[4]*B[3]);
	C[2] += (-A[0]*B[8]+A[1]*B[5]-A[3]*B[2]+A[4]*B[1]);
	C[3] += (A[0]*B[7]-A[1]*B[4]+A[2]*B[2]-A[4]*B[0]);
	C[4] += (A[1]*B[9]-A[2]*B[8]+A[3]*B[7]-A[4]*B[6]);
}
void gp_default_4_3_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[3]+A[1]*B[0]+A[2]*B[1]+A[3]*B[2]);
	C[1] += (-A[0]*B[9]-A[1]*B[5]-A[2]*B[7]+A[4]*B[2]);
	C[2] += (A[0]*B[8]+A[1]*B[4]-A[3]*B[7]-A[4]*B[1]);
	C[3] += (-A[0]*B[6]+A[2]*B[4]+A[3]*B[5]+A[4]*B[0]);
	C[4] += (-A[1]*B[6]-A[2]*B[8]-A[3]*B[9]+A[4]*B[3]);
}
void gp_default_4_3_3(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[7]+A[2]*B[2]-A[3]*B[1]);
	C[1] += (A[0]*B[5]-A[1]*B[2]+A[3]*B[0]);
	C[2] += (-A[0]*B[4]+A[1]*B[1]-A[2]*B[0]);
	C[3] += (-A[1]*B[7]+A[2]*B[5]-A[3]*B[4]);
	C[4] += (-A[0]*B[9]+A[3]*B[3]-A[4]*B[2]);
	C[5] += (A[0]*B[8]-A[2]*B[3]+A[4]*B[1]);
	C[6] += (-A[2]*B[9]+A[3]*B[8]-A[4]*B[7]);
	C[7] += (-A[0]*B[6]+A[1]*B[3]-A[4]*B[0]);
	C[8] += (A[1]*B[9]-A[3]*B[6]+A[4]*B[5]);
	C[9] += (-A[1]*B[8]+A[2]*B[6]-A[4]*B[4]);
}
void gp_default_4_4_0(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[4]-A[1]*B[1]-A[2]*B[2]-A[3]*B[3]+A[4]*B[0]);
}
void gp_default_4_4_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[3]+A[3]*B[0]);
	C[1] += (A[0]*B[2]-A[2]*B[0]);
	C[2] += (-A[2]*B[3]+A[3]*B[2]);
	C[3] += (-A[0]*B[1]+A[1]*B[0]);
	C[4] += (A[1]*B[3]-A[3]*B[1]);
	C[5] += (-A[1]*B[2]+A[2]*B[1]);
	C[6] += (-A[0]*B[4]+A[4]*B[0]);
	C[7] += (-A[3]*B[4]+A[4]*B[3]);
	C[8] += (A[2]*B[4]-A[4]*B[2]);
	C[9] += (-A[1]*B[4]+A[4]*B[1]);
}
void gp_default_4_5_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[3]*B[0];
	C[2] += -A[2]*B[0];
	C[3] += A[1]*B[0];
	C[4] += A[4]*B[0];
}
void gp_default_5_0_5(const double *A, const double *B, double *C) {
	gp_default_0_0_0(A, B, C);
}
void gp_default_5_1_4(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[0];
	C[1] += -A[0]*B[3];
	C[2] += A[0]*B[2];
	C[3] += -A[0]*B[1];
	C[4] += -A[0]*B[4];
}
void gp_default_5_2_3(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[3];
	C[1] += A[0]*B[1];
	C[2] += -A[0]*B[0];
	C[3] += -A[0]*B[6];
	C[4] += -A[0]*B[5];
	C[5] += A[0]*B[4];
	C[6] += -A[0]*B[9];
	C[7] += -A[0]*B[2];
	C[8] += A[0]*B[8];
	C[9] += -A[0]*B[7];
}
void gp_default_5_3_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[2];
	C[1] += -A[0]*B[1];
	C[2] += A[0]*B[7];
	C[3] += A[0]*B[0];
	C[4] += -A[0]*B[5];
	C[5] += A[0]*B[4];
	C[6] += A[0]*B[3];
	C[7] += A[0]*B[9];
	C[8] += -A[0]*B[8];
	C[9] += A[0]*B[6];
}
void gp_default_5_4_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[3];
	C[2] += -A[0]*B[2];
	C[3] += A[0]*B[1];
	C[4] += A[0]*B[4];
}
void gp_default_5_5_0(const double *A, const double *B, double *C) {
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
	C[4] += A[0]*B[4];
}
void gp_euclidean_0_2_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[0]*B[1];
	C[2] += A[0]*B[2];
	C[3] += A[0]*B[3];
	C[4] += A[0]*B[4];
	C[5] += A[0]*B[5];
	C[6] += A[0]*B[6];
	C[7] += A[0]*B[7];
	C[8] += A[0]*B[8];
	C[9] += A[0]*B[9];
}
void gp_euclidean_0_3_3(const double *A, const double *B, double *C) {
	gp_euclidean_0_2_2(A, B, C);
}
void gp_euclidean_0_4_4(const double *A, const double *B, double *C) {
	gp_euclidean_0_1_1(A, B, C);
}
void gp_euclidean_0_5_5(const double *A, const double *B, double *C) {
	gp_euclidean_0_0_0(A, B, C);
}
void gp_euclidean_1_0_1(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
	C[4] += A[4]*B[0];
}
void gp_euclidean_1_1_0(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[0]+A[1]*B[1]+A[2]*B[2]+A[3]*B[3]+A[4]*B[4]);
}
void gp_euclidean_1_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]-A[1]*B[0]);
	C[1] += (A[0]*B[2]-A[2]*B[0]);
	C[2] += (A[1]*B[2]-A[2]*B[1]);
	C[3] += (A[0]*B[3]-A[3]*B[0]);
	C[4] += (A[1]*B[3]-A[3]*B[1]);
	C[5] += (A[2]*B[3]-A[3]*B[2]);
	C[6] += (A[0]*B[4]-A[4]*B[0]);
	C[7] += (A[1]*B[4]-A[4]*B[1]);
	C[8] += (A[2]*B[4]-A[4]*B[2]);
	C[9] += (A[3]*B[4]-A[4]*B[3]);
}
void gp_euclidean_1_2_1(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[0]-A[2]*B[1]-A[3]*B[3]-A[4]*B[6]);
	C[1] += (A[0]*B[0]-A[2]*B[2]-A[3]*B[4]-A[4]*B[7]);
	C[2] += (A[0]*B[1]+A[1]*B[2]-A[3]*B[5]-A[4]*B[8]);
	C[3] += (A[0]*B[3]+A[1]*B[4]+A[2]*B[5]-A[4]*B[9]);
	C[4] += (A[0]*B[6]+A[1]*B[7]+A[2]*B[8]+A[3]*B[9]);
}
void gp_euclidean_1_2_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[4]-A[1]*B[3]+A[3]*B[0]);
	C[2] += (A[0]*B[5]-A[2]*B[3]+A[3]*B[1]);
	C[3] += (A[1]*B[5]-A[2]*B[4]+A[3]*B[2]);
	C[4] += (A[0]*B[7]-A[1]*B[6]+A[4]*B[0]);
	C[5] += (A[0]*B[8]-A[2]*B[6]+A[4]*B[1]);
	C[6] += (A[1]*B[8]-A[2]*B[7]+A[4]*B[2]);
	C[7] += (A[0]*B[9]-A[3]*B[6]+A[4]*B[3]);
	C[8] += (A[1]*B[9]-A[3]*B[7]+A[4]*B[4]);
	C[9] += (A[2]*B[9]-A[3]*B[8]+A[4]*B[5]);
}
void gp_euclidean_1_3_2(const double *A, const double *B, double *C) {
	C[0] += (A[2]*B[0]+A[3]*B[1]+A[4]*B[4]);
	C[1] += (-A[1]*B[0]+A[3]*B[2]+A[4]*B[5]);
	C[2] += (A[0]*B[0]+A[3]*B[3]+A[4]*B[6]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]+A[4]*B[7]);
	C[4] += (A[0]*B[1]-A[2]*B[3]+A[4]*B[8]);
	C[5] += (A[0]*B[2]+A[1]*B[3]+A[4]*B[9]);
	C[6] += (-A[1]*B[4]-A[2]*B[5]-A[3]*B[7]);
	C[7] += (A[0]*B[4]-A[2]*B[6]-A[3]*B[8]);
	C[8] += (A[0]*B[5]+A[1]*B[6]-A[3]*B[9]);
	C[9] += (A[0]*B[7]+A[1]*B[8]+A[2]*B[9]);
}
void gp_euclidean_1_3_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[6]-A[1]*B[5]+A[2]*B[4]-A[4]*B[0]);
	C[2] += (A[0]*B[8]-A[1]*B[7]+A[3]*B[4]-A[4]*B[1]);
	C[3] += (A[0]*B[9]-A[2]*B[7]+A[3]*B[5]-A[4]*B[2]);
	C[4] += (A[1]*B[9]-A[2]*B[8]+A[3]*B[6]-A[4]*B[3]);
}
void gp_euclidean_1_4_3(const double *A, const double *B, double *C) {
	C[0] += (-A[3]*B[0]-A[4]*B[1]);
	C[1] += (A[2]*B[0]-A[4]*B[2]);
	C[2] += (-A[1]*B[0]-A[4]*B[3]);
	C[3] += (A[0]*B[0]-A[4]*B[4]);
	C[4] += (A[2]*B[1]+A[3]*B[2]);
	C[5] += (-A[1]*B[1]+A[3]*B[3]);
	C[6] += (A[0]*B[1]+A[3]*B[4]);
	C[7] += (-A[1]*B[2]-A[2]*B[3]);
	C[8] += (A[0]*B[2]-A[2]*B[4]);
	C[9] += (A[0]*B[3]+A[1]*B[4]);
}
void gp_euclidean_1_4_5(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[4]-A[1]*B[3]+A[2]*B[2]-A[3]*B[1]+A[4]*B[0]);
}
void gp_euclidean_1_5_4(const double *A, const double *B, double *C) {
	C[0] += A[4]*B[0];
	C[1] += -A[3]*B[0];
	C[2] += A[2]*B[0];
	C[3] += -A[1]*B[0];
	C[4] += A[0]*B[0];
}
void gp_euclidean_2_0_2(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[0];
	C[1] += A[1]*B[0];
	C[2] += A[2]*B[0];
	C[3] += A[3]*B[0];
	C[4] += A[4]*B[0];
	C[5] += A[5]*B[0];
	C[6] += A[6]*B[0];
	C[7] += A[7]*B[0];
	C[8] += A[8]*B[0];
	C[9] += A[9]*B[0];
}
void gp_euclidean_2_1_1(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[1]+A[1]*B[2]+A[3]*B[3]+A[6]*B[4]);
	C[1] += (-A[0]*B[0]+A[2]*B[2]+A[4]*B[3]+A[7]*B[4]);
	C[2] += (-A[1]*B[0]-A[2]*B[1]+A[5]*B[3]+A[8]*B[4]);
	C[3] += (-A[3]*B[0]-A[4]*B[1]-A[5]*B[2]+A[9]*B[4]);
	C[4] += (-A[6]*B[0]-A[7]*B[1]-A[8]*B[2]-A[9]*B[3]);
}
void gp_euclidean_2_1_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]-A[1]*B[1]+A[2]*B[0]);
	C[1] += (A[0]*B[3]-A[3]*B[1]+A[4]*B[0]);
	C[2] += (A[1]*B[3]-A[3]*B[2]+A[5]*B[0]);
	C[3] += (A[2]*B[3]-A[4]*B[2]+A[5]*B[1]);
	C[4] += (A[0]*B[4]-A[6]*B[1]+A[7]*B[0]);
	C[5] += (A[1]*B[4]-A[6]*B[2]+A[8]*B[0]);
	C[6] += (A[2]*B[4]-A[7]*B[2]+A[8]*B[1]);
	C[7] += (A[3]*B[4]-A[6]*B[3]+A[9]*B[0]);
	C[8] += (A[4]*B[4]-A[7]*B[3]+A[9]*B[1]);
	C[9] += (A[5]*B[4]-A[8]*B[3]+A[9]*B[2]);
}
void gp_euclidean_2_2_0(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[0]-A[1]*B[1]-A[2]*B[2]-A[3]*B[3]-A[4]*B[4]-A[5]*B[5]-A[6]*B[6]-A[7]*B[7]-A[8]*B[8]-A[9]*B[9]);
}
void gp_euclidean_2_2_2(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[2]+A[2]*B[1]-A[3]*B[4]+A[4]*B[3]-A[6]*B[7]+A[7]*B[6]);
	C[1] += (A[0]*B[2]-A[2]*B[0]-A[3]*B[5]+A[5]*B[3]-A[6]*B[8]+A[8]*B[6]);
	C[2] += (-A[0]*B[1]+A[1]*B[0]-A[4]*B[5]+A[5]*B[4]-A[7]*B[8]+A[8]*B[7]);
	C[3] += (A[0]*B[4]+A[1]*B[5]-A[4]*B[0]-A[5]*B[1]-A[6]*B[9]+A[9]*B[6]);
	C[4] += (-A[0]*B[3]+A[2]*B[5]+A[3]*B[0]-A[5]*B[2]-A[7]*B[9]+A[9]*B[7]);
	C[5] += (-A[1]*B[3]-A[2]*B[4]+A[3]*B[1]+A[4]*B[2]-A[8]*B[9]+A[9]*B[8]);
	C[6] += (A[0]*B[7]+A[1]*B[8]+A[3]*B[9]-A[7]*B[0]-A[8]*B[1]-A[9]*B[3]);
	C[7] += (-A[0]*B[6]+A[2]*B[8]+A[4]*B[9]+A[6]*B[0]-A[8]*B[2]-A[9]*B[4]);
	C[8] += (-A[1]*B[6]-A[2]*B[7]+A[5]*B[9]+A[6]*B[1]+A[7]*B[2]-A[9]*B[5]);
	C[9] += (-A[3]*B[6]-A[4]*B[7]-A[5]*B[8]+A[6]*B[3]+A[7]*B[4]+A[8]*B[5]);
}
void gp_euclidean_2_2_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[5]-A[1]*B[4]+A[2]*B[3]+A[3]*B[2]-A[4]*B[1]+A[5]*B[0]);
	C[1] += (A[0]*B[8]-A[1]*B[7]+A[2]*B[6]+A[6]*B[2]-A[7]*B[1]+A[8]*B[0]);
	C[2] += (A[0]*B[9]-A[3]*B[7]+A[4]*B[6]+A[6]*B[4]-A[7]*B[3]+A[9]*B[0]);
	C[3] += (A[1]*B[9]-A[3]*B[8]+A[5]*B[6]+A[6]*B[5]-A[8]*B[3]+A[9]*B[1]);
	C[4] += (A[2]*B[9]-A[4]*B[8]+A[5]*B[7]+A[7]*B[5]-A[8]*B[4]+A[9]*B[2]);
}
void gp_euclidean_2_3_1(const double *A, const double *B, double *C) {
	C[0] += (-A[2]*B[0]-A[4]*B[1]-A[5]*B[2]-A[7]*B[4]-A[8]*B[5]-A[9]*B[7]);
	C[1] += (A[1]*B[0]+A[3]*B[1]-A[5]*B[3]+A[6]*B[4]-A[8]*B[6]-A[9]*B[8]);
	C[2] += (-A[0]*B[0]+A[3]*B[2]+A[4]*B[3]+A[6]*B[5]+A[7]*B[6]-A[9]*B[9]);
	C[3] += (-A[0]*B[1]-A[1]*B[2]-A[2]*B[3]+A[6]*B[7]+A[7]*B[8]+A[8]*B[9]);
	C[4] += (-A[0]*B[4]-A[1]*B[5]-A[2]*B[6]-A[3]*B[7]-A[4]*B[8]-A[5]*B[9]);
}
void gp_euclidean_2_3_3(const double *A, const double *B, double *C) {
	C[0] += (A[3]*B[3]-A[4]*B[2]+A[5]*B[1]+A[6]*B[6]-A[7]*B[5]+A[8]*B[4]);
	C[1] += (-A[1]*B[3]+A[2]*B[2]-A[5]*B[0]+A[6]*B[8]-A[7]*B[7]+A[9]*B[4]);
	C[2] += (A[0]*B[3]-A[2]*B[1]+A[4]*B[0]+A[6]*B[9]-A[8]*B[7]+A[9]*B[5]);
	C[3] += (-A[0]*B[2]+A[1]*B[1]-A[3]*B[0]+A[7]*B[9]-A[8]*B[8]+A[9]*B[6]);
	C[4] += (-A[1]*B[6]+A[2]*B[5]-A[3]*B[8]+A[4]*B[7]-A[8]*B[0]-A[9]*B[1]);
	C[5] += (A[0]*B[6]-A[2]*B[4]-A[3]*B[9]+A[5]*B[7]+A[7]*B[0]-A[9]*B[2]);
	C[6] += (-A[0]*B[5]+A[1]*B[4]-A[4]*B[9]+A[5]*B[8]-A[6]*B[0]-A[9]*B[3]);
	C[7] += (A[0]*B[8]+A[1]*B[9]-A[4]*B[4]-A[5]*B[5]+A[7]*B[1]+A[8]*B[2]);
	C[8] += (-A[0]*B[7]+A[2]*B[9]+A[3]*B[4]-A[5]*B[6]-A[6]*B[1]+A[8]*B[3]);
	C[9] += (-A[1]*B[7]-A[2]*B[8]+A[3]*B[5]+A[4]*B[6]-A[6]*B[2]-A[7]*B[3]);
}
void gp_euclidean_2_3_5(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[9]-A[1]*B[8]+A[2]*B[7]+A[3]*B[6]-A[4]*B[5]+A[5]*B[4]-A[6]*B[3]+A[7]*B[2]-A[8]*B[1]+A[9]*B[0]);
}
void gp_euclidean_2_4_2(const double *A, const double *B, double *C) {
	C[0] += (-A[5]*B[0]-A[8]*B[1]-A[9]*B[2]);
	C[1] += (A[4]*B[0]+A[7]*B[1]-A[9]*B[3]);
	C[2] += (-A[3]*B[0]-A[6]*B[1]-A[9]*B[4]);
	C[3] += (-A[2]*B[0]+A[7]*B[2]+A[8]*B[3]);
	C[4] += (A[1]*B[0]-A[6]*B[2]+A[8]*B[4]);
	C[5] += (-A[0]*B[0]-A[6]*B[3]-A[7]*B[4]);
	C[6] += (-A[2]*B[1]-A[4]*B[2]-A[5]*B[3]);
	C[7] += (A[1]*B[1]+A[3]*B[2]-A[5]*B[4]);
	C[8] += (-A[0]*B[1]+A[3]*B[3]+A[4]*B[4]);
	C[9] += (-A[0]*B[2]-A[1]*B[3]-A[2]*B[4]);
}
void gp_euclidean_2_4_4(const double *A, const double *B, double *C) {
	C[0] += (-A[6]*B[4]+A[7]*B[3]-A[8]*B[2]+A[9]*B[1]);
	C[1] += (A[3]*B[4]-A[4]*B[3]+A[5]*B[2]-A[9]*B[0]);
	C[2] += (-A[1]*B[4]+A[2]*B[3]-A[5]*B[1]+A[8]*B[0]);
	C[3] += (A[0]*B[4]-A[2]*B[2]+A[4]*B[1]-A[7]*B[0]);
	C[4] += (-A[0]*B[3]+A[1]*B[2]-A[3]*B[1]+A[6]*B[0]);
}
void gp_euclidean_2_5_3(const double *A, const double *B, double *C) {
	C[0] += -A[9]*B[0];
	C[1] += A[8]*B[0];
	C[2] += -A[7]*B[0];
	C[3] += A[6]*B[0];
	C[4] += -A[5]*B[0];
	C[5] += A[4]*B[0];
	C[6] += -A[3]*B[0];
	C[7] += -A[2]*B[0];
	C[8] += A[1]*B[0];
	C[9] += -A[0]*B[0];
}
void gp_euclidean_3_0_3(const double *A, const double *B, double *C) {
	gp_euclidean_2_0_2(A, B, C);
}
void gp_euclidean_3_1_2(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[2]+A[1]*B[3]+A[4]*B[4]);
	C[1] += (-A[0]*B[1]+A[2]*B[3]+A[5]*B[4]);
	C[2] += (A[0]*B[0]+A[3]*B[3]+A[6]*B[4]);
	C[3] += (-A[1]*B[1]-A[2]*B[2]+A[7]*B[4]);
	C[4] += (A[1]*B[0]-A[3]*B[2]+A[8]*B[4]);
	C[5] += (A[2]*B[0]+A[3]*B[1]+A[9]*B[4]);
	C[6] += (-A[4]*B[1]-A[5]*B[2]-A[7]*B[3]);
	C[7] += (A[4]*B[0]-A[6]*B[2]-A[8]*B[3]);
	C[8] += (A[5]*B[0]+A[6]*B[1]-A[9]*B[3]);
	C[9] += (A[7]*B[0]+A[8]*B[1]+A[9]*B[2]);
}
void gp_euclidean_3_1_4(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]-A[1]*B[2]+A[2]*B[1]-A[3]*B[0]);
	C[1] += (A[0]*B[4]-A[4]*B[2]+A[5]*B[1]-A[6]*B[0]);
	C[2] += (A[1]*B[4]-A[4]*B[3]+A[7]*B[1]-A[8]*B[0]);
	C[3] += (A[2]*B[4]-A[5]*B[3]+A[7]*B[2]-A[9]*B[0]);
	C[4] += (A[3]*B[4]-A[6]*B[3]+A[8]*B[2]-A[9]*B[1]);
}
void gp_euclidean_3_2_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[2]-A[1]*B[4]-A[2]*B[5]-A[4]*B[7]-A[5]*B[8]-A[7]*B[9]);
	C[1] += (A[0]*B[1]+A[1]*B[3]-A[3]*B[5]+A[4]*B[6]-A[6]*B[8]-A[8]*B[9]);
	C[2] += (-A[0]*B[0]+A[2]*B[3]+A[3]*B[4]+A[5]*B[6]+A[6]*B[7]-A[9]*B[9]);
	C[3] += (-A[1]*B[0]-A[2]*B[1]-A[3]*B[2]+A[7]*B[6]+A[8]*B[7]+A[9]*B[8]);
	C[4] += (-A[4]*B[0]-A[5]*B[1]-A[6]*B[2]-A[7]*B[3]-A[8]*B[4]-A[9]*B[5]);
}
void gp_euclidean_3_2_3(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[5]+A[2]*B[4]-A[3]*B[3]-A[4]*B[8]+A[5]*B[7]-A[6]*B[6]);
	C[1] += (A[0]*B[5]-A[2]*B[2]+A[3]*B[1]-A[4]*B[9]+A[7]*B[7]-A[8]*B[6]);
	C[2] += (-A[0]*B[4]+A[1]*B[2]-A[3]*B[0]-A[5]*B[9]+A[7]*B[8]-A[9]*B[6]);
	C[3] += (A[0]*B[3]-A[1]*B[1]+A[2]*B[0]-A[6]*B[9]+A[8]*B[8]-A[9]*B[7]);
	C[4] += (A[0]*B[8]+A[1]*B[9]-A[5]*B[2]+A[6]*B[1]-A[7]*B[4]+A[8]*B[3]);
	C[5] += (-A[0]*B[7]+A[2]*B[9]+A[4]*B[2]-A[6]*B[0]-A[7]*B[5]+A[9]*B[3]);
	C[6] += (A[0]*B[6]+A[3]*B[9]-A[4]*B[1]+A[5]*B[0]-A[8]*B[5]+A[9]*B[4]);
	C[7] += (-A[1]*B[7]-A[2]*B[8]+A[4]*B[4]+A[5]*B[5]-A[8]*B[0]-A[9]*B[1]);
	C[8] += (A[1]*B[6]-A[3]*B[8]-A[4]*B[3]+A[6]*B[5]+A[7]*B[0]-A[9]*B[2]);
	C[9] += (A[2]*B[6]+A[3]*B[7]-A[5]*B[3]-A[6]*B[4]+A[7]*B[1]+A[8]*B[2]);
}
void gp_euclidean_3_2_5(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[9]-A[1]*B[8]+A[2]*B[7]-A[3]*B[6]+A[4]*B[5]-A[5]*B[4]+A[6]*B[3]+A[7]*B[2]-A[8]*B[1]+A[9]*B[0]);
}
void gp_euclidean_3_3_0(const double *A, const double *B, double *C) {
	gp_euclidean_2_2_0(A, B, C);
}
void gp_euclidean_3_3_2(const double *A, const double *B, double *C) {
	C[0] += (-A[2]*B[3]+A[3]*B[2]-A[5]*B[6]+A[6]*B[5]-A[7]*B[8]+A[8]*B[7]);
	C[1] += (A[1]*B[3]-A[3]*B[1]+A[4]*B[6]-A[6]*B[4]-A[7]*B[9]+A[9]*B[7]);
	C[2] += (-A[1]*B[2]+A[2]*B[1]-A[4]*B[5]+A[5]*B[4]-A[8]*B[9]+A[9]*B[8]);
	C[3] += (-A[0]*B[3]+A[3]*B[0]+A[4]*B[8]+A[5]*B[9]-A[8]*B[4]-A[9]*B[5]);
	C[4] += (A[0]*B[2]-A[2]*B[0]-A[4]*B[7]+A[6]*B[9]+A[7]*B[4]-A[9]*B[6]);
	C[5] += (-A[0]*B[1]+A[1]*B[0]-A[5]*B[7]-A[6]*B[8]+A[7]*B[5]+A[8]*B[6]);
	C[6] += (-A[0]*B[6]-A[1]*B[8]-A[2]*B[9]+A[6]*B[0]+A[8]*B[1]+A[9]*B[2]);
	C[7] += (A[0]*B[5]+A[1]*B[7]-A[3]*B[9]-A[5]*B[0]-A[7]*B[1]+A[9]*B[3]);
	C[8] += (-A[0]*B[4]+A[2]*B[7]+A[3]*B[8]+A[4]*B[0]-A[7]*B[2]-A[8]*B[3]);
	C[9] += (-A[1]*B[4]-A[2]*B[5]-A[3]*B[6]+A[4]*B[1]+A[5]*B[2]+A[6]*B[3]);
}
void gp_euclidean_3_3_4(const double *A, const double *B, double *C) {
	C[0] += (A[4]*B[9]-A[5]*B[8]+A[6]*B[7]+A[7]*B[6]-A[8]*B[5]+A[9]*B[4]);
	C[1] += (-A[1]*B[9]+A[2]*B[8]-A[3]*B[7]-A[7]*B[3]+A[8]*B[2]-A[9]*B[1]);
	C[2] += (A[0]*B[9]-A[2]*B[6]+A[3]*B[5]+A[5]*B[3]-A[6]*B[2]+A[9]*B[0]);
	C[3] += (-A[0]*B[8]+A[1]*B[6]-A[3]*B[4]-A[4]*B[3]+A[6]*B[1]-A[8]*B[0]);
	C[4] += (A[0]*B[7]-A[1]*B[5]+A[2]*B[4]+A[4]*B[2]-A[5]*B[1]+A[7]*B[0]);
}
void gp_euclidean_3_4_1(const double *A, const double *B, double *C) {
	C[0] += (A[3]*B[0]+A[6]*B[1]+A[8]*B[2]+A[9]*B[3]);
	C[1] += (-A[2]*B[0]-A[5]*B[1]-A[7]*B[2]+A[9]*B[4]);
	C[2] += (A[1]*B[0]+A[4]*B[1]-A[7]*B[3]-A[8]*B[4]);
	C[3] += (-A[0]*B[0]+A[4]*B[2]+A[5]*B[3]+A[6]*B[4]);
	C[4] += (-A[0]*B[1]-A[1]*B[2]-A[2]*B[3]-A[3]*B[4]);
}
void gp_euclidean_3_4_3(const double *A, const double *B, double *C) {
	C[0] += (-A[7]*B[4]+A[8]*B[3]-A[9]*B[2]);
	C[1] += (A[5]*B[4]-A[6]*B[3]+A[9]*B[1]);
	C[2] += (-A[4]*B[4]+A[6]*B[2]-A[8]*B[1]);
	C[3] += (A[4]*B[3]-A[5]*B[2]+A[7]*B[1]);
	C[4] += (-A[2]*B[4]+A[3]*B[3]-A[9]*B[0]);
	C[5] += (A[1]*B[4]-A[3]*B[2]+A[8]*B[0]);
	C[6] += (-A[1]*B[3]+A[2]*B[2]-A[7]*B[0]);
	C[7] += (-A[0]*B[4]+A[3]*B[1]-A[6]*B[0]);
	C[8] += (A[0]*B[3]-A[2]*B[1]+A[5]*B[0]);
	C[9] += (-A[0]*B[2]+A[1]*B[1]-A[4]*B[0]);
}
void gp_euclidean_3_5_2(const double *A, const double *B, double *C) {
	C[0] += -A[9]*B[0];
	C[1] += A[8]*B[0];
	C[2] += -A[7]*B[0];
	C[3] += -A[6]*B[0];
	C[4] += A[5]*B[0];
	C[5] += -A[4]*B[0];
	C[6] += A[3]*B[0];
	C[7] += -A[2]*B[0];
	C[8] += A[1]*B[0];
	C[9] += -A[0]*B[0];
}
void gp_euclidean_4_0_4(const double *A, const double *B, double *C) {
	gp_euclidean_1_0_1(A, B, C);
}
void gp_euclidean_4_1_3(const double *A, const double *B, double *C) {
	C[0] += (A[0]*B[3]+A[1]*B[4]);
	C[1] += (-A[0]*B[2]+A[2]*B[4]);
	C[2] += (A[0]*B[1]+A[3]*B[4]);
	C[3] += (-A[0]*B[0]+A[4]*B[4]);
	C[4] += (-A[1]*B[2]-A[2]*B[3]);
	C[5] += (A[1]*B[1]-A[3]*B[3]);
	C[6] += (-A[1]*B[0]-A[4]*B[3]);
	C[7] += (A[2]*B[1]+A[3]*B[2]);
	C[8] += (-A[2]*B[0]+A[4]*B[2]);
	C[9] += (-A[3]*B[0]-A[4]*B[1]);
}
void gp_euclidean_4_1_5(const double *A, const double *B, double *C) {
	gp_euclidean_1_4_5(A, B, C);
}
void gp_euclidean_4_2_2(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[5]-A[1]*B[8]-A[2]*B[9]);
	C[1] += (A[0]*B[4]+A[1]*B[7]-A[3]*B[9]);
	C[2] += (-A[0]*B[3]-A[1]*B[6]-A[4]*B[9]);
	C[3] += (-A[0]*B[2]+A[2]*B[7]+A[3]*B[8]);
	C[4] += (A[0]*B[1]-A[2]*B[6]+A[4]*B[8]);
	C[5] += (-A[0]*B[0]-A[3]*B[6]-A[4]*B[7]);
	C[6] += (-A[1]*B[2]-A[2]*B[4]-A[3]*B[5]);
	C[7] += (A[1]*B[1]+A[2]*B[3]-A[4]*B[5]);
	C[8] += (-A[1]*B[0]+A[3]*B[3]+A[4]*B[4]);
	C[9] += (-A[2]*B[0]-A[3]*B[1]-A[4]*B[2]);
}
void gp_euclidean_4_2_4(const double *A, const double *B, double *C) {
	C[0] += (-A[1]*B[9]+A[2]*B[8]-A[3]*B[7]+A[4]*B[6]);
	C[1] += (A[0]*B[9]-A[2]*B[5]+A[3]*B[4]-A[4]*B[3]);
	C[2] += (-A[0]*B[8]+A[1]*B[5]-A[3]*B[2]+A[4]*B[1]);
	C[3] += (A[0]*B[7]-A[1]*B[4]+A[2]*B[2]-A[4]*B[0]);
	C[4] += (-A[0]*B[6]+A[1]*B[3]-A[2]*B[1]+A[3]*B[0]);
}
void gp_euclidean_4_3_1(const double *A, const double *B, double *C) {
	C[0] += (-A[0]*B[3]-A[1]*B[6]-A[2]*B[8]-A[3]*B[9]);
	C[1] += (A[0]*B[2]+A[1]*B[5]+A[2]*B[7]-A[4]*B[9]);
	C[2] += (-A[0]*B[1]-A[1]*B[4]+A[3]*B[7]+A[4]*B[8]);
	C[3] += (A[0]*B[0]-A[2]*B[4]-A[3]*B[5]-A[4]*B[6]);
	C[4] += (A[1]*B[0]+A[2]*B[1]+A[3]*B[2]+A[4]*B[3]);
}
void gp_euclidean_4_3_3(const double *A, const double *B, double *C) {
	C[0] += (-A[2]*B[9]+A[3]*B[8]-A[4]*B[7]);
	C[1] += (A[1]*B[9]-A[3]*B[6]+A[4]*B[5]);
	C[2] += (-A[1]*B[8]+A[2]*B[6]-A[4]*B[4]);
	C[3] += (A[1]*B[7]-A[2]*B[5]+A[3]*B[4]);
	C[4] += (-A[0]*B[9]+A[3]*B[3]-A[4]*B[2]);
	C[5] += (A[0]*B[8]-A[2]*B[3]+A[4]*B[1]);
	C[6] += (-A[0]*B[7]+A[2]*B[2]-A[3]*B[1]);
	C[7] += (-A[0]*B[6]+A[1]*B[3]-A[4]*B[0]);
	C[8] += (A[0]*B[5]-A[1]*B[2]+A[3]*B[0]);
	C[9] += (-A[0]*B[4]+A[1]*B[1]-A[2]*B[0]);
}
void gp_euclidean_4_4_0(const double *A, const double *B, double *C) {
	gp_euclidean_1_1_0(A, B, C);
}
void gp_euclidean_4_4_2(const double *A, const double *B, double *C) {
	C[0] += (A[3]*B[4]-A[4]*B[3]);
	C[1] += (-A[2]*B[4]+A[4]*B[2]);
	C[2] += (A[2]*B[3]-A[3]*B[2]);
	C[3] += (A[1]*B[4]-A[4]*B[1]);
	C[4] += (-A[1]*B[3]+A[3]*B[1]);
	C[5] += (A[1]*B[2]-A[2]*B[1]);
	C[6] += (-A[0]*B[4]+A[4]*B[0]);
	C[7] += (A[0]*B[3]-A[3]*B[0]);
	C[8] += (-A[0]*B[2]+A[2]*B[0]);
	C[9] += (A[0]*B[1]-A[1]*B[0]);
}
void gp_euclidean_4_5_1(const double *A, const double *B, double *C) {
	gp_euclidean_1_5_4(A, B, C);
}
void gp_euclidean_5_0_5(const double *A, const double *B, double *C) {
	gp_euclidean_0_0_0(A, B, C);
}
void gp_euclidean_5_1_4(const double *A, const double *B, double *C) {
	C[0] += A[0]*B[4];
	C[1] += -A[0]*B[3];
	C[2] += A[0]*B[2];
	C[3] += -A[0]*B[1];
	C[4] += A[0]*B[0];
}
void gp_euclidean_5_2_3(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[9];
	C[1] += A[0]*B[8];
	C[2] += -A[0]*B[7];
	C[3] += A[0]*B[6];
	C[4] += -A[0]*B[5];
	C[5] += A[0]*B[4];
	C[6] += -A[0]*B[3];
	C[7] += -A[0]*B[2];
	C[8] += A[0]*B[1];
	C[9] += -A[0]*B[0];
}
void gp_euclidean_5_3_2(const double *A, const double *B, double *C) {
	C[0] += -A[0]*B[9];
	C[1] += A[0]*B[8];
	C[2] += -A[0]*B[7];
	C[3] += -A[0]*B[6];
	C[4] += A[0]*B[5];
	C[5] += -A[0]*B[4];
	C[6] += A[0]*B[3];
	C[7] += -A[0]*B[2];
	C[8] += A[0]*B[1];
	C[9] += -A[0]*B[0];
}
void gp_euclidean_5_4_1(const double *A, const double *B, double *C) {
	gp_euclidean_5_1_4(A, B, C);
}
void gp_euclidean_5_5_0(const double *A, const double *B, double *C) {
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
	C[4] = A[4];
}
void copyMul_1(const double *A, double *C, double s) {
	C[0] = A[0]*s;
	C[1] = A[1]*s;
	C[2] = A[2]*s;
	C[3] = A[3]*s;
	C[4] = A[4]*s;
}
void copyDiv_1(const double *A, double *C, double s) {
	C[0] = A[0]/s;
	C[1] = A[1]/s;
	C[2] = A[2]/s;
	C[3] = A[3]/s;
	C[4] = A[4]/s;
}
void add_1(const double *A, double *C) {
	C[0] += A[0];
	C[1] += A[1];
	C[2] += A[2];
	C[3] += A[3];
	C[4] += A[4];
}
void sub_1(const double *A, double *C) {
	C[0] -= A[0];
	C[1] -= A[1];
	C[2] -= A[2];
	C[3] -= A[3];
	C[4] -= A[4];
}
void neg_1(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[1];
	C[2] = -A[2];
	C[3] = -A[3];
	C[4] = -A[4];
}
void add2_1_1(const double *A, const double *B, double *C) {
	C[0] = (A[0]+B[0]);
	C[1] = (A[1]+B[1]);
	C[2] = (A[2]+B[2]);
	C[3] = (A[3]+B[3]);
	C[4] = (A[4]+B[4]);
}
void sub2_1_1(const double *A, const double *B, double *C) {
	C[0] = (A[0]-B[0]);
	C[1] = (A[1]-B[1]);
	C[2] = (A[2]-B[2]);
	C[3] = (A[3]-B[3]);
	C[4] = (A[4]-B[4]);
}
void hp_1_1(const double *A, const double *B, double *C) {
	C[0] = A[0]*B[0];
	C[1] = A[1]*B[1];
	C[2] = A[2]*B[2];
	C[3] = A[3]*B[3];
	C[4] = A[4]*B[4];
}
void ihp_1_1(const double *A, const double *B, double *C) {
	C[0] = A[0]/((B[0]));
	C[1] = A[1]/((B[1]));
	C[2] = A[2]/((B[2]));
	C[3] = A[3]/((B[3]));
	C[4] = A[4]/((B[4]));
}
bool equals_1_1(const double *A, const double *B, double eps) {
		if (((A[0] - B[0]) < -eps) || ((A[0] - B[0]) > eps)) return false;
		if (((A[1] - B[1]) < -eps) || ((A[1] - B[1]) > eps)) return false;
		if (((A[2] - B[2]) < -eps) || ((A[2] - B[2]) > eps)) return false;
		if (((A[3] - B[3]) < -eps) || ((A[3] - B[3]) > eps)) return false;
		if (((A[4] - B[4]) < -eps) || ((A[4] - B[4]) > eps)) return false;
	return true;
}
bool zeroGroup_1(const double *A, double eps) {
		if ((A[0] < -eps) || (A[0] > eps)) return false;
		if ((A[1] < -eps) || (A[1] > eps)) return false;
		if ((A[2] < -eps) || (A[2] > eps)) return false;
		if ((A[3] < -eps) || (A[3] > eps)) return false;
		if ((A[4] < -eps) || (A[4] > eps)) return false;
		return true;
}
void copyGroup_2(const double *A, double *C) {
	C[0] = A[0];
	C[1] = A[1];
	C[2] = A[2];
	C[3] = A[3];
	C[4] = A[4];
	C[5] = A[5];
	C[6] = A[6];
	C[7] = A[7];
	C[8] = A[8];
	C[9] = A[9];
}
void copyMul_2(const double *A, double *C, double s) {
	C[0] = A[0]*s;
	C[1] = A[1]*s;
	C[2] = A[2]*s;
	C[3] = A[3]*s;
	C[4] = A[4]*s;
	C[5] = A[5]*s;
	C[6] = A[6]*s;
	C[7] = A[7]*s;
	C[8] = A[8]*s;
	C[9] = A[9]*s;
}
void copyDiv_2(const double *A, double *C, double s) {
	C[0] = A[0]/s;
	C[1] = A[1]/s;
	C[2] = A[2]/s;
	C[3] = A[3]/s;
	C[4] = A[4]/s;
	C[5] = A[5]/s;
	C[6] = A[6]/s;
	C[7] = A[7]/s;
	C[8] = A[8]/s;
	C[9] = A[9]/s;
}
void add_2(const double *A, double *C) {
	C[0] += A[0];
	C[1] += A[1];
	C[2] += A[2];
	C[3] += A[3];
	C[4] += A[4];
	C[5] += A[5];
	C[6] += A[6];
	C[7] += A[7];
	C[8] += A[8];
	C[9] += A[9];
}
void sub_2(const double *A, double *C) {
	C[0] -= A[0];
	C[1] -= A[1];
	C[2] -= A[2];
	C[3] -= A[3];
	C[4] -= A[4];
	C[5] -= A[5];
	C[6] -= A[6];
	C[7] -= A[7];
	C[8] -= A[8];
	C[9] -= A[9];
}
void neg_2(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[1];
	C[2] = -A[2];
	C[3] = -A[3];
	C[4] = -A[4];
	C[5] = -A[5];
	C[6] = -A[6];
	C[7] = -A[7];
	C[8] = -A[8];
	C[9] = -A[9];
}
void add2_2_2(const double *A, const double *B, double *C) {
	C[0] = (A[0]+B[0]);
	C[1] = (A[1]+B[1]);
	C[2] = (A[2]+B[2]);
	C[3] = (A[3]+B[3]);
	C[4] = (A[4]+B[4]);
	C[5] = (A[5]+B[5]);
	C[6] = (A[6]+B[6]);
	C[7] = (A[7]+B[7]);
	C[8] = (A[8]+B[8]);
	C[9] = (A[9]+B[9]);
}
void sub2_2_2(const double *A, const double *B, double *C) {
	C[0] = (A[0]-B[0]);
	C[1] = (A[1]-B[1]);
	C[2] = (A[2]-B[2]);
	C[3] = (A[3]-B[3]);
	C[4] = (A[4]-B[4]);
	C[5] = (A[5]-B[5]);
	C[6] = (A[6]-B[6]);
	C[7] = (A[7]-B[7]);
	C[8] = (A[8]-B[8]);
	C[9] = (A[9]-B[9]);
}
void hp_2_2(const double *A, const double *B, double *C) {
	C[0] = A[0]*B[0];
	C[1] = A[1]*B[1];
	C[2] = A[2]*B[2];
	C[3] = A[3]*B[3];
	C[4] = A[4]*B[4];
	C[5] = A[5]*B[5];
	C[6] = A[6]*B[6];
	C[7] = A[7]*B[7];
	C[8] = A[8]*B[8];
	C[9] = A[9]*B[9];
}
void ihp_2_2(const double *A, const double *B, double *C) {
	C[0] = A[0]/((B[0]));
	C[1] = A[1]/((B[1]));
	C[2] = A[2]/((B[2]));
	C[3] = A[3]/((B[3]));
	C[4] = A[4]/((B[4]));
	C[5] = A[5]/((B[5]));
	C[6] = A[6]/((B[6]));
	C[7] = A[7]/((B[7]));
	C[8] = A[8]/((B[8]));
	C[9] = A[9]/((B[9]));
}
bool equals_2_2(const double *A, const double *B, double eps) {
		if (((A[0] - B[0]) < -eps) || ((A[0] - B[0]) > eps)) return false;
		if (((A[1] - B[1]) < -eps) || ((A[1] - B[1]) > eps)) return false;
		if (((A[2] - B[2]) < -eps) || ((A[2] - B[2]) > eps)) return false;
		if (((A[3] - B[3]) < -eps) || ((A[3] - B[3]) > eps)) return false;
		if (((A[4] - B[4]) < -eps) || ((A[4] - B[4]) > eps)) return false;
		if (((A[5] - B[5]) < -eps) || ((A[5] - B[5]) > eps)) return false;
		if (((A[6] - B[6]) < -eps) || ((A[6] - B[6]) > eps)) return false;
		if (((A[7] - B[7]) < -eps) || ((A[7] - B[7]) > eps)) return false;
		if (((A[8] - B[8]) < -eps) || ((A[8] - B[8]) > eps)) return false;
		if (((A[9] - B[9]) < -eps) || ((A[9] - B[9]) > eps)) return false;
	return true;
}
bool zeroGroup_2(const double *A, double eps) {
		if ((A[0] < -eps) || (A[0] > eps)) return false;
		if ((A[1] < -eps) || (A[1] > eps)) return false;
		if ((A[2] < -eps) || (A[2] > eps)) return false;
		if ((A[3] < -eps) || (A[3] > eps)) return false;
		if ((A[4] < -eps) || (A[4] > eps)) return false;
		if ((A[5] < -eps) || (A[5] > eps)) return false;
		if ((A[6] < -eps) || (A[6] > eps)) return false;
		if ((A[7] < -eps) || (A[7] > eps)) return false;
		if ((A[8] < -eps) || (A[8] > eps)) return false;
		if ((A[9] < -eps) || (A[9] > eps)) return false;
		return true;
}
void copyGroup_3(const double *A, double *C) {
	copyGroup_2(A, C);
}
void copyMul_3(const double *A, double *C, double s) {
	copyMul_2(A, C, s);
}
void copyDiv_3(const double *A, double *C, double s) {
	copyDiv_2(A, C, s);
}
void add_3(const double *A, double *C) {
	add_2(A, C);
}
void sub_3(const double *A, double *C) {
	sub_2(A, C);
}
void neg_3(const double *A, double *C) {
	neg_2(A, C);
}
void add2_3_3(const double *A, const double *B, double *C) {
	add2_2_2(A, B, C);
}
void sub2_3_3(const double *A, const double *B, double *C) {
	sub2_2_2(A, B, C);
}
void hp_3_3(const double *A, const double *B, double *C) {
	hp_2_2(A, B, C);
}
void ihp_3_3(const double *A, const double *B, double *C) {
	ihp_2_2(A, B, C);
}
bool equals_3_3(const double *A, const double *B, double eps) {
	return equals_2_2(A, B, eps);
}
bool zeroGroup_3(const double *A, double eps) {
	return zeroGroup_2(A, eps);
}
void copyGroup_4(const double *A, double *C) {
	copyGroup_1(A, C);
}
void copyMul_4(const double *A, double *C, double s) {
	copyMul_1(A, C, s);
}
void copyDiv_4(const double *A, double *C, double s) {
	copyDiv_1(A, C, s);
}
void add_4(const double *A, double *C) {
	add_1(A, C);
}
void sub_4(const double *A, double *C) {
	sub_1(A, C);
}
void neg_4(const double *A, double *C) {
	neg_1(A, C);
}
void add2_4_4(const double *A, const double *B, double *C) {
	add2_1_1(A, B, C);
}
void sub2_4_4(const double *A, const double *B, double *C) {
	sub2_1_1(A, B, C);
}
void hp_4_4(const double *A, const double *B, double *C) {
	hp_1_1(A, B, C);
}
void ihp_4_4(const double *A, const double *B, double *C) {
	ihp_1_1(A, B, C);
}
bool equals_4_4(const double *A, const double *B, double eps) {
	return equals_1_1(A, B, eps);
}
bool zeroGroup_4(const double *A, double eps) {
	return zeroGroup_1(A, eps);
}
void copyGroup_5(const double *A, double *C) {
	copyGroup_0(A, C);
}
void copyMul_5(const double *A, double *C, double s) {
	copyMul_0(A, C, s);
}
void copyDiv_5(const double *A, double *C, double s) {
	copyDiv_0(A, C, s);
}
void add_5(const double *A, double *C) {
	add_0(A, C);
}
void sub_5(const double *A, double *C) {
	sub_0(A, C);
}
void neg_5(const double *A, double *C) {
	neg_0(A, C);
}
void add2_5_5(const double *A, const double *B, double *C) {
	add2_0_0(A, B, C);
}
void sub2_5_5(const double *A, const double *B, double *C) {
	sub2_0_0(A, B, C);
}
void hp_5_5(const double *A, const double *B, double *C) {
	hp_0_0(A, B, C);
}
void ihp_5_5(const double *A, const double *B, double *C) {
	ihp_0_0(A, B, C);
}
bool equals_5_5(const double *A, const double *B, double eps) {
	return equals_0_0(A, B, eps);
}
bool zeroGroup_5(const double *A, double eps) {
	return zeroGroup_0(A, eps);
}
void dual_default_0_5(const double *A, double *C) {
	C[0] = -A[0];
}
void undual_default_0_5(const double *A, double *C) {
	C[0] = A[0];
}
void dual_default_1_4(const double *A, double *C) {
	C[0] = A[0];
	C[1] = A[3];
	C[2] = -A[2];
	C[3] = A[1];
	C[4] = A[4];
}
void undual_default_1_4(const double *A, double *C) {
	C[0] = -A[0];
	C[1] = -A[3];
	C[2] = A[2];
	C[3] = -A[1];
	C[4] = -A[4];
}
void dual_default_2_3(const double *A, double *C) {
	C[0] = A[3];
	C[1] = -A[1];
	C[2] = A[0];
	C[3] = A[6];
	C[4] = A[5];
	C[5] = -A[4];
	C[6] = A[9];
	C[7] = A[2];
	C[8] = -A[8];
	C[9] = A[7];
}
void undual_default_2_3(const double *A, double *C) {
	C[0] = -A[3];
	C[1] = A[1];
	C[2] = -A[0];
	C[3] = -A[6];
	C[4] = -A[5];
	C[5] = A[4];
	C[6] = -A[9];
	C[7] = -A[2];
	C[8] = A[8];
	C[9] = -A[7];
}
void dual_default_3_2(const double *A, double *C) {
	C[0] = -A[2];
	C[1] = A[1];
	C[2] = -A[7];
	C[3] = -A[0];
	C[4] = A[5];
	C[5] = -A[4];
	C[6] = -A[3];
	C[7] = -A[9];
	C[8] = A[8];
	C[9] = -A[6];
}
void undual_default_3_2(const double *A, double *C) {
	C[0] = A[2];
	C[1] = -A[1];
	C[2] = A[7];
	C[3] = A[0];
	C[4] = -A[5];
	C[5] = A[4];
	C[6] = A[3];
	C[7] = A[9];
	C[8] = -A[8];
	C[9] = A[6];
}
void dual_default_4_1(const double *A, double *C) {
	undual_default_1_4(A, C);
}
void undual_default_4_1(const double *A, double *C) {
	dual_default_1_4(A, C);
}
void dual_default_5_0(const double *A, double *C) {
	undual_default_0_5(A, C);
}
void undual_default_5_0(const double *A, double *C) {
	dual_default_0_5(A, C);
}
void dual_euclidean_0_5(const double *A, double *C) {
	undual_default_0_5(A, C);
}
void undual_euclidean_0_5(const double *A, double *C) {
	undual_default_0_5(A, C);
}
void dual_euclidean_1_4(const double *A, double *C) {
	C[0] = A[4];
	C[1] = -A[3];
	C[2] = A[2];
	C[3] = -A[1];
	C[4] = A[0];
}
void undual_euclidean_1_4(const double *A, double *C) {
	dual_euclidean_1_4(A, C);
}
void dual_euclidean_2_3(const double *A, double *C) {
	C[0] = -A[9];
	C[1] = A[8];
	C[2] = -A[7];
	C[3] = A[6];
	C[4] = -A[5];
	C[5] = A[4];
	C[6] = -A[3];
	C[7] = -A[2];
	C[8] = A[1];
	C[9] = -A[0];
}
void undual_euclidean_2_3(const double *A, double *C) {
	dual_euclidean_2_3(A, C);
}
void dual_euclidean_3_2(const double *A, double *C) {
	C[0] = -A[9];
	C[1] = A[8];
	C[2] = -A[7];
	C[3] = -A[6];
	C[4] = A[5];
	C[5] = -A[4];
	C[6] = A[3];
	C[7] = -A[2];
	C[8] = A[1];
	C[9] = -A[0];
}
void undual_euclidean_3_2(const double *A, double *C) {
	dual_euclidean_3_2(A, C);
}
void dual_euclidean_4_1(const double *A, double *C) {
	dual_euclidean_1_4(A, C);
}
void undual_euclidean_4_1(const double *A, double *C) {
	dual_euclidean_1_4(A, C);
}
void dual_euclidean_5_0(const double *A, double *C) {
	undual_default_0_5(A, C);
}
void undual_euclidean_5_0(const double *A, double *C) {
	undual_default_0_5(A, C);
}
void applyGomGmv_1_1(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m1[0]+A[1]*O.m_m1[1]+A[2]*O.m_m1[2]+A[3]*O.m_m1[3]+A[4]*O.m_m1[4]);
	C[1] = (A[0]*O.m_m1[5]+A[1]*O.m_m1[6]+A[2]*O.m_m1[7]+A[3]*O.m_m1[8]+A[4]*O.m_m1[9]);
	C[2] = (A[0]*O.m_m1[10]+A[1]*O.m_m1[11]+A[2]*O.m_m1[12]+A[3]*O.m_m1[13]+A[4]*O.m_m1[14]);
	C[3] = (A[0]*O.m_m1[15]+A[1]*O.m_m1[16]+A[2]*O.m_m1[17]+A[3]*O.m_m1[18]+A[4]*O.m_m1[19]);
	C[4] = (A[0]*O.m_m1[20]+A[1]*O.m_m1[21]+A[2]*O.m_m1[22]+A[3]*O.m_m1[23]+A[4]*O.m_m1[24]);
}
void applyGomGmv_2_2(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m2[0]+A[1]*O.m_m2[1]+A[2]*O.m_m2[2]+A[3]*O.m_m2[3]+A[4]*O.m_m2[4]+A[5]*O.m_m2[5]+A[6]*O.m_m2[6]+A[7]*O.m_m2[7]+A[8]*O.m_m2[8]+A[9]*O.m_m2[9]);
	C[1] = (A[0]*O.m_m2[10]+A[1]*O.m_m2[11]+A[2]*O.m_m2[12]+A[3]*O.m_m2[13]+A[4]*O.m_m2[14]+A[5]*O.m_m2[15]+A[6]*O.m_m2[16]+A[7]*O.m_m2[17]+A[8]*O.m_m2[18]+A[9]*O.m_m2[19]);
	C[2] = (A[0]*O.m_m2[20]+A[1]*O.m_m2[21]+A[2]*O.m_m2[22]+A[3]*O.m_m2[23]+A[4]*O.m_m2[24]+A[5]*O.m_m2[25]+A[6]*O.m_m2[26]+A[7]*O.m_m2[27]+A[8]*O.m_m2[28]+A[9]*O.m_m2[29]);
	C[3] = (A[0]*O.m_m2[30]+A[1]*O.m_m2[31]+A[2]*O.m_m2[32]+A[3]*O.m_m2[33]+A[4]*O.m_m2[34]+A[5]*O.m_m2[35]+A[6]*O.m_m2[36]+A[7]*O.m_m2[37]+A[8]*O.m_m2[38]+A[9]*O.m_m2[39]);
	C[4] = (A[0]*O.m_m2[40]+A[1]*O.m_m2[41]+A[2]*O.m_m2[42]+A[3]*O.m_m2[43]+A[4]*O.m_m2[44]+A[5]*O.m_m2[45]+A[6]*O.m_m2[46]+A[7]*O.m_m2[47]+A[8]*O.m_m2[48]+A[9]*O.m_m2[49]);
	C[5] = (A[0]*O.m_m2[50]+A[1]*O.m_m2[51]+A[2]*O.m_m2[52]+A[3]*O.m_m2[53]+A[4]*O.m_m2[54]+A[5]*O.m_m2[55]+A[6]*O.m_m2[56]+A[7]*O.m_m2[57]+A[8]*O.m_m2[58]+A[9]*O.m_m2[59]);
	C[6] = (A[0]*O.m_m2[60]+A[1]*O.m_m2[61]+A[2]*O.m_m2[62]+A[3]*O.m_m2[63]+A[4]*O.m_m2[64]+A[5]*O.m_m2[65]+A[6]*O.m_m2[66]+A[7]*O.m_m2[67]+A[8]*O.m_m2[68]+A[9]*O.m_m2[69]);
	C[7] = (A[0]*O.m_m2[70]+A[1]*O.m_m2[71]+A[2]*O.m_m2[72]+A[3]*O.m_m2[73]+A[4]*O.m_m2[74]+A[5]*O.m_m2[75]+A[6]*O.m_m2[76]+A[7]*O.m_m2[77]+A[8]*O.m_m2[78]+A[9]*O.m_m2[79]);
	C[8] = (A[0]*O.m_m2[80]+A[1]*O.m_m2[81]+A[2]*O.m_m2[82]+A[3]*O.m_m2[83]+A[4]*O.m_m2[84]+A[5]*O.m_m2[85]+A[6]*O.m_m2[86]+A[7]*O.m_m2[87]+A[8]*O.m_m2[88]+A[9]*O.m_m2[89]);
	C[9] = (A[0]*O.m_m2[90]+A[1]*O.m_m2[91]+A[2]*O.m_m2[92]+A[3]*O.m_m2[93]+A[4]*O.m_m2[94]+A[5]*O.m_m2[95]+A[6]*O.m_m2[96]+A[7]*O.m_m2[97]+A[8]*O.m_m2[98]+A[9]*O.m_m2[99]);
}
void applyGomGmv_3_3(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m3[0]+A[1]*O.m_m3[1]+A[2]*O.m_m3[2]+A[3]*O.m_m3[3]+A[4]*O.m_m3[4]+A[5]*O.m_m3[5]+A[6]*O.m_m3[6]+A[7]*O.m_m3[7]+A[8]*O.m_m3[8]+A[9]*O.m_m3[9]);
	C[1] = (A[0]*O.m_m3[10]+A[1]*O.m_m3[11]+A[2]*O.m_m3[12]+A[3]*O.m_m3[13]+A[4]*O.m_m3[14]+A[5]*O.m_m3[15]+A[6]*O.m_m3[16]+A[7]*O.m_m3[17]+A[8]*O.m_m3[18]+A[9]*O.m_m3[19]);
	C[2] = (A[0]*O.m_m3[20]+A[1]*O.m_m3[21]+A[2]*O.m_m3[22]+A[3]*O.m_m3[23]+A[4]*O.m_m3[24]+A[5]*O.m_m3[25]+A[6]*O.m_m3[26]+A[7]*O.m_m3[27]+A[8]*O.m_m3[28]+A[9]*O.m_m3[29]);
	C[3] = (A[0]*O.m_m3[30]+A[1]*O.m_m3[31]+A[2]*O.m_m3[32]+A[3]*O.m_m3[33]+A[4]*O.m_m3[34]+A[5]*O.m_m3[35]+A[6]*O.m_m3[36]+A[7]*O.m_m3[37]+A[8]*O.m_m3[38]+A[9]*O.m_m3[39]);
	C[4] = (A[0]*O.m_m3[40]+A[1]*O.m_m3[41]+A[2]*O.m_m3[42]+A[3]*O.m_m3[43]+A[4]*O.m_m3[44]+A[5]*O.m_m3[45]+A[6]*O.m_m3[46]+A[7]*O.m_m3[47]+A[8]*O.m_m3[48]+A[9]*O.m_m3[49]);
	C[5] = (A[0]*O.m_m3[50]+A[1]*O.m_m3[51]+A[2]*O.m_m3[52]+A[3]*O.m_m3[53]+A[4]*O.m_m3[54]+A[5]*O.m_m3[55]+A[6]*O.m_m3[56]+A[7]*O.m_m3[57]+A[8]*O.m_m3[58]+A[9]*O.m_m3[59]);
	C[6] = (A[0]*O.m_m3[60]+A[1]*O.m_m3[61]+A[2]*O.m_m3[62]+A[3]*O.m_m3[63]+A[4]*O.m_m3[64]+A[5]*O.m_m3[65]+A[6]*O.m_m3[66]+A[7]*O.m_m3[67]+A[8]*O.m_m3[68]+A[9]*O.m_m3[69]);
	C[7] = (A[0]*O.m_m3[70]+A[1]*O.m_m3[71]+A[2]*O.m_m3[72]+A[3]*O.m_m3[73]+A[4]*O.m_m3[74]+A[5]*O.m_m3[75]+A[6]*O.m_m3[76]+A[7]*O.m_m3[77]+A[8]*O.m_m3[78]+A[9]*O.m_m3[79]);
	C[8] = (A[0]*O.m_m3[80]+A[1]*O.m_m3[81]+A[2]*O.m_m3[82]+A[3]*O.m_m3[83]+A[4]*O.m_m3[84]+A[5]*O.m_m3[85]+A[6]*O.m_m3[86]+A[7]*O.m_m3[87]+A[8]*O.m_m3[88]+A[9]*O.m_m3[89]);
	C[9] = (A[0]*O.m_m3[90]+A[1]*O.m_m3[91]+A[2]*O.m_m3[92]+A[3]*O.m_m3[93]+A[4]*O.m_m3[94]+A[5]*O.m_m3[95]+A[6]*O.m_m3[96]+A[7]*O.m_m3[97]+A[8]*O.m_m3[98]+A[9]*O.m_m3[99]);
}
void applyGomGmv_4_4(const om &O, const double *A, double *C) {
	C[0] = (A[0]*O.m_m4[0]+A[1]*O.m_m4[1]+A[2]*O.m_m4[2]+A[3]*O.m_m4[3]+A[4]*O.m_m4[4]);
	C[1] = (A[0]*O.m_m4[5]+A[1]*O.m_m4[6]+A[2]*O.m_m4[7]+A[3]*O.m_m4[8]+A[4]*O.m_m4[9]);
	C[2] = (A[0]*O.m_m4[10]+A[1]*O.m_m4[11]+A[2]*O.m_m4[12]+A[3]*O.m_m4[13]+A[4]*O.m_m4[14]);
	C[3] = (A[0]*O.m_m4[15]+A[1]*O.m_m4[16]+A[2]*O.m_m4[17]+A[3]*O.m_m4[18]+A[4]*O.m_m4[19]);
	C[4] = (A[0]*O.m_m4[20]+A[1]*O.m_m4[21]+A[2]*O.m_m4[22]+A[3]*O.m_m4[23]+A[4]*O.m_m4[24]);
}
void applyGomGmv_5_5(const om &O, const double *A, double *C) {
	C[0] = A[0]*O.m_m5[0];
}

double mv::largestCoordinate() const {
	double maxValue = 0.0;
	int nbC = c3ga_mvSize[m_gu], i;
	for (i = 0; i < nbC; i++)
		if (fabs(m_c[i]) > maxValue) maxValue = fabs(m_c[i]);
	return maxValue;
}

double mv::largestBasisBlade(unsigned int &bm) const {
	int nc = c3ga_mvSize[gu()];
	double maxC = -1.0, C;

	int idx = 0;
	int group = 0;
	int i = 0;
	
	bm = 0;

	while (i < nc) {
		if (gu() & (1 << group)) {
			for (int j = 0; j < c3ga_groupSize[group]; j++) {
				C = fabs(m_c[i]);
				if (C > maxC) {
					maxC = C;
					bm = c3ga_basisElementBitmapByIndex[idx];
				}
				idx++;
				i++;
			}
		}
		else idx += c3ga_groupSize[group];
		group++;
	}

	return maxC;
} // end of mv::largestBasisBlade()




void mv::compress(double epsilon /*= 0.0*/) {
	set(mv_compress(m_c, epsilon, m_gu));
}

mv mv_compress(const double *c, double epsilon /*= 0.0*/, int gu /*= 63*/) {
	int i, j, ia = 0, ib = 0, f, s;
	int cgu = 0;
	double cc[32];

	// for all grade parts...
	for (i = 0; i < 6; i++) {
		// check if grade part has memory use:
		if (!(gu & (1 << i))) continue;

		// check if abs coordinates of grade part are all < epsilon
		s = c3ga_groupSize[i];
		j = ia + s;
		f = 0;
		for (; ia < j; ia++)
			if (fabs(c[ia]) > epsilon) {f = 1; break;}
		ia = j;
		if (f) {
			c3ga::copy_N(cc + ib, c + ia - s, s);
			ib += s;
			cgu |= (1 << i);
		}
	}
	
	return mv(cgu, cc);
}

mv mv_compress(int nbBlades, const unsigned int *bitmaps, const double *coords) {
	// convert basis blade compression to regular coordinate array:
	double A[32];
	for (int i = 0; i < 32; i++) A[i] = 0.0;

	for (int i = 0; i < nbBlades; i++) {
		A[c3ga_basisElementIndexByBitmap[bitmaps[i]]] = coords[i] * (double)c3ga_basisElementSignByBitmap[bitmaps[i]];		
	}

	return mv_compress(A);
}


void mv::expand(const double *ptrs[6], bool nulls /* = true */) const {
	const double *c = m_c; // this pointer gets incremented below
	
	if (m_gu & 1) {
		ptrs[0] =  c;
		c += 1;
	}
	else ptrs[0] = (nulls) ? NULL : nullFloats();
	if (m_gu & 2) {
		ptrs[1] =  c;
		c += 5;
	}
	else ptrs[1] = (nulls) ? NULL : nullFloats();
	if (m_gu & 4) {
		ptrs[2] =  c;
		c += 10;
	}
	else ptrs[2] = (nulls) ? NULL : nullFloats();
	if (m_gu & 8) {
		ptrs[3] =  c;
		c += 10;
	}
	else ptrs[3] = (nulls) ? NULL : nullFloats();
	if (m_gu & 16) {
		ptrs[4] =  c;
		c += 5;
	}
	else ptrs[4] = (nulls) ? NULL : nullFloats();
	if (m_gu & 32) {
		ptrs[5] =  c;
	}
	else ptrs[5] = (nulls) ? NULL : nullFloats();
}

void om::set_grade_2_0()
{
	// Set image of no^e1
	m_m2[0] = (m_m1[0]*m_m1[6]-m_m1[1]*m_m1[5]);
	m_m2[10] = (m_m1[0]*m_m1[11]-m_m1[1]*m_m1[10]);
	m_m2[20] = (-m_m1[10]*m_m1[6]+m_m1[11]*m_m1[5]);
	m_m2[30] = (m_m1[0]*m_m1[16]-m_m1[1]*m_m1[15]);
	m_m2[40] = (-m_m1[15]*m_m1[6]+m_m1[16]*m_m1[5]);
	m_m2[50] = (m_m1[10]*m_m1[16]-m_m1[11]*m_m1[15]);
	m_m2[60] = (m_m1[0]*m_m1[21]-m_m1[1]*m_m1[20]);
	m_m2[70] = (-m_m1[20]*m_m1[6]+m_m1[21]*m_m1[5]);
	m_m2[80] = (m_m1[10]*m_m1[21]-m_m1[11]*m_m1[20]);
	m_m2[90] = (m_m1[15]*m_m1[21]-m_m1[16]*m_m1[20]);

}
void om::set_grade_2_1()
{
	// Set image of no^e2
	m_m2[1] = (m_m1[0]*m_m1[7]-m_m1[2]*m_m1[5]);
	m_m2[11] = (m_m1[0]*m_m1[12]-m_m1[10]*m_m1[2]);
	m_m2[21] = (-m_m1[10]*m_m1[7]+m_m1[12]*m_m1[5]);
	m_m2[31] = (m_m1[0]*m_m1[17]-m_m1[15]*m_m1[2]);
	m_m2[41] = (-m_m1[15]*m_m1[7]+m_m1[17]*m_m1[5]);
	m_m2[51] = (m_m1[10]*m_m1[17]-m_m1[12]*m_m1[15]);
	m_m2[61] = (m_m1[0]*m_m1[22]-m_m1[2]*m_m1[20]);
	m_m2[71] = (-m_m1[20]*m_m1[7]+m_m1[22]*m_m1[5]);
	m_m2[81] = (m_m1[10]*m_m1[22]-m_m1[12]*m_m1[20]);
	m_m2[91] = (m_m1[15]*m_m1[22]-m_m1[17]*m_m1[20]);

}
void om::set_grade_2_2()
{
	// Set image of e1^e2
	m_m2[2] = (m_m1[1]*m_m1[7]-m_m1[2]*m_m1[6]);
	m_m2[12] = (m_m1[1]*m_m1[12]-m_m1[11]*m_m1[2]);
	m_m2[22] = (-m_m1[11]*m_m1[7]+m_m1[12]*m_m1[6]);
	m_m2[32] = (m_m1[1]*m_m1[17]-m_m1[16]*m_m1[2]);
	m_m2[42] = (-m_m1[16]*m_m1[7]+m_m1[17]*m_m1[6]);
	m_m2[52] = (m_m1[11]*m_m1[17]-m_m1[12]*m_m1[16]);
	m_m2[62] = (m_m1[1]*m_m1[22]-m_m1[2]*m_m1[21]);
	m_m2[72] = (-m_m1[21]*m_m1[7]+m_m1[22]*m_m1[6]);
	m_m2[82] = (m_m1[11]*m_m1[22]-m_m1[12]*m_m1[21]);
	m_m2[92] = (m_m1[16]*m_m1[22]-m_m1[17]*m_m1[21]);

}
void om::set_grade_2_3()
{
	// Set image of no^e3
	m_m2[3] = (m_m1[0]*m_m1[8]-m_m1[3]*m_m1[5]);
	m_m2[13] = (m_m1[0]*m_m1[13]-m_m1[10]*m_m1[3]);
	m_m2[23] = (-m_m1[10]*m_m1[8]+m_m1[13]*m_m1[5]);
	m_m2[33] = (m_m1[0]*m_m1[18]-m_m1[15]*m_m1[3]);
	m_m2[43] = (-m_m1[15]*m_m1[8]+m_m1[18]*m_m1[5]);
	m_m2[53] = (m_m1[10]*m_m1[18]-m_m1[13]*m_m1[15]);
	m_m2[63] = (m_m1[0]*m_m1[23]-m_m1[20]*m_m1[3]);
	m_m2[73] = (-m_m1[20]*m_m1[8]+m_m1[23]*m_m1[5]);
	m_m2[83] = (m_m1[10]*m_m1[23]-m_m1[13]*m_m1[20]);
	m_m2[93] = (m_m1[15]*m_m1[23]-m_m1[18]*m_m1[20]);

}
void om::set_grade_2_4()
{
	// Set image of e1^e3
	m_m2[4] = (m_m1[1]*m_m1[8]-m_m1[3]*m_m1[6]);
	m_m2[14] = (m_m1[1]*m_m1[13]-m_m1[11]*m_m1[3]);
	m_m2[24] = (-m_m1[11]*m_m1[8]+m_m1[13]*m_m1[6]);
	m_m2[34] = (m_m1[1]*m_m1[18]-m_m1[16]*m_m1[3]);
	m_m2[44] = (-m_m1[16]*m_m1[8]+m_m1[18]*m_m1[6]);
	m_m2[54] = (m_m1[11]*m_m1[18]-m_m1[13]*m_m1[16]);
	m_m2[64] = (m_m1[1]*m_m1[23]-m_m1[21]*m_m1[3]);
	m_m2[74] = (-m_m1[21]*m_m1[8]+m_m1[23]*m_m1[6]);
	m_m2[84] = (m_m1[11]*m_m1[23]-m_m1[13]*m_m1[21]);
	m_m2[94] = (m_m1[16]*m_m1[23]-m_m1[18]*m_m1[21]);

}
void om::set_grade_2_5()
{
	// Set image of e2^e3
	m_m2[5] = (m_m1[2]*m_m1[8]-m_m1[3]*m_m1[7]);
	m_m2[15] = (-m_m1[12]*m_m1[3]+m_m1[13]*m_m1[2]);
	m_m2[25] = (-m_m1[12]*m_m1[8]+m_m1[13]*m_m1[7]);
	m_m2[35] = (-m_m1[17]*m_m1[3]+m_m1[18]*m_m1[2]);
	m_m2[45] = (-m_m1[17]*m_m1[8]+m_m1[18]*m_m1[7]);
	m_m2[55] = (m_m1[12]*m_m1[18]-m_m1[13]*m_m1[17]);
	m_m2[65] = (m_m1[2]*m_m1[23]-m_m1[22]*m_m1[3]);
	m_m2[75] = (-m_m1[22]*m_m1[8]+m_m1[23]*m_m1[7]);
	m_m2[85] = (m_m1[12]*m_m1[23]-m_m1[13]*m_m1[22]);
	m_m2[95] = (m_m1[17]*m_m1[23]-m_m1[18]*m_m1[22]);

}
void om::set_grade_2_6()
{
	// Set image of no^ni
	m_m2[6] = (m_m1[0]*m_m1[9]-m_m1[4]*m_m1[5]);
	m_m2[16] = (m_m1[0]*m_m1[14]-m_m1[10]*m_m1[4]);
	m_m2[26] = (-m_m1[10]*m_m1[9]+m_m1[14]*m_m1[5]);
	m_m2[36] = (m_m1[0]*m_m1[19]-m_m1[15]*m_m1[4]);
	m_m2[46] = (-m_m1[15]*m_m1[9]+m_m1[19]*m_m1[5]);
	m_m2[56] = (m_m1[10]*m_m1[19]-m_m1[14]*m_m1[15]);
	m_m2[66] = (m_m1[0]*m_m1[24]-m_m1[20]*m_m1[4]);
	m_m2[76] = (-m_m1[20]*m_m1[9]+m_m1[24]*m_m1[5]);
	m_m2[86] = (m_m1[10]*m_m1[24]-m_m1[14]*m_m1[20]);
	m_m2[96] = (m_m1[15]*m_m1[24]-m_m1[19]*m_m1[20]);

}
void om::set_grade_2_7()
{
	// Set image of e1^ni
	m_m2[7] = (m_m1[1]*m_m1[9]-m_m1[4]*m_m1[6]);
	m_m2[17] = (m_m1[1]*m_m1[14]-m_m1[11]*m_m1[4]);
	m_m2[27] = (-m_m1[11]*m_m1[9]+m_m1[14]*m_m1[6]);
	m_m2[37] = (m_m1[1]*m_m1[19]-m_m1[16]*m_m1[4]);
	m_m2[47] = (-m_m1[16]*m_m1[9]+m_m1[19]*m_m1[6]);
	m_m2[57] = (m_m1[11]*m_m1[19]-m_m1[14]*m_m1[16]);
	m_m2[67] = (m_m1[1]*m_m1[24]-m_m1[21]*m_m1[4]);
	m_m2[77] = (-m_m1[21]*m_m1[9]+m_m1[24]*m_m1[6]);
	m_m2[87] = (m_m1[11]*m_m1[24]-m_m1[14]*m_m1[21]);
	m_m2[97] = (m_m1[16]*m_m1[24]-m_m1[19]*m_m1[21]);

}
void om::set_grade_2_8()
{
	// Set image of e2^ni
	m_m2[8] = (m_m1[2]*m_m1[9]-m_m1[4]*m_m1[7]);
	m_m2[18] = (-m_m1[12]*m_m1[4]+m_m1[14]*m_m1[2]);
	m_m2[28] = (-m_m1[12]*m_m1[9]+m_m1[14]*m_m1[7]);
	m_m2[38] = (-m_m1[17]*m_m1[4]+m_m1[19]*m_m1[2]);
	m_m2[48] = (-m_m1[17]*m_m1[9]+m_m1[19]*m_m1[7]);
	m_m2[58] = (m_m1[12]*m_m1[19]-m_m1[14]*m_m1[17]);
	m_m2[68] = (m_m1[2]*m_m1[24]-m_m1[22]*m_m1[4]);
	m_m2[78] = (-m_m1[22]*m_m1[9]+m_m1[24]*m_m1[7]);
	m_m2[88] = (m_m1[12]*m_m1[24]-m_m1[14]*m_m1[22]);
	m_m2[98] = (m_m1[17]*m_m1[24]-m_m1[19]*m_m1[22]);

}
void om::set_grade_2_9()
{
	// Set image of e3^ni
	m_m2[9] = (m_m1[3]*m_m1[9]-m_m1[4]*m_m1[8]);
	m_m2[19] = (-m_m1[13]*m_m1[4]+m_m1[14]*m_m1[3]);
	m_m2[29] = (-m_m1[13]*m_m1[9]+m_m1[14]*m_m1[8]);
	m_m2[39] = (-m_m1[18]*m_m1[4]+m_m1[19]*m_m1[3]);
	m_m2[49] = (-m_m1[18]*m_m1[9]+m_m1[19]*m_m1[8]);
	m_m2[59] = (m_m1[13]*m_m1[19]-m_m1[14]*m_m1[18]);
	m_m2[69] = (-m_m1[23]*m_m1[4]+m_m1[24]*m_m1[3]);
	m_m2[79] = (-m_m1[23]*m_m1[9]+m_m1[24]*m_m1[8]);
	m_m2[89] = (m_m1[13]*m_m1[24]-m_m1[14]*m_m1[23]);
	m_m2[99] = (m_m1[18]*m_m1[24]-m_m1[19]*m_m1[23]);

}
void om::set_grade_3_0()
{
	// Set image of no^e1^e2
	m_m3[0] = (m_m1[0]*m_m2[22]+m_m1[10]*m_m2[2]-m_m1[5]*m_m2[12]);
	m_m3[10] = (m_m1[0]*m_m2[42]+m_m1[15]*m_m2[2]-m_m1[5]*m_m2[32]);
	m_m3[20] = (m_m1[0]*m_m2[52]-m_m1[10]*m_m2[32]+m_m1[15]*m_m2[12]);
	m_m3[30] = (-m_m1[10]*m_m2[42]+m_m1[15]*m_m2[22]+m_m1[5]*m_m2[52]);
	m_m3[40] = (m_m1[0]*m_m2[72]+m_m1[20]*m_m2[2]-m_m1[5]*m_m2[62]);
	m_m3[50] = (m_m1[0]*m_m2[82]-m_m1[10]*m_m2[62]+m_m1[20]*m_m2[12]);
	m_m3[60] = (-m_m1[10]*m_m2[72]+m_m1[20]*m_m2[22]+m_m1[5]*m_m2[82]);
	m_m3[70] = (m_m1[0]*m_m2[92]-m_m1[15]*m_m2[62]+m_m1[20]*m_m2[32]);
	m_m3[80] = (-m_m1[15]*m_m2[72]+m_m1[20]*m_m2[42]+m_m1[5]*m_m2[92]);
	m_m3[90] = (m_m1[10]*m_m2[92]-m_m1[15]*m_m2[82]+m_m1[20]*m_m2[52]);

}
void om::set_grade_3_1()
{
	// Set image of no^e1^e3
	m_m3[1] = (m_m1[0]*m_m2[24]+m_m1[10]*m_m2[4]-m_m1[5]*m_m2[14]);
	m_m3[11] = (m_m1[0]*m_m2[44]+m_m1[15]*m_m2[4]-m_m1[5]*m_m2[34]);
	m_m3[21] = (m_m1[0]*m_m2[54]-m_m1[10]*m_m2[34]+m_m1[15]*m_m2[14]);
	m_m3[31] = (-m_m1[10]*m_m2[44]+m_m1[15]*m_m2[24]+m_m1[5]*m_m2[54]);
	m_m3[41] = (m_m1[0]*m_m2[74]+m_m1[20]*m_m2[4]-m_m1[5]*m_m2[64]);
	m_m3[51] = (m_m1[0]*m_m2[84]-m_m1[10]*m_m2[64]+m_m1[20]*m_m2[14]);
	m_m3[61] = (-m_m1[10]*m_m2[74]+m_m1[20]*m_m2[24]+m_m1[5]*m_m2[84]);
	m_m3[71] = (m_m1[0]*m_m2[94]-m_m1[15]*m_m2[64]+m_m1[20]*m_m2[34]);
	m_m3[81] = (-m_m1[15]*m_m2[74]+m_m1[20]*m_m2[44]+m_m1[5]*m_m2[94]);
	m_m3[91] = (m_m1[10]*m_m2[94]-m_m1[15]*m_m2[84]+m_m1[20]*m_m2[54]);

}
void om::set_grade_3_2()
{
	// Set image of no^e2^e3
	m_m3[2] = (m_m1[0]*m_m2[25]+m_m1[10]*m_m2[5]-m_m1[5]*m_m2[15]);
	m_m3[12] = (m_m1[0]*m_m2[45]+m_m1[15]*m_m2[5]-m_m1[5]*m_m2[35]);
	m_m3[22] = (m_m1[0]*m_m2[55]-m_m1[10]*m_m2[35]+m_m1[15]*m_m2[15]);
	m_m3[32] = (-m_m1[10]*m_m2[45]+m_m1[15]*m_m2[25]+m_m1[5]*m_m2[55]);
	m_m3[42] = (m_m1[0]*m_m2[75]+m_m1[20]*m_m2[5]-m_m1[5]*m_m2[65]);
	m_m3[52] = (m_m1[0]*m_m2[85]-m_m1[10]*m_m2[65]+m_m1[20]*m_m2[15]);
	m_m3[62] = (-m_m1[10]*m_m2[75]+m_m1[20]*m_m2[25]+m_m1[5]*m_m2[85]);
	m_m3[72] = (m_m1[0]*m_m2[95]-m_m1[15]*m_m2[65]+m_m1[20]*m_m2[35]);
	m_m3[82] = (-m_m1[15]*m_m2[75]+m_m1[20]*m_m2[45]+m_m1[5]*m_m2[95]);
	m_m3[92] = (m_m1[10]*m_m2[95]-m_m1[15]*m_m2[85]+m_m1[20]*m_m2[55]);

}
void om::set_grade_3_3()
{
	// Set image of e1^e2^e3
	m_m3[3] = (m_m1[1]*m_m2[25]+m_m1[11]*m_m2[5]-m_m1[6]*m_m2[15]);
	m_m3[13] = (m_m1[1]*m_m2[45]+m_m1[16]*m_m2[5]-m_m1[6]*m_m2[35]);
	m_m3[23] = (m_m1[1]*m_m2[55]-m_m1[11]*m_m2[35]+m_m1[16]*m_m2[15]);
	m_m3[33] = (-m_m1[11]*m_m2[45]+m_m1[16]*m_m2[25]+m_m1[6]*m_m2[55]);
	m_m3[43] = (m_m1[1]*m_m2[75]+m_m1[21]*m_m2[5]-m_m1[6]*m_m2[65]);
	m_m3[53] = (m_m1[1]*m_m2[85]-m_m1[11]*m_m2[65]+m_m1[21]*m_m2[15]);
	m_m3[63] = (-m_m1[11]*m_m2[75]+m_m1[21]*m_m2[25]+m_m1[6]*m_m2[85]);
	m_m3[73] = (m_m1[1]*m_m2[95]-m_m1[16]*m_m2[65]+m_m1[21]*m_m2[35]);
	m_m3[83] = (-m_m1[16]*m_m2[75]+m_m1[21]*m_m2[45]+m_m1[6]*m_m2[95]);
	m_m3[93] = (m_m1[11]*m_m2[95]-m_m1[16]*m_m2[85]+m_m1[21]*m_m2[55]);

}
void om::set_grade_3_4()
{
	// Set image of no^e1^ni
	m_m3[4] = (m_m1[0]*m_m2[27]+m_m1[10]*m_m2[7]-m_m1[5]*m_m2[17]);
	m_m3[14] = (m_m1[0]*m_m2[47]+m_m1[15]*m_m2[7]-m_m1[5]*m_m2[37]);
	m_m3[24] = (m_m1[0]*m_m2[57]-m_m1[10]*m_m2[37]+m_m1[15]*m_m2[17]);
	m_m3[34] = (-m_m1[10]*m_m2[47]+m_m1[15]*m_m2[27]+m_m1[5]*m_m2[57]);
	m_m3[44] = (m_m1[0]*m_m2[77]+m_m1[20]*m_m2[7]-m_m1[5]*m_m2[67]);
	m_m3[54] = (m_m1[0]*m_m2[87]-m_m1[10]*m_m2[67]+m_m1[20]*m_m2[17]);
	m_m3[64] = (-m_m1[10]*m_m2[77]+m_m1[20]*m_m2[27]+m_m1[5]*m_m2[87]);
	m_m3[74] = (m_m1[0]*m_m2[97]-m_m1[15]*m_m2[67]+m_m1[20]*m_m2[37]);
	m_m3[84] = (-m_m1[15]*m_m2[77]+m_m1[20]*m_m2[47]+m_m1[5]*m_m2[97]);
	m_m3[94] = (m_m1[10]*m_m2[97]-m_m1[15]*m_m2[87]+m_m1[20]*m_m2[57]);

}
void om::set_grade_3_5()
{
	// Set image of no^e2^ni
	m_m3[5] = (m_m1[0]*m_m2[28]+m_m1[10]*m_m2[8]-m_m1[5]*m_m2[18]);
	m_m3[15] = (m_m1[0]*m_m2[48]+m_m1[15]*m_m2[8]-m_m1[5]*m_m2[38]);
	m_m3[25] = (m_m1[0]*m_m2[58]-m_m1[10]*m_m2[38]+m_m1[15]*m_m2[18]);
	m_m3[35] = (-m_m1[10]*m_m2[48]+m_m1[15]*m_m2[28]+m_m1[5]*m_m2[58]);
	m_m3[45] = (m_m1[0]*m_m2[78]+m_m1[20]*m_m2[8]-m_m1[5]*m_m2[68]);
	m_m3[55] = (m_m1[0]*m_m2[88]-m_m1[10]*m_m2[68]+m_m1[20]*m_m2[18]);
	m_m3[65] = (-m_m1[10]*m_m2[78]+m_m1[20]*m_m2[28]+m_m1[5]*m_m2[88]);
	m_m3[75] = (m_m1[0]*m_m2[98]-m_m1[15]*m_m2[68]+m_m1[20]*m_m2[38]);
	m_m3[85] = (-m_m1[15]*m_m2[78]+m_m1[20]*m_m2[48]+m_m1[5]*m_m2[98]);
	m_m3[95] = (m_m1[10]*m_m2[98]-m_m1[15]*m_m2[88]+m_m1[20]*m_m2[58]);

}
void om::set_grade_3_6()
{
	// Set image of e1^e2^ni
	m_m3[6] = (m_m1[1]*m_m2[28]+m_m1[11]*m_m2[8]-m_m1[6]*m_m2[18]);
	m_m3[16] = (m_m1[1]*m_m2[48]+m_m1[16]*m_m2[8]-m_m1[6]*m_m2[38]);
	m_m3[26] = (m_m1[1]*m_m2[58]-m_m1[11]*m_m2[38]+m_m1[16]*m_m2[18]);
	m_m3[36] = (-m_m1[11]*m_m2[48]+m_m1[16]*m_m2[28]+m_m1[6]*m_m2[58]);
	m_m3[46] = (m_m1[1]*m_m2[78]+m_m1[21]*m_m2[8]-m_m1[6]*m_m2[68]);
	m_m3[56] = (m_m1[1]*m_m2[88]-m_m1[11]*m_m2[68]+m_m1[21]*m_m2[18]);
	m_m3[66] = (-m_m1[11]*m_m2[78]+m_m1[21]*m_m2[28]+m_m1[6]*m_m2[88]);
	m_m3[76] = (m_m1[1]*m_m2[98]-m_m1[16]*m_m2[68]+m_m1[21]*m_m2[38]);
	m_m3[86] = (-m_m1[16]*m_m2[78]+m_m1[21]*m_m2[48]+m_m1[6]*m_m2[98]);
	m_m3[96] = (m_m1[11]*m_m2[98]-m_m1[16]*m_m2[88]+m_m1[21]*m_m2[58]);

}
void om::set_grade_3_7()
{
	// Set image of no^e3^ni
	m_m3[7] = (m_m1[0]*m_m2[29]+m_m1[10]*m_m2[9]-m_m1[5]*m_m2[19]);
	m_m3[17] = (m_m1[0]*m_m2[49]+m_m1[15]*m_m2[9]-m_m1[5]*m_m2[39]);
	m_m3[27] = (m_m1[0]*m_m2[59]-m_m1[10]*m_m2[39]+m_m1[15]*m_m2[19]);
	m_m3[37] = (-m_m1[10]*m_m2[49]+m_m1[15]*m_m2[29]+m_m1[5]*m_m2[59]);
	m_m3[47] = (m_m1[0]*m_m2[79]+m_m1[20]*m_m2[9]-m_m1[5]*m_m2[69]);
	m_m3[57] = (m_m1[0]*m_m2[89]-m_m1[10]*m_m2[69]+m_m1[20]*m_m2[19]);
	m_m3[67] = (-m_m1[10]*m_m2[79]+m_m1[20]*m_m2[29]+m_m1[5]*m_m2[89]);
	m_m3[77] = (m_m1[0]*m_m2[99]-m_m1[15]*m_m2[69]+m_m1[20]*m_m2[39]);
	m_m3[87] = (-m_m1[15]*m_m2[79]+m_m1[20]*m_m2[49]+m_m1[5]*m_m2[99]);
	m_m3[97] = (m_m1[10]*m_m2[99]-m_m1[15]*m_m2[89]+m_m1[20]*m_m2[59]);

}
void om::set_grade_3_8()
{
	// Set image of e1^e3^ni
	m_m3[8] = (m_m1[1]*m_m2[29]+m_m1[11]*m_m2[9]-m_m1[6]*m_m2[19]);
	m_m3[18] = (m_m1[1]*m_m2[49]+m_m1[16]*m_m2[9]-m_m1[6]*m_m2[39]);
	m_m3[28] = (m_m1[1]*m_m2[59]-m_m1[11]*m_m2[39]+m_m1[16]*m_m2[19]);
	m_m3[38] = (-m_m1[11]*m_m2[49]+m_m1[16]*m_m2[29]+m_m1[6]*m_m2[59]);
	m_m3[48] = (m_m1[1]*m_m2[79]+m_m1[21]*m_m2[9]-m_m1[6]*m_m2[69]);
	m_m3[58] = (m_m1[1]*m_m2[89]-m_m1[11]*m_m2[69]+m_m1[21]*m_m2[19]);
	m_m3[68] = (-m_m1[11]*m_m2[79]+m_m1[21]*m_m2[29]+m_m1[6]*m_m2[89]);
	m_m3[78] = (m_m1[1]*m_m2[99]-m_m1[16]*m_m2[69]+m_m1[21]*m_m2[39]);
	m_m3[88] = (-m_m1[16]*m_m2[79]+m_m1[21]*m_m2[49]+m_m1[6]*m_m2[99]);
	m_m3[98] = (m_m1[11]*m_m2[99]-m_m1[16]*m_m2[89]+m_m1[21]*m_m2[59]);

}
void om::set_grade_3_9()
{
	// Set image of e2^e3^ni
	m_m3[9] = (m_m1[12]*m_m2[9]+m_m1[2]*m_m2[29]-m_m1[7]*m_m2[19]);
	m_m3[19] = (m_m1[17]*m_m2[9]+m_m1[2]*m_m2[49]-m_m1[7]*m_m2[39]);
	m_m3[29] = (-m_m1[12]*m_m2[39]+m_m1[17]*m_m2[19]+m_m1[2]*m_m2[59]);
	m_m3[39] = (-m_m1[12]*m_m2[49]+m_m1[17]*m_m2[29]+m_m1[7]*m_m2[59]);
	m_m3[49] = (m_m1[2]*m_m2[79]+m_m1[22]*m_m2[9]-m_m1[7]*m_m2[69]);
	m_m3[59] = (-m_m1[12]*m_m2[69]+m_m1[2]*m_m2[89]+m_m1[22]*m_m2[19]);
	m_m3[69] = (-m_m1[12]*m_m2[79]+m_m1[22]*m_m2[29]+m_m1[7]*m_m2[89]);
	m_m3[79] = (-m_m1[17]*m_m2[69]+m_m1[2]*m_m2[99]+m_m1[22]*m_m2[39]);
	m_m3[89] = (-m_m1[17]*m_m2[79]+m_m1[22]*m_m2[49]+m_m1[7]*m_m2[99]);
	m_m3[99] = (m_m1[12]*m_m2[99]-m_m1[17]*m_m2[89]+m_m1[22]*m_m2[59]);

}
void om::set_grade_4_0()
{
	// Set image of no^e1^e2^e3
	m_m4[0] = (m_m1[0]*m_m3[33]+m_m1[10]*m_m3[13]-m_m1[15]*m_m3[3]-m_m1[5]*m_m3[23]);
	m_m4[5] = (m_m1[0]*m_m3[63]+m_m1[10]*m_m3[43]-m_m1[20]*m_m3[3]-m_m1[5]*m_m3[53]);
	m_m4[10] = (m_m1[0]*m_m3[83]+m_m1[15]*m_m3[43]-m_m1[20]*m_m3[13]-m_m1[5]*m_m3[73]);
	m_m4[15] = (m_m1[0]*m_m3[93]-m_m1[10]*m_m3[73]+m_m1[15]*m_m3[53]-m_m1[20]*m_m3[23]);
	m_m4[20] = (-m_m1[10]*m_m3[83]+m_m1[15]*m_m3[63]-m_m1[20]*m_m3[33]+m_m1[5]*m_m3[93]);

}
void om::set_grade_4_1()
{
	// Set image of no^e1^e2^ni
	m_m4[1] = (m_m1[0]*m_m3[36]+m_m1[10]*m_m3[16]-m_m1[15]*m_m3[6]-m_m1[5]*m_m3[26]);
	m_m4[6] = (m_m1[0]*m_m3[66]+m_m1[10]*m_m3[46]-m_m1[20]*m_m3[6]-m_m1[5]*m_m3[56]);
	m_m4[11] = (m_m1[0]*m_m3[86]+m_m1[15]*m_m3[46]-m_m1[20]*m_m3[16]-m_m1[5]*m_m3[76]);
	m_m4[16] = (m_m1[0]*m_m3[96]-m_m1[10]*m_m3[76]+m_m1[15]*m_m3[56]-m_m1[20]*m_m3[26]);
	m_m4[21] = (-m_m1[10]*m_m3[86]+m_m1[15]*m_m3[66]-m_m1[20]*m_m3[36]+m_m1[5]*m_m3[96]);

}
void om::set_grade_4_2()
{
	// Set image of no^e1^e3^ni
	m_m4[2] = (m_m1[0]*m_m3[38]+m_m1[10]*m_m3[18]-m_m1[15]*m_m3[8]-m_m1[5]*m_m3[28]);
	m_m4[7] = (m_m1[0]*m_m3[68]+m_m1[10]*m_m3[48]-m_m1[20]*m_m3[8]-m_m1[5]*m_m3[58]);
	m_m4[12] = (m_m1[0]*m_m3[88]+m_m1[15]*m_m3[48]-m_m1[20]*m_m3[18]-m_m1[5]*m_m3[78]);
	m_m4[17] = (m_m1[0]*m_m3[98]-m_m1[10]*m_m3[78]+m_m1[15]*m_m3[58]-m_m1[20]*m_m3[28]);
	m_m4[22] = (-m_m1[10]*m_m3[88]+m_m1[15]*m_m3[68]-m_m1[20]*m_m3[38]+m_m1[5]*m_m3[98]);

}
void om::set_grade_4_3()
{
	// Set image of no^e2^e3^ni
	m_m4[3] = (m_m1[0]*m_m3[39]+m_m1[10]*m_m3[19]-m_m1[15]*m_m3[9]-m_m1[5]*m_m3[29]);
	m_m4[8] = (m_m1[0]*m_m3[69]+m_m1[10]*m_m3[49]-m_m1[20]*m_m3[9]-m_m1[5]*m_m3[59]);
	m_m4[13] = (m_m1[0]*m_m3[89]+m_m1[15]*m_m3[49]-m_m1[20]*m_m3[19]-m_m1[5]*m_m3[79]);
	m_m4[18] = (m_m1[0]*m_m3[99]-m_m1[10]*m_m3[79]+m_m1[15]*m_m3[59]-m_m1[20]*m_m3[29]);
	m_m4[23] = (-m_m1[10]*m_m3[89]+m_m1[15]*m_m3[69]-m_m1[20]*m_m3[39]+m_m1[5]*m_m3[99]);

}
void om::set_grade_4_4()
{
	// Set image of e1^e2^e3^ni
	m_m4[4] = (m_m1[1]*m_m3[39]+m_m1[11]*m_m3[19]-m_m1[16]*m_m3[9]-m_m1[6]*m_m3[29]);
	m_m4[9] = (m_m1[1]*m_m3[69]+m_m1[11]*m_m3[49]-m_m1[21]*m_m3[9]-m_m1[6]*m_m3[59]);
	m_m4[14] = (m_m1[1]*m_m3[89]+m_m1[16]*m_m3[49]-m_m1[21]*m_m3[19]-m_m1[6]*m_m3[79]);
	m_m4[19] = (m_m1[1]*m_m3[99]-m_m1[11]*m_m3[79]+m_m1[16]*m_m3[59]-m_m1[21]*m_m3[29]);
	m_m4[24] = (-m_m1[11]*m_m3[89]+m_m1[16]*m_m3[69]-m_m1[21]*m_m3[39]+m_m1[6]*m_m3[99]);

}
void om::set_grade_5_0()
{
	// Set image of no^e1^e2^e3^ni
	m_m5[0] = (m_m1[0]*m_m4[24]+m_m1[10]*m_m4[14]-m_m1[15]*m_m4[9]+m_m1[20]*m_m4[4]-m_m1[5]*m_m4[19]);

}
void om::set(const dualSphere &ino, const dualSphere &ie1, const dualSphere &ie2, const dualSphere &ie3, const dualSphere &ini)
{
	// Set image of no
	m_m1[0] = ino.m_no;
	m_m1[5] = ino.m_e1;
	m_m1[10] = ino.m_e2;
	m_m1[15] = ino.m_e3;
	m_m1[20] = ino.m_ni;

	// Set image of e1
	m_m1[1] = ie1.m_no;
	m_m1[6] = ie1.m_e1;
	m_m1[11] = ie1.m_e2;
	m_m1[16] = ie1.m_e3;
	m_m1[21] = ie1.m_ni;

	// Set image of e2
	m_m1[2] = ie2.m_no;
	m_m1[7] = ie2.m_e1;
	m_m1[12] = ie2.m_e2;
	m_m1[17] = ie2.m_e3;
	m_m1[22] = ie2.m_ni;

	// Set image of e3
	m_m1[3] = ie3.m_no;
	m_m1[8] = ie3.m_e1;
	m_m1[13] = ie3.m_e2;
	m_m1[18] = ie3.m_e3;
	m_m1[23] = ie3.m_ni;

	// Set image of ni
	m_m1[4] = ini.m_no;
	m_m1[9] = ini.m_e1;
	m_m1[14] = ini.m_e2;
	m_m1[19] = ini.m_e3;
	m_m1[24] = ini.m_ni;

	om::set_grade_2_0();
	om::set_grade_2_1();
	om::set_grade_2_2();
	om::set_grade_2_3();
	om::set_grade_2_4();
	om::set_grade_2_5();
	om::set_grade_2_6();
	om::set_grade_2_7();
	om::set_grade_2_8();
	om::set_grade_2_9();
	om::set_grade_3_0();
	om::set_grade_3_1();
	om::set_grade_3_2();
	om::set_grade_3_3();
	om::set_grade_3_4();
	om::set_grade_3_5();
	om::set_grade_3_6();
	om::set_grade_3_7();
	om::set_grade_3_8();
	om::set_grade_3_9();
	om::set_grade_4_0();
	om::set_grade_4_1();
	om::set_grade_4_2();
	om::set_grade_4_3();
	om::set_grade_4_4();
	om::set_grade_5_0();
}
void om::set(const double *M)
{
	// Set image of no
	m_m1[0] = M[0];
	m_m1[5] = M[5];
	m_m1[10] = M[10];
	m_m1[15] = M[15];
	m_m1[20] = M[20];

	// Set image of e1
	m_m1[1] = M[1];
	m_m1[6] = M[6];
	m_m1[11] = M[11];
	m_m1[16] = M[16];
	m_m1[21] = M[21];

	// Set image of e2
	m_m1[2] = M[2];
	m_m1[7] = M[7];
	m_m1[12] = M[12];
	m_m1[17] = M[17];
	m_m1[22] = M[22];

	// Set image of e3
	m_m1[3] = M[3];
	m_m1[8] = M[8];
	m_m1[13] = M[13];
	m_m1[18] = M[18];
	m_m1[23] = M[23];

	// Set image of ni
	m_m1[4] = M[4];
	m_m1[9] = M[9];
	m_m1[14] = M[14];
	m_m1[19] = M[19];
	m_m1[24] = M[24];

	om::set_grade_2_0();
	om::set_grade_2_1();
	om::set_grade_2_2();
	om::set_grade_2_3();
	om::set_grade_2_4();
	om::set_grade_2_5();
	om::set_grade_2_6();
	om::set_grade_2_7();
	om::set_grade_2_8();
	om::set_grade_2_9();
	om::set_grade_3_0();
	om::set_grade_3_1();
	om::set_grade_3_2();
	om::set_grade_3_3();
	om::set_grade_3_4();
	om::set_grade_3_5();
	om::set_grade_3_6();
	om::set_grade_3_7();
	om::set_grade_3_8();
	om::set_grade_3_9();
	om::set_grade_4_0();
	om::set_grade_4_1();
	om::set_grade_4_2();
	om::set_grade_4_3();
	om::set_grade_4_4();
	om::set_grade_5_0();
}
mv add(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() | b.gu();
	double c[32];
	
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
			bidx += 5;
		}
		else copyGroup_1(a.getC() + aidx, c + cidx);
		aidx += 5;
		cidx += 5;
	}
	else if (b.gu() & 2) {
		copyGroup_1(b.getC() + bidx, c + cidx);
		bidx += 5;
		cidx += 5;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			add2_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
		}
		else copyGroup_2(a.getC() + aidx, c + cidx);
		aidx += 10;
		cidx += 10;
	}
	else if (b.gu() & 4) {
		copyGroup_2(b.getC() + bidx, c + cidx);
		bidx += 10;
		cidx += 10;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			add2_3_3(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
		}
		else copyGroup_3(a.getC() + aidx, c + cidx);
		aidx += 10;
		cidx += 10;
	}
	else if (b.gu() & 8) {
		copyGroup_3(b.getC() + bidx, c + cidx);
		bidx += 10;
		cidx += 10;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			add2_4_4(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 5;
		}
		else copyGroup_4(a.getC() + aidx, c + cidx);
		aidx += 5;
		cidx += 5;
	}
	else if (b.gu() & 16) {
		copyGroup_4(b.getC() + bidx, c + cidx);
		bidx += 5;
		cidx += 5;
	}
	
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			add2_5_5(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
		else copyGroup_5(a.getC() + aidx, c + cidx);
		cidx += 1;
	}
	else if (b.gu() & 32) {
		copyGroup_5(b.getC() + bidx, c + cidx);
		cidx += 1;
	}
	return mv(gu, c);
}
mv subtract(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() | b.gu();
	double c[32];
	
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
			bidx += 5;
		}
		else copyGroup_1(a.getC() + aidx, c + cidx);
		aidx += 5;
		cidx += 5;
	}
	else if (b.gu() & 2) {
		neg_1(b.getC() + bidx, c + cidx);
		bidx += 5;
		cidx += 5;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			sub2_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
		}
		else copyGroup_2(a.getC() + aidx, c + cidx);
		aidx += 10;
		cidx += 10;
	}
	else if (b.gu() & 4) {
		neg_2(b.getC() + bidx, c + cidx);
		bidx += 10;
		cidx += 10;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			sub2_3_3(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
		}
		else copyGroup_3(a.getC() + aidx, c + cidx);
		aidx += 10;
		cidx += 10;
	}
	else if (b.gu() & 8) {
		neg_3(b.getC() + bidx, c + cidx);
		bidx += 10;
		cidx += 10;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			sub2_4_4(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 5;
		}
		else copyGroup_4(a.getC() + aidx, c + cidx);
		aidx += 5;
		cidx += 5;
	}
	else if (b.gu() & 16) {
		neg_4(b.getC() + bidx, c + cidx);
		bidx += 5;
		cidx += 5;
	}
	
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			sub2_5_5(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
		else copyGroup_5(a.getC() + aidx, c + cidx);
		cidx += 1;
	}
	else if (b.gu() & 32) {
		neg_5(b.getC() + bidx, c + cidx);
		cidx += 1;
	}
	return mv(gu, c);
}
mv applyOM(const om &a, const mv &b)
{
	int idxB = 0, gu = 0, idxC = 0;
	double c[32];
	if (b.gu() & 1) {
		idxB += 1;
		idxC += 1;
	
	}
	
	if (b.gu() & 2) {
		applyGomGmv_1_1(a, b.getC() + idxB, c + idxC + 0);
		idxB += 5;
		idxC += 5;
	
	}
	
	if (b.gu() & 4) {
		applyGomGmv_2_2(a, b.getC() + idxB, c + idxC + 0);
		idxB += 10;
		idxC += 10;
	
	}
	
	if (b.gu() & 8) {
		applyGomGmv_3_3(a, b.getC() + idxB, c + idxC + 0);
		idxB += 10;
		idxC += 10;
	
	}
	
	if (b.gu() & 16) {
		applyGomGmv_4_4(a, b.getC() + idxB, c + idxC + 0);
		idxB += 5;
		idxC += 5;
	
	}
	
	if (b.gu() & 32) {
		applyGomGmv_5_5(a, b.getC() + idxB, c + idxC + 0);
	}
	
	return mv_compress(c, 0.0, b.gu());
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
	double c[32];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, b);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, b);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, b);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		copyDiv_3(a.getC() + idx, c + idx, b);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyDiv_4(a.getC() + idx, c + idx, b);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		copyDiv_5(a.getC() + idx, c + idx, b);
	}
	return mv(gu, c);
}
mv dual(const mv &a)
{
	int idx = 0;
	double c[32];
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		dual_default_0_5(a.getC() + idx, c + 31);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		dual_default_1_4(a.getC() + idx, c + 26);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		dual_default_2_3(a.getC() + idx, c + 16);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		dual_default_3_2(a.getC() + idx, c + 6);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		dual_default_4_1(a.getC() + idx, c + 1);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		dual_default_5_0(a.getC() + idx, c + 0);
	}
	
	return mv_compress(c, 0.0, 63);
}
mv undual(const mv &a)
{
	int idx = 0;
	double c[32];
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		undual_default_0_5(a.getC() + idx, c + 31);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		undual_default_1_4(a.getC() + idx, c + 26);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		undual_default_2_3(a.getC() + idx, c + 16);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		undual_default_3_2(a.getC() + idx, c + 6);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		undual_default_4_1(a.getC() + idx, c + 1);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		undual_default_5_0(a.getC() + idx, c + 0);
	}
	
	return mv_compress(c, 0.0, 63);
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
			bidx += 5;
		}
		else if (!zeroGroup_1(a.getC() + bidx, c)) return false;
		aidx += 5;
	}
	else if (b.gu() & 2) {
		if (!zeroGroup_1(b.getC() + bidx, c)) return false;
		bidx += 5;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			if (!equals_2_2(a.getC() + aidx, b.getC() + bidx, c)) return false;
			bidx += 10;
		}
		else if (!zeroGroup_2(a.getC() + bidx, c)) return false;
		aidx += 10;
	}
	else if (b.gu() & 4) {
		if (!zeroGroup_2(b.getC() + bidx, c)) return false;
		bidx += 10;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			if (!equals_3_3(a.getC() + aidx, b.getC() + bidx, c)) return false;
			bidx += 10;
		}
		else if (!zeroGroup_3(a.getC() + bidx, c)) return false;
		aidx += 10;
	}
	else if (b.gu() & 8) {
		if (!zeroGroup_3(b.getC() + bidx, c)) return false;
		bidx += 10;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			if (!equals_4_4(a.getC() + aidx, b.getC() + bidx, c)) return false;
			bidx += 5;
		}
		else if (!zeroGroup_4(a.getC() + bidx, c)) return false;
		aidx += 5;
	}
	else if (b.gu() & 16) {
		if (!zeroGroup_4(b.getC() + bidx, c)) return false;
		bidx += 5;
	}
	
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			if (!equals_5_5(a.getC() + aidx, b.getC() + bidx, c)) return false;
		}
		else if (!zeroGroup_5(a.getC() + bidx, c)) return false;
	}
	else if (b.gu() & 32) {
		if (!zeroGroup_5(b.getC() + bidx, c)) return false;
	}
	return true;
}
mv extractGrade(const mv &a, int groupBitmap)
{
	int aidx = 0, cidx = 0;
	int gu =  a.gu() & groupBitmap;
	double c[32];
	
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
			cidx += 5;
		}
		aidx += 5;
	}
	
	if (a.gu() & 4) {
		if (groupBitmap & 4) {
			copyGroup_2(a.getC() + aidx, c + cidx);
			cidx += 10;
		}
		aidx += 10;
	}
	
	if (a.gu() & 8) {
		if (groupBitmap & 8) {
			copyGroup_3(a.getC() + aidx, c + cidx);
			cidx += 10;
		}
		aidx += 10;
	}
	
	if (a.gu() & 16) {
		if (groupBitmap & 16) {
			copyGroup_4(a.getC() + aidx, c + cidx);
			cidx += 5;
		}
		aidx += 5;
	}
	
	if (a.gu() & 32) {
		if (groupBitmap & 32) {
			copyGroup_5(a.getC() + aidx, c + cidx);
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
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_0_3_3(_a[0], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_default_0_4_4(_a[0], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_default_0_5_5(_a[0], _b[5], c + 31);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
			gp_default_1_1_2(_a[1], _b[1], c + 6);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
			gp_default_1_2_3(_a[1], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_default_1_3_2(_a[1], _b[3], c + 6);
			gp_default_1_3_4(_a[1], _b[3], c + 26);
		}
		if (b.gu() & 16) {
			gp_default_1_4_3(_a[1], _b[4], c + 16);
			gp_default_1_4_5(_a[1], _b[4], c + 31);
		}
		if (b.gu() & 32) {
			gp_default_1_5_4(_a[1], _b[5], c + 26);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 6);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
			gp_default_2_1_3(_a[2], _b[1], c + 16);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
			gp_default_2_2_2(_a[2], _b[2], c + 6);
			gp_default_2_2_4(_a[2], _b[2], c + 26);
		}
		if (b.gu() & 8) {
			gp_default_2_3_1(_a[2], _b[3], c + 1);
			gp_default_2_3_3(_a[2], _b[3], c + 16);
			gp_default_2_3_5(_a[2], _b[3], c + 31);
		}
		if (b.gu() & 16) {
			gp_default_2_4_2(_a[2], _b[4], c + 6);
			gp_default_2_4_4(_a[2], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_default_2_5_3(_a[2], _b[5], c + 16);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_default_3_0_3(_a[3], _b[0], c + 16);
		}
		if (b.gu() & 2) {
			gp_default_3_1_2(_a[3], _b[1], c + 6);
			gp_default_3_1_4(_a[3], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_default_3_2_1(_a[3], _b[2], c + 1);
			gp_default_3_2_3(_a[3], _b[2], c + 16);
			gp_default_3_2_5(_a[3], _b[2], c + 31);
		}
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
			gp_default_3_3_2(_a[3], _b[3], c + 6);
			gp_default_3_3_4(_a[3], _b[3], c + 26);
		}
		if (b.gu() & 16) {
			gp_default_3_4_1(_a[3], _b[4], c + 1);
			gp_default_3_4_3(_a[3], _b[4], c + 16);
		}
		if (b.gu() & 32) {
			gp_default_3_5_2(_a[3], _b[5], c + 6);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_default_4_0_4(_a[4], _b[0], c + 26);
		}
		if (b.gu() & 2) {
			gp_default_4_1_3(_a[4], _b[1], c + 16);
			gp_default_4_1_5(_a[4], _b[1], c + 31);
		}
		if (b.gu() & 4) {
			gp_default_4_2_2(_a[4], _b[2], c + 6);
			gp_default_4_2_4(_a[4], _b[2], c + 26);
		}
		if (b.gu() & 8) {
			gp_default_4_3_1(_a[4], _b[3], c + 1);
			gp_default_4_3_3(_a[4], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
			gp_default_4_4_2(_a[4], _b[4], c + 6);
		}
		if (b.gu() & 32) {
			gp_default_4_5_1(_a[4], _b[5], c + 1);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 1) {
			gp_default_5_0_5(_a[5], _b[0], c + 31);
		}
		if (b.gu() & 2) {
			gp_default_5_1_4(_a[5], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_default_5_2_3(_a[5], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_default_5_3_2(_a[5], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_5_4_1(_a[5], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return mv_compress(c, 0.0, 63);
}
mv gp_em(const mv &a, const mv &b)
{
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_euclidean_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_euclidean_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_euclidean_0_2_2(_a[0], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_euclidean_0_3_3(_a[0], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_euclidean_0_4_4(_a[0], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_euclidean_0_5_5(_a[0], _b[5], c + 31);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_euclidean_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_euclidean_1_1_0(_a[1], _b[1], c + 0);
			gp_euclidean_1_1_2(_a[1], _b[1], c + 6);
		}
		if (b.gu() & 4) {
			gp_euclidean_1_2_1(_a[1], _b[2], c + 1);
			gp_euclidean_1_2_3(_a[1], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_euclidean_1_3_2(_a[1], _b[3], c + 6);
			gp_euclidean_1_3_4(_a[1], _b[3], c + 26);
		}
		if (b.gu() & 16) {
			gp_euclidean_1_4_3(_a[1], _b[4], c + 16);
			gp_euclidean_1_4_5(_a[1], _b[4], c + 31);
		}
		if (b.gu() & 32) {
			gp_euclidean_1_5_4(_a[1], _b[5], c + 26);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_euclidean_2_0_2(_a[2], _b[0], c + 6);
		}
		if (b.gu() & 2) {
			gp_euclidean_2_1_1(_a[2], _b[1], c + 1);
			gp_euclidean_2_1_3(_a[2], _b[1], c + 16);
		}
		if (b.gu() & 4) {
			gp_euclidean_2_2_0(_a[2], _b[2], c + 0);
			gp_euclidean_2_2_2(_a[2], _b[2], c + 6);
			gp_euclidean_2_2_4(_a[2], _b[2], c + 26);
		}
		if (b.gu() & 8) {
			gp_euclidean_2_3_1(_a[2], _b[3], c + 1);
			gp_euclidean_2_3_3(_a[2], _b[3], c + 16);
			gp_euclidean_2_3_5(_a[2], _b[3], c + 31);
		}
		if (b.gu() & 16) {
			gp_euclidean_2_4_2(_a[2], _b[4], c + 6);
			gp_euclidean_2_4_4(_a[2], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_euclidean_2_5_3(_a[2], _b[5], c + 16);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_euclidean_3_0_3(_a[3], _b[0], c + 16);
		}
		if (b.gu() & 2) {
			gp_euclidean_3_1_2(_a[3], _b[1], c + 6);
			gp_euclidean_3_1_4(_a[3], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_euclidean_3_2_1(_a[3], _b[2], c + 1);
			gp_euclidean_3_2_3(_a[3], _b[2], c + 16);
			gp_euclidean_3_2_5(_a[3], _b[2], c + 31);
		}
		if (b.gu() & 8) {
			gp_euclidean_3_3_0(_a[3], _b[3], c + 0);
			gp_euclidean_3_3_2(_a[3], _b[3], c + 6);
			gp_euclidean_3_3_4(_a[3], _b[3], c + 26);
		}
		if (b.gu() & 16) {
			gp_euclidean_3_4_1(_a[3], _b[4], c + 1);
			gp_euclidean_3_4_3(_a[3], _b[4], c + 16);
		}
		if (b.gu() & 32) {
			gp_euclidean_3_5_2(_a[3], _b[5], c + 6);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_euclidean_4_0_4(_a[4], _b[0], c + 26);
		}
		if (b.gu() & 2) {
			gp_euclidean_4_1_3(_a[4], _b[1], c + 16);
			gp_euclidean_4_1_5(_a[4], _b[1], c + 31);
		}
		if (b.gu() & 4) {
			gp_euclidean_4_2_2(_a[4], _b[2], c + 6);
			gp_euclidean_4_2_4(_a[4], _b[2], c + 26);
		}
		if (b.gu() & 8) {
			gp_euclidean_4_3_1(_a[4], _b[3], c + 1);
			gp_euclidean_4_3_3(_a[4], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_euclidean_4_4_0(_a[4], _b[4], c + 0);
			gp_euclidean_4_4_2(_a[4], _b[4], c + 6);
		}
		if (b.gu() & 32) {
			gp_euclidean_4_5_1(_a[4], _b[5], c + 1);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 1) {
			gp_euclidean_5_0_5(_a[5], _b[0], c + 31);
		}
		if (b.gu() & 2) {
			gp_euclidean_5_1_4(_a[5], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_euclidean_5_2_3(_a[5], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_euclidean_5_3_2(_a[5], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_euclidean_5_4_1(_a[5], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_euclidean_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return mv_compress(c, 0.0, 63);
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
		idx += 5;
	}
	
	if (a.gu() & 4) {
		if (!zeroGroup_2(a.getC() + idx, b)) bitmap |= 4;
		idx += 10;
	}
	
	if (a.gu() & 8) {
		if (!zeroGroup_3(a.getC() + idx, b)) bitmap |= 8;
		idx += 10;
	}
	
	if (a.gu() & 16) {
		if (!zeroGroup_4(a.getC() + idx, b)) bitmap |= 16;
		idx += 5;
	}
	
	if (a.gu() & 32) {
		if (!zeroGroup_5(a.getC() + idx, b)) bitmap |= 32;
	}
	return bitmap;
}
mv hp(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() & b.gu();
	double c[32];
	
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
			bidx += 5;
			cidx += 5;
		}
		aidx += 5;
	}
	else if (b.gu() & 2) {
		bidx += 5;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			hp_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
			cidx += 10;
		}
		aidx += 10;
	}
	else if (b.gu() & 4) {
		bidx += 10;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			hp_3_3(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
			cidx += 10;
		}
		aidx += 10;
	}
	else if (b.gu() & 8) {
		bidx += 10;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			hp_4_4(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 5;
			cidx += 5;
		}
		aidx += 5;
	}
	else if (b.gu() & 16) {
		bidx += 5;
	}
	
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			hp_5_5(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
	}
	else if (b.gu() & 32) {
	}
	return mv(gu, c);
}
mv ihp(const mv &a, const mv &b)
{
	int aidx = 0, bidx = 0, cidx = 0;
	int gu = a.gu() & b.gu();
	double c[32];
	
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
			bidx += 5;
			cidx += 5;
		}
		aidx += 5;
	}
	else if (b.gu() & 2) {
		bidx += 5;
	}
	
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			ihp_2_2(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
			cidx += 10;
		}
		aidx += 10;
	}
	else if (b.gu() & 4) {
		bidx += 10;
	}
	
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			ihp_3_3(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 10;
			cidx += 10;
		}
		aidx += 10;
	}
	else if (b.gu() & 8) {
		bidx += 10;
	}
	
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			ihp_4_4(a.getC() + aidx, b.getC() + bidx, c + cidx);
			bidx += 5;
			cidx += 5;
		}
		aidx += 5;
	}
	else if (b.gu() & 16) {
		bidx += 5;
	}
	
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			ihp_5_5(a.getC() + aidx, b.getC() + bidx, c + cidx);
		}
	}
	else if (b.gu() & 32) {
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
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_1(c);
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
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			gp_default_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return c[0];
}
mv lc(const mv &a, const mv &b)
{
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_0_3_3(_a[0], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_default_0_4_4(_a[0], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_default_0_5_5(_a[0], _b[5], c + 31);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
		}
		if (b.gu() & 8) {
			gp_default_1_3_2(_a[1], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_1_4_3(_a[1], _b[4], c + 16);
		}
		if (b.gu() & 32) {
			gp_default_1_5_4(_a[1], _b[5], c + 26);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
		if (b.gu() & 8) {
			gp_default_2_3_1(_a[2], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_2_4_2(_a[2], _b[4], c + 6);
		}
		if (b.gu() & 32) {
			gp_default_2_5_3(_a[2], _b[5], c + 16);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
		}
		if (b.gu() & 16) {
			gp_default_3_4_1(_a[3], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_3_5_2(_a[3], _b[5], c + 6);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
		}
		if (b.gu() & 32) {
			gp_default_4_5_1(_a[4], _b[5], c + 1);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 32) {
			gp_default_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return mv_compress(c, 0.0, 63);
}
mv rc(const mv &a, const mv &b)
{
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
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
			gp_default_2_0_2(_a[2], _b[0], c + 6);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_default_3_0_3(_a[3], _b[0], c + 16);
		}
		if (b.gu() & 2) {
			gp_default_3_1_2(_a[3], _b[1], c + 6);
		}
		if (b.gu() & 4) {
			gp_default_3_2_1(_a[3], _b[2], c + 1);
		}
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_default_4_0_4(_a[4], _b[0], c + 26);
		}
		if (b.gu() & 2) {
			gp_default_4_1_3(_a[4], _b[1], c + 16);
		}
		if (b.gu() & 4) {
			gp_default_4_2_2(_a[4], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_4_3_1(_a[4], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 1) {
			gp_default_5_0_5(_a[5], _b[0], c + 31);
		}
		if (b.gu() & 2) {
			gp_default_5_1_4(_a[5], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_default_5_2_3(_a[5], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_default_5_3_2(_a[5], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_5_4_1(_a[5], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return mv_compress(c, 0.0, 63);
}
mv hip(const mv &a, const mv &b)
{
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
	if (a.gu() & 2) {
		if (b.gu() & 2) {
			gp_default_1_1_0(_a[1], _b[1], c + 0);
		}
		if (b.gu() & 4) {
			gp_default_1_2_1(_a[1], _b[2], c + 1);
		}
		if (b.gu() & 8) {
			gp_default_1_3_2(_a[1], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_1_4_3(_a[1], _b[4], c + 16);
		}
		if (b.gu() & 32) {
			gp_default_1_5_4(_a[1], _b[5], c + 26);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
		if (b.gu() & 8) {
			gp_default_2_3_1(_a[2], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_2_4_2(_a[2], _b[4], c + 6);
		}
		if (b.gu() & 32) {
			gp_default_2_5_3(_a[2], _b[5], c + 16);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 2) {
			gp_default_3_1_2(_a[3], _b[1], c + 6);
		}
		if (b.gu() & 4) {
			gp_default_3_2_1(_a[3], _b[2], c + 1);
		}
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
		}
		if (b.gu() & 16) {
			gp_default_3_4_1(_a[3], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_3_5_2(_a[3], _b[5], c + 6);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 2) {
			gp_default_4_1_3(_a[4], _b[1], c + 16);
		}
		if (b.gu() & 4) {
			gp_default_4_2_2(_a[4], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_4_3_1(_a[4], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
		}
		if (b.gu() & 32) {
			gp_default_4_5_1(_a[4], _b[5], c + 1);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 2) {
			gp_default_5_1_4(_a[5], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_default_5_2_3(_a[5], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_default_5_3_2(_a[5], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_5_4_1(_a[5], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return mv_compress(c, 0.0, 63);
}
mv mhip(const mv &a, const mv &b)
{
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_0_3_3(_a[0], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_default_0_4_4(_a[0], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_default_0_5_5(_a[0], _b[5], c + 31);
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
		if (b.gu() & 8) {
			gp_default_1_3_2(_a[1], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_1_4_3(_a[1], _b[4], c + 16);
		}
		if (b.gu() & 32) {
			gp_default_1_5_4(_a[1], _b[5], c + 26);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 6);
		}
		if (b.gu() & 2) {
			gp_default_2_1_1(_a[2], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_2_2_0(_a[2], _b[2], c + 0);
		}
		if (b.gu() & 8) {
			gp_default_2_3_1(_a[2], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_2_4_2(_a[2], _b[4], c + 6);
		}
		if (b.gu() & 32) {
			gp_default_2_5_3(_a[2], _b[5], c + 16);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_default_3_0_3(_a[3], _b[0], c + 16);
		}
		if (b.gu() & 2) {
			gp_default_3_1_2(_a[3], _b[1], c + 6);
		}
		if (b.gu() & 4) {
			gp_default_3_2_1(_a[3], _b[2], c + 1);
		}
		if (b.gu() & 8) {
			gp_default_3_3_0(_a[3], _b[3], c + 0);
		}
		if (b.gu() & 16) {
			gp_default_3_4_1(_a[3], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_3_5_2(_a[3], _b[5], c + 6);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_default_4_0_4(_a[4], _b[0], c + 26);
		}
		if (b.gu() & 2) {
			gp_default_4_1_3(_a[4], _b[1], c + 16);
		}
		if (b.gu() & 4) {
			gp_default_4_2_2(_a[4], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_4_3_1(_a[4], _b[3], c + 1);
		}
		if (b.gu() & 16) {
			gp_default_4_4_0(_a[4], _b[4], c + 0);
		}
		if (b.gu() & 32) {
			gp_default_4_5_1(_a[4], _b[5], c + 1);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 1) {
			gp_default_5_0_5(_a[5], _b[0], c + 31);
		}
		if (b.gu() & 2) {
			gp_default_5_1_4(_a[5], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_default_5_2_3(_a[5], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_default_5_3_2(_a[5], _b[3], c + 6);
		}
		if (b.gu() & 16) {
			gp_default_5_4_1(_a[5], _b[4], c + 1);
		}
		if (b.gu() & 32) {
			gp_default_5_5_0(_a[5], _b[5], c + 0);
		}
	}
	return mv_compress(c, 0.0, 63);
}
double norm(const mv &a)
{
	double n2 = 0.0;
	double c[1];
	const double* _a[6];
	a.expand(_a);
	c3ga::zero_1(c);
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
	if (_a[5] != NULL) {  /* group 5 (grade 5) */
		c[0] = 0.0;
			gp_default_5_5_0(_a[5], _a[5], c);
		n2 += c[0];
	}
	return ((n2 < 0.0) ? ::sqrt(-n2) : ::sqrt(n2));
}
double norm2(const mv &a)
{
	double n2 = 0.0;
	double c[1];
	const double* _a[6];
	a.expand(_a);
	c3ga::zero_1(c);
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
	if (_a[5] != NULL) {  /* group 5 (grade 5) */
		c[0] = 0.0;
			gp_default_5_5_0(_a[5], _a[5], c);
		n2 += c[0];
	}
	return n2;
}
double norm_em(const mv &a)
{
	double c[1];
	double n2 = 0.0;
	int idx = 0;
	
	if (a.gu() & 1) { /* group 0 (grade 0) */
		c[0] = 0.0;
		gp_euclidean_0_0_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 1;
	}
	
	if (a.gu() & 2) { /* group 1 (grade 1) */
		c[0] = 0.0;
		gp_euclidean_1_1_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 5;
	}
	
	if (a.gu() & 4) { /* group 2 (grade 2) */
		c[0] = 0.0;
		gp_euclidean_2_2_0(a.getC() + idx, a.getC() + idx, c);
		n2 -= c[0];
		idx += 10;
	}
	
	if (a.gu() & 8) { /* group 3 (grade 3) */
		c[0] = 0.0;
		gp_euclidean_3_3_0(a.getC() + idx, a.getC() + idx, c);
		n2 -= c[0];
		idx += 10;
	}
	
	if (a.gu() & 16) { /* group 4 (grade 4) */
		c[0] = 0.0;
		gp_euclidean_4_4_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 5;
	}
	
	if (a.gu() & 32) { /* group 5 (grade 5) */
		c[0] = 0.0;
		gp_euclidean_5_5_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
	}
	return ::sqrt(n2);
}
double norm2_em(const mv &a)
{
	double c[1];
	double n2 = 0.0;
	int idx = 0;
	
	if (a.gu() & 1) { /* group 0 (grade 0) */
		c[0] = 0.0;
		gp_euclidean_0_0_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 1;
	}
	
	if (a.gu() & 2) { /* group 1 (grade 1) */
		c[0] = 0.0;
		gp_euclidean_1_1_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 5;
	}
	
	if (a.gu() & 4) { /* group 2 (grade 2) */
		c[0] = 0.0;
		gp_euclidean_2_2_0(a.getC() + idx, a.getC() + idx, c);
		n2 -= c[0];
		idx += 10;
	}
	
	if (a.gu() & 8) { /* group 3 (grade 3) */
		c[0] = 0.0;
		gp_euclidean_3_3_0(a.getC() + idx, a.getC() + idx, c);
		n2 -= c[0];
		idx += 10;
	}
	
	if (a.gu() & 16) { /* group 4 (grade 4) */
		c[0] = 0.0;
		gp_euclidean_4_4_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
		idx += 5;
	}
	
	if (a.gu() & 32) { /* group 5 (grade 5) */
		c[0] = 0.0;
		gp_euclidean_5_5_0(a.getC() + idx, a.getC() + idx, c);
		n2 += c[0];
	}
	return n2;
}
mv op(const mv &a, const mv &b)
{
	double c[32];
	const double* _a[6];
	const double* _b[6];
	a.expand(_a);
	b.expand(_b);
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
		if (b.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
		}
		if (b.gu() & 2) {
			gp_default_0_1_1(_a[0], _b[1], c + 1);
		}
		if (b.gu() & 4) {
			gp_default_0_2_2(_a[0], _b[2], c + 6);
		}
		if (b.gu() & 8) {
			gp_default_0_3_3(_a[0], _b[3], c + 16);
		}
		if (b.gu() & 16) {
			gp_default_0_4_4(_a[0], _b[4], c + 26);
		}
		if (b.gu() & 32) {
			gp_default_0_5_5(_a[0], _b[5], c + 31);
		}
	}
	if (a.gu() & 2) {
		if (b.gu() & 1) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
		}
		if (b.gu() & 2) {
			gp_default_1_1_2(_a[1], _b[1], c + 6);
		}
		if (b.gu() & 4) {
			gp_default_1_2_3(_a[1], _b[2], c + 16);
		}
		if (b.gu() & 8) {
			gp_default_1_3_4(_a[1], _b[3], c + 26);
		}
		if (b.gu() & 16) {
			gp_default_1_4_5(_a[1], _b[4], c + 31);
		}
	}
	if (a.gu() & 4) {
		if (b.gu() & 1) {
			gp_default_2_0_2(_a[2], _b[0], c + 6);
		}
		if (b.gu() & 2) {
			gp_default_2_1_3(_a[2], _b[1], c + 16);
		}
		if (b.gu() & 4) {
			gp_default_2_2_4(_a[2], _b[2], c + 26);
		}
		if (b.gu() & 8) {
			gp_default_2_3_5(_a[2], _b[3], c + 31);
		}
	}
	if (a.gu() & 8) {
		if (b.gu() & 1) {
			gp_default_3_0_3(_a[3], _b[0], c + 16);
		}
		if (b.gu() & 2) {
			gp_default_3_1_4(_a[3], _b[1], c + 26);
		}
		if (b.gu() & 4) {
			gp_default_3_2_5(_a[3], _b[2], c + 31);
		}
	}
	if (a.gu() & 16) {
		if (b.gu() & 1) {
			gp_default_4_0_4(_a[4], _b[0], c + 26);
		}
		if (b.gu() & 2) {
			gp_default_4_1_5(_a[4], _b[1], c + 31);
		}
	}
	if (a.gu() & 32) {
		if (b.gu() & 1) {
			gp_default_5_0_5(_a[5], _b[0], c + 31);
		}
	}
	return mv_compress(c, 0.0, 63);
}
mv gp(const mv &a, const double b)
{
	double c[32];
	const double* _a[6];
	const double* _b[1] = {&b};
	a.expand(_a);
	c3ga::zero_N(c, 32);
	if (a.gu() & 1) {
			gp_default_0_0_0(_a[0], _b[0], c + 0);
	}
	if (a.gu() & 2) {
			gp_default_1_0_1(_a[1], _b[0], c + 1);
	}
	if (a.gu() & 4) {
			gp_default_2_0_2(_a[2], _b[0], c + 6);
	}
	if (a.gu() & 8) {
			gp_default_3_0_3(_a[3], _b[0], c + 16);
	}
	if (a.gu() & 16) {
			gp_default_4_0_4(_a[4], _b[0], c + 26);
	}
	if (a.gu() & 32) {
			gp_default_5_0_5(_a[5], _b[0], c + 31);
	}
	return mv_compress(c, 0.0, 63);
}
mv sas(const mv &a, const double b, const double c)
{
	int idxa = 0, idxc = 0;
	int gu = a.gu() | ((c != 0.0) ? GRADE_0 : 0); // '| GRADE_0' to make sure the scalar part is included
	double C[32];
	
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
		idxa += 5;
		idxc += 5;
	}
	
	if (a.gu() & 4) {
		copyMul_2(a.getC() + idxa, C + idxc, b);
		idxa += 10;
		idxc += 10;
	}
	
	if (a.gu() & 8) {
		copyMul_3(a.getC() + idxa, C + idxc, b);
		idxa += 10;
		idxc += 10;
	}
	
	if (a.gu() & 16) {
		copyMul_4(a.getC() + idxa, C + idxc, b);
		idxa += 5;
		idxc += 5;
	}
	
	if (a.gu() & 32) {
		copyMul_5(a.getC() + idxa, C + idxc, b);
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



evenVersor exp(const pointPair &x, int order /*  = 12 */) {
	unsigned long maxC;
	int scale = 1;
	pointPair xScaled;
	evenVersor tmp1, tmp2; // temp evenVersor used for various purposes
	evenVersor xPow1, xPow2;
	evenVersor *result1 = &tmp1, *result2 = &tmp2;
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

evenVersor cos(const pointPair &x, int order) {
	evenVersor tmp1, tmp2; // temp evenVersor used for various purposes
	evenVersor xPow1, xPow2; // xScaled^... goes here
	evenVersor *result1 = &tmp1, *result2 = &tmp2; // accumulated result goes here (note: 'result1' = 'R')
	double s_x2;
	int i;
   
	// First try special cases: check if (x * x) is scalar
	tmp1 = gp(x, x);
	s_x2 = _double(tmp1);
	if ((norm2_returns_scalar(tmp1) - s_x2 * s_x2) < 1E-14) {
		// OK (x * x == ~scalar), so use special cases:
		if (s_x2 > 0.0) {
			return evenVersor(::cos(::sqrt(s_x2)));
		}
		else if (s_x2 < 0.0) {
			return evenVersor(::cosh(::sqrt(-s_x2)));
		}
		else {
			return evenVersor(1.0);
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



evenVersor sin(const pointPair &x, int order) {
	evenVersor tmp1, tmp2; // temp evenVersor used for various purposes
	evenVersor xPow1, xPow2; // xScaled^... goes here
	evenVersor *result1 = &tmp1, *result2 = &tmp2;
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
			return _evenVersor(x);
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
	double c[32];
	
	if (a.gu() & 1) {
		neg_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		neg_1(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		neg_2(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		neg_3(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		neg_4(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		neg_5(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv reverse(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyGroup_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyGroup_1(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		neg_2(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		neg_3(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyGroup_4(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		copyGroup_5(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv cliffordConjugate(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyGroup_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		neg_1(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		neg_2(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		copyGroup_3(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyGroup_4(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		neg_5(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv gradeInvolution(const mv &a)
{
	int idx = 0;
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyGroup_0(a.getC() + idx, c + idx);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		neg_1(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		copyGroup_2(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		neg_3(a.getC() + idx, c + idx);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyGroup_4(a.getC() + idx, c + idx);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		neg_5(a.getC() + idx, c + idx);
	}
	return mv(gu, c);
}
mv unit(const mv &a)
{
	int idx = 0;
	double n = norm_returns_scalar(a);
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, n);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, n);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, n);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		copyDiv_3(a.getC() + idx, c + idx, n);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyDiv_4(a.getC() + idx, c + idx, n);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		copyDiv_5(a.getC() + idx, c + idx, n);
	}
	return mv(gu, c);
}
mv unit_em(const mv &a)
{
	int idx = 0;
	double n = norm_em_returns_scalar(a);
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, n);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, n);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, n);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		copyDiv_3(a.getC() + idx, c + idx, n);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyDiv_4(a.getC() + idx, c + idx, n);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		copyDiv_5(a.getC() + idx, c + idx, n);
	}
	return mv(gu, c);
}
mv versorInverse(const mv &a)
{
	int idx = 0;
	double n2 = norm2_returns_scalar(a);
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, n2);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, n2);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, -n2);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		copyDiv_3(a.getC() + idx, c + idx, -n2);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyDiv_4(a.getC() + idx, c + idx, n2);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		copyDiv_5(a.getC() + idx, c + idx, n2);
	}
	return mv(gu, c);
}
mv versorInverse_em(const mv &a)
{
	int idx = 0;
	double n2 = norm2_em_returns_scalar(a);
	int gu = a.gu();
	double c[32];
	
	if (a.gu() & 1) {
		copyDiv_0(a.getC() + idx, c + idx, n2);
		idx += 1;
	}
	
	if (a.gu() & 2) {
		copyDiv_1(a.getC() + idx, c + idx, n2);
		idx += 5;
	}
	
	if (a.gu() & 4) {
		copyDiv_2(a.getC() + idx, c + idx, -n2);
		idx += 10;
	}
	
	if (a.gu() & 8) {
		copyDiv_3(a.getC() + idx, c + idx, -n2);
		idx += 10;
	}
	
	if (a.gu() & 16) {
		copyDiv_4(a.getC() + idx, c + idx, n2);
		idx += 5;
	}
	
	if (a.gu() & 32) {
		copyDiv_5(a.getC() + idx, c + idx, n2);
	}
	return mv(gu, c);
}
} // end of namespace c3ga
