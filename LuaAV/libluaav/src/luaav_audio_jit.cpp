
#include "luaav_audio_jit.hpp"


#include "luaav_audio.hpp"

struct Box {
	Synth * ptr;
};

struct Synth::Channel {
	Channel();
	int isSignal;
	sample * ptr;
	sample value;
};

// upvals: ctx, metatable, jit
// args: user arguments (table)
Synth :: Synth(int numchannels, Flags flags) 
:	cbaudio(NULL),
	cbfree(NULL),
	mStart(0), 
	mEnd(LUAAV_AUDIO_BLOCKSIZE), 
	mLuaRefs(0), 
	mStateL(CREATED), 
	mStateA(CREATED),
	mFlags(flags),
	mContext(NULL),
	mNumChannels(numchannels),
	mChannels(NULL),
	mJit(NULL)
{
	mChannels = new Channel[mNumChannels];
}

Synth :: ~Synth() {
	delete[] mChannels;
	if (mJit) mJit->release();
}

Synth::Channel :: Channel() : value(0), ptr(&value), isSignal(0) {}
	
void Synth :: init(lua_State * L) {
	static const int ctxIdx = lua_upvalueindex(1);
	static const int protoIdx = lua_upvalueindex(2);
	static const int metaIdx = lua_upvalueindex(3);
	static const int jitIdx = lua_upvalueindex(4);
	
	if (lua_gettop(L) == 0) {
		lua_newtable(L);
	} else {
		luaL_checktype(L, 1, LUA_TTABLE);		// parameters
	}
	
	mContext = Glue<Context>::to(L, 2);
	if (mContext == NULL) { 
		lua_settop(L, 1);
		lua_pushvalue(L, ctxIdx);	
		mContext = Glue<Context>::to(L, 2);
	}
	
	// jit retain
	mJit = Glue<JIT>::checkto(L, jitIdx);
	if (mJit) mJit->retain();
		
	Box * udata = (Box *)lua_newuserdata(L, sizeof(Box));
	int udIdx = lua_gettop(L);
	udata->ptr = this;
	retain(); 	// one reference in Lua
	lua_pushvalue(L, metaIdx);
	lua_setmetatable(L, udIdx);
	
	// give the userdata a fenv table
	lua_newtable(L);	
	lua_setfenv(L, udIdx);

	// apply prototype:
	lua_getmetatable(L, udIdx);
	lua_getfield(L, -1, "__newindex");
	int newindexIdx = lua_gettop(L);
	lua_pushnil(L);  // first key
	// iterate through the prototype table
	while (lua_next(L, protoIdx) != 0) {
		// uses 'key' (at index -2 (6)) and 'value' (at index -1 (7))
		
		// set up a call to Synth[key] = value
		lua_pushvalue(L, newindexIdx);	// ud.__newindex function
		lua_pushvalue(L, udIdx);		// ud
		lua_pushvalue(L, -4);			// prototype key
		
		// string fields may be overridden by args[value] or ctx[value]:
		if (lua_isstring(L, -1)) {	
			const char * name = lua_tostring(L, -1);
			// try looking in the args table:
			lua_getfield(L, 1, name);
			if (lua_isnoneornil(L, -1)) {
				lua_pop(L, 1);	// skip it
				// try looking in the ctx:
				lua_getfield(L, ctxIdx, name);
				if (lua_isnoneornil(L, -1)) {
					lua_pop(L, 1);	// skip it
					// els: use the default:
					lua_pushvalue(L, -4);	// prototype value
				}	
			}
		} else {
			lua_pushvalue(L, -4);	// prototype value
		}

		lua_call(L, 3, 0);				// ud.__newindex(ud, key, value)
		// removes 'value'; keeps 'key' for next iteration
		lua_pop(L, 1);
	}
	lua_pop(L, 2); // metatable, __newindex
}

