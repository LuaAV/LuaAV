#include "luaav_state.hpp"
#include "lua_glue.h"

/* Apache Portable Runtime */
#include "../allocore/private/al_ImplAPR.h"
#ifdef AL_LINUX
	#include "apr-1.0/apr_network_io.h"
#else
	#include "apr-1/apr_network_io.h"
#endif

/* oscpack */
#include "../oscpack/osc/OscPacketListener.h"
#include "../oscpack/osc/OscOutboundPacketStream.h"
#include "../oscpack/osc/OscTypes.h"

#define OSC_DEFAULT_PORT 7007
#define OSC_DEFAULT_MAXPACKETSIZE 4096

static apr_status_t lua_check_apr(lua_State * L, apr_status_t err);


/*! Open Sound Control over the network 
	

	@module osc
*/


/*! Get the current machine's host name
	@LuaMethod METHOD
	@name M.hostname
*/
int lua_osc_hostname(lua_State * L) {
	//LuaAVState * S = luaav_fromstate(L);
	char hostname[APRMAXHOSTLEN+1];
	
	al::ImplAPR apr;
	lua_check_apr(L, apr_gethostname(hostname, APRMAXHOSTLEN+1, apr.pool()));
	lua_pushstring(L, hostname);
	return 1;
}


/* APR utility */
static apr_status_t lua_check_apr(lua_State * L, apr_status_t err) {
	char errstr[1024];
	if (err != APR_SUCCESS) {
		apr_strerror(err, errstr, 1024);
		luaL_error(L, "%s\n", errstr);
	}
	return err;
}

/* doesn't throw the error, just pushes it on the stack */
static int lua_status_apr(lua_State * L, apr_status_t err) {
	char errstr[1024];
	if (err != APR_SUCCESS) {
		apr_strerror(err, errstr, 1024);
		lua_pushnil(L);
		lua_pushfstring(L, "%s\n", errstr);
		return 2;
	}
	return 0;
}

/*
	Binding to osc::Blob
*/
#pragma mark osc::Blob
template<> const char * Glue<osc::Blob>::usr_name() { return "Blob"; }
template<> void Glue<osc::Blob>::usr_gc(lua_State * L, osc::Blob * u) { 
	delete u;
}
template<> void Glue<osc::Blob>::usr_mt(lua_State * L) {
	// any method to retrieve the data?
}

