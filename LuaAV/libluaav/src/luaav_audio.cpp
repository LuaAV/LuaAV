#include "luaav_state.hpp"
#include "lua_utility.h"
#include <stack>

void msg_synth_play(al_sec t, Synth * self, Context * ctx);
void msg_synth_stop(al_sec t, Synth * self);
void msg_synth_delete(al_sec t, Synth * self);

void msgAudioReleaseArrayHandle(al_sec t, ArrayHandle handle) {
	handle->release();
}

void msgSetArrayHandle(al_sec t, ArrayHandle * dst, ArrayHandle handle) {
	if (*dst) {
		luaav_global_outbox().send(t, msgAudioReleaseArrayHandle, *dst);
	}
	*dst = handle;
}


static void msg_context_addsignal(al_sec t, Context * self, Signal * u) {
	self->addSignal(u);
}


#define LuaAV_Bus_Metatable "LuaAV_Bus_Metatable"
static int bus_index(lua_State * L) {
	// get num channels
	int len = lua_objlen(L, 1);
	if (!lua_isnumber(L, 2)) 
		luaL_error(L, "bus can only accept integer indices");
	int chan = abs(lua_tointeger(L, 2));
	int n = 1+((chan+len-1) % len);
	lua_rawgeti(L, 1, n);
	return 1;
}

static int bus_newindex(lua_State * L) {
	luaL_error(L, "bus is read only");
	return 0;
}

static int signal_create(lua_State * L, Context * ctx, int chans) {
	LuaAVState * S = luaav_fromstate(L);
	lua_createtable(L, chans, 0);
	for (int i=1; i<=chans; i++) {
		Signal * u = new Signal();
		
		Glue<Signal>::push(L, u); //lua_pushlightuserdata(L, b);
		lua_rawseti(L, -2, i);
		
		// send msg to add bus:
		S->inbox().send(ctx->now(), msg_context_addsignal, ctx, u);
	}
	luaL_getmetatable(L, LuaAV_Bus_Metatable);
	lua_setmetatable(L, -2);
	return 1;
}

#pragma mark Context

template<>
const char * Glue<Context>::mt_name(lua_State * L) {
	return "LuaAV_Context_Metatable";
}

template<> 
const char * Glue<Context>::usr_name() {
	return "Context";
}

template<> 
void Glue<Context> :: usr_push(lua_State * L, Context * self) {
	self->retain();
	//printf("Context push %d\n", self->luarefs);
	
	// context is a stack index 1; 
	lua_newtable(L);
		if (lua_gettop(L) > 2) {
			// inherit parent:
			lua_pushvalue(L, 1);
			lua_setfield(L, -2, "_parent");
		}
		lua_newtable(L);
		lua_setfield(L, -2, "_busses");
	lua_setfenv(L, -2);
}

template<> 
void Glue<Context> :: usr_gc(lua_State * L, Context * self) { 
	LuaAVState * S = luaav_fromstate(L);
	
	// clear metatable:
	lua_pushnil(L);
	lua_setmetatable(L, 1);
	
	//printf("Context __gc %d--\n", self->luarefs);
	
	// if Lua no longer references the Node, trigger the stopping of this node:
	if (self->release()) {
		//printf("releasing Context\n");
		S->inbox().send(self->now(), msg_synth_stop, (Synth *)self);
	}
}

template<> bool Glue<Context>::usr_has_index() { return true; }

template<> 
void Glue<Context>::usr_index(lua_State * L, Context * u) {
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2);
		lua_getfenv(L, 1);
		lua_getfield(L, -1, "_busses");
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if(lua_isnil(L, -1)) {
			lua_pop(L, 2);
			lua_getfield(L, -1, "_parent");
			if(!lua_isnil(L, -1)) {
				lua_pushvalue(L, 2);
				lua_gettable(L, -2);
			}	
		}
	}
}

