extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	
	#include "ZenGarden.h"
}

#include "luaav.h"
#include "luaav_audio_jit.hpp"
#include "lua_glue.h"









// utility to copy a buffer of one type to a buffer of another
template<typename TI, typename TO>
void copy_converting_buffer(TI * src, TO * dst, int count) {
	for (int i=0; i<count; i++) { dst[i] = (TO)src[i]; }
}


// what to do with messages from PD: 
extern "C" void printCallbackFunction(ZGCallbackFunction function, void *userData, void *ptr) {
	switch (function) {
		case ZG_PRINT_STD: {
			printf("%s\n", (char *) ptr);
			break;
		}
		case ZG_PRINT_ERR: {
			printf("ERROR: %s\n", (char *) ptr);
			break;
		}
		default: {
			break;
		}
	}
}



struct PdSynth {
	Synth synth;
	
	PdSynth(lua_State * L);	
	~PdSynth();
	
	void audioCB();
	
	PdContext * pd() { return mPd; }
	
	PdContext * mPd;
	float * mInputBuffers;
	float * mOutputBuffers;
	int mIns, mOuts;
	int mBlockSize;
};

PdSynth :: PdSynth(lua_State * L) 
:	synth(5), mPd(NULL) {
	synth.init(L);
	
	luaav_audio_config config = luaav_audio_config_current();
	void * userdata = this;
	mPd = zg_new_context(config.inchannels, config.outchannels, config.blocksize, config.samplerate, printCallbackFunction, userdata);
	
	mIns = config.inchannels;
	mOuts = config.outchannels;
	mBlockSize = config.blocksize;
	
	// create some buffers for PD to use:
	// (Can't use LuaAV signals, because PD wants floats...)
	mInputBuffers = (float *)calloc(mIns * mBlockSize, sizeof(float));
	mOutputBuffers = (float *)calloc(mOuts * mBlockSize, sizeof(float));
}

PdSynth :: ~PdSynth() {
	//printf("deleting PdContextNode\n");
	zg_delete_context(mPd);
	free(mInputBuffers);
	free(mOutputBuffers);
}

void PdSynth :: audioCB() {
	if (mPd == NULL) return;
	
	int from = synth.mStart;
	int to = synth.mEnd;
	
	PortReader input_p1(synth, 0);
	PortReader input_p2(synth, 1);
	PortReader out_p1(synth, 2);
	PortReader out_p2(synth, 3);
	PortReader amp_p1(synth, 4);
		
	luaav_audio_config cfg = luaav_audio_config_current();
	//int blocksize = cfg.blocksize;
//	int nchnls =  csound->GetNchnls();
//	int ksmps = csound->GetKsmps();
//	double scale = csound->Get0dBFS();
//	double inv_scale = 1.0 / scale;
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
	
	
	// copy buffers in:
	// TODO: use a Control array instead of globals
	float * dst1 = mInputBuffers;
	float * dst2 = mInputBuffers + mBlockSize;
	for (int i=0; i<mBlockSize; i++) {
		dst1[i] = (float)input_p1[i]; 
		dst2[i] = (float)input_p2[i]; 
	}
	
	// let PD do the work:
	zg_process(mPd, mInputBuffers, mOutputBuffers);	
	
	// copy buffers back out:
	// TODO: use a Control array instead of globals
	float * src1 = mOutputBuffers;
	float * src2 = mOutputBuffers + mBlockSize;
	for (int i=0; i<mBlockSize; i++) {
		sample amp = amp_p1[i];
		out_p1[i] += amp * (sample)src1[i]; 
		out_p2[i] += amp * (sample)src2[i]; 
	}
	
	//
//		// because csound is interleaved, and LuaAV isn't. 
//		MYFLT * csin = csound->GetSpin();
//		for (int i=0; i<ksmps; i++) {
//			const sample input1 = input_p1[i+offset];
//			const sample input2 = input_p1[i+offset];
//		
//			csin[    i*nchnls] = input1 * scale; 
//			csin[1 + i*nchnls] = input2 * scale; 
//		}
//
//		/*
//		luaav_sample v1 = inbuffers[0][0];	
//		luaav_sample v2 = csin[0]; 
//		printf("%f %f\n", v1, v2); // ok, got data!
//		*/
//
//		// run one ksmps of audio processing in csound 
//		csound->PerformKsmps(); 
//		
//		//convert csound interleaved audio back into LuaAV non-interleaved audio 
//		MYFLT * csout = csound->GetSpout();
//		for (int i=0; i<ksmps; i++) {
//			sample& out1 = out_p1[i+offset];
//			sample& out2 = out_p2[i+offset];
//			const sample amp1 = amp_p1[i+offset];
//		
//			out1 += csout[    i*nchnls] * inv_scale * amp1; 
//			out2 += csout[1 + i*nchnls] * inv_scale * amp1; 
//		}
}


