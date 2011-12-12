#ifndef INCLUDE_AL_MSG_TUBE_HPP
#define INCLUDE_AL_MSG_TUBE_HPP

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

#ifdef __cplusplus

#include "allocore/types/jack/ringbuffer.h"
#include "allocore/system/al_Time.h"
#include <string.h>
#include <queue>
#include <cstring>

#define AL_MSGTUBE_DEFAULT_SIZE_BITS (14) // 16384 bytes

/*
	A C++ wrapper around the JACK ringbuffer for deferred function calls
	Using templates for type-checked functions of variable arguments
	Adding a cache queue to seamlessly handle buffer overflow
*/

namespace al {

class MsgTube {
public:

	/* 
		Messages in the ringbuffer have the following header structure:
	*/
	struct Header {
		size_t size;
		al_sec t;
		void (*func)(char * args);
	};

	/*
		JACK ringbuffer (single-reader single-writer lock-free fifo)
	*/
	jack_ringbuffer_t * rb;
	size_t memsize;
	
	/*
		Cache of messages, when the ringbuffer is full
		TODO: set a cache limit? track when flushing fails for a prolonged period?
	*/
	std::queue<char *> cacheq;
	
	MsgTube(int bits = AL_MSGTUBE_DEFAULT_SIZE_BITS);
	~MsgTube();
	
	void executeUntil(al_sec until);
	
	/*
		Copies 'data', so you can safely free it after this call
	*/
	void send_data(al_sec t, void (*func)(al_sec t, char * data), char * data, size_t size);
	
	void send(al_sec t, void (*f)(al_sec t)) {
		struct Data {
			Header header;
			void (*f)(al_sec t);
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f };
		writeData((char *)&data, sizeof(Data));
	}
	
	template<typename A1>
	void send(al_sec t, void (*f)(al_sec t, A1 a1), A1 a1) {
		struct Data {
			Header header;
			void (*f)(al_sec t, A1 a1);
			A1 a1;	
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t, d->a1);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f, a1 };
		writeData((char *)&data, sizeof(Data));
	}
	
	template<typename A1, typename A2>
	void send(al_sec t, void (*f)(al_sec t, A1 a1, A2 a2), A1 a1, A2 a2) {
		struct Data {
			Header header;
			void (*f)(al_sec t, A1 a1, A2 a2);
			A1 a1; A2 a2;
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t, d->a1, d->a2);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f, a1, a2 };
		writeData((char *)&data, sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3>
	void send(al_sec t, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3), A1 a1, A2 a2, A3 a3) {
		struct Data {
			Header header;
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3);
			A1 a1; A2 a2; A3 a3; 
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t, d->a1, d->a2, d->a3);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f, a1, a2, a3 };
		writeData((char *)&data, sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3, typename A4>
	void send(al_sec t, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4), A1 a1, A2 a2, A3 a3, A4 a4) {
		struct Data {
			Header header;
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4);
			A1 a1; A2 a2; A3 a3; A4 a4;
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t, d->a1, d->a2, d->a3, d->a4);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f, a1, a2, a3, a4 };
		writeData((char *)&data, sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3, typename A4, typename A5>
	void send(al_sec t, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
		struct Data {
			Header header;
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);
			A1 a1; A2 a2; A3 a3; A4 a4; A5 a5;
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t, d->a1, d->a2, d->a3, d->a4, d->a5);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f, a1, a2, a3, a4, a5 };
		writeData((char *)&data, sizeof(Data));
	}
	
	template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	void send(al_sec t, void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
		struct Data {
			Header header;
			void (*f)(al_sec t, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);
			A1 a1; A2 a2; A3 a3; A4 a4; A5 a5; A6 a6;
			static void call(char * args) {
				const Data * d = (Data *)args;
				(d->f)(d->header.t, d->a1, d->a2, d->a3, d->a4, d->a5, d->a6);
			}
		};
		Data data = { { sizeof(Data), t, Data::call }, f, a1, a2, a3, a4, a5, a6 };
		writeData((char *)&data, sizeof(Data));
	}
	
protected:	

	//al_sec& mNow;

	void cache(const void * src, size_t size) {
		char * mem = new char[size];
		memcpy(mem, src, size);
		cacheq.push(mem);
	}
	
	bool flushCache() {
		while (!cacheq.empty()) {
			char * data = cacheq.front();
			size_t size = *((size_t *)data);
			if (size >= jack_ringbuffer_write_space(rb)) {
				return false;
			}
			
			// send cached message:
			jack_ringbuffer_write(rb, data, size);
			delete[] data;
			cacheq.pop();
		}
		return true;
	}
	
	void writeData(char * data, size_t size) {
		if (size >= memsize) {
			printf("ERROR WRITING TO RINGBUFFER\n");
		} else
		if (flushCache() && jack_ringbuffer_write_space(rb) >= size) {
			//printf("scheduled message\n");
			jack_ringbuffer_write(rb, data, size);
		} else {
			//printf("cached message\n");
			char * cpy = new char[size];
			memcpy(cpy, data, size);
			cache(cpy, size);
		}
	}
};

/*
	Inline Implementation
*/
#pragma mark Inline Implementation

inline MsgTube :: MsgTube(int bits) {
	memsize = 1<<bits;
	rb = jack_ringbuffer_create(memsize);
	//printf("created buffer of %d size\n", memsize);
	jack_ringbuffer_reset(rb);
}

inline MsgTube :: ~MsgTube() {
	// TODO: empty the cache
	// empty the ringbuffer
	jack_ringbuffer_free(rb);
}

inline void MsgTube :: executeUntil(al_sec until) {
	Header header;
	while (jack_ringbuffer_read_space(rb) > sizeof(header)) {
		jack_ringbuffer_peek(rb, (char *)&header, sizeof(header));
		if (header.t > until) {
			return;
		}
		char buf[header.size];
		jack_ringbuffer_read(rb, buf, header.size);
		//mNow = header.t;
		(header.func)(buf);
	}
	//mNow = until;	// the questionable line... 
}

/*
	Copies 'data', so you can safely free it after this call
*/
inline void MsgTube :: send_data(al_sec t, void (*func)(al_sec t, char * data), char * data, size_t size) {
	struct Data {
		Header header;
		void (*f)(al_sec t, char * args);
		
		static void call(char * args) {
			const Data * d = (Data *)args;
			(d->f)(d->header.t, args + sizeof(Data));
		}
	};

	size_t packetsize = sizeof(Data) + size;
	char packet[packetsize];
	
	Data * d = (Data *)packet;
	d->header.size = packetsize;
	d->header.t = t;
	d->header.func = Data::call;
	d->f = func;
	memcpy(packet+sizeof(Data), data, size);
	writeData(packet, packetsize);
}
	
	
} // al::

#endif // __cplusplus
#endif /* include guard */
