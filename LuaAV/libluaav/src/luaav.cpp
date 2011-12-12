#include "luaav_state.hpp"
#include "luaav_revision.h"
#include "luaav_utility.hpp"

#include "allocore/types/al_MsgQueue.hpp"
#include "allocore/types/al_MsgTube.hpp"
#include "Gamma/SoundFile.h"

#include "lua_utility.h"

#include "Sync.h"

#ifdef AL_LINUX
	#include "apr-1.0/apr_general.h"
	#include "apr-1.0/apr_errno.h"
	#include "apr-1.0/apr_pools.h"
	#include "apr-1.0/apr_thread_mutex.h"
#else
	#include "apr-1/apr_general.h"
	#include "apr-1/apr_errno.h"
	#include "apr-1/apr_pools.h"
	#include "apr-1/apr_thread_mutex.h"
#endif

#include <map>
#include <string>
#include <stdlib.h>

//#undef LUAAV_USE_LUAJIT
#ifdef LUAAV_USE_LUAJIT
#include "luajit.h"
#endif

#define ddebug(...) 
//#define ddebug(...) printf(__VA_ARGS__)

#define LUAAV_SCRIPT_LITERAL "LUAAV_SCRIPT_LITERAL"
#define LUAAV_ROOTSTATE_LITERAL "LUAAV_ROOTSTATE_LITERAL"
#define LUAAV_SCRIPT_META_LITERAL "LUAAV_SCRIPT_META_LITERAL"

#define LUAAV_DEFAULT_AUDIO_LATENCY (0.05)


void luaav_default_error_handler(const char *msg) {
	fprintf(stderr, msg);
}

err_handler g_luaav_error_handler = luaav_default_error_handler;

void luaav_error_msg(const char *fmt, ...) {
	char buffer[4096];
	
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	int len = strlen(buffer);
	buffer[len] = '\n';
	buffer[len+1] = '\0';
	
	g_luaav_error_handler(buffer);
}


extern "C" {

	/* library-internal Lua modules */
	AL_API int luaopen_luaav(lua_State * L);
	AL_API int luaopen_luaav_time(lua_State * L);
	AL_API int luaopen_luaav_debug(lua_State * L);
	AL_API int luaopen_audio(lua_State * L);
	AL_API int luaopen_audio_soundfile(lua_State * L);
	AL_API int luaopen_audio_Reverb(lua_State * L);
	AL_API int luaopen_audio_debug(lua_State * L);
	AL_API int luaopen_midi(lua_State * L);
	AL_API int luaopen_osc(lua_State * L);
	AL_API int luaopen_Array(lua_State * L);

	/*
	** LuaFileSystem
	** Copyright Kepler Project 2003 (http://www.keplerproject.org/luafilesystem)
	**
	** $Id: luaav.cpp 4292 2011-10-28 03:29:54Z whsmith $
	*/
	AL_API int luaopen_lfs(lua_State *L);
	
	/*
	** $Id: luaav.cpp 4292 2011-10-28 03:29:54Z whsmith $
	** LPeg - PEG pattern matching for Lua
	** Copyright 2007, Lua.org & PUC-Rio  (see 'lpeg.html' for license)
	** written by Roberto Ierusalimschy
	*/
	AL_API int luaopen_lpeg (lua_State *L);
	AL_API int luaopen_listlpeg(lua_State *L);	
}

const char * luaav_revision() {
	static const char * revision = LuaAVRevisionNumber;
	return revision;
}

void luaav_error_handler(err_handler handler) {
	g_luaav_error_handler = handler;
}



/**! 
	internal usage: installs the main/opt modules in a lua_State
*/
AL_API void luaav_install_modules(lua_State * L);

/**! 
	internal usage: locks / unlocks the audio mutex.
*/
AL_API void luaav_audio_lock();
AL_API void luaav_audio_unlock();


/* one application-lifetime pool for dedicated access in main thread */
static apr_pool_t * luaav_main_pool;
/* one application-lifetime pool for dedicated access in audio thread */
static apr_pool_t * luaav_audio_pool;
/* two pools for audio block processing (current & next block lifetimes) */
static apr_pool_t * luaav_audio_block_pool[2];

/* custom allocator function for pq using apr_pool_t */
//void * luaav_apr_allocator(void * userdata, size_t size) {
//	ddebug("palloc %d\n", size);
//	return apr_palloc((apr_pool_t *)userdata, size);
//}

void * luaav_arena_allocator(void * userdata, size_t size) {
	return ((al::Arena *)userdata)->alloc(size);
}
	
static apr_thread_mutex_t * luaav_audio_mutex;
static apr_status_t check_apr(apr_status_t err) {
	char errstr[1024];
	if (err != APR_SUCCESS) {
		apr_strerror(err, errstr, 1024);
		printf("apr error: %s\n", errstr);
	}
	return err;
}