// stack: argstable, context, synth
//void Synth :: portinit(lua_State * L, const char * name, int offset, int channels) {
//	static const int udIdx = 3;
//	static const int ctxIdx = 2;
//	static const int protoIdx = lua_upvalueindex(2);
//	
//	lua_getfield(L, udIdx, name);
//	// if the userdata has no value for port[name]
//	if (lua_isnoneornil(L, -1)) {
//		// then read the value from the context
//		lua_getfield(L, ctxIdx, name);
//		if (lua_isnoneornil(L, -1)) {
//			// if not in context, read from defaults:
//			lua_pop(L, 1);
//			lua_getfield(L, protoIdx, name);
//			printf("port %s set from proto\n", name);
//		} else {
//			printf("port %s set from ctx\n", name);
//		}
//		lua::dump(L, name);
//		// and set it on the userdata (winds up calling luaav_synth_portset)
//		lua_setfield(L, udIdx, name);
//	} else {
//		printf("port %s already set\n", name);
//	}
//
//	lua_pop(L, 1);
//}

void msg_synth_play(al_sec t, Synth * self, Context * ctx) {
	if (self->mStateA == Synth::CREATED) {
		luaav_audio_config cfg = luaav_audio_config_current();
		self->mStart = (int)(samplestamp(t*cfg.samplerate) % cfg.blocksize);
		ctx->add(self);
	} else {
		printf("warning: received unexpected play command for node %p (%d/%d)\n", self, self->mStateL, self->mStateA);
	}
}

void Synth :: play() {
	if (mStateL == CREATED) {
		mStateL = PLAYING;
		mContext->inbox().send(mContext->now(), msg_synth_play, this, mContext);
	} else {
		printf("warning: attempted multiple play of node %p\n", this);
	}
}

void msg_synth_delete(al_sec t, Synth * self) {
	if (self->mLuaRefs <= 0 && self->mStateL == Synth::DEAD && self->mStateA == Synth::DEAD) {
		//delete self;
		self->cbfree(self);
	} else {
		printf("error: msg delete with wrong state\n");
	}
}

void msg_synth_stop(al_sec t, Synth * self) {
	//printf("stopping node ");
	if (self->mStateA == Synth::PLAYING) {
		luaav_audio_config cfg = luaav_audio_config_current();
		self->mEnd = (int)(samplestamp(t*cfg.samplerate) % cfg.blocksize);
		self->mStateA = Synth::STOPPING; 
	} else {
		// maybe the synth already stopped itself.
	}
}

void Synth :: stop() {
	if (mStateL == Synth::PLAYING) {
		mStateL = Synth::STOPPED;
		if (mStateA <= Synth::PLAYING) {
			mContext->inbox().send(mContext->now(), msg_synth_stop, this);
		}
	} else {
		printf("WARNING: attempt to stop node that was not playing\n");
	}
}

bool Synth :: release() { 
	--mLuaRefs;
	if (mLuaRefs < 0) {
		printf("THIS SHOULDN'T HAPPEN!\n");
	}
	return (mLuaRefs <= 0); 
}


Synth * luaav_synth_to(lua_State * L, int idx) {
	Synth * u = 0;
	lua_pushvalue(L, idx);
	if (lua_isuserdata(L, -1)) {
		if (lua_getmetatable(L, -1)) {
			// TODO: how to check for matching metatable?
			u = ((Box *)lua_touserdata(L, -2))->ptr;
		}
		lua_pop(L, 1);	// metatable
	}
	lua_pop(L, 1);
	return u;
}

Synth * luaav_synth_checkto(lua_State * L, int idx) {
	Synth * u = luaav_synth_to(L, idx);
	if (u == 0) luaL_error(L, "Synth not found (index %d)", idx);
	return u;
}

static int luaav_synth_stop(lua_State * L) {
	Synth * self = luaav_synth_checkto(L, 1);
	//printf("Synth stop\n");
	self->stop();
	return 0;
}

static int luaav_synth_running(lua_State * L) {
	Synth * self = luaav_synth_checkto(L, 1);
	lua_pushboolean(L, self->valid());
	return 1;
}