/* 
	OSC packet parser 
*/
#pragma mark packet parser
int osc_parsemessage(lua_State * L, const osc::ReceivedMessage & p) {
	lua_newtable(L);
		
	lua_pushstring(L, "addr");
	lua_pushstring(L, p.AddressPattern() ? p.AddressPattern() : "");
	lua_rawset(L, -3);
	
	unsigned long argc = p.ArgumentCount();
	const char *tags   = p.TypeTags() ? p.TypeTags() : "";
	lua_pushstring(L, "types");
	lua_pushstring(L, tags);
	lua_rawset(L, -3);
	
	osc::ReceivedMessageArgumentStream args = p.ArgumentStream();
	
	for(int i=0; i < argc; i++) {
		try {
			switch(tags[i])
			{
				/*
					Standard OSC types
				*/
				case 'f': {
					float v; args >> v;
					lua_pushnumber(L, v);
				} break;
				case 'i': {
					osc::int32 v; args >> v;
					lua_pushinteger(L, v);
				} break;
				case 's': {
					const char * v; args >> v;
					lua_pushstring(L, v);
				} break;
				case 'b': {
					osc::Blob * v = new osc::Blob();
					args >> *v;
					Glue<osc::Blob>::push(L, v);
					
				} break;

				/*
					Non-Standard OSC types
				*/
				case 'h': {
					osc::int64 v; args >> v;
					lua_pushstring(L, "64-bit ints not handled yet");
				} break;

				case 't': {
					osc::TimeTag v; args >> v;
					lua_pushstring(L, "time-tags not handled yet");
				} break;

				case 'd': {
					double v; args >> v;
					lua_pushnumber(L, v);
				} break;
				case 'S': {
					const char * v; args >> v;
					lua_pushstring(L, v);
				} break;
				case 'c': {
					char v; args >> v;
					lua_pushstring(L, &v);
				} break;

				case 'r': {
					osc::RgbaColor v; args >> v;
					lua_pushstring(L, "RGBA-tags not handled yet");
				} break;
				case 'm': {
					osc::MidiMessage v; args >> v;
					lua_pushstring(L, "MIDI-tags not handled yet");
				} break;

				case 'T': {
					args.grahamWantsToIgnoreThisArgument();
					lua_pushboolean(L, true);
				} break;
				case 'F': {
					args.grahamWantsToIgnoreThisArgument();
					lua_pushboolean(L, false);
				} break;
				case 'N': {
					args.grahamWantsToIgnoreThisArgument();
					lua_pushnil(L);
				} break;
				case 'I': {
					args.grahamWantsToIgnoreThisArgument();
					lua_pushnumber(L, 696969);
				} break;

				case '[':
				case ']': {
					args.grahamWantsToIgnoreThisArgument();
					lua_pushstring(L, "[]-tags not handled yet");
				} break;

				default: {
					// q: how to increment args stream??
					args.grahamWantsToIgnoreThisArgument();
					lua_pushnil(L);
				} break;
			}
		
		}
		catch(const osc::WrongArgumentTypeException &e) {
			fprintf(stderr, "Recv.recv: wrong argument in message\n");
			lua_pushnil(L);
		}
		catch(...) {	// all other exceptions
			fprintf(stderr, "Recv.recv: error handling messages\n");
			lua_pushnil(L);
		}	
		lua_rawseti(L, -2, i+1);
	}
	return 1;
}

int osc_parsebundle(lua_State * L, const osc::ReceivedBundle & p) {
	int nret = 0;
	for(osc::ReceivedBundle::const_iterator i=p.ElementsBegin(); i != p.ElementsEnd(); ++i) {
		if(i->IsBundle()) {
			nret += osc_parsebundle(L, osc::ReceivedBundle(*i));
		} else {
			nret += osc_parsemessage(L, osc::ReceivedMessage(*i));
		}
	}
	return nret;
}

int osc_parse(lua_State * L, const char * buf, size_t size) {
	osc::ReceivedPacket p(buf, size);
	if(p.IsBundle()) {
		return osc_parsebundle(L, osc::ReceivedBundle(p));
	} else {
		return osc_parsemessage(L, osc::ReceivedMessage(p));
	}
}


/*	Open Sound Control network socket receiver
	@module osc.Recv
*/
#pragma mark osc.Recv

struct Recv : public al::ImplAPR {
	Recv() : al::ImplAPR() {}
	
	apr_sockaddr_t * sa;
	apr_socket_t * sock;
	apr_port_t port;
};

template<> const char * Glue<Recv>::usr_name() { return "Recv"; }
//template<> const char * Glue<Recv>::usr_supername() { return "Instruction"; }


template<> Recv * Glue<Recv>::usr_new(lua_State * L) {
	apr_port_t port = (apr_port_t)luaL_optint(L, 1, OSC_DEFAULT_PORT);
	//LuaAVState * S = luaav_fromstate(L);
	
	Recv * u = new Recv();
	apr_pool_t * pool = u->pool();
	
	/* @see http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial-13.html */
	// create socket:
	apr_sockaddr_t * sa;
	apr_socket_t * sock;
	
	lua_check_apr(L, apr_sockaddr_info_get(&sa, NULL, APR_INET, port, 0, pool));
	// for TCP, use SOCK_STREAM and APR_PROTO_TCP instead
	lua_check_apr(L, apr_socket_create(&sock, sa->family, SOCK_DGRAM, APR_PROTO_UDP, pool));
	// bind socket to address:
	lua_check_apr(L, apr_socket_bind(sock, sa));
	lua_check_apr(L, apr_socket_opt_set(sock, APR_SO_NONBLOCK, 1));
	
	u->port = port;
	u->sa = sa;
	u->sock = sock;
	return u;
}