static int luastate_hook_count = 0;
static int luastate_hook_step = 1000;
static int luastate_hook_limit = 100;
static void luastate_hook(lua_State *L, lua_Debug *ar) {
	switch (ar->event) {
		case LUA_HOOKCOUNT:
			//printf("count hook %d\n", ++luastate_hook_count);
			
			if (luastate_hook_count > luastate_hook_limit) {
				/* 
					we're probably in an infinite loop in Lua
					somehow need to abort L and return to its entry point
				*/
				luastate_hook_count = 0;
				printf("count hook exceeded limit %d", luastate_hook_step * luastate_hook_limit);
				lua_pushfstring(L, "count hook exceeded limit %d", luastate_hook_step * luastate_hook_limit);
				lua_error(L); //<-- caused a crash. why? is it a luajit problem?
			}
			
			break;
		default:
			break;
	}
}

static bool initialized = false;

static al::MsgTube luaav_inbox, luaav_outbox;
static samplestamp audio_time = 0;

static std::list<LuaAVState *> states;

/* audio configuration */
static const luaav_audio_config default_audio_config = { 44100.0, LUAAV_AUDIO_BLOCKSIZE, 2, 2 };
static luaav_audio_config audio_config = { 44100.0, LUAAV_AUDIO_BLOCKSIZE, 0, 0 };

/* audio IO busses */
static std::vector<sample *> luaav_audio_inputs(16);
static std::vector<sample *> luaav_audio_outputs(16);

static sample * audio_fadein;
static sample * audio_fadeout;
static sample * audio_silentbuffer;


#pragma mark LuaAVState



void * stateRecordingThreadFunction(void * userData);

LuaAVState :: LuaAVState(const char * name, void * userdata) 
:	Context(NULL, "LuaAVState"), 
	elapsed(0), 
	audio_latency(LUAAV_DEFAULT_AUDIO_LATENCY), 
	defer_state(0), 
	name(name), 
	recording(false),
	recorder(2, 8192 * 32), // lots of buffer!
	scoping(false),
	scope(2, 512, 32),
	userdata(userdata) {

	//A = NULL;
		
//	// allocate memory pools:
//	check_apr(apr_pool_create(&main_pool, NULL));
//	check_apr(apr_pool_create(&audio_pool, NULL));
	// mainpq can use the state-local main-thread pool to allocate messages:
	mainpq = al_pq_create(128, 0, luaav_arena_allocator, &main_pool);
	// audiopq can use the state-local audio-thread pool to allocate messages:
//	audiopq = al_pq_create(128, 0, luaav_apr_allocator, audio_pool);
//	inbox = al_tube_create(12);
//	outbox = al_tube_create(12);
	birth = al_time();
	
	this->L = lua_open();
	
	// install hook:
	//lua_sethook(this->L, luastate_hook, /*LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE |*/ LUA_MASKCOUNT, luastate_hook_step);
	
	//printf("state %s with graph %p\n", name, this->G);
	
	lua_pushlightuserdata(L, this);
	lua_setfield(L, LUA_REGISTRYINDEX, LUAAV_SCRIPT_LITERAL);
	lua_pushlightuserdata(L, L);
	lua_setfield(L, LUA_REGISTRYINDEX, LUAAV_ROOTSTATE_LITERAL);
	
	luaav_install_modules(L);
	
	/* fetches debug.traceback to registry */
	lua_getglobal(L, "debug");
	if(!lua_isnil(L, -1)) {
		lua_pushliteral(L, "traceback");
		lua_gettable(L, -2);
		lua_pushliteral(L, LUAAV_DEBUG_TRACEBACK_LITERAL);
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_REGISTRYINDEX);
		lua_pop(L, 2);
	} else {
		lua_pop(L, 1);
	}
	
	lua_newtable(L);
	lua_setglobal(L, "script");
	
	/* need to lock out audio while we it to the list */
	luaav_audio_lock();
	
	states.push_back(this);
	
	/* audio may now continue */
	luaav_audio_unlock();
}

