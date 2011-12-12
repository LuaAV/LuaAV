#ifndef INCLUDE_LUAAV_H
#define INCLUDE_LUAAV_H

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

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* dependecy on allo code */
#include "allocore/types/al_Array.h"
#include "allocore/system/al_Config.h"

#include "types/al_pq.h"

#ifdef AL_OSX
	#define LUA_AV_OS_NAME "OSX"
	#define LUAAV_API 
#elif AL_LINUX
	#define LUA_AV_OS_NAME "LINUX"
	#define LUAAV_API 
#elif AL_WIN32
	#define LUA_AV_OS_NAME "WIN32"
	#define LUAAV_API 
#else
	#define LUA_AV_OS_NAME "UNKNOWN"
	#ifdef LUAAV_EXPORTS
		#define LUAAV_API __declspec(dllexport)
	#else
		#define LUAAV_API __declspec(dllimport)
	#endif
#endif

typedef enum luaav_status
{
	LUAAV_OK = 0,
	LUAAV_GENERIC_ERR = 0x29a
} luaav_status;

typedef void (*err_handler)(const char *);

#pragma mark Library

/**!
	Library version (revision number)
*/
AL_API const char * luaav_revision();

/**!
	Initialize LuaAV library
*/
AL_API void luaav_init(int argc, char ** argv);

/**!
	Exit LuaAV library
*/
AL_API void luaav_quit();

/**!
	Set the LuaAV error handler
*/
AL_API void luaav_error_handler(err_handler handler);


/**!
	Post an error in LuaAV
*/
AL_API void luaav_error_msg(const char *fmt, ...);

/**!
	Main-thread recurrent entry point
*/
AL_API void luaav_main_tick();

/**!
	Audio-thread recurrent entry point
*/
AL_API void luaav_audio_tick();

/**!
	Allocate memory for the lifetime of LuaAV
*/
AL_API void * luaav_palloc(size_t size);

/**!
	Add module functions (luaopen_xxx) to the pre-installed or pre-loaded lists:
*/
AL_API void luaav_add_critical_module(const char * name, lua_CFunction func);
AL_API void luaav_add_main_module(const char * name, lua_CFunction func);
AL_API void luaav_add_opt_module(const char * name, lua_CFunction func);

#pragma mark States

/**!
	State associated with every independent script in LuaAV:
*/
typedef struct LuaAVState luaav_state;
typedef struct LuaAVState * luaav_state_handle;

/**!
	Create a LuaAVState. name and userdata are optional.
*/
AL_API luaav_state * luaav_state_create(const char * name, void * userdata);
AL_API void luaav_state_close(luaav_state ** S);

/**!
	Retrieve lua_State associated with a script & vice-versa
*/
AL_API lua_State * luaav_getstate(luaav_state * S);
AL_API lua_State * luaav_getrootstate(lua_State * L);
AL_API luaav_state * luaav_fromstate(lua_State * L);

/**!
	Call a function via 'go' (i.e. in the scheduler)
*/
AL_API void luaav_state_gocall(lua_State * L, int nargs);

/**!
	Get the main-thread priority queue associated with a state
*/
AL_API pq luaav_state_mainpq(luaav_state * S);
AL_API pq luaav_script_mainpq(lua_State * L);

/**!
	Time:
*/
AL_API al_sec luaav_state_now(luaav_state * S);
AL_API al_sec luaav_state_reallatency(luaav_state * S);
AL_API al_sec luaav_state_latency(luaav_state * S);

/**! 
	Retrieve the userdata associated via luaav_state_create()
*/
AL_API void * luaav_state_userdata(luaav_state * S);

/**!
	Allocate memory from the state memory pool. This memory will only be recovered when the state closes.
*/
AL_API void * luaav_state_palloc(luaav_state * S, size_t size);

/**!
	Running scripts:
*/
AL_API int luaav_state_loadfile(luaav_state * S, const char * filename, const char * filepath);
AL_API int luaav_state_dofile(luaav_state * S, const char * filename, const char * filepath);
AL_API int luaav_state_gofile(luaav_state * S, const char * filename, const char * filepath);
AL_API int luaav_state_dostring(luaav_state * S, const char * source, const char * sourcename);
AL_API int luaav_state_gostring(luaav_state * S, const char * source, const char * sourcename);