template<> void Glue<Recv>::usr_gc(lua_State * L, Recv * u) { 
	lua_check_apr(L, apr_socket_close(u->sock));
	delete u;
}

static int recv_iter(lua_State * L) {
	Recv * u = Glue<Recv>::to(L, lua_upvalueindex(1));
	apr_size_t maxsize = OSC_DEFAULT_MAXPACKETSIZE; //lua_tointeger(L, lua_upvalueindex(2));
	
	apr_size_t len = maxsize;
	char data[maxsize];
	apr_status_t res = apr_socket_recv(u->sock, data, &len);
	if (res != 0 || len == 0) {
		lua_pushnil(L);
		return 1;
	}
	
	return osc_parse(L, data, len); 
}


/*! Constructor
	@param port optional port number (defaults to 4110)
	@ret The Recv object
	@name Recv
	@function
*/


/*! Poll messages (retrieve from the network socket queue)
	Returns an iterator; each iteration returns a message as a table (m), with 
	message arguments in the array portion, and message types and address in the 
	m.types and m.addr fields.  
	
	Note that bundles will be unrolled.
	Timetags are not supported yet.
	
	Example: 
	
	<luacode>
	for m in r:recv() do
		print(m.addr, m.types, unpack(m))
	end
	</luacode>
	
	Example of how to poll:
	<luacode>
	go(function()
		while(true) do
			for m in r:recv() do
				print(m.addr, m.types, unpack(m))
			end
			
			wait(/1/30)	-- polling interval
		end
	end)
	</luacode>
	
	@ret iterator
	@name M:recv
*/
static int recv_recv(lua_State * L) {
	Glue<Recv>::checkto(L, 1);
	lua_pushcclosure(L, recv_iter, 1);
	return 1;
}

/*! Get IP address
	@ret address as a string
	
	@name M:ip
*/
static int recv_ip(lua_State * L) {
	Recv * u = Glue<Recv>::checkto(L, 1);
	char * ip;
	lua_check_apr(L, apr_sockaddr_ip_get(&ip, u->sa));
	lua_pushstring(L, ip);
	return 1;
}

/*! Get port
	@ret port as a number
	
	@name M:port
*/
static int recv_port(lua_State * L) {
	Recv * u = Glue<Recv>::checkto(L, 1);
	lua_pushinteger(L, u->port);
	return 1;
}

template<> void Glue<Recv>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, recv_ip); lua_setfield(L, -2, "ip");
	lua_pushcfunction(L, recv_port); lua_setfield(L, -2, "port");
	lua_pushcfunction(L, recv_recv); lua_setfield(L, -2, "recv");
}


/*	Open Sound Control network socket sender
	@module osc.Send
*/
#pragma mark osc.Send

struct Send : public al::ImplAPR {
	Send() : al::ImplAPR() {}
	
	apr_sockaddr_t * sa;
	apr_socket_t * sock;
	apr_port_t port;
};

template<> const char * Glue<Send>::usr_name() { return "Send"; }

// dummy comment for constructor
/*! Constructor
	@param host optional hostname or IP address (defaults to 'localhost')
	@param port optional port number (defaults to 4110)
	@ret The Send object
	@name Send
	@function
*/
template<> Send * Glue<Send>::usr_new(lua_State * L) {
	const char * host = luaL_optstring(L, 1, "localhost");
	apr_port_t port = (apr_port_t)luaL_optint(L, 2, OSC_DEFAULT_PORT);
	//LuaAVState * S = luaav_fromstate(L);
	
	Send * u = new Send();
	apr_pool_t * pool = u->pool();
	
	/* @see http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial-13.html */
	// create socket:
	apr_sockaddr_t * sa;
	apr_socket_t * sock;
	
	lua_check_apr(L, apr_sockaddr_info_get(&sa, host, APR_INET, port, 0, pool));
	lua_check_apr(L, apr_socket_create(&sock, sa->family, SOCK_DGRAM, APR_PROTO_UDP, pool));
	lua_check_apr(L, apr_socket_connect(sock, sa));
	lua_check_apr(L, apr_socket_opt_set(sock, APR_SO_NONBLOCK, 1));
	
	u->port = port;
	u->sa = sa;
	u->sock = sock;
	return u;
}

