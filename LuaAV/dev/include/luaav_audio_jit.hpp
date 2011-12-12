#ifndef INCLUDE_LUAAV_AUDIO_JIT_H
#define INCLUDE_LUAAV_AUDIO_JIT_H 1

#include "lua.hpp"

/*
	This header kept as simple as possible to ensure fast JIT
*/

typedef double sample;

/*
	Internally, blocks are processed at a fixed number of frames. 
	Driver IO will typically be an integer multiple of this.
*/	
#define LUAAV_AUDIO_BLOCKSIZE (64)
#define LUAAV_AUDIO_BLOCKWRAP (LUAAV_AUDIO_BLOCKSIZE-1)
#define LUAAV_AUDIO_MAXIOCHANNELS (16)

// forward declarations:
class Context;
namespace al { class JIT; }

struct Synth {
	typedef void (*callback)(Synth *);
	
	struct Channel;
	
	enum Flags {
		SYNTH_NONE = 0x00,
		SYNTH_HAS_ENV = 0x01,
		SYNTH_ENV_IS_RELEASED = 0x02,
		SYNTH_OWNS_OUTS = 0x04
	};

	enum State {
		CREATED = 0,	// initial state when allocated
		PLAYING,		
		STOPPING,		// used by audio thread only
		STOPPED,
		DEAD
	};
	
	Synth(int numchannels, Flags flags = SYNTH_NONE);
	~Synth();
	
	void init(lua_State * L);
	void setprototype(lua_State * L);
	//void portinit(lua_State * L, const char * name, int offset, int channels);
	void play();
	void stop();
	void close();
	
	///! reference counting for Lua
	void retain() { mLuaRefs++; }
	bool release();
	
	///! testing bitflags:
	bool hasEnv() { return mFlags & SYNTH_HAS_ENV; }
	void envRelease() { 
		mFlags = (Flags)(mFlags | Synth::SYNTH_ENV_IS_RELEASED); 
	}
	bool envIsReleased() { return mFlags & SYNTH_ENV_IS_RELEASED; }
	
	bool ownsOuts() { return mFlags & SYNTH_OWNS_OUTS; }
	
	
	///! return true if setting data on this node makes any sense
	bool valid() { return mStateL < STOPPED && mStateA < STOPPED; }
	
	// *** AUDIO THREAD METHODS: *** //
	
	// called prior to each audioCB:
	void prepare(int start=0, int end=LUAAV_AUDIO_BLOCKSIZE) { 
		mStart = start; 
		mEnd = end; 
	}
	
	///! Signal this node to stop synthesizing audio
	void audio_stop() { if (mStateA == PLAYING) { mStateA = STOPPING; } }
	
	callback cbaudio;
	callback cbfree;
	int mStart, mEnd;
	int mLuaRefs;
	Context * mContext;
	State mStateL, mStateA;
	Flags mFlags;
	int mNumChannels;
	Channel * mChannels;
	al::JIT * mJit;
};

void luaav_synth_newmetatable(lua_State * L, luaL_reg lib[], luaL_reg getters[], luaL_reg setters[]);
Synth * luaav_synth_to(lua_State * L, int idx);
Synth * luaav_synth_checkto(lua_State * L, int idx);
int luaav_synth_portset(lua_State * L, int offset, int channels);

template<typename T>
struct SynthBinding {

	static void audioCB(Synth * n) { 
		((T *)n)->audioCB(); 
	}
	
	static void audioFree(Synth * n) {
		//n->close();
		delete (T *)n; 
	}
	
	// upvalues: ctx, proto, meta, jit
	// args: arg table
	static int create(lua_State * L) {
		T * self = new T(L);
		self->synth.cbaudio = &audioCB;
		self->synth.cbfree = &audioFree;
		self->synth.play();
		return 1;
	}
	
	static T * to(lua_State * L, int idx) { return (T *)luaav_synth_to(L, idx); }
	static T * checkto(lua_State * L, int idx) { return (T *)luaav_synth_checkto(L, idx); }
};



template<int O, int C> 
int portset(lua_State * L) { return luaav_synth_portset(L, O, C); }

struct PortReader {
	PortReader(Synth& synth, int offset);
	sample& operator[](int i) { return ptr[isSignal * i]; }
	
	sample * ptr;
	int isSignal;
};

#endif /* include guard */
