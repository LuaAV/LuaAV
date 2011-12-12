#ifndef INCLUDE_LUAAV_AUDIO_SYTEM_H
#define INCLUDE_LUAAV_AUDIO_SYTEM_H

#include "luaav_audio_jit.hpp"

#include "allocore/types/al_MsgTube.hpp"
#include "luaav.h"
#include "lua_glue.h"
#include "lua_array.h"

#include "lua_compiler.hpp"
using al::JIT;
using al::JitObject;

#include <stack>
#include <list>
#include <vector>

#define LUAAV_AUDIO_ROOT "LUAAV_AUDIO_ROOT"

extern "C" {
	sample * luaav_audiobuffer_request();
	void luaav_audiobuffer_release(sample * x);
	void luaav_audiobuffer_zero(sample * x);
}

/*!
	For sending messages from audio thread to main thread:
*/
al::MsgTube& luaav_global_outbox();

/*!
	Container for audio signal data
*/
class Signal {
public:
	Signal() : mData(NULL), mOwner(false) { request(); }
	~Signal() { release(); }
	
	sample * data() { return mData; }
	
	void set(sample * ptr) {
		release();
		mOwner = false;
		mData = ptr;
	}
	
	void request() {
		if (mData == NULL) {
			mData = luaav_audiobuffer_request();
			mOwner = true;
		}
	}
	
	void release() {
		if (mOwner) {
			luaav_audiobuffer_release(mData);
		}
		mData = NULL;
	}
	
	void zero() {
		if (mData) luaav_audiobuffer_zero(mData);
	}
	
	sample& operator[](uint32_t i)      { return mData[i]; }
	sample  operator[](uint32_t i) const{ return mData[i]; }
	
protected:
	sample * mData;
	bool mOwner;	// true if this Signal request()ed the data; false if it was set().
};


class Scope {
public:
	Scope(int nchannels=2, int bufferSize=512, int nbuffers=32);
	~Scope();
	
	void nchannels(int n);
	int nchannels() {return mChannels.size();}
	
	al::ArrayWrapper * channel(int idx) {
		if(idx >= 0 && idx < mChannels.size()) {
			return mChannels[idx];
		}
		else {
			return 0;
		}
	}
	
	void bufferSize(int sz);
	int bufferSize() {return mBufferSize;}
	
	void advance(int n);
	void write(float leftSample, float rightSample);
	
	int position() {return mSamplePos;}

protected:
	int mSamplePos;
	int mBufferSize;
	int mNBuffers;
	int mNSamples;
	std::vector<al::ArrayWrapper *> mChannels;

};


