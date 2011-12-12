#ifndef INCLUDE_AL_SYSTEM_PRINTING_H
#define INCLUDE_AL_SYSTEM_PRINTING_H

/*
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <stdio.h>
#include "allocore/system/al_Config.h"

namespace al{

/// Returns an ASCII character most closely matching an intensity value in [0,1].
char intensityToASCII(float v);

/// Prints 2D pixel array
template<class T> void print2D(T* pix, int nx, int ny, FILE * fp=stdout);

/// Print signed unit value on a horizontal plot.

/// @param[in]	value	Normalized value to plot
/// @param[in]	width	Character width of plot excluding center point
/// @param[in]	spaces	Print extra filling spaces to the right
/// @param[in]	point	The print character for points
void printPlot(float value, uint32_t width=50, bool spaces=true, const char * point="o");

/// Prints error messge to stderr and optionally calls exit()
void err(const char * msg, const char * src="", bool exits=true);

/// Prints warning messge to stderr
void warn(const char * msg, const char * src="");





// StateChange --------------------------------------------------------------

inline char intensityToASCII(float v){
	static const char map[] =
	" .,;-~_+<>i!lI?/|)(1}{][rcvunxzjftLCJUYXZO0Qoahkbdpqwm*WMB8&%$#@";
	//"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
//	 123456789.123456789.123456789.123456789.123456789.123456789.1234
	static const int N = sizeof(map)-1;
	v = v<0 ? 0 : (v>0.9999999f ? 0.9999999f : v);
	return map[int(N*v)];
}

template<class T> void print2D(T* pix, int nx, int ny, FILE * fp){
	for(int j=0; j<nx; ++j){
	for(int i=0; i<ny; ++i){
		float v = pix[j*nx + i];
		fprintf(fp, "%c ", intensityToASCII(v));
	} printf("\n"); }
}

} // ::al::

#endif