LuaAVState :: ~LuaAVState() {
	
	/* At this point, we need to disable audio processing so we can safely flush & free the audiopq etc. */
	luaav_audio_lock();
	
	states.remove(this);
	
	/* now safe to unlock the audio process */
	luaav_audio_unlock();
	
	lua_getglobal(L, "onclose");
	if (lua_isfunction(L, -1)) {
		lua_pcall(L, 0, 0, 0);
	}
	
	/* stop recording if we were recording */
	if (recording) {
		recording = false;
		recorderThread.wait();
	}
	
	/* triggers __gc in any active userdata */
	lua_close(L);
	
	/* At this point, we need to disable audio processing so we can safely flush & free the audiopq etc. */
	luaav_audio_lock();
	
	/* problem: some of these queues may have important messages in them, e.g. to free memory */
	/* OTOH, we can't just call all the messages, because they might try to schedule more messages... */
//	al_pq_free(&audiopq, 0);
	al_pq_free(&mainpq, 0);
	
//	al_tube_free(&inbox);
//	al_tube_free(&outbox);
	
	/* also automatically destroys any child pools */
//	apr_pool_destroy(main_pool);
//	apr_pool_destroy(audio_pool);
	
	/* now safe to unlock the audio process */
	luaav_audio_unlock();
}

void LuaAVState :: audioTick() {
	
	// run the audio system:
	prepare(0, LUAAV_AUDIO_BLOCKSIZE);
	audioCB();
	elapsed = elapsed + LUAAV_AUDIO_BLOCKSIZE;
	
	// record audio:
	if (recording) {
		for (int i=0; i<LUAAV_AUDIO_BLOCKSIZE; i++) {
			recorder.write(outsignals[0][i], outsignals[1][i], 0);
		}
	}
	
	// scope audio:
	if (scoping) {
		for (int i=0; i<LUAAV_AUDIO_BLOCKSIZE; i++) {
			scope.write(outsignals[0][i], outsignals[1][i]);
			scope.advance(1);
		}
	}
}

void LuaAVState :: record(bool start, std::string filename) {
	if (start) {
		if (!recording) {
			recording = true;
			recorderFileName = filename;
			recorderThread.start(stateRecordingThreadFunction, this);
		}
	} else {
		recording = false;
	}
}

void * stateRecordingThreadFunction(void * userData) {
	LuaAVState * S = (LuaAVState *)userData;
	
	//printf("started recording\n");
	
	// create & open soundfile:
	gam::SoundFile sf(S->recorderFileName);
	sf.format(gam::SoundFile::WAV);
	sf.encoding(gam::SoundFile::FLOAT);
	sf.channels(2);
	sf.frameRate(luaav_audio_config_current().samplerate);
	sf.openWrite();
	
	while (S->recording) {
		
		// grab frames from the recorder:
		float * buf;
		int frames = S->recorder.read(buf);
		if (frames) {
			//printf("frames %i\n", frames);
			// write to soundfile:
			sf.write(buf, frames);
		}
		
		al_sleep(0.01);	// what is a good amount?
	}
	
	sf.close();
	return NULL;
}

#pragma mark Modules

static std::map<std::string, lua_CFunction> criticalmodules;
static std::map<std::string, lua_CFunction> mainmodules;
static std::map<std::string, lua_CFunction> optmodules;

void luaav_add_critical_module(const char * name, lua_CFunction func) {
	criticalmodules[name] = func;
}

void luaav_add_main_module(const char * name, lua_CFunction func) {
	mainmodules[name] = func;
}

void luaav_add_opt_module(const char * name, lua_CFunction func) {	
	optmodules[name] = func;
}

void luaav_install_modules(lua_State * L) {

	std::map<std::string, lua_CFunction>::iterator it;
	
	// critical libs:
	for (it = criticalmodules.begin(); it != criticalmodules.end(); it++) {
		const char * name = it->first.c_str();
		lua_CFunction func = it->second;
		lua_pushcfunction(L, func);
		lua_pushstring(L, name);
		if (lua_pcall(L, 1, 1, 0)) {
			luaav_error_msg("loadlib error: %s", lua_tostring(L, -1));
		}
		lua_settop(L, 0);		
	}
	lua_settop(L, 0);
	
	// main libs:
	for (it = mainmodules.begin(); it != mainmodules.end(); it++) {
		const char * name = it->first.c_str();
		lua_CFunction func = it->second;
		lua_pushcfunction(L, func);
		lua_pushstring(L, name);
		if (lua_pcall(L, 1, 1, 0)) {
			luaav_error_msg("loadlib error: %s", lua_tostring(L, -1));
		}
		lua_settop(L, 0);		
	}
	lua_settop(L, 0);
	
	// opt libs:
	for (it = optmodules.begin(); it != optmodules.end(); it++) {
		const char * name = it->first.c_str();
		lua_CFunction func = it->second;
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "preload");
		lua_pushcfunction(L, func);
		lua_setfield(L, -2, name);
		lua_settop(L, 0);	
	}
	lua_settop(L, 0);
	
}

#pragma mark C API