template<> 
void Glue<Context>::usr_newindex(lua_State * L, Context * u) {	
	int chans = luaL_checknumber(L, 3);
	lua_getfenv(L, 1);
	lua_getfield(L, -1, "_busses");
	int busses = lua_gettop(L);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	if (!lua_isnoneornil(L, -1)) {
		luaL_error(L, "cannot redefine existing %s", lua_tostring(L, 2));
	}
	lua_settop(L, busses);
	
	signal_create(L, u, chans);
	
	// set in fenv:
	lua_pushvalue(L, 2);	// key
	lua_pushvalue(L, -2);	// Bus
	lua_rawset(L, busses);
}

template<> Context * Glue<Context>::usr_new(lua_State * L) {
	Context * parent = Glue<Context>::to(L, 1);
	std::string name(luaL_optstring(L, 2, "LContext"));
	if (parent) {
		Context * c = new Context(parent, name);
		parent->inbox().send(parent->now(), msg_synth_play, (Synth *)c, parent);
		return c;
	} else {
		return NULL;
	}

}

template<> void Glue<Context>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "stop", Glue<Context>::gc },
		{ "close", Glue<Context>::gc },
		{ NULL, NULL }
	};
	for(int i=0; methods[i].name; i++) {
		lua_pushcfunction(L, methods[i].func);
		lua_setfield(L, -2, methods[i].name);
	}
}


Context * context_fromstate(lua_State * L, int idx) {
	Context * c = Glue<Context>::to(L, idx);
	if (!c) {
		c = luaav_fromstate(L);
	}
	return c;
}

Context :: ~Context() {
	// release nodes:
	for (std::list<Synth *>::iterator it=mSynths.begin(); it!=mSynths.end(); it++) {
		Synth * n = *it;
		n->mStateL = n->mStateA = Synth::DEAD;
		msg_synth_delete(0, n);
	}
	
	// release busses:
	for (int i=0; i<mSignals.size(); i++) {
		//printf("release bus %d\n", i);
		mSignals[i]->release();	// except for the main IO signals that is...
	}
}

void Context :: audioCB() {	

	// clear signals:
	for (int i=0; i<mSignals.size(); i++) {
		mSignals[i]->zero();
	}
	
	// executing incoming messages:
	luaav_audio_config cfg = luaav_audio_config_current();
	al_sec until = (al_sec)(root()->elapsed + LUAAV_AUDIO_BLOCKSIZE) / cfg.samplerate;
	inbox().executeUntil(until);
	
	// execute sub-nodes:
	// todo: use a schedule-list & flush, rather than pushing stack... 
	for (std::list<Synth *>::iterator it=mSynths.begin(); it!=mSynths.end();) {
		Synth& n = **it;
		
		if (!(n.mStateA == Synth::PLAYING || n.mStateA == Synth::STOPPING)) {
			printf("ERROR: node should not be here\n");
			continue;
		}
		
		int s = MAX(n.mStart, mStart);
		int e = MIN(n.mEnd, mEnd);
		n.prepare(s, e);
		n.cbaudio(&n);
		
		if (n.mStateA == Synth::STOPPING) {
			//printf("(audio) stopped playing node %p\n", &n);
			it = mSynths.erase(it);
			n.mContext = NULL;
			if (n.mLuaRefs == 0) {
				// delete immediately:
				n.mStateA = n.mStateL = Synth::DEAD;
				// send message to main thread:
				luaav_global_outbox().send(0, msg_synth_delete, &n);
			} else {
				n.mStateA = Synth::STOPPED;
			}
		} else {
			n.mStart = 0;
			it++;
		}
	}
}

int node_userdata_initialize(lua_State * L, int node_idx, int params_idx) {
	// grab initialization params:
	lua_pushnil(L);  /* first key */
	while (lua_next(L, params_idx) != 0) {
		lua_pushvalue(L, -2); // extra copy of key for next loop
		lua_insert(L, -2);
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		lua_settable(L, node_idx);
	}
	return 0;
}

int lua_audio_Bus(lua_State * L) {
	if (!lua_isuserdata(L, 1)) {
		lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_AUDIO_ROOT); 
		lua_insert(L, 1);
	}

	Context * ctx = Glue<Context>::to(L, 1);
	luaL_checkstring(L, 2);
	if (!lua_isnumber(L, 3)) {
		lua_settop(L, 2);
		lua_pushnumber(L, 1);
	}
	
	Glue<Context>::usr_newindex(L, ctx);
	return 1;
}

