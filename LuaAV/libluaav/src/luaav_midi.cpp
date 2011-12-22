#include "luaav_state.hpp"
#include "lua_glue.h"
#include "RtMidi.h"

#define MODULE_NAME (midi) 
#define MIDI_NOTEOFF (0x80)
#define MIDI_NOTEON (0x90)
#define MIDI_AFTERTOUCH (0xA0)
#define MIDI_CONTROLCHANGE (0xB0)
#define MIDI_PROGRAMCHANGE (0xC0)
#define MIDI_CHANNELPRESSURE (0xD0)
#define MIDI_PITCHWHEEL (0xE0)
#define MIDI_STATUS (0xF0)

/*! Real-time MIDI IO
	
	@module midi
*/


/*!	Get list of available MIDI input ports
	@ret table list of input port names
	@name inputs
*/
int lua_midi_inports(lua_State * L)
{
	RtMidiIn * u;
	try {
		u = new RtMidiIn();
	} catch (RtError & error) {
		luaL_error(L, "%s\n", error.getMessageString());
		return 0;
	}
	
	unsigned int nPorts;
	try {
		nPorts = u->getPortCount();
	}
	catch ( RtError &error ) {
		luaL_error(L, "%s\n", error.getMessageString());
		delete u;
		return 0;
	}
	lua_createtable(L, nPorts, 0);
	
	std::string portName;
	for ( unsigned int i=0; i<nPorts; i++ ) {
		try {
			portName = u->getPortName(i);
			lua_pushstring(L, portName.c_str());
		}
		catch ( RtError &error ) {
			// ignore?
			luaL_error(L, "%s\n", error.getMessageString());
			delete u;
			return 0;
		}
		lua_rawseti(L, -2, i+1);
	}
	
	delete u;
	
	return 1;
}

/*!	Get list of available MIDI output ports
	@ret table list of output port names
	@name outputs
*/
int lua_midi_outports(lua_State * L)
{
	RtMidiOut * u;
	try {
		u = new RtMidiOut();
	} catch (RtError & error) {
		luaL_error(L, "%s\n", error.getMessageString());
		return 0;
	}
	
	unsigned int nPorts;
	try {
		nPorts = u->getPortCount();
	}
	catch ( RtError &error ) {
		luaL_error(L, "%s\n", error.getMessageString());
		delete u;
		return 0;
	}
	lua_createtable(L, nPorts, 0);
	
	std::string portName;
	for ( unsigned int i=0; i<nPorts; i++ ) {
		try {
			portName = u->getPortName(i);
			lua_pushstring(L, portName.c_str());
		}
		catch ( RtError &error ) {
			// ignore?
			luaL_error(L, "%s\n", error.getMessageString());
			delete u;
			return 0;
		}
		lua_rawseti(L, -2, i+1);
	}
	
	delete u;
	
	return 1;
}


unsigned short combineBytes(unsigned char first, unsigned char second) {
	unsigned short _14bit = (unsigned short)second;
	_14bit <<= 7;
	_14bit |= (unsigned short)first;
	return _14bit;
}

template<typename PORT>
int findport(PORT * a, std::string name) {
	unsigned int nPorts;
	try {
		nPorts = a->getPortCount();
	}
	catch ( RtError &error ) {
		printf("%s\n", error.getMessageString());
		return 0;
	}
	
	std::string portName;
	for ( unsigned int i=0; i<nPorts; i++ ) {
		try {
			portName = a->getPortName(i);
			if (portName == name) {
				return i;	
			}
		} catch ( RtError &error ) {
			// ignore for now
			printf("%s\n", error.getMessageString());
		}
	} 
	return 0;
}


/*	Real-time MIDI input
	@module midi.Input
*/
#pragma mark midi.Input

template<> const char * Glue<RtMidiIn>::usr_name() { return "Input"; }

template<> RtMidiIn * Glue<RtMidiIn>::usr_new(lua_State * L) {
	RtMidiIn * u;
	try {
		u = new RtMidiIn();
	} catch (RtError & error) {
		luaL_error(L, "%s\n", error.getMessageString());
		return NULL;
	}
	
	try {
		// handle optional arguments:
		if (lua_isnumber(L, 1)) {
			// integer port number
			u->openPort(abs(luaL_optnumber(L, 1, 1)-1));
		} else if (lua_isstring(L, 1)) {
			// string port name?
			std::string portname = std::string(lua_tostring(L, 1));
			int matchedport = findport<RtMidiIn>(u, portname);
			if (matchedport >= 0) {							
				u->openPort(matchedport);
			} else {
				// then string new virtual port name
				u->openVirtualPort(portname);
			}
		} else {
			// open default port
			u->openPort();
		}
	} catch (RtError &error) {
		delete u;
		luaL_error(L, "%s\n", error.getMessageString());
		return NULL;	
	}
	
	return u;
}