void luaav_init(int argc, char ** argv) {
	if (initialized) return;
	initialized = true;

	/* LuaAV uses Apache Portable Runtime */
	check_apr(apr_initialize());
	check_apr(apr_pool_initialize());
	check_apr(apr_pool_create(&luaav_main_pool, NULL));
	check_apr(apr_pool_create(&luaav_audio_pool, NULL));
	check_apr(apr_pool_create(&luaav_audio_block_pool[0], luaav_audio_pool));
	check_apr(apr_pool_create(&luaav_audio_block_pool[1], luaav_audio_pool));
	check_apr(apr_thread_mutex_create(&luaav_audio_mutex, APR_THREAD_MUTEX_DEFAULT, luaav_main_pool));
	
//	luaav_inbox = al_tube_create(8);
//	luaav_outbox = al_tube_create(8);
	
	/* configure audio system */
	luaav_audio_configure(default_audio_config);
	
	/* libluaav modules only here */
	
	/* standard lua modules */
	luaav_add_critical_module( LUA_COLIBNAME, luaopen_base );
	luaav_add_critical_module( LUA_TABLIBNAME, luaopen_table );
	luaav_add_critical_module( LUA_IOLIBNAME, luaopen_io );
	luaav_add_critical_module( LUA_OSLIBNAME, luaopen_os );
	luaav_add_critical_module( LUA_STRLIBNAME, luaopen_string );
	luaav_add_critical_module( LUA_MATHLIBNAME, luaopen_math );
	luaav_add_critical_module( LUA_LOADLIBNAME, luaopen_package );
	luaav_add_critical_module( LUA_DBLIBNAME, luaopen_debug );
#if defined(LUAAV_USE_LUAJIT) 
	// luajit:
//	luaav_add_main_module( LUA_JITLIBNAME, luaopen_jit );
//	luaav_add_opt_module( LUA_BITLIBNAME, luaopen_bit ); 
#endif	
	/* LuaAV modules */
	luaav_add_main_module( "LuaAV", luaopen_luaav );
	luaav_add_main_module( "LuaAV.time", luaopen_luaav_time );
	// massive memory leaks if this isn't auto-included
	luaav_add_main_module( "Array", luaopen_Array );
	luaav_add_opt_module( "LuaAV.debug", luaopen_luaav_debug );
	luaav_add_opt_module( "audio.soundfile", luaopen_audio_soundfile );
	luaav_add_opt_module( "audio.Reverb", luaopen_audio_Reverb);
	// more modules
	luaav_add_main_module( "audio", luaopen_audio );
	luaav_add_opt_module( "audio.debug", luaopen_audio_debug );
//	luaav_add_opt_module( "audio.synth", luaopen_audio_synth );
//	luaav_add_opt_module( "audio.std", luaopen_audio_std );
	
	luaav_add_opt_module( "lfs", luaopen_lfs );
	luaav_add_opt_module( "lpeg", luaopen_lpeg );
	luaav_add_opt_module( "listlpeg", luaopen_listlpeg );
	luaav_add_opt_module( "midi", luaopen_midi );
	luaav_add_opt_module( "osc", luaopen_osc );
}

void luaav_quit() {
	if (!initialized) return;
	initialized = false;
	
	// ensure audio thread is not active:
	check_apr(apr_thread_mutex_lock(luaav_audio_mutex));
	
	// free busses
	for (int i=0; i<luaav_audio_inputs.size(); i++) {
		luaav_audiobuffer_release(luaav_audio_inputs[i]);
	}
	for (int i=0; i<luaav_audio_outputs.size(); i++) {
		luaav_audiobuffer_release(luaav_audio_outputs[i]);
	}
	
//	al_tube_free(&luaav_inbox);
//	al_tube_free(&luaav_outbox);
	
	// free apr:
	apr_thread_mutex_destroy(luaav_audio_mutex);
	apr_pool_destroy(luaav_main_pool);
	apr_pool_destroy(luaav_audio_pool);
	apr_pool_terminate();
	apr_terminate();
	
	check_apr(apr_thread_mutex_unlock(luaav_audio_mutex));
}


void luaav_main_tick() {
	if (!initialized) return;
	
	
	/* read real-time (used to test for latency slops) */
	al_sec rt = al_time();
	
	/* execute any global call received from the audio thread */
	//al_tube_reader_advance(luaav_outbox, rt);
	luaav_outbox.executeUntil(rt);
	
	std::list<LuaAVState *>::iterator it = states.begin();
	bool iterating = true;
	while (iterating && it != states.end()) {
		LuaAVState * S = *it;
		
		/* 
			figure out our desired target time
				- note that the main thread here is in the future with respect to the audio thread, 
				but that it *follows* the audio thread's actual performance.
		*/
		samplestamp before = S->elapsed;
		al_sec target = (before / audio_config.samplerate) + S->audio_latency;
		
		/* 
			handle any events to the current timestamp in the main priority queue: 
				(first, transfer in any incoming tube messages)
		*/
//		al_tube_pq_transfer(S->outbox, S->mainpq, target);
		al_pq_update(S->mainpq, target, S->defer_state);
		
		luastate_hook_count = 0;
		
		S->outbox.executeUntil(target);	
		
		/*
			if main thread took too long, 
				defer non-time-critical events in the next tick()
		*/
		al_sec dur = (S->elapsed - before) / audio_config.samplerate;
		S->defer_state = dur > S->audio_latency;
		if (S->defer_state) ddebug("------ scheduler slip %fs ------\n", dur - S->audio_latency);
		
		if (it != states.end()) {
			it++;
		} else {
			iterating = false;
		}
		
	}
}