//class PdContextNode : public Node {
//public:
//	Port adc;
//	Port out;
//	
//	PdContextNode(Context * C) 
//	:	Node(C, "Pd"),
//		adc(2),
//		out(2)
//	{
//		luaav_audio_config config = luaav_audio_config_current();
//		void * userdata = this;
//		mPd = zg_new_context(config.inchannels, config.outchannels, config.blocksize, config.samplerate, printCallbackFunction, userdata);
//		
//		mIns = config.inchannels;
//		mOuts = config.outchannels;
//		mBlockSize = config.blocksize;
//		
//		// create some buffers for PD to use:
//		// (Can't use LuaAV signals, because PD wants floats...)
//		mInputBuffers = (float *)calloc(mIns * mBlockSize, sizeof(float));
//		mOutputBuffers = (float *)calloc(mOuts * mBlockSize, sizeof(float));
//	}
//	
//	~PdContextNode() {
//		//printf("deleting PdContextNode\n");
//		zg_delete_context(mPd);
//		free(mInputBuffers);
//		free(mOutputBuffers);
//	}
//	
//	int init(lua_State * L) {
//		port_init(L, "out", 3, 2);
//		return 0;
//	}
//	
//	PdContext * pd() { return mPd; }
//	
//	virtual int audioCB(AudioData& io) {
//		// copy buffers in:
//		// TODO: use a Control array instead of globals
//		for (int c=0; c<mOuts; c++) {
//			Port::Channel& src = adc[c];
//			float * dst = mInputBuffers + c*mBlockSize;
//			for (int i=0; i<mBlockSize; i++) {
//				dst[i] = (float)src[i]; 
//			}
//		}
//		
//		// let PD do the work:
//		zg_process(mPd, mInputBuffers, mOutputBuffers);	
//		
//		// copy buffers back out:
//		// TODO: use a Control array instead of globals
//		for (int c=0; c<mOuts; c++) {
//			float * src = mOutputBuffers + c*mBlockSize;
//			Port::Channel& dst = out[c];
//			for (int i=0; i<mBlockSize; i++) {
//				//if (i==0) printf("%f\n", src[i]);
//				dst[i] += (sample)src[i]; 
//			}
//		}
//		
//		return 0;
//	}
//	
//	PdContext * mPd;
//	float * mInputBuffers;
//	float * mOutputBuffers;
//	int mIns, mOuts;
//	int mBlockSize;
//};







//template<> const char * Glue<PdGraph>::usr_name() { return "PdGraph"; }
//template<> PdGraph * Glue<PdGraph>::usr_new(lua_State * L) { 
//	PdContext * context = LNode<PdContextNode>::checkto(L, 1)->pd();
//	PdGraph * graph = NULL;
//	if (lua_gettop(L) > 1) {
//		const char * filepath = luaL_checkstring(L, 2);
//		// todo: check that filepath terminates with a slash... 
//		const char * filename = luaL_checkstring(L, 3);
//		graph = zg_new_graph(context, (char *)filepath, (char *)filename);
//	} else {
//		// just an empty graph
//		graph = zg_new_empty_graph(context);
//	}
//	if (graph) {
//		zg_attach_graph(context, graph);
//	}
//	return graph;
//}


int lua_pdcontext_open(lua_State * L) {
	PdSynth * self = SynthBinding<PdSynth>::checkto(L, 1);
	PdGraph * graph = NULL;
	if (lua_gettop(L) > 1) {
		const char * filepath = luaL_checkstring(L, 2);
		// todo: check that filepath terminates with a slash... 
		const char * filename = luaL_checkstring(L, 3);
		graph = zg_new_graph(self->pd(), (char *)filepath, (char *)filename);
	} else {
		// just an empty graph
		graph = zg_new_empty_graph(self->pd());
	}
	if (graph) {
		zg_attach_graph(self->pd(), graph);
	}

	//return Glue<PdGraph>::create(L);
	return 0;
}


