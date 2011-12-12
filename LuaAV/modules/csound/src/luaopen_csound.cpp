extern "C" {
#include "lua.h"
#include "lauxlib.h"
extern int luaopen_csound(lua_State * L);
}

#include "luaav.h"
#include "luaav_audio.hpp"

#define USE_DOUBLE
#include <csound.h>
#include <CppSound.hpp>


/* just a flag to prevent CppSound being initialized twice */
static int initialized = 0;

/*
	get a callback when the value at stack index idx is collected:
*/
static void gc_sentinel(lua_State * L, int idx, lua_CFunction callback) {

	lua_pushvalue(L, idx); // value @idx
	lua_newuserdata(L, sizeof(void *)); // sentinel userdata
		lua_newtable(L);	// userdata metatable with __gc = callback
		lua_pushcfunction(L, callback);

		lua_setfield(L, -2, "__gc");
		lua_setmetatable(L, -2);

	/* check for (weak-valued) sentinel table; create if needed */
	lua_getfield(L, LUA_REGISTRYINDEX, "LuaAV.gc_sentinels");
	if (lua_isnoneornil(L, -1)) {
		lua_pop(L, 1);
		lua_newtable(L);

		// make weak-keyed
		lua_pushstring(L, "v");
		lua_setfield(L, -2, "__mode");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_pushvalue(L, -1);
		lua_setmetatable(L, -2);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaAV.gc_sentinels");
	}

	lua_insert(L, -3);
	lua_insert(L, -2);
	lua_settable(L, -3); // lua::sentinel[value @idx] = sentinel userdata
	lua_pop(L, 1); // lua::sentinel
}

/* general printer */
void csoundprint(CSOUND * c, int attr, const char *format, va_list valist) {
	static char fmt[256];
	sprintf(fmt, "csound: %s\n", format);
	vprintf(fmt, valist);
}
void csoundnoprint(CSOUND * c, int attr, const char *format, va_list valist) {}





#pragma mark CppSoundSynth


struct CSynth {
	Synth synth;
	
	CppSound * csound;
	int ok;
	
	CSynth(lua_State * L);	
	~CSynth();
	
	void audioCB();
};


CSynth :: CSynth(lua_State * L) 
:	synth(5), csound(NULL), ok(0) {
	synth.init(L);
	
	luaav_audio_config cfg = luaav_audio_config_current();
	
	const char * score;
	const char * orchestra;
	int result;
	//int ksmps;
	
	const char * deforc = "sr = 44100\n "
				"ksmps = 64\n "
				"nchnls = 2\n "
				"0dbfs = 1\n ";

	lua::dump(L, "CSynth");
	if (lua_istable(L, 1)) {
		
		lua_getfield(L, 1, "orchestra");
		orchestra = luaL_optstring(L, -1, deforc );
		lua_pop(L, 1);
		lua_getfield(L, 1, "score");
		score = luaL_optstring(L, -1,  " " );
		lua_pop(L, 1);
	} 
	
	char *sr, *kr;  
	// sampling-rate override
	sr = new char[32];
	sprintf(sr,"--sample-rate=%d", (int)cfg.samplerate);
	
	// ksmps override
	kr = new char[32];
	sprintf(kr,"-k %f", cfg.samplerate/(double)cfg.blocksize);

	csound = new CppSound;

	if (lua_toboolean(L, 2)) {
		csound->SetMessageCallback(csoundprint); 
	} else {
		csound->SetMessageCallback(csoundnoprint);
	}
	csound->PreCompile();
	csound->SetHostImplementedAudioIO(1, cfg.blocksize);

	char * filename = (char *)csound->generateFilename().data();
	//csound->setFilename(filename);
	
	csound->setScore(score);
	csound->setOrchestra(orchestra);
	char cmdoptions[128];
	sprintf(cmdoptions,"-n -d --sample-rate=%d -k %f", (int)cfg.samplerate, cfg.samplerate/(double)cfg.blocksize);
	csound->setCommand(cmdoptions);

	//printf("command %s\n", csound->getCommand().data());
	
	csound->save(filename);
	csound->load(filename);
	result = csound->Compile(filename);

	//result = csound->compile(); //filename, "-d", "-n", sr, kr);
	if (result == CSOUND_ERROR || result == CSOUND_MEMORY) {
		delete csound;
		csound = NULL;
		luaL_error(L, "csound compile error");
	}

	ok = csound->getIsCompiled();
	ok = 1;
}