int latency(lua_State * L) {
	luaav_state * S = luaav_fromstate(L);
	if (lua_isnumber(L, 1))	{
		// change latency. Should be safe to do from main thread?
		S->audio_latency = fabs(lua_tonumber(L, 1));
	}
	lua_pushnumber(L, S->audio_latency);
	return 1;
}

int samplerate(lua_State * L) {
	const luaav_audio_config cfg = luaav_audio_config_current();
	lua_pushnumber(L, cfg.samplerate);
	return 1;
}

int lua_audio_record(lua_State * L) {
	LuaAVState * S = luaav_fromstate(L);
	bool recording = lua_toboolean(L, 1);
	if (recording) {
		// or an optional string?
		const char * str = luaL_checkstring(L, 2);
		S->record(true, str);
	} else {
		S->record(false);
	}

	return 0;
}

int lua_audio_scope(lua_State * L) {
	LuaAVState * S = luaav_fromstate(L);
	if(lua_isnumber(L, 1)) {
		int cidx = lua::to<int>(L, 1);
		al::ArrayWrapper *c = S->scope.channel(cidx);
		if(c) {
			Glue<al::ArrayWrapper>::push(L, c);
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}
	else if(lua_isboolean(L, 1)) {
		bool enable = lua::to<bool>(L, 1);
		S->scoping = enable;
	}
	else {
		lua::push<int>(L, S->scope.position());
		return 1;
	}

	return 0;
}

extern "C" int luaopen_audio(lua_State * L) {
	const luaav_audio_config cfg = luaav_audio_config_current();
	
	//Glue<Node>::define(L);
	//Glue<Bus1>::define(L);
	Glue<Signal>::define(L);
	Glue<Context>::define(L);
	
	LuaAVState * S = luaav_fromstate(L);
		
	// define bus metatable:
	luaL_newmetatable(L, LuaAV_Bus_Metatable);
		lua_pushcfunction(L, bus_index); lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, bus_newindex); lua_setfield(L, -2, "__newindex");
	lua_pop(L, 1);
	
	struct luaL_reg lib[] = {
		{ "latency", latency },
		{ "samplerate", samplerate },
		{ "Bus", lua_audio_Bus },
		{ "record", lua_audio_record },
		{ "scope", lua_audio_scope },
		{ NULL, NULL },
	};
	luaL_register(L, "audio", lib);
	Glue<Context>::register_ctor(L);
	
//	if (S->A == NULL) {
//		//printf("creating root context\n");
//		S->A = new Context(S, NULL);
//	}
	
	// create a context:
	Glue<Context>::push(L, S);
//		// use it as upvalue for audio_play:
//		lua_pushvalue(L, -1);
//		lua_pushcclosure(L, audio_play, 1);
//		lua_setfield(L, -3, "play");
		
//		// install default output bus:
//		lua_pushnumber(L, cfg.outchannels);
//		lua_setfield(L, -2, "out");
		
		// install pointers to raw IO busses
		lua_getfenv(L, -1);
		lua_getfield(L, -1, "_busses");
			lua_createtable(L, cfg.outchannels, 0);
				for (int i=0; i<cfg.outchannels; i++) {
					//lua_pushlightuserdata(L, (void *)(S->outbusses+i));
					Glue<Signal>::push(L, S->outsignals+i);
					lua_rawseti(L, -2, i+1);
				}
				luaL_getmetatable(L, LuaAV_Bus_Metatable);
				lua_setmetatable(L, -2);
			lua_setfield(L, -2, "out");
			
			lua_createtable(L, cfg.inchannels, 0);
				for (int i=0; i<cfg.inchannels; i++) {
					//lua_pushlightuserdata(L, (void *)(S->inbusses+i));
					Glue<Signal>::push(L, S->insignals+i);
					lua_rawseti(L, -2, i+1);
				}
				luaL_getmetatable(L, LuaAV_Bus_Metatable);
				lua_setmetatable(L, -2);
			lua_setfield(L, -2, "in");
		lua_pop(L, 2);
	lua_pushvalue(L, -1);
	lua_setfield(L, LUA_REGISTRYINDEX, LUAAV_AUDIO_ROOT); 
	lua_setfield(L, -2, "root");
	
	// make sure all 
	
	return 1;
}