#pragma mark Audio

/**! internal timestamps in terms of samples */
typedef long long int samplestamp;

#define LUAAV_NEVER (0x7fffffffffffffffLL)
#define LUAAV_ALMOST_NEVER (LUAAV_NEVER-1)

#define LUAAV_AUDIO_DEFAULT_FADEIN_DUR (0.003)
#define LUAAV_AUDIO_DEFAULT_FADEOUT_DUR (0.02)

/**! 64-bit processing */
typedef double sample;

/**!
	A struct used to get/set global audio configuration:
*/
typedef struct {
	double samplerate;
	samplestamp blocksize;	/* should be a power of 2 */
	unsigned int inchannels, outchannels;
	sample * fadein, * fadeout; /* pre-loaded linear-fade buffers, equal to blocksize */
} luaav_audio_config;

/**! 
	Retrieve the current audio configuration 
*/
AL_API luaav_audio_config luaav_audio_config_current();
/**! 
	Retrieve the default audio configuration 
*/
AL_API luaav_audio_config luaav_audio_config_default();
/**! 
	Change the audio configuration 
*/
AL_API void luaav_audio_configure(luaav_audio_config cfg);
/**! 
	Retrieve the current audio samplerate 
*/
AL_API double luaav_audio_samplerate_current();

/*
	Allocate permanent memory in the audio thread.
		This memory will only be released on application shutdown.
	
	NOTE: NOT THREAD-SAFE! ONLY USE FROM AUDIO THREAD
*/
AL_API void * luaav_audio_palloc(size_t size);

/**!
	Allocate memory from the state memory pool. 
		This memory will only be recovered when the state closes.
		
	NOTE: NOT THREAD-SAFE! ONLY USE FROM AUDIO THREAD
*/
AL_API void * luaav_audio_state_palloc(luaav_state * S, size_t size);

/**!
	Request temporary memory in the audio thread
		If block == 0, memory will persist until the end of the current block
		if block != 0, memory will persist until the end of the subsequent block
		
	NOTE: NOT THREAD-SAFE! ONLY USE FROM AUDIO THREAD
	NOTE: DO NOT USE TO ALLOCATE LARGE CHUNKS OF MEMORY!
*/
AL_API void * luaav_audio_balloc(size_t size, int next);

/*
	Request/release a single-channel block of audio samples
	NOTE: NOT THREAD-SAFE! ONLY USE FROM AUDIO THREAD
*/
AL_API sample * luaav_audiobuffer_request();
AL_API void luaav_audiobuffer_release(sample * x);
AL_API void luaav_audiobuffer_zero(sample * x);

/**! 
	Retrieve the main input/output sample buffers
*/
AL_API sample * luaav_audio_input(int channel);
AL_API sample * luaav_audio_output(int channel);

/**! 
	Retrieve a buffer of samples set to 0
*/
AL_API sample * luaav_audio_silentbuffer();

/**!
	Get the main/audio tubes from a state:
*/
//AL_API tube luaav_state_inbox(luaav_state * S);
//AL_API tube luaav_state_outbox(luaav_state * S);

/**! 
	Current samplestamp time in audio process
*/
AL_API samplestamp luaav_audio_elapsed(luaav_state * S);


/**! 
	Audio node callback type definitions
*/
typedef int (*luaav_audio_calc)(void * state, int from, int to);
typedef void (*luaav_audio_gc)(void * state);

AL_API void audio_param_set(lua_State * L, void ** address, void * value);

#pragma mark Driver Implementation API
/* 
	For use by driver code only
*/
AL_API const sample * luaav_audio_driver_output(int channel);
AL_API sample * luaav_audio_driver_input(int channel);

#ifdef __cplusplus
}

class Context;
class Node;


Context * context_fromstate(lua_State * L, int idx = 1);

int node_create_userdata(lua_State * L, Node * n, int fenv_idx);
int node_userdata_initialize(lua_State * L, int node_idx, int params_idx);


struct ParamChange {
	unsigned int id;
	union  {
		double num;
		void * ptr;
	};
};
void node_send_paramchange(Node * n, ParamChange p);
void node_send_paramchange_double(Node * n, unsigned int id, double num);
void node_send_paramchange_ptr(Node * n, unsigned int id, void * ptr);

#endif



#endif /* include guard */