CSynth :: ~CSynth() {
	printf("destroyed csound\n");
	if (csound) {
		if (csound->getIsPerforming()) {
			csound->stop();
		}
		csound->cleanup();
		delete csound;
	}
}

void CSynth :: audioCB() {
	if (!ok || csound == NULL) return;
	
	int from = synth.mStart;
	int to = synth.mEnd;
	
	PortReader input_p1(synth, 0);
	PortReader input_p2(synth, 1);
	PortReader out_p1(synth, 2);
	PortReader out_p2(synth, 3);
	PortReader amp_p1(synth, 4);
		
	luaav_audio_config cfg = luaav_audio_config_current();
	//int blocksize = cfg.blocksize;
	int nchnls =  csound->GetNchnls();
	int ksmps = csound->GetKsmps();
	double scale = csound->Get0dBFS();
	double inv_scale = 1.0 / scale;
	//long inbufsize = csound->GetInputBufferSize();
	//long outbufsize = csound->GetOutputBufferSize();
	
	
//	sample * outbuffers[2];
//	outbuffers[0] = luaav3_OutPort_data(&self->synth.output, 0);	
//	outbuffers[1] = luaav3_OutPort_data(&self->synth.output, 1);
//
//	sample const * inbuffers[2];
//	inbuffers[0] = luaav3_InPort_data(&self->input, 0); 
//	inbuffers[1] = luaav3_InPort_data(&self->input, 1); 
	
	// if ksmps is less than blocksize, need to run multiple passes 
	int offset = 0;
	for (offset=0; offset<to; offset+=ksmps) {
	
		// because csound is interleaved, and LuaAV isn't. 
		MYFLT * csin = csound->GetSpin();
		for (int i=0; i<ksmps; i++) {
			const sample input1 = input_p1[i+offset];
			const sample input2 = input_p1[i+offset];
		
			csin[    i*nchnls] = input1 * scale; 
			csin[1 + i*nchnls] = input2 * scale; 
		}

		/*
		luaav_sample v1 = inbuffers[0][0];	
		luaav_sample v2 = csin[0]; 
		printf("%f %f\n", v1, v2); // ok, got data!
		*/

		// run one ksmps of audio processing in csound 
		csound->PerformKsmps(); 
		
		//convert csound interleaved audio back into LuaAV non-interleaved audio 
		MYFLT * csout = csound->GetSpout();
		for (int i=0; i<ksmps; i++) {
			sample& out1 = out_p1[i+offset];
			sample& out2 = out_p2[i+offset];
			const sample amp1 = amp_p1[i+offset];
		
			out1 += csout[    i*nchnls] * inv_scale * amp1; 
			out2 += csout[1 + i*nchnls] * inv_scale * amp1; 
		}
	} 
}



//typedef struct CppSoundSynth {
//	luaav3_Synth synth;
//	luaav3_InPort input;
//	CppSound * csound;
//	int ok;
//} CppSoundSynth;