int lua_pdcontext_send_message(lua_State * L) {
	PdSynth * self = SynthBinding<PdSynth>::checkto(L, 1);
	const char * name = luaL_checkstring(L, 2);
	switch (lua_type(L, 3)) {
		case LUA_TNONE:
		case LUA_TNIL:
			zg_send_message(self->pd(), name, "b");
			break;
		case LUA_TBOOLEAN:
		case LUA_TNUMBER:
			zg_send_message(self->pd(), name, "f", (float)lua_tonumber(L, 3));
			break;
		case LUA_TSTRING:
			zg_send_message(self->pd(), name, "s", lua_tostring(L, 3));
			break;
		
		case LUA_TTABLE:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		case LUA_TLIGHTUSERDATA:
			luaL_error(L, "cannot send message of type %s", lua_typename(L, lua_type(L, 3)));
			break;
	}
	return 0;
}

//  /**
//   * Send a message to the named receiver with the given format at the given block index. If the
//   * block index is negative or greater than the block size (given when instantiating the graph)
//   * the the message will be sent at the very beginning of the next block. A fractional block index
//   * may be given, and the message will be evaluated between rendered samples. If the given block
//   * index falls outside of the block size (either positive or negative), then the message will be
//   * delivered at the beginning of the block. If no receiver exists with the given name, then this
//   * funtion does nothing.
//   * This function is equivalent to e.g., zg_send_message(graph, "#accelerate", 0.0, "fff", 0.0f, 0.0f, 0.0f)
//   * E.g., zg_send_message_at_blockindex(graph, "#accelerate", 56.3, "fff", 0.0f, 0.0f, 0.0f);
//   * sends a message containing three floats, each with value 0.0f, to all receivers named "#accelerate"
//   * between samples 56th and 57th samples (counting from zero) of the block.
//   */
//  void zg_send_message_at_blockindex(ZGContext *context, const char *receiverName, double blockIndex,
//      const char *messageFormat, ...);
//  
//  /**
//   * Send a midi note message on the given channel to occur at the given block index. The
//   * <code>blockIndex</code> parameter behaves in the same way as in <code>zg_send_message_at_blockindex()</code>.
//   * All messages are sent to <code>notein</code> objects, i.e. omni. Channels are zero-index and only
//   * 16 are supported. A note off message is generally interpreted as having velocity zero.
//   */
//  void zg_send_midinote(ZGContext *context, int channel, int noteNumber, int velocity, double blockIndex);

// registering the methods and properties:
//template<> void Glue<PdContextNode>::usr_mt(lua_State * L) {
//	static luaL_reg funcs[] = {
//		{ "send", lua_pdcontext_send_message },
//		{ "open", lua_pdcontext_open },
//		{ NULL, NULL }
//	};
//	luaL_register(L, NULL, funcs);
//}



//int lua_graph_add_object(lua_State * L) {
//	PdGraph * graph = Glue<PdGraph>::checkto(L, 1);
//	ZGObject * object = Glue<ZGObject>::checkto(L, 1);
//	zg_add_object(graph, object);
//	return 0;
//}
// also void zg_remove_object(ZGGraph *graph, ZGObject *object);



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


/* require */
extern "C" int luaopen_zengarden(lua_State * L) {


	



	// define the module table 
	luaL_Reg lib[] = {
		//{ "clear_search_path", lua_clear_search_path },
		//{ "add_to_search_path", lua_add_to_search_path },
		{ NULL, NULL }
	};
	luaL_register(L, "zengarden", lib);
	
	
	// define the prototype:
	lua_newtable(L);
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "out");
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "input");
	lua_pushnumber(L, 1);
	lua_setfield(L, -2, "amp");
	
	// define the metatable:
	static luaL_reg methods[] = {
		{ "send", lua_pdcontext_send_message },
		{ "open", lua_pdcontext_open },
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
	lua_pushcclosure(L, SynthBinding<PdSynth>::create, 3);

	lua_setfield(L, -2, "PdContext");
	
	
//	static luaL_reg methods[] = {
//		{ "send", lua_pdcontext_send_message },
//		{ "open", lua_pdcontext_open },
//	};
//	static luaL_reg getters[] = {
//		{ NULL, NULL }
//	};
//	static luaL_reg setters[] = {
//		{ "out", LNode<PdContextNode>::port_set<&PdContextNode::out> },
//		//{ "depth", LNode<PdContextNode>::set<sample, &PdContextNode::depth> },
//		//{ "freq", LNode<PdContextNode>::port_set<&PdContextNode::freq> },
//		{ NULL, NULL }
//	};
//	LNode<PdContextNode>::define(L, methods, getters, setters);
//	lua_setfield(L, -2, "PdContext");
//	
//	Glue<PdGraph>::define(L);
	
	return 1;
}