static int luaav_synth_gc(lua_State * L) {
	Synth * self = luaav_synth_checkto(L, 1);
	//lua::dump(L, "Synth gc");
	
	if (self->release()) {
		if (self->mStateA == Synth::STOPPED) {
			// node is dead to both Lua and to Audio
			// delete it immediately.
			self->cbfree(self);
		} else if (self->mStateL <= Synth::PLAYING) {
			// additional logic:
			// this should only apply if the synth is still actually playing
			// synths with Envelopes can use them to release the hold;
			// synths without Envelopes can trigger :stop()
			if (self->hasEnv()) {
				if (!self->envIsReleased()) {
					// release env now:
					self->envRelease();
				} else {
					// nothing to do: it's already in the release phase.
				}
			} else {
				// __gc maps to stop()
				luaav_synth_stop(L);
			}
		} else {
			// dead to Lua.
			self->mStateL = Synth::DEAD;
		}
	} 
	return 0;
}

static int luaav_synth_prototype(lua_State * L) {
	lua_getfenv(L, 1);
	return 1;
}

static int luaav_synth_tostring(lua_State * L) {
	lua_pushfstring(L, "Synth: %p", luaav_synth_to(L, 1));
	return 1;
}

static int luaav_synth_eq(lua_State * L) {
	lua_pushboolean(L, luaav_synth_to(L, 1) == luaav_synth_to(L, 2));
	return 1;
}

static int luaav_synth_index(lua_State *L) {
	Synth * self = luaav_synth_to(L, 1);
	// first check metatable:
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);	// bad result; but leave metatable there... 
		// next try __getters:
		lua_pushstring(L, "__getters");
		lua_rawget(L, -2);
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if(self->valid() && lua_type(L, -1) == LUA_TFUNCTION) {
			lua_pushvalue(L, 1);
			int err = lua_pcall(L, 1, 1, 0);
			if(err) {
				luaL_error(L, "error indexing %s.%s: %s\n",
										lua_tostring(L, 1),
										lua_tostring(L, 2),
										lua_tostring(L, -1));
			}
		} else {
			lua_pop(L, 3); // non-getter, __getters, metatable
			// try fenv
			lua_getfenv(L, 1);
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
		}
	}
	return 1;
}

static int luaav_synth_newindex(lua_State * L) {
	lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	lua_pushvalue(L, -3);
	const int udIdx = lua_gettop(L)-2;
	const int keyIdx = lua_gettop(L)-1;
	const int valIdx = lua_gettop(L);
	
	Synth * self = luaav_synth_to(L, udIdx);
	if (self->valid()) {
		// find setter:
		lua_getmetatable(L, udIdx);
		lua_pushstring(L, "__setters");
		lua_rawget(L, -2);
			lua_pushvalue(L, keyIdx);
			lua_rawget(L, -2);
		
		// call setter:
		if(lua_type(L, -1) == LUA_TFUNCTION) {
			lua_pushvalue(L, udIdx);
			//lua_pushvalue(L, keyIdx);
			lua_pushvalue(L, valIdx);
			int err = lua_pcall(L, 2, 0, 0);
			if(err) {
				luaL_error(L, "error indexing %s.%s: %s\n",
											lua_tostring(L, udIdx),
											lua_tostring(L, keyIdx),
											lua_tostring(L, -1));
			}
		} else {
			lua_pop(L, 1); // non-setter
		}
		lua_pop(L, 2); // __setters, metatable
	
		// store the value in the fenv:
		lua_getfenv(L, udIdx);
		lua_pushvalue(L, keyIdx);
		lua_pushvalue(L, valIdx);
		lua_rawset(L, -3);
		lua_pop(L, 1);	// fenv
	} else {
		//luaL_error(L, "synth in invalid state");
	}
	return 0;
}