void luaav_audio_tick() {
	if (!initialized) {
		printf("audio not initialized yet\n");
		return;
	}
	
	/* try to lock audio, but don't wait if you can't */
	if (APR_STATUS_IS_EBUSY(apr_thread_mutex_trylock(luaav_audio_mutex))) {
		printf("audio locked\n");
		return;
	}
	//luaav_audio_config cfg = luaav_audio_config_current();
	
	/* execute any global call received from the main thread */
	audio_time += LUAAV_AUDIO_BLOCKSIZE;
	//al_tube_reader_advance(luaav_inbox, audio_time / audio_config.samplerate);
	al_sec at = audio_time / audio_config.samplerate;
	luaav_inbox.executeUntil(at);
	
	std::list<LuaAVState *>::iterator it = states.begin();
	while (it != states.end()) {
		LuaAVState * S = *it++;
		
		//const samplestamp elapsed = S->elapsed;
		
		for (int ch=0; ch < audio_config.inchannels; ch++) {
			//S->inbusses[ch].set(luaav_audio_input(ch));
			S->insignals[ch].set(luaav_audio_input(ch));
		}
		for (int ch=0; ch < audio_config.outchannels; ch++) {
			//S->outbusses[ch].set(luaav_audio_output(ch));
			S->outsignals[ch].set(luaav_audio_output(ch));
		}
		
		S->audioTick();
		
		
	}
	
	//if (luaav_audio_output(0)[0] == 0.) printf("zero\n");
	
	/* 
		mark memory allocated until end of this block (luaav_audio_block_pool[0]) as reusable  
		swap pointers in block pools for next block	
	*/
	apr_pool_clear(luaav_audio_block_pool[0]);
	apr_pool_t * const tmp = luaav_audio_block_pool[0];
	luaav_audio_block_pool[0] = luaav_audio_block_pool[1];
	luaav_audio_block_pool[1] = tmp;
		
	/* unlock audio - lets any main thread configuration calls happen only between audio_tick calls */
	check_apr(apr_thread_mutex_unlock(luaav_audio_mutex));
}

void * luaav_palloc(size_t size) {
	return apr_palloc(luaav_main_pool, size);
}

//al::MsgTube& luaav_global_inbox() { return luaav_inbox; }
al::MsgTube& luaav_global_outbox() { return luaav_outbox; }

luaav_state * luaav_state_create(const char * name, void * userdata) {
	return new LuaAVState(name, userdata);
}

void luaav_state_close(luaav_state ** S) {
	if (S && *S) {
		delete *S;
		*S = NULL;
	}
}

lua_State * luaav_getstate(luaav_state * S) {
	return S->L;
}

lua_State * luaav_getrootstate(lua_State * L) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_ROOTSTATE_LITERAL);
	lua_State * T = (lua_State * )lua_touserdata(L, -1);
	lua_pop(L, 1);
	return T;
}

luaav_state * luaav_fromstate(lua_State * L) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_SCRIPT_LITERAL);
	luaav_state * S = (luaav_state *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	return S;
}

pq luaav_state_mainpq(luaav_state * S) {
	return S->mainpq;
}

pq luaav_script_mainpq(lua_State * L) {
	return luaav_fromstate(L)->mainpq;
}

al_sec luaav_state_now(luaav_state * S) {
	return S->mainpq->now;
}

al_sec luaav_state_reallatency(luaav_state * S) {
	return ((S->elapsed / audio_config.samplerate) + S->audio_latency) - S->mainpq->now;
}

al_sec luaav_state_latency(luaav_state * S) {
	return S->audio_latency;
}

void * luaav_state_userdata(luaav_state * S) {
	return S->userdata;
}

void * luaav_state_palloc(luaav_state * S, size_t size) {
	return S->main_pool.alloc(size); //apr_palloc(S->main_pool, size);
}

void luaav_state_gocall(lua_State * L, int nargs) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_GO_LITERAL);
	nargs++;
	lua_insert(L, lua_gettop(L)-nargs);
	lua_call(L, nargs, 1);
}

