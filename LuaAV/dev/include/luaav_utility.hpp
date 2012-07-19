#ifndef _LUAAV_UTILITY_H_
#define _LUAAV_UTILITY_H_ 1

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

#include "luaav.h"
#include <stdlib.h>
#include <math.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/*
	Math utilities:
*/
#ifdef __cplusplus

namespace al {
	template<typename T> 
	inline T fixnan(T value) { return value == value ? value : 0.; }
	
	#define LUAAV_HALF_RAND_MAX (RAND_MAX * 0.5)
	inline static double urandom() { return rand() / (double)RAND_MAX; }
	inline static double srandom() { return 1.0 - (rand() / (double)LUAAV_HALF_RAND_MAX); }

	inline static int urandom(int m) { return rand() % m; }
	inline static int srandom(int m) { return m - (rand() % (2*m)); }
} // al::

template <class T>
int luaav_msg_delete(al_sec t, T * u) { delete u; return LUAAV_OK; }

/*
	Utilities to reduce the cost of denormalized floating point numbers
	See http://ldesoras.free.fr/doc/articles/denormal-en.pdf
	Use these wherever tight or decaying feedback is likely in an algorithm 
		to avoid the sharp CPU spikes caused by denormal calculations
*/
namespace denormal {

	/*
		+ Replace denormal numbers by true 0’s
		+ Does not alter normal numbers
		— Test may break the instruction pipeline, slowing down the code.
		- Requires a memory store to access the bits.
		- Must be repeated at each stage of the process.
	*/
	static void test_and_kill (float & val);
	
	/*
		+ Replaces denormal numbers by true 0’s
		+ Quantification has a preventive effect on the outputted numbers
		+ Quite fast, only two additions.
		— Processing already denormal numbers slightly slows down the computation.
		- Quantifies the smallest numbers, adding relative error.
		- Must be repeated at each stage of the process.
	*/
	static double kill_by_quantization (double val);
	
	/*
		+ Fills the spectrum uniformly.
		+ Propagates to next stages.
		— Not that fast.
		- Requires a memory store/load to access the number bit-field.
		- Alters the smallest numbers by quantifying them.
	*/
	static void prevent_by_computed_noise (float &val);
	
	/*
		+ Fast.
		+ Fills the spectrum almost uniformly.
		+ Propagates to next stages.
		— Pollutes the cache memory (creating a static buffer of 512 floats)
			- Requires init_buffered_noise() call in startup
		- Alters the smallest numbers by quantifying them.
	*/
	static void init_buffered_noise();
	static void prevent_by_buffered_noise (float &val);
	
	/*
		+ Fast.
		+ Propagates to next stages.
		— Cancelled by DC-blockers or high-pass filters.
		- Alters the smallest numbers by quantifying them.
	*/
	static void prevent_by_adding_dc (float & val);
	
	/*
		+ Very fast.
		+ Fills the spectrum almost uniformly.
		+ Propagates to next stages.
		— It does not eliminate denormal numbers, but prevents their apparition.
		- To be maximally efficient, requires block-processing algorithm (using VSL_BLOCK_SIZE)
		- Alters the smallest numbers by quantifying them
	*/
	static void prevent_by_block_pulses(float * buffer, int blocksize);

} // denormal::

#endif

/*
	Graphics utilities:
*/
struct luaav_rect {
public:
    int x;
    int y;
    int width;
    int height;

	luaav_rect(int x=0, int y=0, int width=0, int height=0)
	:	x(x), y(y),
		width(width), height(height)
	{}
};

struct luaav_pt {
public:
	int x;
	int y;

	luaav_pt(int x=0, int y=0)
	:	x(x), y(y)
	{}
};

AL_API bool luaav_rects_are_equal(luaav_rect &r1, luaav_rect &r2);
AL_API bool luaav_within_rect(luaav_pt &p, luaav_rect &rect);

#endif /* include guard */