template<> void Glue<Send>::usr_gc(lua_State * L, Send * u) { 
	lua_check_apr(L, apr_socket_close(u->sock));
	delete u;
}


static int add_bundle_or_message(lua_State * L, int idx, osc::OutboundPacketStream& packet);

static int add_message(lua_State * L, int idx, osc::OutboundPacketStream& packet, const char * addr) {
	packet << osc::BeginMessage(addr);
	
	int elems = lua_objlen(L, idx);
	for (int i=1; i<=elems; i++) {
		lua_rawgeti(L, idx, i);	// argument
		switch(lua_type(L, -1))
		{
			case LUA_TNIL: {
				osc::NilType niltype;
				packet << niltype;
			} break;
			case LUA_TBOOLEAN: {
				bool tvalue = (bool)(lua_toboolean(L, -1));
				packet << tvalue;
			} break;
			case LUA_TNUMBER: {
				float v = (float)lua_tonumber(L, -1);
				packet << v;
			} break;
			case LUA_TSTRING: {
				const char * v = lua_tostring(L, -1);
				packet << v;
			} break;
			// LUA_TTABLE - not handled
			// LUA_TFUNCTION
				// can an attempt be made to serialize a function (string.dump)?
			// LUA_TUSERDATA
				// simple userdata types might be squished into a Blob?
				// BUT: need to know the size of the userdata type.
			default: {
				lua_pushfstring(L, "%s (%p)", lua_typename(L, lua_type(L, -1)), lua_topointer(L, -1));
				const char * v = lua_tostring(L, -1);
				lua_pop(L, 1);
				packet << v;
			} 
		}
		lua_pop(L, 1); // argument
	}
	
	packet << osc::EndMessage;
	return 0;
}


static int add_bundle(lua_State * L, int idx, osc::OutboundPacketStream& packet, uint64_t time = 0) {
	::osc::TimeTag timeTag(time);
	packet << ::osc::BeginBundle(timeTag);
	
	// TODO: read timetag from table
	
	int elems = lua_objlen(L, idx);
	for (int i=1; i<=elems; i++) {
		lua_rawgeti(L, idx, i);
		if (lua_istable(L, -1)) {
			add_bundle_or_message(L, lua_gettop(L), packet);
		} else {
			// nothing. error?
			printf("unexpected type of bundle element %s", lua_typename(L, lua_type(L, -1)));
		}
		lua_pop(L, 1); // idx[i]
	}
	packet << osc::EndBundle;
	return 0;
}

// assumes there is a table at idx:
int add_bundle_or_message(lua_State * L, int idx, osc::OutboundPacketStream& packet) {
	lua_getfield(L, idx, "addr");
	// handle this as a message or bundle
	
	uint64_t time = 0;
	
	if (lua_isstring(L, -1)) {
		const char * addr = lua_tostring(L, -1);
		lua_pop(L, 1); // "addr"
		// it's a message
		return add_message(L, idx, packet, addr);
	} else {
		lua_pop(L, 1); // "addr"
		// its a bundle:
		return add_bundle(L, idx, packet, time);
	} 
}