int luaav_state_loadfile(luaav_state * S, const char * filename, const char * filepath) {
	char fullpath[1024];
	sprintf(fullpath, "%s/%s", filepath, filename);	
	
	lua_getglobal(S->L, "script");
	lua_pushfstring(S->L, "%s", filepath); lua_setfield(S->L, -2, "path");
	lua_pushfstring(S->L, "%s", filename); lua_setfield(S->L, -2, "name");
	lua_pop(S->L, 1);
	
	return luaL_loadfile(S->L, fullpath);
}

int luaav_state_dofile(luaav_state * S, const char * filename, const char * filepath) {
	if (luaav_state_loadfile(S, filename, filepath)) {
		luaav_error_msg("luaL_loadfile error: %s\n", lua_tostring(S->L, -1));
		return 1;
	}
	if(lua::pcall(S->L, 0)) {
		const char *err = lua_tostring(S->L, -1);
		//luaL_error(S->L, "dofile error: %s", err);
		luaav_error_msg("error: %s\n", err);
		return 1;
	}
	return 0;
}

int luaav_state_gofile(luaav_state * S, const char * filename, const char * filepath) {
	if (luaav_state_loadfile(S, filename, filepath)) {
		luaav_error_msg("luaL_loadfile error: %s\n", lua_tostring(S->L, -1));
		return 1;
	}
	luaav_state_gocall(S->L, 0);
	lua_pop(S->L, 1); // don't care about returned coro
	return 0;
}

int luaav_state_dostring(luaav_state * S, const char * source, const char * sourcename) {
	char name[1024];
	snprintf(name, 1024, "=%s", sourcename);
	lua_getfield(S->L, LUA_REGISTRYINDEX, LUAAV_DEBUG_TRACEBACK_LITERAL);
	if (luaL_loadbuffer(S->L, source, strlen(source), name) || lua_pcall(S->L, 0, LUA_MULTRET, -2)) {
		luaav_error_msg("luaL_loadbuffer error: %s\n", lua_tostring(S->L, -1));
		return 1;
	}
	lua_remove(S->L, 1); // remove LUAAV_DEBUG_TRACEBACK_LITERAL
	return 0;
}

int luaav_state_gostring(luaav_state * S, const char * source, const char * sourcename) {
	char name[1024];
	snprintf(name, 1024, "=%s", sourcename);
	lua_getfield(S->L, LUA_REGISTRYINDEX, LUAAV_DEBUG_TRACEBACK_LITERAL);
	if (luaL_loadbuffer(S->L, source, strlen(source), name)) {
		luaav_error_msg("luaL_loadbuffer error: %s\n", lua_tostring(S->L, -1));
		return 1;
	}
	luaav_state_gocall(S->L, 0);
	lua_pop(S->L, 1); // don't care about returned coro
	return 0;
}

bool luaav_rects_are_equal(luaav_rect &r1, luaav_rect &r2) {
	return	r1.x == r2.x && 
			r1.y == r2.y &&
			r1.width == r2.width && 
			r1.height == r2.height;
}

bool luaav_within_rect(luaav_pt &p, luaav_rect &rect) {
	return	rect.x <= p.x && 
			(rect.x + rect.width) > p.x && 
			rect.y <= p.y && 
			(rect.y + rect.height) > p.y;
}

#pragma mark Audio API

luaav_audio_config luaav_audio_config_current() {
	return audio_config;
}
luaav_audio_config luaav_audio_config_default() {
	return default_audio_config;
}
double luaav_audio_samplerate_current() {
	return audio_config.samplerate;
}