///*!
//	An active node in the synthesis graph (contexts & synths)
//*/
//class Node {
//public:
//
//	enum State {
//		CREATED = 0,	// initial state when allocated
//		PLAYING,		
//		STOPPING,		// used by audio thread only
//		STOPPED,
//		DEAD
//	};
//	
//	virtual void audioCB() = 0;
//	
//	///! Both context and impl are *required*
//	Node(Context * context, std::string name);
//	virtual ~Node() {};
//	
//	///! Return name (class/type identifier):
//	const std::string name() const { return mName; }
//	
//	///! get the context of this Node:
//	Context * context() const { return mContext; }
//	
//	// *** MAIN THREAD METHODS: *** //
//	
//	///! schedule this Node in its context
//	// (call after all initialization is complete)
//	void play();
//	///! schedule the removal of this Node
//	void stop();
//	
//	
//	
//	///! reference counting for Lua userdata:
//	void retain() { mLuaRefs++; }
//	bool release();
//	int refs() const { return mLuaRefs; }
//	
//	///! current state of Node (in each thread):
//	State& stateL() { return mStateL; }
//	State stateA() const { return mStateA; }
//	
//	///! return true if setting data on this node makes any sense
//	bool valid() { return mStateL < STOPPED && mStateA < STOPPED; }
//	
//	// *** AUDIO THREAD METHODS: *** //
//	
//	///! Iterate frame counter, returning true while more frames
//	bool io() { return (mStateA == PLAYING) && ((++mFrame) < mEnd); }
//	
//	///! Signal this node to stop synthesizing audio
//	void audio_stop() { if (mStateA == PLAYING) { mStateA = STOPPING; } }
//	
//	///! Get current frame number
//	int frame() const { return mFrame; }
//	
//	// trigger the deletion of node from within audio thread
//	// (sends as message to main thread):
//	void audio_delete();
//	
//	/// Get input sample at specified channel and frame
//	// const sample& in(int chan) const { return tmp; }
//
//	/// Get output sample at current frame iteration on specified channel
//	// sample& out(int chan) { return tmp; }
//	
//	/// Add value to current output sample on specified channel
//	//void sum(sample v, int chan) { out(chan)+=v; }
//	
//	/// Add value to current output sample on specified channels
//	//void sum(sample v, int ch1, int ch2) { sum(v, ch1); sum(v,ch2); }
//
//	/// Get bus sample at current frame iteration on specified channel
//	//sample& bus(int chan) const;
//	
//	//void * user() const{ return mUser; } ///< Get pointer to user data
//	//template<class UserDataType>
//	//UserDataType& user() const { return *(static_cast<UserDataType *>(mUser)); }
//	
////	int channelsIn () const;			///< Get effective number of input channels
////	int channelsOut() const;			///< Get effective number of output channels
////	int channelsBus() const;			///< Get number of allocated bus channels
////	int channelsInDevice() const;		///< Get number of channels opened on input device
////	int channelsOutDevice() const;		///< Get number of channels opened on output device
////	int framesPerBuffer() const;		///< Get frames/buffer of audio I/O stream
////	double framesPerSecond() const;		///< Get frames/second of audio I/O streams
////	double fps() const { return framesPerSecond(); } /// AKA samplerate
////	double secondsPerBuffer() const;	///< Get seconds/buffer of audio I/O stream
////	double time() const;				///< Get current stream time in seconds
////	double time(int frame) const;		///< Get current stream time in seconds of frame
////	void zeroBus();						///< Zeros all the bus buffers
//	
//	// called prior to each audioCB:
//	void prepare(int start=0, int end=LUAAV_AUDIO_BLOCKSIZE) { 
//		mStart = start; 
//		mEnd = end; 
//		mFrame = start-1; 
//	}
//	
//	static void msg_play(al_sec t, Node * node, Context * self);
//	static void msg_stop(al_sec t, Node * self);
//	static void msg_delete(al_sec t, Node * self);
//	
//protected:
//	friend class Context;
//	friend class Glue<Node>;
//	
//	std::string mName;
//	Context * mContext;
//	int mStart, mEnd;
//	int mFrame;
//	int mLuaRefs;
//	State mStateL, mStateA; // state in Lua and Audio threads respectively
//};	


/*!
	Contexts can hold:
		a growable set of busses,
		a modifiable list of active sub-synths and sub-contexts
*/
class Context : public Synth {
public:
	
	Context(Context * parent, std::string name) 
	:	Synth(0), 
		mRoot(parent?parent->root():(luaav_state *)this)
	{
		mContext = parent;
	}
		
	virtual ~Context();
	
	///! Each context has its own queue of incoming messages
	al::MsgTube& inbox() { return mInbox; }
	
	///! Contexts belong to a single LuaAVState:
	luaav_state * root() { return mRoot; }
	
	///! Audio thread: add node to context's list
	void add(Synth * n) { 
		if (n->mStateA == CREATED) {
			n->mStateA = Synth::PLAYING; 
			mSynths.push_front(n); 
		} else {
			printf("WARNING: attempt to add a node that is already added\n");
		}
	}
	
	void addSignal(Signal * s) { mSignals.push_back(s); }
	
	virtual const char * name() { return "Context"; }
	virtual void audioCB();	
	
	al_sec now() const { return luaav_state_now(mRoot); }
	
protected:
	
	///! growable stack of signal busses
	std::vector<Signal *> mSignals;
	
	///! modifiable ordered list of sub-nodes:
	std::list<Synth *> mSynths;
	
	///! Reference back to the containing LuaAVState:
	luaav_state * mRoot;
	
	///! Messaging to the audio thread:
	al::MsgTube mInbox;
	
	std::string mName;
};


#pragma mark Lua bindings: Signal

template<>
inline const char * Glue<Signal>::mt_name(lua_State * L) {
	return "LuaAV_Signal_Metatable";
}

template<> 
inline const char * Glue<Signal>::usr_name() {
	return "Signal";
}