// perform is called in the audio thread
//void CppSoundSynth_perform(luaav3_Node * node, int from, int to) {
//	luaav_audio_config cfg = luaav_audio_config_current();
//	CppSoundSynth * const self = (CppSoundSynth *)node;
//	CppSound * csound = self->csound;
//	if (!self->ok || csound == NULL) return;
//	
//	luaav_sample * outbuffers[2];
//	outbuffers[0] = luaav3_OutPort_data(&self->synth.output, 0);	
//	outbuffers[1] = luaav3_OutPort_data(&self->synth.output, 1);
//
//	luaav_sample const * inbuffers[2];
//	inbuffers[0] = luaav3_InPort_data(&self->input, 0); 
//	inbuffers[1] = luaav3_InPort_data(&self->input, 1); 
//	
//	luaav_sample const * const amp = luaav3_Fader_data(&self->synth.amp, from, to);	
//	
//	//int blocksize = cfg.blocksize;
//	int nchnls =  csound->GetNchnls();
//	int ksmps = csound->GetKsmps();
//	double scale = csound->Get0dBFS();
//	double inv_scale = 1.0 / scale;
//	//long inbufsize = csound->GetInputBufferSize();
//	//long outbufsize = csound->GetOutputBufferSize();
//	
//	// if ksmps is less than blocksize, need to run multiple passes 
//	int offset = 0;
//	//for (offset=0; offset<to; offset+=ksmps) {
//	
//	// because csound is interleaved, and LuaAV isn't. 
//	MYFLT * csin = csound->GetSpin();
//	for (int ch=0; ch<2; ch++) {
//		const luaav_sample * in = inbuffers[ch]+offset;	
//		//for (i=from; i<to; i++)
//		for (int i=0; i<ksmps; i++) {
//			csin[ch + i*nchnls] = in[i] * scale; 
//		}
//	}
//
//	/*
//	luaav_sample v1 = inbuffers[0][0];	
//	luaav_sample v2 = csin[0]; 
//	printf("%f %f\n", v1, v2); // ok, got data!
//	*/
//
//	// run one ksmps of audio processing in csound 
//	csound->PerformKsmps(); 
//	
//	//convert csound interleaved audio back into LuaAV non-interleaved audio 
//	MYFLT * csout = csound->GetSpout();
//	for (int ch=0; ch<2; ch++) {
//		luaav_sample * out = outbuffers[ch]+offset;
//		//for (i=from; i<to; i++)
//		for (int i=0; i<ksmps; i++) {
//			out[i] += csout[ch + i*nchnls] * inv_scale * amp[i]; 
//		}
//	}
//	
//	luaav3_InPort_clear(&self->input);  	
//}

// destroy and create are called in the main thread
//void CppSoundSynth_destroy(al_sec t, luaav3_Node * node) {
//	printf("destroyed csound\n");
//	CppSoundSynth * self = (CppSoundSynth *)node;
//	CppSound * csound = self->csound;
//	if (csound) {
//		if (csound->getIsPerforming()) {
//			csound->stop();
//		}
//		csound->cleanup();
//		delete csound;
//	}
//	luaav3_OutPort_release(&self->synth.output);
//	luaav3_Fader_release(&self->synth.amp);
//	luaav3_InPort_release(&self->input);	
//	delete self;
//}

