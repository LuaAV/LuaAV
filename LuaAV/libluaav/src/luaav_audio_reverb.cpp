
#include "luaav_audio.hpp"
#include "luaav_audio_reverb.hpp"

class ReverbSynth {
public:
	Synth synth;
	Reverb reverb;
	
	ReverbSynth(lua_State * L) 
	:	synth(3) {
		synth.init(L);
	}
	
	void audioCB() {
		PortReader out_p1(synth, 0);
		PortReader out_p2(synth, 1);
		PortReader in_p1(synth, 2);
		for (int i=synth.mStart; i<synth.mEnd; i++) {
			sample& out1 = out_p1[i];
			sample& out2 = out_p2[i];
			sample& in1 = in_p1[i];
			sample l, r;
			reverb(in1, l, r);
			out1 += l;
			out2 += r;
		}
	}
	
};

/// Set input signal bandwidth, [0,1]
int lua_reverb_bandwidth(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.bandwidth(luaL_checknumber(L, 2));
	return 0;
}

/// Set high-frequency damping amount, [0, 1]
int lua_reverb_damping(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.damping(luaL_checknumber(L, 2));
	return 0;
}

/// Set decay rate, [0, 1)
int lua_reverb_decay(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.decay(luaL_checknumber(L, 2));
	return 0;
}

/// Set diffusion amounts
/// The recommended range of these coefficients is from 0.0 to 0.9999999
int lua_reverb_diffusion(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.diffusion(
		luaL_checknumber(L, 2),
		luaL_checknumber(L, 3),
		luaL_checknumber(L, 4),
		luaL_checknumber(L, 5)
	);
	return 0;
}

///// Set input diffusion 1 amount, [0,1)
int lua_reverb_diffusionIn1(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.diffusionIn1(luaL_checknumber(L, 2));
	return 0;
}

///// Set input diffusion 2 amount, [0,1)
int lua_reverb_diffusionIn2(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.diffusionIn2(luaL_checknumber(L, 2));
	return 0;
}

///// Set input diffusion 1 amount, [0,1)
int lua_reverb_diffusionDecay1(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.diffusionDecay1(luaL_checknumber(L, 2));
	return 0;
}

///// Set input diffusion 2 amount, [0,1)
int lua_reverb_diffusionDecay2(lua_State * L) {
	ReverbSynth * self = SynthBinding<ReverbSynth>::checkto(L, 1);
	self->reverb.diffusionDecay2(luaL_checknumber(L, 2));
	return 0;
}

extern "C" int luaopen_audio_Reverb(lua_State * L) {
	// define the prototype:
	lua_newtable(L);
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "out");
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "input");
	lua_pushnumber(L, 0.85);
	lua_setfield(L, -2, "decay");
	lua_pushnumber(L, 0.9995);
	lua_setfield(L, -2, "bandwidth");
	lua_pushnumber(L, 0.4);
	lua_setfield(L, -2, "damping");
	lua_pushnumber(L, 0.76);
	lua_setfield(L, -2, "diffusionIn1");
	lua_pushnumber(L, 0.666);
	lua_setfield(L, -2, "diffusionIn2");
	lua_pushnumber(L, 0.707);
	lua_setfield(L, -2, "diffusionDecay1");
	lua_pushnumber(L, 0.571);
	lua_setfield(L, -2, "diffusionDecay2");
	
	// define the metatable:
	static luaL_reg lib[] = {
		{ "diffusion", lua_reverb_diffusion },
		{ NULL, NULL }
	};
	static luaL_reg getters[] = {
		{ NULL, NULL }
	};
	static luaL_reg setters[] = {
		{ "out", portset<0, 2> },
		{ "input", portset<2, 1> },
		{ "decay", lua_reverb_decay },
		{ "bandwidth", lua_reverb_bandwidth},
		{ "damping", lua_reverb_damping },
		{ "diffusionIn1", lua_reverb_diffusionIn1 },
		{ "diffusionIn2", lua_reverb_diffusionIn2 },
		{ "diffusionDecay1", lua_reverb_diffusionDecay1 },
		{ "diffusionDecay2", lua_reverb_diffusionDecay2 },
		{ NULL, NULL }
	};
	luaav_synth_newmetatable(L, lib, getters, setters);
	
	lua_pushcclosure(L, SynthBinding<ReverbSynth>::create, 3);
	
	return 1;
}
	