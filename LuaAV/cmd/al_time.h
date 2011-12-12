#ifndef INCLUDE_AL_TIME_H
#define INCLUDE_AL_TIME_H

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

/*
	Timing and sleep functions with millisecond (Win32) or nanosecond (Unix)
	resolution. Win32 requires linking to winmm.lib for multimedia timers.
*/

#include "al_config.h"
#include <limits.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long long int al_nsec;				/**< nanoseconds type (accurate to +/- 292.5 years) */
typedef double al_sec;						/**< seconds type */

/**! temporal limits */
#define AL_TIME_NSEC_NEVER (ULLONG_MAX)

/**! conversion factors for nanoseconds/seconds */
#define al_time_ns2s		1.0e-9
#define al_time_s2ns		1.0e9

/**! Get current time from OS */
extern al_sec al_time();					
extern al_nsec al_time_nsec();				

/**! Suspend calling thread's execution for dt sec/nsec */
extern void al_sleep(al_sec dt);		
extern void al_sleep_nsec(al_nsec dt);	

/**! convenience function to sleep until a target wall-clock time */
extern void al_sleep_until(al_sec target);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INCLUDE_AL_TIME_H */