//int lua_CppSoundSynth_create(lua_State * L) {
//	luaav3_Graph * G = luaav3_graph_fromstate(L);
//	luaav_audio_config cfg = luaav_audio_config_current();
//	CppSoundSynth * self = new CppSoundSynth;
//	self->csound = 0;	
//
//	luaav3_Node_init((luaav3_Node *)self, G, CppSoundSynth_perform, CppSoundSynth_destroy);
//	luaav3_OutPort_init(&self->synth.output, (luaav3_Node *)self);
//	luaav3_Fader_init(&self->synth.amp, 1, 1, 0);
//	luaav3_InPort_init(&self->input);
//	
//	const char * score;
//	const char * orchestra;
//	int result;
//	//int ksmps;
//	CppSound * csound;
//
//	const char * deforc = "sr = 44100\n "
//				"ksmps = 64\n "
//				"nchnls = 2\n "
//				"0dbfs = 1\n ";
//
//	if (lua_istable(L, 1)) {
//		lua_getfield(L, 1, "output");	
//		luaav3_OutPort_set(L, -1, (luaav3_Node *)self, &self->synth.output);
//		lua_pop(L, 1);
//		lua_getfield(L, 1, "amp");		
//		luaav3_Fader_set(L, -1, (luaav3_Node *)self, &self->synth.amp, 1);
//		lua_pop(L, 1);
//		lua_getfield(L, 1, "input");		
//		luaav3_InPort_set(L, -1, (luaav3_Node *)self, &self->input, 0);
//		lua_pop(L, 1);
//		
//		lua_getfield(L, 1, "orchestra");
//		orchestra = luaL_optstring(L, -1, deforc );
//		lua_pop(L, 1);
//		lua_getfield(L, 1, "score");
//		score = luaL_optstring(L, -1,  " " );
//		lua_pop(L, 1);
//	} 
//	
//	self->ok = 0;
//	
//	char *sr, *kr;  
//	// sampling-rate override
//	sr = new char[32];
//	sprintf(sr,"--sample-rate=%d", (int)cfg.samplerate);
//	
//	// ksmps override
//	kr = new char[32];
//	sprintf(kr,"-k %f", cfg.samplerate/(double)cfg.blocksize);
//
//	self->csound = csound = new CppSound;
//
//	if (lua_toboolean(L, 2)) {
//		csound->SetMessageCallback(csoundprint); 
//	} else {
//		csound->SetMessageCallback(csoundnoprint);
//	}
//	csound->PreCompile();
//	csound->SetHostImplementedAudioIO(1, cfg.blocksize);
//
//	char * filename = (char *)csound->generateFilename().data();
//	//csound->setFilename(filename);
//	
//	csound->setScore(score);
//	csound->setOrchestra(orchestra);
//	char cmdoptions[128];
//	sprintf(cmdoptions,"-n -d --sample-rate=%d -k %f", (int)cfg.samplerate, cfg.samplerate/(double)cfg.blocksize);
//	csound->setCommand(cmdoptions);
//
//	//printf("command %s\n", csound->getCommand().data());
//	
//	csound->save(filename);
//	csound->load(filename);
//	result = csound->Compile(filename);
//
//	//result = csound->compile(); //filename, "-d", "-n", sr, kr);
//	if (result == CSOUND_ERROR || result == CSOUND_MEMORY) {
//		delete csound;
//		delete self;
//		return luaL_error(L, "csound compile error");
//	}
//
//	self->ok = csound->getIsCompiled();
//	self->ok = 1;
//
//	luaav3_Node_send((luaav3_Node *)self);
//	luaav3_Synth_push(L, &self->synth, 1,  "CppSoundSynth_metatable");
//	return 1;
//}

/*
int lua_CppSoundSynth_gc(lua_State * L) { 
	printf("gc csound\n");

	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	luaav3_Node_remove((luaav3_Node *)self, 0);
	lua_pushnil(L);
	lua_setmetatable(L, 1);
	return 0;
}

static int lua_CppSoundSynth_tostring(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushfstring(L, "csound %p", self);
	return 1;
}
*/

static int CppSoundSynth_verbose(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	int verbose = lua_toboolean(L, 2);
	if (verbose)
		csound->SetMessageCallback(csoundprint);
	else
		csound->SetMessageCallback(csoundnoprint);
	return 1;
}

static int CppSoundSynth_nchnls(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushinteger(L, csound->GetNchnls());
	return 1;
}
static int CppSoundSynth_sr(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushinteger(L, csound->GetSr());
	return 1;
}
static int CppSoundSynth_kr(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushinteger(L, csound->GetKr());
	return 1;
}
static int CppSoundSynth_ksmps(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushinteger(L, csound->GetKsmps());
	return 1;
}
static int CppSoundSynth_0dbfs(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushnumber(L, csound->Get0dBFS());
	return 1;
}
static int CppSoundSynth_scoretime(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushnumber(L, csound->GetScoreTime());
	return 1;
}
static int CppSoundSynth_scorepending(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushboolean(L, csound->IsScorePending());
	return 1;
}
static int CppSoundSynth_scorerewind(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	csound->RewindScore();
	return 0;
}
static int CppSoundSynth_messagelevel(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	if (lua_isnumber(L, 2)) {
		csound->SetMessageLevel(lua_tonumber(L, 2));
	}
	lua_pushinteger(L, csound->GetMessageLevel());
	return 1;
}
static int CppSoundSynth_message(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	self->csound->InputMessage(lua_tostring(L, 2));
	return 0;
}

