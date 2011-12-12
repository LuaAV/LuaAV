#ifndef INCLUDE_AL_PQ_HPP
#define INCLUDE_AL_PQ_HPP

/*
 *	Priority queue (C++ wrappers)
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

#include "al_pq.h"
#include <string.h>

/*
	Handy C++ wrapper around al_pq
	Supports sending functions of arbitrary argument types
*/

namespace al {

	typedef al_sec sec;
	typedef al_msg_func msg_func;
	
	class PQHelper {
		pq queue;
		
		inline bool sendany(sec delay, sec retry, msg_func f, const size_t size, char * args) {
			// TODO: assert (size <= AL_PQ_MSG_ARGS_SIZE);
			char * buf = al_pq_msg(queue);
			if (!buf) {
				return false;
			}
			memcpy(buf, args, size);
			al_pq_sched(queue, queue->now + delay, retry, f);
			return true;
		}
		
	public:
		
		PQHelper(pq p) {
			queue = p;
		}
		
		int used() { return al_pq_used(queue); }
		
		inline bool send(sec delay, sec retry, int (*f)(sec t)) {
			struct data {
				int (*f)(sec t);
				static int call(al_sec t, char * args) {
					const data * d = (data *)args;
					return (d->f)(t);
				}
			};	
			const data d = { f };
			return sendany(delay, retry, data::call, sizeof(data), (char *)(&d));
		}
		
		template<typename A1>
		inline bool send(sec delay, sec retry, int (*f)(sec t, A1 a1), A1 a1) {
			struct data {
				int (*f)(sec t, A1 a1);
				A1 a1;				
				static int call(al_sec t, char * args) {
					const data * d = (data *)args;
					return (d->f)(t, d->a1);
				}
			};	
			const data d = { f, a1 };
			return sendany(delay, retry, data::call, sizeof(data), (char *)(&d));
		}
		
		template<typename A1, typename A2>
		inline bool send(sec delay, sec retry, int (*f)(sec t, A1 a1, A2 a2), A1 a1, A2 a2) {
			struct data {
				int (*f)(sec t, A1 a1, A2 a2);
				A1 a1; A2 a2; 				
				static int call(al_sec t, char * args) {
					const data * d = (data *)args;
					return (d->f)(t, d->a1, d->a2);
				}
			};	
			const data d = { f, a1, a2 };
			return sendany(delay, retry, data::call, sizeof(data), (char *)(&d));
		}
		
		template<typename A1, typename A2, typename A3>
		inline bool send(sec delay, sec retry, int (*f)(sec t, A1 a1, A2 a2, A3 a3), A1 a1, A2 a2, A3 a3) {
			struct data {
				int (*f)(sec t, A1 a1, A2 a2, A3 a3);
				A1 a1; A2 a2; A3 a3;				
				static int call(al_sec t, char * args) {
					const data * d = (data *)args;
					return (d->f)(t, d->a1, d->a2, d->a3);
				}
			};	
			const data d = { f, a1, a2, a3 };
			return sendany(delay, retry, data::call, sizeof(data), (char *)(&d));
		}
		
		template<typename A1, typename A2, typename A3, typename A4>
		inline bool send(sec delay, sec retry, int (*f)(sec t, A1 a1, A2 a2, A3 a3, A4 a4), A1 a1, A2 a2, A3 a3, A4 a4) {
			struct data {
				int (*f)(sec t, A1 a1, A2 a2, A3 a3, A4 a4);
				A1 a1; A2 a2; A3 a3; A4 a4; 			
				static int call(al_sec t, char * args) {
					const data * d = (data *)args;
					return (d->f)(t, d->a1, d->a2, d->a3, d->a4);
				}
			};	
			const data d = { f, a1, a2, a3, a4 };
			return sendany(delay, retry, data::call, sizeof(data), (char *)(&d));
		}
		
		template<typename A1, typename A2, typename A3, typename A4, typename A5>
		inline bool send(sec delay, sec retry, int (*f)(sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
			struct data {
				int (*f)(sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);
				A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;				
				static int call(al_sec t, char * args) {
					const data * d = (data *)args;
					return (d->f)(t, d->a1, d->a2, d->a3, d->a4, d->a5);
				}
			};	
			const data d = { f, a1, a2, a3, a4, a5 };
			return sendany(delay, retry, data::call, sizeof(data), (char *)(&d));
		}
	};
	
} // al::

#endif //INCLUDE_AL_PQ_HPP
