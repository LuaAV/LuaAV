#ifndef INCLUDE_LUAAV_STATE_H
#define INCLUDE_LUAAV_STATE_H

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
#include "luaav_audio.hpp"
#include "allocore/types/al_MsgQueue.hpp"
#include "allocore/system/al_Memory.hpp"
#include "allocore/system/al_Thread.hpp"
#include "Gamma/Recorder.h"

#include <string>
#include <new>
#include <list>
#include <vector>

#define LUAAV_CACHE_LITERAL "LUAAV_CACHE_LITERAL"
#define LUAAV_EVENTS_LITERAL "LUAAV_EVENTS_LITERAL"
#define LUAAV_DEBUG_TRACEBACK_LITERAL "LUAAV_DEBUG_TRACEBACK_LITERAL"
#define LUAAV_GO_LITERAL "LUAAV_GO_LITERAL"

/*
	Custom memory allocator for al::pq
*/
AL_API void * luaav_arena_allocator(void * userdata, size_t size);


/*
	State associated with every independent script in LuaAV:
*/
struct LuaAVState : public Context {
	
	lua_State * L;	
	std::string name;
	
	/*
		Audio system:
	*/
	
	///! Audio IO signals
	Signal insignals[LUAAV_AUDIO_MAXIOCHANNELS];
	Signal outsignals[LUAAV_AUDIO_MAXIOCHANNELS];
	
	///! Messages from the audio thread
	al::MsgTube outbox;
	
	/* 
		Memory pools
		Allocate from these pools for memory that you want to persist 
			until the end of this LuaAVState
	*/
	al::Arena main_pool;
	al::Arena audio_pool;
	
	/* scheduling */
	samplestamp elapsed;		// audio thread
	al_sec audio_latency;		// amount by which audio thread is behind the main thread
	int defer_state;			// flag to indicate scheduling underrun
	al_sec birth;				// cpu time at which state was created
	/* main thread scheduled events */
	pq mainpq; //, audiopq;
	//al::MsgQueue mainpq;
	
	// for writing to disk.
	bool recording;
	gam::Recorder recorder;
	al::Thread recorderThread;
	std::string recorderFileName;
	
	bool scoping;
	Scope scope;
	
	
	void * userdata;
	
	
	
	LuaAVState(const char * name, void * userdata);
	virtual ~LuaAVState();
	
	void audioTick();
	
	void record(bool start, std::string filename="");
	

};

#endif /* include guard */