static int CppSoundSynth_scoreevent(lua_State * L) {
	int i;
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	if (self->csound == NULL) return 0;
	const char * type = luaL_checkstring(L, 2);
	int numfields = lua_gettop(L) - 2;
	MYFLT pFields[numfields];
	for (i=0; i<numfields; i++) {
		pFields[i] = luaL_checknumber(L, i+3);
	}
	self->csound->ScoreEvent(type[0], pFields, numfields);
	lua_pushvalue(L, 4);	// return duration
	return 1;
}

static int CppSoundSynth_listopcodes(lua_State * L) {
	int i;
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	opcodeListEntry * opcodelist;
	int total = self->csound->NewOpcodeList(opcodelist);
	lua_createtable(L, total, 0);
	for (i=0; i<total; i++) {
		lua_createtable(L, 0, 3);
		lua_pushfstring(L, opcodelist[i].opname);
		lua_setfield(L, -2, "opname");
		lua_pushfstring(L, opcodelist[i].intypes);
		lua_setfield(L, -2, "intypes");
		lua_pushfstring(L, opcodelist[i].outypes);
		lua_setfield(L, -2, "outtypes");
		lua_rawseti(L, -2, i+1);
	}
	csound->DisposeOpcodeList(opcodelist);
	return 1;
}

static int CppSoundSynth_getInstrumentCount(lua_State * L) {
	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
	CppSound * csound = self->csound;
	lua_pushinteger(L, csound->getInstrumentCount());
	return 1;
}

//static int CppSoundSynth_getSpin(lua_State * L) {
//	CSynth * self = SynthBinding<CSynth>::checkto(L, 1);
//	luaav3_Bus_push(L, NULL); //self->inbus);
//	return 1;
//}

