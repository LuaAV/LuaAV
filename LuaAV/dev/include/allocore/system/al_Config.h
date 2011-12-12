#ifndef INCLUDE_AL_SYSTEM_CONFIG_H
#define INCLUDE_AL_SYSTEM_CONFIG_H 1

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

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "allocore/system/pstdint.h"

#define AL_SYSTEM_LIB_VERSION 0.01

#if defined(WIN32) || defined(__WINDOWS_MM__)
	#define AL_WIN32 1
	#include <windows.h>
	#ifdef AL_EXPORTS
		#define AL_API __declspec(dllexport)
	#else
		#define AL_API __declspec(dllimport)
	#endif
#elif defined( __APPLE__ ) && defined( __MACH__ )
	#define AL_OSX 1
	#define AL_API extern
#else
	#define AL_LINUX 1
	#define AL_API extern
#endif

/* 
	primitive typedefs
*/
typedef long long int al_nsec;				/**< nanoseconds type (accurate to +/- 292.5 years) */
typedef double al_sec;						/**< seconds type */


#if !defined(MIN)
    #define MIN(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
    #define MAX(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

#endif /* INCLUDE_AL_SYSTEM_CONFIG_H */