/*!	Send an OSC message

	<luacode>
	-- list form:
	sender:send("/address", 1, 2., "three", true)
	
	-- table form:
	sender:send{ addr="/address", 1, 2., "three", true }
	
	-- bundle example (Timetags are not supported yet.)
	sender:send{
		{ addr="/address1", 1, 2., },
		-- nested bundle:
		{	
			{ addr="/address2", "three", },
			{ addr="/address3", true, },
		}
	}
	</luacode>

	@param message or bundle (table) address (string OSC address)
	@param ... (numbers, strings, booleans, nils) arguments for single message 
	@ret Number of bytes sent
	
	@name M:send
*/
static int send_send(lua_State * L) {
	Send * u = Glue<Send>::checkto(L, 1);
	
	char data[OSC_DEFAULT_MAXPACKETSIZE];
	osc::OutboundPacketStream packet(data, OSC_DEFAULT_MAXPACKETSIZE);
		
	if (lua_istable(L, 2)) {
		add_bundle_or_message(L, 2, packet);
	} else {
		const char * addr = luaL_checkstring(L, 2);
		int args = lua_gettop(L);
		
		packet << osc::BeginMessage(addr);
		
		for(int i=3; i <= args; i++) {
			switch(lua_type(L, i))
			{
				case LUA_TNIL: {
					osc::NilType niltype;
					packet << niltype;
				} break;
				case LUA_TBOOLEAN: {
					bool tvalue = (bool)(lua_toboolean(L, i));
					packet << tvalue;
				} break;
				case LUA_TNUMBER: {
					float v = (float)lua_tonumber(L, i);
					packet << v;
				} break;
				case LUA_TSTRING: {
					const char * v = lua_tostring(L, i);
					packet << v;
				} break;
				case LUA_TTABLE: {
					// tables treated as bundles
					lua_getfield(L, i, "addr");
					//const char * bundle_addr = luaL_optstring(L, -1, addr);
					//int nargs = lua_objlen(L, i);
				} break;
				// LUA_TFUNCTION
					// can an attempt be made to serialize a function (string.dump)?
				// LUA_TUSERDATA
					// simple userdata types might be squished into a Blob?
					// BUT: need to know the size of the userdata type.
				default: {
					lua_pushfstring(L, "%s (%p)", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
					const char * v = lua_tostring(L, -1);
					lua_pop(L, 1);
					packet << v;
				} 
			}
		}
		
		packet << osc::EndMessage;
	}
	apr_size_t size = packet.Size();
	apr_status_t status = apr_socket_send(u->sock, packet.Data(), &size);
	if (size) {
		lua_pushinteger(L, size);
	} else {
		return lua_status_apr(L, status);
	}
	return 1;
}

/*! Get target port
	@ret target port as a number
	
	@name M:port
*/
static int send_port(lua_State * L) {
	Send * u = Glue<Send>::checkto(L, 1);
	lua_pushinteger(L, u->port);
	return 1;
}

/*! Get target IP
	@ret target IP as a string
	
	@name M:ip
*/
static int send_ip(lua_State * L) {
	Send * u = Glue<Send>::checkto(L, 1);
	char * ip;
	lua_check_apr(L, apr_sockaddr_ip_get(&ip, u->sa));
	lua_pushstring(L, ip);
	return 1;
}

/*!	Get target host name
	@ret host name as a string
	
	@name M:host
*/
static int send_host(lua_State * L) {
	Send * u = Glue<Send>::checkto(L, 1);
	char * hostname;
	lua_check_apr(L, apr_getnameinfo(&hostname, u->sa, 0));
	lua_pushstring(L, hostname);
	return 1;
}

template<> void Glue<Send>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, send_ip); lua_setfield(L, -2, "ip");
	lua_pushcfunction(L, send_port); lua_setfield(L, -2, "port");
	lua_pushcfunction(L, send_host); lua_setfield(L, -2, "host");
	lua_pushcfunction(L, send_send); lua_setfield(L, -2, "send");
}

/*
	luaopen_osc
*/
#pragma mark luaopen_osc


extern "C" int luaopen_osc(lua_State * L) {
	const char * libname = luaL_optstring(L, 1, "osc");
	struct luaL_reg lib[] = {
		{"hostname", lua_osc_hostname},
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);
	
	Glue<Send>::define(L);	Glue<Send>::register_ctor(L);
	Glue<Recv>::define(L);	Glue<Recv>::register_ctor(L);
	
	return 1;
}
