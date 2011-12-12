#ifndef INCLUDE_AL_PQ_H
#define INCLUDE_AL_PQ_H

/*
 *	Priority queue
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

#include "allocore/system/al_Time.h"

#include <stdlib.h>
#include <limits.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Standard function signature for the handler of a message */
typedef int (*al_msg_func)(al_sec t, char *);

/* Maximum memory footprint of a message */
#define AL_PQ_MSG_ARGS_SIZE (52)
struct al_msg {
	al_sec t;
	al_msg_func func;
	char mem[AL_PQ_MSG_ARGS_SIZE];
};
typedef struct al_msg * msg;

/*
	A priority queue
		random access insertion, sorted access retrieval/removal
		for single-threaded application
		sorts on insertion, with fast-path for first/last positions
*/
/* A message in the queue */
struct al_pq_msg {
	struct al_msg msg;
	struct al_pq_msg * next;
	al_sec retry;	// retry period, or 0 for non-deferable messages
};
typedef struct al_pq_msg * pq_msg;

typedef void * (*pq_custom_allocator)(void * userdata, size_t size);

struct al_pq {
	al_sec now; /* scheduler logical time */
	pq_msg head, tail, pool;
	int len;
	al_sec retry_period; /* if a message call fails */
	
	/* resource to use in case pool is empty */
	pq_custom_allocator alloc_func;
	void * alloc_func_userdata;
};
typedef struct al_pq * pq;

/* 
	allocate a new priority queue 
	alloc/alloc_userdata can be null, in which case the default allocator (malloc) is used
*/
extern pq al_pq_create(int size, al_sec birth, pq_custom_allocator alloc, void * alloc_userdata);
/* free a priority queue */
extern void al_pq_free(pq * ptr, int flush);

extern int al_pq_used(pq x);

/* grab memory for the next message */
extern char * al_pq_msg(pq x);
/* schedule it */
extern void al_pq_sched(pq x, al_sec t, al_sec retry, al_msg_func f);

/* remove any scheduled messages with *mem == ptr (matching addresses) */
extern void al_pq_cancel_ptr(pq x, void * ptr);

/* read the top message */
extern pq_msg al_pq_top(pq x);
/* pop the top message, return the next */
extern pq_msg al_pq_pop(pq x);

/* alternative API: */
/* convenience wrapper to call all scheduled functions up to a given timestamp */
extern void al_pq_update(pq x, al_sec until, int defer);
extern void al_pq_advance(pq x, al_sec step, int defer);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INCLUDE_AL_TIME_H */