void luaav_synth_newmetatable(lua_State * L, luaL_reg lib[], luaL_reg getters[], luaL_reg setters[]) {
	if (!Glue<Context>::to(L, 1)) {
		// need Context:
		lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_AUDIO_ROOT);
		lua_replace(L, 1);
	}
	
	// define metatable:
	lua_newtable(L);
	luaL_register(L, NULL, lib);
	static luaL_reg lib_standard[] = {
		{ "stop", luaav_synth_stop },
		{ "prototype", luaav_synth_prototype },
		{ "running", luaav_synth_running },
		{ "__gc", luaav_synth_gc },
		{ "__eq", luaav_synth_eq },
		{ "__tostring", luaav_synth_tostring },
		{ "__index", luaav_synth_index },
		{ "__newindex", luaav_synth_newindex },
		{ NULL, NULL }
	};
	luaL_register(L, NULL, lib_standard);
	
	lua_newtable(L);
	luaL_register(L, NULL, getters);
	lua_setfield(L, -2, "__getters");
	
	lua_newtable(L);
	luaL_register(L, NULL, setters);
	lua_setfield(L, -2, "__setters");	
}

void msg_portset_value(al_sec t, Synth * self, int offset, int channels, sample value) {
	for (int c = offset; c<offset+channels; c++) {
		Synth::Channel& ch = self->mChannels[c];
		ch.value = value;
		ch.ptr = &ch.value;
		ch.isSignal = 0;
	}
}
void msg_portset_ptr(al_sec t, Synth * self, int offset, int channels, Signal * sig) {
	sample * ptr = sig->data();
	for (int c = offset; c<offset+channels; c++) {
		Synth::Channel& ch = self->mChannels[c];
		ch.ptr = ptr;
		ch.isSignal = 1;
	}
}

int luaav_synth_portset(lua_State * L, int offset, int chans) {
	static const int udIdx = 1;
	static const int valIdx = 2;
	Synth * self = luaav_synth_checkto(L, udIdx);
	if (self->valid()) {
		bool immediate = self->mStateL == Synth::CREATED;
		al::MsgTube& inbox = self->mContext->inbox();
		al_sec t = self->mContext->now();
		switch(lua_type(L, valIdx)) {
			case LUA_TNUMBER: {
				if (immediate) {
					msg_portset_value(t, self, offset, chans, lua_tonumber(L, valIdx));
				} else {
					inbox.send(t, msg_portset_value, self, offset, chans, lua_tonumber(L, valIdx));
				}
			}
			break;
			case LUA_TUSERDATA: {
				if (immediate) {
					msg_portset_ptr(t, self, offset, chans, Glue<Signal>::checkto(L, valIdx));
				} else {
					inbox.send(t, msg_portset_ptr, self, offset, chans, Glue<Signal>::checkto(L, valIdx));
				}
			}
			break;
			case LUA_TTABLE:
				for (int i=0; i<chans; i++) {
					lua_rawgeti(L, valIdx, i+1);
					switch (lua_type(L, -1)) {
						case LUA_TNUMBER: {
								if (immediate) {
									msg_portset_value(t, self, offset+i, 1, lua_tonumber(L, -1));
								} else {
									inbox.send(t, msg_portset_value, self, offset+i, 1, lua_tonumber(L, -1));
								}
							}
							break;
						case LUA_TUSERDATA: {
								if (immediate) {
									msg_portset_ptr(t, self, offset+i, 1, Glue<Signal>::checkto(L, -1));
								} else {
									inbox.send(t, msg_portset_ptr, self, offset+i, 1, Glue<Signal>::checkto(L, -1));
								}
							}
							break;
						default:
							break;
					}
					lua_pop(L, 1);
				}	
			default:
			break;
		}
	}
	return 0;
}

PortReader :: PortReader(Synth& synth, int offset) {
	//assert (offset < synth.mNumChannels) {
	Synth::Channel& ch = synth.mChannels[offset];
	ptr = ch.ptr;
	isSignal = ch.isSignal;
}