void luaav_audio_configure(luaav_audio_config config) {
	/* only a multiple of 64 is safe */
	config.blocksize = config.blocksize - (config.blocksize % LUAAV_AUDIO_BLOCKSIZE);
	config.blocksize = config.blocksize > LUAAV_AUDIO_BLOCKSIZE ? config.blocksize : LUAAV_AUDIO_BLOCKSIZE;
	
	audio_config.samplerate = config.samplerate;
	audio_config.blocksize = config.blocksize;
	
	gam::Sync::master().spu(audio_config.samplerate);
	
	/* allocate a silent buffer */
	audio_silentbuffer = (sample *)calloc(LUAAV_AUDIO_BLOCKSIZE, sizeof(sample));
	
	/* we only ever expand the channel count: */
	if (config.inchannels > luaav_audio_inputs.size()) {
		luaav_audio_inputs.resize(config.inchannels);
	}	
	audio_config.inchannels = config.inchannels;
	for (int i=0; i<luaav_audio_inputs.size(); i++) {
		if (luaav_audio_inputs[i] == 0) {	
			luaav_audio_inputs[i] = luaav_audiobuffer_request();
			luaav_audiobuffer_zero(luaav_audio_inputs[i]);
		}
	}
	
	/* we only ever expand the channel count: */
	if (config.outchannels > luaav_audio_outputs.size()) {
		luaav_audio_outputs.resize(config.outchannels);
	}	
	audio_config.outchannels = config.outchannels;
	for (int i=0; i<luaav_audio_outputs.size(); i++) {
		if (luaav_audio_outputs[i] == 0) {	
			luaav_audio_outputs[i] = luaav_audiobuffer_request();
			luaav_audiobuffer_zero(luaav_audio_outputs[i]);
		}
	}
	
	/* recreate the fade buffers */
	if (audio_fadein) free(audio_fadein);
	audio_fadein = (sample *)malloc(audio_config.blocksize * sizeof(sample));
	if (audio_fadeout) free(audio_fadeout);
	audio_fadeout = (sample *)malloc(audio_config.blocksize * sizeof(sample));
	
	for (int i=0; i<audio_config.blocksize; i++) {
		audio_fadein[i] = ((sample)i)/((sample)audio_config.blocksize);
		audio_fadeout[i] = 1.f - audio_fadein[i];
	}
	
}

sample * luaav_audio_input(int channel) {
	return luaav_audio_inputs[channel];
}

sample * luaav_audio_output(int channel) {
	return luaav_audio_outputs[channel];
}

const sample * luaav_audio_driver_output(int channel) {
	return luaav_audio_outputs[channel];
}

sample * luaav_audio_driver_input(int channel) {
	return luaav_audio_inputs[channel];
}

sample * luaav_audio_silentbuffer() {
	return audio_silentbuffer;
}

sample * luaav_fadein() {
	return audio_fadein;
}
sample * luaav_fadeout() {
	return audio_fadeout;
}

samplestamp luaav_audio_elapsed(luaav_state * S) {
	return S->elapsed;
}

void * luaav_audio_palloc(size_t size) {
	return apr_palloc(luaav_audio_pool, size);
}

void * luaav_audio_state_palloc(luaav_state * S, size_t size) {
	return S->audio_pool.alloc(size); //apr_palloc(S->audio_pool, size);
}

void * luaav_audio_balloc(size_t size, int next) {
	return apr_palloc(luaav_audio_block_pool[next != 0], size);
}

void luaav_audio_lock() {
	check_apr(apr_thread_mutex_lock(luaav_audio_mutex));
}
void luaav_audio_unlock() {
	check_apr(apr_thread_mutex_unlock(luaav_audio_mutex));
}

#pragma mark Lua API

static int child_gc(lua_State * L) {	
	printf_gcdebug("LuaAVState child_gc\n");
	LuaAVState * S = (LuaAVState *)luaL_checkudata(L, 1, LUAAV_SCRIPT_META_LITERAL);
	lua_pushnil(L);
	lua_setmetatable(L, 1);
	//printf("gc of child LuaAVState %p\n", S);
	if (S) {
		S->~LuaAVState();	// explicit destructor to match placement new
	}
	return 0;
}

static int child_tostring(lua_State * L) {
	lua_pushfstring(L, "LuaAVState %p", luaL_checkudata(L, 1, LUAAV_SCRIPT_META_LITERAL));
	return 1;
}

static int child_toptr(lua_State * L) {
	lua_pushlightuserdata(L, luaL_checkudata(L, 1, LUAAV_SCRIPT_META_LITERAL));
	return 1;
}

static int child_dofile(lua_State * L) {
	LuaAVState * S = (LuaAVState *)luaL_checkudata(L, 1, LUAAV_SCRIPT_META_LITERAL);
	const char * filename = luaL_checkstring(L, 2);
	const char * filepath = luaL_optstring(L, 3, "");
	
	if (luaav_state_loadfile(S, filename, filepath)) {
		luaL_error(L, "luaL_loadfile error: %s", lua_tostring(S->L, -1));
	}
	int nargs = lua::copy_values(L, S->L, 4);
		
	luaav_state_gocall(S->L, nargs);
	lua_pop(S->L, 1);	// don't care about the returned coroutine
	return 0;
}

static int child_dostring(lua_State * L) {
	LuaAVState * S = (LuaAVState *)luaL_checkudata(L, 1, LUAAV_SCRIPT_META_LITERAL);
	const char * code = luaL_checkstring(L, 2);	
	int top = lua_gettop(S->L);
	luaav_state_dostring(S, code, "child_dostring");
	return lua::copy_values(S->L, L, top+1);
}

