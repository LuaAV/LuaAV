#ifndef INCLUDE_AL_MSGQUEUE_HPP
#define INCLUDE_AL_MSGQUEUE_HPP

/*
 *	Priority queue of scheduled function calls
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

#ifdef __cplusplus

#include <string.h>
#include <list>

#include "allocore/system/al_Config.h"

namespace al {

// messages that are larger than this will be heap copied
#define AL_MSGQUEUE_ARGS_SIZE (76)

class MsgQueue {
public:	

	typedef void (*msg_func)(al_sec t, char * args);
	typedef void * (*malloc_func)(size_t size);
	typedef void (*free_func)(void * ptr);
	
	MsgQueue(int size = 128, malloc_func mfunc = NULL, free_func ffunc = NULL);
	~MsgQueue();
	
	// for truly accurate scheduling, always use this as logical time:
	al_sec now() { return mNow; }
	
	// trigger registered callbacks
	void update(al_sec until, bool defer = false);
	void advance(al_sec period, bool defer = false) { update(mNow + period, defer); }
	
	// how many messages are scheduled?
	int len() { return mLen; }
	
	// template wrappers for multi-argument functions
	// be sure to cast the send arguments to exactly match the function argument types!
	void send(al_sec at, void (*f)(al_sec t)) {
		struct Data {
			void (*f)(al_sec t);
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t);
			}
		};
		Data data = { f };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}

	template<typename A1>
	void send(al_sec at, void (*f)(al_sec t, A1 a1), A1 a1) {
		struct Data {
			void (*f)(al_sec t, A1 a1);
			A1 a1;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t, d->a1);
			}
		};
		Data data = { f, a1 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename A1, typename A2>
	void send(al_sec at, void (*f)(al_sec t, A1 a1, A2 a2), A1 a1, A2 a2) {
		struct Data {
			void (*f)(al_sec t, A1 a1, A2 a2);
			A1 a1; A2 a2;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t, d->a1, d->a2);
			}
		};
		Data data = { f, a1, a2 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3>
	void send(al_sec at, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3), A1 a1, A2 a2, A3 a3) {
		struct Data {
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3);
			A1 a1; A2 a2; A3 a3; 
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t, d->a1, d->a2, d->a3);
			}
		};
		Data data = { f, a1, a2, a3 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3, typename A4>
	void send(al_sec at, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4 ), A1 a1, A2 a2, A3 a3, A4 a4 ) {
		struct Data {
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4 );
			A1 a1; A2 a2; A3 a3; A4 a4; 
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t, d->a1, d->a2, d->a3, d->a4 );
			}
		};
		Data data = { f, a1, a2, a3, a4 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3, typename A4, typename A5>
	void send(al_sec at, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
		struct Data {
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);
			A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t, d->a1, d->a2, d->a3, d->a4, d->a5);
			}
		};
		Data data = { f, a1, a2, a3, a4, a5 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	void send(al_sec at, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
		struct Data {
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);
			A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				(d->f)(t, d->a1, d->a2, d->a3, d->a4, d->a5, d->a6);
			}
		};
		Data data = { f, a1, a2, a3, a4, a5, a6 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	/*
		Equivalents for object->method calls:
	*/
	template<typename T>
	void send(al_sec at, T * self, void (T::*f)(al_sec t)) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t);
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t);
			}
		};
		Data data = { self, f };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename T, typename A1>
	void send(al_sec at, T * self, void (T::*f)(al_sec t, A1 a1), A1 a1) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t, A1 a1);
			A1 a1;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t, d->a1);
			}
		};
		Data data = { self, f, a1 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename T, typename A1, typename A2>
	void send(al_sec at, T * self, void (T::*f)(al_sec t, A1 a1, A2 a2), A1 a1, A2 a2) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t, A1 a1, A2 a2);
			A1 a1; A2 a2;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t, d->a1, d->a2);
			}
		};
		Data data = { self, f, a1, a2 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename T, typename A1, typename A2, typename A3>
	void send(al_sec at, T * self, void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3), A1 a1, A2 a2, A3 a3) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3);
			A1 a1; A2 a2; A3 a3; 
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t, d->a1, d->a2, d->a3);
			}
		};
		Data data = { self, f, a1, a2, a3 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename T, typename A1, typename A2, typename A3, typename A4>
	void send(al_sec at, T * self, void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4 ), A1 a1, A2 a2, A3 a3, A4 a4 ) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4 );
			A1 a1; A2 a2; A3 a3; A4 a4; 
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t, d->a1, d->a2, d->a3, d->a4 );
			}
		};
		Data data = { self, f, a1, a2, a3, a4 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
	void send(al_sec at, T * self, void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);
			A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t, d->a1, d->a2, d->a3, d->a4, d->a5);
			}
		};
		Data data = { self, f, a1, a2, a3, a4, a5 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	
	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	void send(al_sec at, T * self, void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
		struct Data {
			T * self;
			void (T::*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);
			A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
			static void call(al_sec t, char * args) {
				const Data * d = (Data *)args;
				((d->self)->*(d->f))(t, d->a1, d->a2, d->a3, d->a4, d->a5, d->a6);
			}
		};
		Data data = { self, f, a1, a2, a3, a4, a5, a6 };
		sched(at, &Data::call, (char *)(&data), sizeof(Data));
	}
	

	// generic method to schedule a callback
	void sched(al_sec at, msg_func func, char * data, size_t size);
	
protected:

	struct Msg {
		struct Msg * next;
		size_t size;
		al_sec t;
		msg_func func;
		char mArgs[AL_MSGQUEUE_ARGS_SIZE];
		
		bool isBigMessage() { return size > AL_MSGQUEUE_ARGS_SIZE; }
		char * args() { return isBigMessage() ? *(char **)(mArgs) : mArgs; }
	};
	
	Msg * mHead;
	Msg * mTail;
	Msg * mPool;
	int mLen, mChunkSize;
	al_sec mNow;
	malloc_func mMalloc;
	free_func mFree;
	
	void growPool();
	void recycle(Msg * m);
};






	
} // al::

#endif // __cplusplus
#endif // include guard