#pragma mark Scope

Scope :: Scope(int nchannels, int bufferSize, int nbuffers) 
:	mSamplePos(0),
	mBufferSize(bufferSize),
	mNBuffers(nbuffers),
	mNSamples(bufferSize*nbuffers)
{
	for(int i=0; i < nchannels; i++) {
		al::ArrayWrapper *c = new al::ArrayWrapper;
		c->formatAligned(1, AlloFloat32Ty, bufferSize, nbuffers, 1);
		c->retain();
		mChannels.push_back(c);
	}
}

Scope :: ~Scope() {
	while(mChannels.size() > 0) {
		al::ArrayWrapper *c = mChannels.back();
		mChannels.pop_back();
		c->release();
	}
}
	
void Scope :: nchannels(int n) {
	if(n > mChannels.size()) {
		for(int i=mChannels.size(); i < n; i++) {
			al::ArrayWrapper *c = new al::ArrayWrapper;
			c->formatAligned(1, AlloFloat32Ty, mBufferSize, mNBuffers, 1);
			c->retain();
			mChannels.push_back(c);
		}
	}
	else if(n < mChannels.size()) {
		for(int i=mChannels.size()-1; i >= n; i--) {
			al::ArrayWrapper *c = mChannels.back();
			mChannels.pop_back();
			c->release();
		}
	}
}

void Scope :: bufferSize(int sz) {
	mNSamples = mBufferSize*mNBuffers;
	if(sz != mBufferSize && sz > 0) {
		for(int i=0; i < mChannels.size(); i++) {
			mChannels[i]->formatAligned(1, AlloFloat32Ty, mBufferSize, mNBuffers, 1);
		}
	}
}

void Scope :: advance(int n) {
	mSamplePos = (mSamplePos+n)%mNSamples;
}

/*
void Scope :: record(int channel, int n, float *samples) {
	float *c = ((float *)mChannels[channel]->data.ptr);
	for(int i=0, j=mSamplePos; i < n; j++, i++) {
		j = j%mBufferSize;
		c[j] = samples[i];
	}
}
*/

void Scope :: write(float leftSample, float rightSample) {
	float *left = ((float *)mChannels[0]->data.ptr);
	float *right = ((float *)mChannels[1]->data.ptr);
	left[mSamplePos] = leftSample;
	right[mSamplePos] = rightSample;
}
	



#pragma mark Buffers

static std::stack<sample *> gBuffers;
static int gBuffersAllocated = 0;

sample * luaav_audiobuffer_request() {
	sample * x;
	if (gBuffers.empty()) {
		x = new sample[LUAAV_AUDIO_BLOCKSIZE];
		//x = (sample *)malloc(sizeof(sample) * LUAAV_AUDIO_BLOCKSIZE);
		gBuffersAllocated++;
		//printf("ALLOCATED BUFFER %i\n", gBuffersAllocated);
		luaav_audiobuffer_zero(x);
	} else {
		x = gBuffers.top();
		gBuffers.pop();
	}
	luaav_audiobuffer_zero(x);
	return x;
}

void luaav_audiobuffer_release(sample * x) {
	if (x) {
		//printf("\nRECYCLED BUFFER %i\n", gBuffersAllocated);
		gBuffers.push(x);
	}
}

void luaav_audiobuffer_zero(sample * x) {
	//memset(x, 0, sizeof(sample) * LUAAV_AUDIO_BLOCKSIZE);
	for (int i=0; i<LUAAV_AUDIO_BLOCKSIZE; i++) { x[i] = 0; }
}


// returns the current buffer use count:
int luaav_audio_debug_buffers(lua_State * L) {
	lua_pushnumber(L, gBuffersAllocated);
	lua_pushnumber(L, gBuffers.size());
	return 2;
}

extern "C" int luaopen_audio_debug(lua_State * L) {
	struct luaL_reg lib[] = {
		{"buffers", luaav_audio_debug_buffers },
		{ NULL, NULL },
	};
	luaL_register(L, "audio.debug", lib);
	
	return 1;
}