//#pragma mark Lua bindings: Node
//
//template<>
//inline const char * Glue<Node>::mt_name(lua_State * L) {
//	return "LuaAV_Node_Metatable";
//}
//
//template<> 
//inline const char * Glue<Node>::usr_name() {
//	return "Node";
//}
//
////template <> 
////inline int Glue<Node> :: usr_tostring(lua_State * L, Node * u) { 
////	lua_pushfstring(L, "Node(%s): %p", u->implName(), u); 
////	return 1; 
////}
//
//template<> 
//inline void Glue<Node> :: usr_push(lua_State * L, Node * self) {
//	self->retain();	// one ref for Lua
//}
//
//template<> 
//inline void Glue<Node> :: usr_gc(lua_State * L, Node * self) {
//	// if Lua no longer references the Node, and the Node is not scheduled,
//	// delete the Node now.
//	if (self->release()) {
//		if (self->mStateA == Node::STOPPED) {
//			delete self;	// node is dead to Lua and to Audio
//		} else {
//			self->mStateL = Node::DEAD;	// this node is dead to Lua
//		}
//	} 
//}
//
//template<> 
//inline void Glue<Node>::usr_index(lua_State * L, Node * u) {
//	Node * n = Glue<Node>::checkto(L, 1);
//	if (n->mStateL <= Node::PLAYING) {
//		lua_getmetatable(L, 1);
//		lua_pushvalue(L, 2);
//		lua_rawget(L, -2);
//		if(lua_isnil(L, -1)) {
//			lua_pop(L, 2);
//			lua_getfenv(L, 1);
//			lua_pushvalue(L, 2);
//			lua_rawget(L, -2);
//		}
//	} 
//}
//
//template<> 
//inline void Glue<Node>::usr_newindex(lua_State * L, Node * n) {
//	if (n->mStateL <= Node::PLAYING) {
//	//if (u->active()) {
//		lua_getfenv(L, 1); 
//		lua_pushvalue(L, 2);
//		lua_gettable(L, -2);
//		if (lua_isfunction(L, -1)) {
//			lua_pushvalue(L, 1);
//			lua_pushvalue(L, 3);
//			lua_call(L, 2, 0);
//		} else {
//			luaL_error(L, "attempt to set non-existent member %s", lua_tostring(L, 2));
//		}
//	} else {
//		// expired node: get rid of the metatable
//		luaL_error(L, "attempt to set dead Node");
//	}
//}

//
///// conveniently conform to the lua:: utility specializations:
//namespace lua {
//	template<> 
//	inline void to(lua_State * L, int idx, Node * v) { 
//		v = Glue<Node>::checkto(L, idx); 
//	}
//	
//	template<> 
//	inline Node * to(lua_State * L, int idx) { 
//		return Glue<Node>::checkto(L, idx); 
//	}
//	
//	template<> 
//	inline bool is<Node>(lua_State * L, int idx) { 
//		return NULL != Glue<Node>::to(L, idx); 
//	}
//	
//	template<> 
//	inline const Node * opt(lua_State * L, int idx, Node * v) {
//		Node * r = Glue<Node>::to(L, idx);
//		return r ? r : v;
//	}
//	
//	template<> 
//	inline Node * opt(lua_State * L, int idx, Node * v) {
//		Node * r = Glue<Node>::to(L, idx);
//		return r ? r : v;
//	}
//
//} // lua::



#pragma mark AudioArray

void msgAudioReleaseArrayHandle(al_sec t, ArrayHandle handle);

///*!
//	Base-class for any ugen-like object that makes use of an Array
//*/
//struct AudioArray {
//
//	AudioArray() : mArray(NULL) {}
//	virtual ~AudioArray() { 
//		if (mArray) mArray->release();
//	}
//	
//	al::Array * array() const { return mArray; }
//	
//	void write(sample v, int idx) {
//		float value = (float)v;
//		if (mArray) mArray->write(&value, idx % mArray->header.dim[0]);
//	}
//	
//	// entry point for Lua:
//	template <typename T, AudioArray T::* member>
//	static int lua_set(lua_State * L) {
//		Node * node = Glue<Node>::checkto(L, 1);
//		ArrayHandle a = lua_array_checkto(L, 2);
//		T * self; // = node->impl<T>();
//		a->retain();
//		//printf("setting array %d\n", a->refs());
//		node->context()->inbox().send(msg_set<T, member>, self, a);
//		return 0;
//	}
//	
//protected:
//	// called in audio thread:
//	void set(ArrayHandle a) {
//		if (mArray) luaav_global_outbox().send(0, msgAudioReleaseArrayHandle, mArray);
//		mArray = a;
//	}
//	
//	// message sent from Lua to audio thread:
//	template<typename T, AudioArray T::* member>
//	static void msg_set(al_sec t, T * self, ArrayHandle a) {
//		(self->*member).set(a);
//	}
//
//	ArrayHandle mArray;
//};