// TODO: does this close the port?
template<> void Glue<RtMidiIn>::usr_gc(lua_State * L, RtMidiIn * u) { 
	delete u;
}


/*! Constructor
	@param port optional port number or port name
	@ret Input
	@name Input
	@see midi.inputs
	@function
*/

/*!	Set MIDI internal queue buffer size
	@param bytes integer size
	@name M:queuesize
*/
static int lua_midi_input_queuesize(lua_State * L) {
	RtMidiIn * u = Glue<RtMidiIn>::checkto(L, 1);
	u->setQueueSizeLimit(abs(luaL_checkint(L, 2)));
	return 0;
}

/*!	Set MIDI message types to ignore
	@param sysex boolean ignore MIDI SysEx messages
	@param time boolean ignore MIDI timing messages
	@param sense boolean ignore MIDI sense messages
	@name M:ignoretypes
*/
static int lua_midi_input_ignoretypes(lua_State * L) {
	RtMidiIn * u = Glue<RtMidiIn>::checkto(L, 1);
	bool midiSysex = lua_toboolean(L, 2);
	bool midiTime = lua_toboolean(L, 3);
	bool midiSense = lua_toboolean(L, 4);
	u->ignoreTypes( midiSysex, midiTime, midiSense );
	return 0;
}

static int midi_parse(lua_State * L, std::vector<unsigned char> & message) {
	int nBytes = message.size();
	int nReturns = 0;
	for (int i=0; i<nBytes; ) {
		
		unsigned char statusbyte = message[i++];
		
		unsigned int channel, type;
		type = ((statusbyte >> 4) & 0xf);
		channel = (statusbyte & 0xf);
		
		switch(type) {
			case 8:
				lua_pushstring(L, "noteoff");			lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				break;
			case 9:
				lua_pushstring(L, "noteon"); 			lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				break;
			case 10:
				lua_pushstring(L, "aftertouch"); 		lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				break;
			case 11:
				lua_pushstring(L, "cc");				lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				break;
			case 12:
				lua_pushstring(L, "program");			lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				break;
			case 13:
				lua_pushstring(L, "pressure");			lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				lua_pushinteger(L, (int)message[i++]);	lua_rawseti(L, -2, ++nReturns);
				break;
			case 14: {
				lua_pushstring(L, "wheel");				lua_setfield(L, -2, "event");
				lua_pushinteger(L, channel);			lua_rawseti(L, -2, ++nReturns);
				unsigned char first = message[i++];
				unsigned char second = message[i++];
				lua_pushinteger(L, combineBytes(first, second));	lua_rawseti(L, -2, ++nReturns);
				break;
			}
			default:
				switch (statusbyte) {
					case 0xF0:
						lua_pushstring(L, "sysex_begin");				lua_setfield(L, -2, "event");
						// at this point, we should 'while loop' until OxF7.  TODO
						break;
					case 0xF1:
						lua_pushstring(L, "mtc");						lua_setfield(L, -2, "event");
						lua_pushinteger(L, (int)message[i++]);			lua_rawseti(L, -2, ++nReturns);
						break;
					case 0xF2: {
						lua_pushstring(L, "song_position"); 			lua_setfield(L, -2, "event");
						unsigned char first = message[i++];
						unsigned char second = message[i++];
						lua_pushinteger(L, combineBytes(first, second)); lua_rawseti(L, -2, ++nReturns);
						break;
					}
					case 0xF3:
						lua_pushstring(L, "song_select");				lua_setfield(L, -2, "event");
						lua_pushinteger(L, (int)message[i++]);			lua_rawseti(L, -2, ++nReturns);
						break;
					case 0xF4:
						lua_pushstring(L, "0xF4");						lua_setfield(L, -2, "event");
						break;
					case 0xF5:
						lua_pushstring(L, "0xF5");						lua_setfield(L, -2, "event");
						break;
					case 0xF6:
						lua_pushstring(L, "tune_request");				lua_setfield(L, -2, "event");
						break;
					case 0xF7:
						lua_pushstring(L, "sysex_end");					lua_setfield(L, -2, "event");
						break;
					case 0xF8:
						lua_pushstring(L, "clock");						lua_setfield(L, -2, "event");
						break;
					case 0xF9:
						lua_pushstring(L, "tick");						lua_setfield(L, -2, "event");
						break;
					case 0xFA:
						lua_pushstring(L, "start");						lua_setfield(L, -2, "event");
						break;
					case 0xFB:
						lua_pushstring(L, "continue");					lua_setfield(L, -2, "event");
						break;
					case 0xFC:
						lua_pushstring(L, "stop");						lua_setfield(L, -2, "event");
						break;
					case 0xFD:
						lua_pushstring(L, "0xFD");						lua_setfield(L, -2, "event");
						break;
					case 0xFE:
						lua_pushstring(L, "active_sense");				lua_setfield(L, -2, "event");
						break;
					default:
						lua_pushstring(L, "reset");						lua_setfield(L, -2, "event");
						break;
				}
		}
	}
	return 1;
}