/* 
// CASEY'S NEW STUFF BEGIN
// question: why is score rewind rewind score

static int CppSoundSynth_version(lua_State * L) {
	lua_pushinteger(L, csoundGetVersion());
	return 1;	
}
static int CppSoundSynth_apiversion(lua_State * L) {
	lua_pushinteger(L, csoundGetAPIVersion());
	return 1;	
}
static int CppSoundSynth_getstrvarmaxlen(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetStrVarMaxLen(self->c));
	return 1;	
}
static int CppSoundSynth_sampleformat(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetSampleFormat(self->c));
	return 1;	
}
static int CppSoundSynth_samplesize(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetSampleSize(self->c));
	return 1;	
}
// this is already used above, but here is the wrapped function anyway
static int CppSoundSynth_inputbuffersize(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetInputBufferSize(self->c));
	return 1;	
}
// this is already used above, but here is the wrapped function anyway
static int CppSoundSynth_outputbuffersize(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetOutputBufferSize(self->c));
	return 1;	
}
// not sure about this one
static int CppSoundSynth_setscorepending(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		int pending = lua_tointeger(L, 2);
		csoundSetScorePending(self->c, pending);
	}
	return 0;	
}

// two functions in one, as above
static int CppSoundSynth_scoreoffsetseconds(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		MYFLT offset = lua_tonumber(L, 2);
		csoundSetScoreOffsetSeconds(self->c, offset);
	}
	else lua_pushnumber(L, csoundGetScoreOffsetSeconds(self->c));
	return 1;	
}
// not sure about this
static int CppSoundSynth_keypress(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isstring(L, 2)) {
		//this is probably not so nice
		char c = lua_tostring(L, 2)[0];
		csoundKeyPress(self->c, c);
	}
	return 0;	
}
// two functions in one
static int CppSoundSynth_debug(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		int debug = lua_tointeger(L, 2);
		csoundSetDebug(self->c, debug);
	}
	else lua_pushinteger(L, csoundGetDebug(self->c));
	return 1;	
}
static int CppSoundSynth_tablelength(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		int table = lua_tointeger(L, 2);
		lua_pushinteger(L, csoundTableLength(self->c, table));
	}
	return 1;	
}
static int CppSoundSynth_tableget(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
		int table = lua_tointeger(L, 2);
		int index = lua_tointeger(L, 3);
		lua_pushnumber(L, csoundTableGet(self->c, table, index));
	}
	return 1;	
}

static int CppSoundSynth_tableset(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
		int table = lua_tointeger(L, 2);
		int index = lua_tointeger(L, 3);
		MYFLT value = lua_tonumber(L, 4);
		csoundTableSet(self->c, table, index, value);
	}
	return 0;	
}
// not sure we want this
static int CppSoundSynth_sleep(lua_State * L) {
	if (lua_isnumber(L, 1)) {
		size_t milliseconds = lua_tointeger(L, 1);
		csoundSleep(milliseconds);
	}
	return 0;	
}
static int CppSoundSynth_getsizeofmyflt(lua_State * L) {
	lua_pushinteger(L, csoundGetSizeOfMYFLT());
	return 1;	
}
static int CppSoundSynth_rand31(lua_State * L) {
	if (lua_isnumber(L, 1)) {
		int seed = lua_tointeger(L, 1);	
		lua_pushinteger(L, csoundRand31(&seed));
	}
	return 1;	
}
static int CppSoundSynth_chanikset(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
		MYFLT value = lua_tointeger(L, 2);
		int n = lua_tointeger(L, 3);
		lua_pushinteger(L, csoundChanIKSet(self->c, value, n));
	}
	return 1;	
}
// not sure about next 3 here, the effects of the dereference & here are probably bad
static int CppSoundSynth_chanokget(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
		MYFLT value = lua_tointeger(L, 2);
		int n = lua_tointeger(L, 3);
		lua_pushinteger(L, csoundChanOKGet(self->c, &value, n));
	}
	return 1;	
}
static int CppSoundSynth_chaniaset(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
		MYFLT inp = lua_tointeger(L, 2);
		const MYFLT * value = &inp;
		int n = lua_tointeger(L, 3);
		lua_pushinteger(L, csoundChanIASet(self->c, value, n));
	}
	return 1;	
}
static int CppSoundSynth_chanoaget(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
		MYFLT value = lua_tointeger(L, 2);
		int n = lua_tointeger(L, 3);
		lua_pushinteger(L, csoundChanOAGet(self->c, &value, n));
	}
	return 1;	
}
static int CppSoundSynth_getfirstmessage(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushstring(L, csoundGetFirstMessage(self->c));
	return 1;	
}
static int CppSoundSynth_getfirstmessageattr(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetFirstMessageAttr(self->c));
	return 1;	
}
static int CppSoundSynth_popfirstmessage(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	csoundPopFirstMessage(self->c);
	return 0;	
}

static int CppSoundSynth_getmessagecnt(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	lua_pushinteger(L, csoundGetMessageCnt(self->c));
	return 1;	
}
// do we want this?
static int CppSoundSynth_destroymessagebuffer(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	csoundDestroyMessageBuffer(self->c);
	return 0;	
}

// Do I need MYFLT* p[128]; ?
MYFLT* p; 
// this probably needs to be expanded CSOUND_INPUT_CHANNEL etc.
static int CppSoundSynth_getchannelptr(lua_State * L) {
	CppSoundSynth * self = (CppSoundSynth *)luaav3_Synth_checkto(L, 1);
	if (lua_isstring(L, 2) && lua_isnumber(L, 3)) {
		const char * parameter = lua_tostring(L, 2);
		MYFLT value = lua_tonumber(L, 3);
		if(csoundGetChannelPtr(self->c, &p, parameter, CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) == 0) *p = value;
	}
	return 0;	
}
	

int unload(lua_State * L) {
	printf("sentinel_gc\n");
	
	return 0;
}

/* require "csound" */
int luaopen_csound(lua_State * L) {
	
	if (luaL_loadstring(L, "require 'audio'") || lua_pcall(L, 0, 0, 0)) {
		return luaL_error(L, "failed to require audio; %s", lua_tostring(L, -1));
	}
	
	/* make sure that both LuaAV and CppSound are initialized */
	if (!initialized) {
		if (csoundInitialize(0, NULL, 0) < 0) {
			return luaL_error(L, "csound failed to initialize");
		}
		
		CppSound dummy;
	}
	
//	// hack the module loader to prevent unloading 
//	luaL_getmetatable(L, "_LOADLIB");
//	lua_pushnil(L);
//	lua_setfield(L, -2, "__gc");
//	lua_pop(L, 1);
	
//	// create a metatable for CSOUND instances
//	luaL_newmetatable(L, "meta_csound");
//	lua_pushvalue(L, -1);
//	lua_setfield(L, -2, "__index");
/*
		// CASEY'S NEW THINGS BEGIN
		{ "version", CppSoundSynth_version },
		{ "apiversion", CppSoundSynth_apiversion },
		{ "getstrvarmaxlen", CppSoundSynth_getstrvarmaxlen },
		{ "sampleformat", CppSoundSynth_sampleformat },
		{ "samplesize", CppSoundSynth_samplesize },
		{ "inputbuffersize", CppSoundSynth_inputbuffersize },
		{ "outputbuffersize", CppSoundSynth_outputbuffersize },
		{ "setscorepending", CppSoundSynth_setscorepending },
		{ "scoreoffsetseconds", CppSoundSynth_scoreoffsetseconds },
		{ "keypress", CppSoundSynth_keypress },
		{ "debug", CppSoundSynth_debug },
		{ "tablelength", CppSoundSynth_tablelength },
		{ "tableget", CppSoundSynth_tableget },
		{ "tableset", CppSoundSynth_tableset },
		{ "sleep", CppSoundSynth_sleep },
		{ "getsizeofmyflt", CppSoundSynth_getsizeofmyflt },
		{ "rand31", CppSoundSynth_rand31 },
		{ "chanikset", CppSoundSynth_chanikset },
		{ "chanokget", CppSoundSynth_chanokget },
		{ "chaniaset", CppSoundSynth_chaniaset },
		{ "chanoaget", CppSoundSynth_chanoaget },
		{ "getfirstmessage", CppSoundSynth_getfirstmessage },
		{ "getfirstmessageattr", CppSoundSynth_getfirstmessageattr },
		{ "popfirstmessage", CppSoundSynth_popfirstmessage },
		{ "getmessagecnt", CppSoundSynth_getmessagecnt },
		{ "destroymessagebuffer", CppSoundSynth_destroymessagebuffer },
		{ "getchannelptr", CppSoundSynth_getchannelptr },
		// CASEY'S NEW THINGS END	
	
	*/
	
	// define the prototype:
	lua_newtable(L);
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "out");
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "input");
	lua_pushnumber(L, 1);
	lua_setfield(L, -2, "amp");
	
	
	// define the metatable:
	static luaL_reg lib[] = {
		
		{ "verbose", CppSoundSynth_verbose },
		{ "nchnls", CppSoundSynth_nchnls },
		{ "ksmps", CppSoundSynth_ksmps },
		{ "kr", CppSoundSynth_kr },
		{ "sr", CppSoundSynth_sr },
		{ "0dbfs", CppSoundSynth_0dbfs },
		{ "scoretime", CppSoundSynth_scoretime },
		{ "scorepending", CppSoundSynth_scorepending },
		{ "scorerewind", CppSoundSynth_scorerewind },
		{ "scoreevent", CppSoundSynth_scoreevent },
		{ "messagelevel", CppSoundSynth_messagelevel },
		{ "message", CppSoundSynth_message },
		{ "listopcodes", CppSoundSynth_listopcodes },

		{ "getinstrumentcount", CppSoundSynth_getInstrumentCount },
		//{ "ins", CppSoundSynth_getSpin },
		{ NULL, NULL }
	};
	static luaL_reg getters[] = {
		{ NULL, NULL }
	};
	static luaL_reg setters[] = {
		{ "input", portset<0, 2> },
		{ "out", portset<2, 2> },
		{ "amp", portset<4, 1> },
		{ NULL, NULL }
	};
	luaav_synth_newmetatable(L, lib, getters, setters);
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_pushcclosure(L, SynthBinding<CSynth>::create, 3);
	
	// sentinel on unload 
	//gc_sentinel(L, -1, unload);
	
//	// define the module table 
//	luaL_Reg module[] = {
//		{ "create", lua_CppSoundSynth_create },
//		{ NULL, NULL }
//	};
//	luaL_register(L, "csound", module);
//	lua_pushinteger(L, csoundGetVersion());
//	lua_setfield(L, -2, "version");
	
	return 1;
}