#pragma mark Port

///*!
//	Ports allow nodes to (optionally) communicate via Signals
//*/
//class Port {
//public:
//
//	/*
//		An individual port channel (can be a signal pointer or numeric value
//	*/
//	class Channel {
//	public:
//		uint32_t mIsSignal;
//		sample * mPtr;
//		sample mValue;
//		
//		Channel(sample value = 0) : mValue(value), mPtr(&mValue), mIsSignal(0) {}
//		Channel(Signal * value) : mPtr(value->data()), mIsSignal(1) {}
//		Channel(const Channel& cpy) : mIsSignal(cpy.mIsSignal), mValue(cpy.mValue), mPtr(mIsSignal ? cpy.mPtr : &mValue) { /*printf("PortChannel copy ctor\n");*/ }
//		
//		Channel& operator = (const Channel& other) {
//			mValue = other.mValue;
//			mIsSignal = other.mIsSignal;
//			mPtr = mIsSignal ? other.mPtr : &mValue;
//			return *this;
//		}
//		
//		void setvalue(sample n) { 
//			mValue = n; 
//			mIsSignal = 0; 
//			mPtr = &mValue;
//		}
//		
//		void set(Signal * p) { 
//			mPtr = p->data(); 
//			mIsSignal = 1; 
//		}
//		
//		sample& operator[](uint32_t i) {
//			return mPtr[mIsSignal * i];
//		}
//	};
//	
//	
//	/*
//		Convenience wrapper to read from a PortChannel in the audio callback:
//	*/
//	class Reader {
//	public:
//		
//		// constructor/assignment operator set the AudioIOData source of the PortReader
//		Reader(Node& owner, Channel data);
//		
//		// retrieve the current value of this PortReader
//		sample& operator()();	
//
//		friend class Node;	
//		Node& mOwner;
//		Channel mData;
//	};
//
//
//	Port(int chans=1) { channels(chans); }
//	void channels(int chans) { mChannels.resize(chans); }
//	int channels() const { return mChannels.size(); }
//
//	Reader read(Node& owner, int channel) {
//		return Reader(owner, mChannels[channel]);
//	}
//	
//	template<typename T, Port T::*member>
//	static void msg(al_sec t, T * self, int offset, int chans, sample v) {
//		for (int i=offset; i<offset+chans; i++) {
//			(self->*member).mChannels[i].setvalue(v);
//		}
//	}
//	
//	template<typename T, Port T::*member>
//	static void msg(al_sec t, T * self, int offset, int chans, Signal * v) {
//		for (int i=offset; i<offset+chans; i++) {
//			(self->*member).mChannels[i].set(v);
//		}
//	}
//	
//	Channel& operator[](uint32_t i) { return mChannels[i]; }
//protected:
//	std::vector<Channel> mChannels;
//};




#pragma mark inline

//inline void Node :: play() {
//	if (mStateL == CREATED) {
//		mStateL = PLAYING;
//		mContext->inbox().send(mContext->now(), Node::msg_play, this, mContext);
//	} else {
//		printf("warning: attempted multiple play of node %p\n", this);
//	}
//}
//
//inline void Node :: audio_delete() {
//	luaav_global_outbox().send(mContext->now(), Node::msg_delete, this);
//}
//
//inline Port::Reader :: Reader(Node& owner, Port::Channel data) 
//: mOwner(owner), mData(data) {}
//
//inline sample& Port::Reader :: operator()() {
//	return mData.mPtr[mData.mIsSignal * mOwner.frame()];
//}	


void msgSetArrayHandle(al_sec t, ArrayHandle * dst, ArrayHandle handle);

inline void setArrayHandle(Context * ctx, ArrayHandle * dst, ArrayHandle handle) {
	handle->retain();
	ctx->inbox().send(ctx->now(), msgSetArrayHandle, dst, handle);
}


#endif /* include guard */