static int lua_midi_input_iter(lua_State * L) {
	RtMidiIn * u = Glue<RtMidiIn>::to(L, lua_upvalueindex(1));
	double deltaTime;
	std::vector<unsigned char> message;
	try {
		deltaTime = u->getMessage( &message );
		if (message.size() == 0) {
			lua_pushnil(L);
			return 1;
		}
	} catch(RtError & error) {
		luaL_error(L, "%s\n", error.getMessageString());
		return 0;
	}
	
	lua_newtable(L);
	lua_pushnumber(L, deltaTime); lua_setfield(L, -2, "t");
	return midi_parse(L, message); 
}

/*!	Poll messages (retrieve from the driver queue)
	Returns an iterator; each iteration returns a message as a table (m), with message arguments in the array portion, and message event type in the m.event field. The main message event types are:
	<ul>
		<li>noteon (channel, note, velocity)</li>
		<li>noteoff (channel, note, velocity)</li>
		<li>aftertouch (channel, note, velocity)</li>
		<li>cc (channel, controller, value)</li>
		<li>program (channel, program)</li>
		<li>pressure (channel, pressure)</li>
		<li>wheel (channel, bendvalue)</li>
	</ul>
	
	<br />Other message events include: reset, sysex_begin, sysex_end, mtc, song_position, 
	song_select, tune_request, clock, tick, start, continue, stop, active_sense
	Example: 
	<luacode>
	for m in input:poll() do
		print(m.event, unpack(m))
	end
	</luacode>
	
	@ret iterator
	@name M:poll
*/
static int lua_midi_input_poll(lua_State * L) {
	Glue<RtMidiIn>::checkto(L, 1);
	lua_pushcclosure(L, lua_midi_input_iter, 1);
	return 1;
}


template<> void Glue<RtMidiIn>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, lua_midi_input_ignoretypes); lua_setfield(L, -2, "ignoretypes");
	lua_pushcfunction(L, lua_midi_input_queuesize); lua_setfield(L, -2, "queuesize");
	lua_pushcfunction(L, lua_midi_input_poll); lua_setfield(L, -2, "poll");
}


/*	Real-time MIDI output
	@module midi.Output
*/
#pragma mark midi.Output

template<> const char * Glue<RtMidiOut>::usr_name() { return "Output"; }

template<> RtMidiOut * Glue<RtMidiOut>::usr_new(lua_State * L) {
	RtMidiOut * u;
	try {
		u = new RtMidiOut();
	} catch (RtError & error) {
		luaL_error(L, "%s\n", error.getMessageString());
		return NULL;
	}
	
	try {
		// handle optional arguments:
		if (lua_isnumber(L, 1)) {
			// integer port number
			u->openPort(abs(luaL_optnumber(L, 1, 1)-1));
		} else if (lua_isstring(L, 1)) {
			// string port name?
			std::string portname = std::string(lua_tostring(L, 1));
			int matchedport = findport<RtMidiOut>(u, portname);
			if (matchedport >= 0) {							
				u->openPort(matchedport);
			} else {
				// then string new virtual port name
				u->openVirtualPort(portname);
			}
		} else {
			// open default port
			u->openPort();
		}
	} catch (RtError &error) {
		delete u;
		luaL_error(L, "%s\n", error.getMessageString());
		return NULL;	
	}
	
	return u;
}

/*! Constructor
	@param port optional port number or port name
	@ret Output
	@name Output
	@see midi.outputs
	@function
*/

/*!	Send noteon message
	@param channel MIDI channel
	@param note MIDI note number (0..127)
	@param velocity MIDI velocity (0..127)
	@ret string

	@name M:noteon
*/
int lua_midi_output_noteon(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		std::vector<unsigned char> message;
		message.push_back((unsigned char)(((int)(luaL_optnumber(L, 2, 1) - 1)) & 15) + MIDI_NOTEON);
		message.push_back((unsigned char)(luaL_optnumber(L, 3, 64)));
		message.push_back((unsigned char)(luaL_optnumber(L, 4, 64)));
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}
	return 0;
}

/*!	Send noteoff message
	@param channel MIDI channel
	@param note MIDI note number (0..127)
	@param velocity MIDI release velocity (0..127)
	@ret string

	@name M:noteoff
*/
int lua_midi_output_noteoff(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		std::vector<unsigned char> message;
		message.push_back((unsigned char)(((int)(luaL_optnumber(L, 2, 1) - 1)) & 15) + MIDI_NOTEOFF);
		message.push_back((unsigned char)(luaL_optnumber(L, 3, 64)));
		message.push_back((unsigned char)(luaL_optnumber(L, 4, 0)));
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}
	return 0;
}