static int child_gostring(lua_State * L) {
	LuaAVState * S = (LuaAVState *)luaL_checkudata(L, 1, LUAAV_SCRIPT_META_LITERAL);
	const char * code = luaL_checkstring(L, 2);
	
	if (luaL_loadstring(S->L, code)) {
		luaL_error(L, "luaL_loadstring error: %s", lua_tostring(S->L, -1));
	}
	int nargs = lua::copy_values(L, S->L, 3);
	
	luaav_state_gocall(S->L, nargs);
	lua_pop(S->L, 1);	// don't care about the returned coroutine
	
	return 0;
}

static int child_new(lua_State * L) {	
	const char * name = luaL_checkstring(L, 1);
	
	LuaAVState * P = luaav_fromstate(L);
	
	/* create new LuaAVState as a userdata in the parent state */
	LuaAVState * S = (LuaAVState *)lua_newuserdata(L, sizeof(LuaAVState));
	new (S) LuaAVState(name, P->userdata);	
	luaL_getmetatable(L, LUAAV_SCRIPT_META_LITERAL);
	lua_setmetatable(L, -2);

	return 1;
}

//static int luaav_debug_inboxevents(lua_State * L) {
//	luaav_state * S = luaav_fromstate(L);
//	lua_pushnumber(L, al_tube_used(luaav_state_inbox(S)));
//	return 1;
//}
//
//static int luaav_debug_outboxevents(lua_State * L) {
//	luaav_state * S = luaav_fromstate(L);
//	lua_pushnumber(L, al_tube_used(luaav_state_outbox(S)));
//	return 1;
//}

static int luaav_debug_mainevents(lua_State * L) {
	luaav_state * S = luaav_fromstate(L);
	lua_pushnumber(L, al_pq_used(luaav_state_mainpq(S)));
	return 1;
}

static int luaav_debug_elapsed(lua_State * L) {
	luaav_state * S = luaav_fromstate(L);
	const luaav_audio_config audio_config = luaav_audio_config_current();
	lua_pushnumber(L, (S->elapsed / audio_config.samplerate) + S->audio_latency);
	return 1;
}

/* inspect the mainpq */
static int luaav_debug_scheduler(lua_State * L) {
	luaav_state * S = luaav_fromstate(L);
	pq x = S->mainpq;
	lua_createtable(L, x->len, 0);
	int i = 1;
	pq_msg m = al_pq_top(x);
	while (m) {
		lua_newtable(L);
		// TODO: what do you want to know about the task?
		lua_pushnumber(L, m->msg.t);
		lua_setfield(L, -2, "t");
		
		lua_pushlightuserdata(L, (void *)m->msg.func);
		lua_setfield(L, -2, "coro");
		
		lua_pushlightuserdata(L, m->msg.mem);
		lua_setfield(L, -2, "data");
		
		lua_rawseti(L, -2, i++);
		m = m->next;
	}
	return 1;
}

/* for debugging purposes only */
static int luaav_debug_sleep(lua_State * L) {
	al_sleep(luaL_optnumber(L, 1, 1.));
	return 0;
}

int luaopen_luaav_debug(lua_State * L) {
	const char * libname = lua_tostring(L, -1);
	
	struct luaL_reg lib[] = {
		{"sleep", luaav_debug_sleep },
		{"elapsed", luaav_debug_elapsed },
		{"mainevents", luaav_debug_mainevents },
//		{"inboxevents", luaav_debug_inboxevents },
//		{"outboxevents", luaav_debug_outboxevents },
		{"scheduler", luaav_debug_scheduler },
		{ NULL, NULL },
	};
	luaL_register(L, libname, lib);
	
	return 1;
}

int luaav_version(lua_State *L) {
	lua_pushstring(L, luaav_revision());
	return 1;
}

int luaav_error(lua_State *L) {
	luaav_error_msg(lua_tostring(L, 1));
	return 0;
}

int luaopen_luaav(lua_State * L) {
	const char * libname = lua_tostring(L, -1);
	
	/* define LuaAVState metatable (for child instances) */
	struct luaL_reg methods[] = {
		{ "__tostring", child_tostring },
		{ "__gc", child_gc },
		{ "close", child_gc },
		{ "toptr", child_toptr },
		
		{ "dofile", child_dofile },
		{ "dostring", child_dostring },
		{ "gostring", child_gostring },
		
		{ NULL, NULL },
	};
	luaL_newmetatable(L, LUAAV_SCRIPT_META_LITERAL);
	lua_pushvalue(L, -1);	
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, methods);
	
	struct luaL_reg lib[] = {
		{ "new", child_new },
		{ "version", luaav_version },
		{ "error", luaav_error },
		{ NULL, NULL },
	};
	luaL_register(L, libname, lib);
	
	lua_pushstring(L, LUA_AV_OS_NAME); 
	lua_setfield(L, -2, "platform");

	return 1;
}