/*!	Send aftertouch message
	@param channel MIDI channel
	@param note MIDI note number (0..127)
	@param velocity MIDI velocity (0..127)
	@ret string

	@name M:aftertouch
*/
int lua_midi_output_aftertouch(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		std::vector<unsigned char> message;
		message.push_back((unsigned char)(((int)(luaL_optnumber(L, 2, 1) - 1)) & 15) + MIDI_AFTERTOUCH);
		message.push_back((unsigned char)(luaL_optnumber(L, 3, 64)));
		message.push_back((unsigned char)(luaL_optnumber(L, 4, 64)));
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}
	return 0;
}

/*!	Send control change message
	@param channel MIDI channel
	@param controller MIDI controller number (0..127)
	@param value MIDI value (0..127)
	@ret string

	@name M:cc
*/
int lua_midi_output_cc(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		std::vector<unsigned char> message;
		message.push_back((unsigned char)(((int)(luaL_optnumber(L, 2, 1) - 1)) & 15) + MIDI_CONTROLCHANGE);
		message.push_back((unsigned char)(luaL_optnumber(L, 3, 7)));
		message.push_back((unsigned char)(luaL_optnumber(L, 4, 64)));
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}
	return 0;
}

/*!	Send program change message
	@param channel MIDI channel
	@param program MIDI program number (0..127)
	@ret string

	@name M:program
*/
int lua_midi_output_program(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		std::vector<unsigned char> message;
		message.push_back((unsigned char)(((int)(luaL_optnumber(L, 2, 1) - 1)) & 15) + MIDI_PROGRAMCHANGE);
		message.push_back((unsigned char)(luaL_optnumber(L, 3, 1)));
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}
	return 0;
}

/*!	All notes off
	@param channel MIDI channel
	@ret string

	@name M:allnotesoff
*/
int lua_midi_output_allnotesoff(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		printf("allnotesoff\n");
		std::vector<unsigned char> message;
		message.push_back((unsigned char)(((int)(luaL_optnumber(L, 2, 1) - 1)) & 15) + MIDI_CONTROLCHANGE);
		message.push_back((unsigned char)123);
		message.push_back((unsigned char)0);
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}
	return 0;
}

/*!	Send raw MIDI bytes
	@param ... numbers (MIDI bytecodes)

	@name M:bytes
*/
int lua_midi_output_writebytes(lua_State * L) {
	RtMidiOut * a = Glue<RtMidiOut>::checkto(L, 1);
	if (a) {
		std::vector<unsigned char> message;
		
		// read in message here
		//if (lua_istable(L, -1)) {
//				// iterate table and push each value
//			} else {
			// iterate arguments and push each value
			for (int i=2; i<=lua_gettop(L); i++) {
				message.push_back((unsigned char)((unsigned int)luaL_optnumber(L, i, 0)));
			}
		//}
		
		try {
			a->sendMessage(&message);
		} catch (RtError &error) {
			printf("%s\n", error.getMessageString());
		}
	}	
	return 0;
}

template<> void Glue<RtMidiOut>::usr_gc(lua_State * L, RtMidiOut * u) { 
	lua_midi_output_allnotesoff(L);
	delete u;
}

template<> void Glue<RtMidiOut>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, lua_midi_output_noteon); lua_setfield(L, -2, "noteon");
	lua_pushcfunction(L, lua_midi_output_noteoff); lua_setfield(L, -2, "noteoff");
	lua_pushcfunction(L, lua_midi_output_aftertouch); lua_setfield(L, -2, "aftertouch");
	lua_pushcfunction(L, lua_midi_output_cc); lua_setfield(L, -2, "cc");
	lua_pushcfunction(L, lua_midi_output_program); lua_setfield(L, -2, "program");
	lua_pushcfunction(L, lua_midi_output_allnotesoff); lua_setfield(L, -2, "allnotesoff");
	lua_pushcfunction(L, lua_midi_output_writebytes); lua_setfield(L, -2, "bytes");
}

/*
	luaopen_midi
*/
#pragma mark luaopen_midi


extern "C" int luaopen_midi(lua_State * L) {
	const char * libname = luaL_optstring(L, 1, "midi");
	struct luaL_reg lib[] = {
		{"inports", lua_midi_inports},
		{"outports", lua_midi_outports},
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);
	
	Glue<RtMidiIn>::define(L);		Glue<RtMidiIn>::register_ctor(L);
	Glue<RtMidiOut>::define(L);	Glue<RtMidiOut>::register_ctor(L);
	
	return 1;
}
