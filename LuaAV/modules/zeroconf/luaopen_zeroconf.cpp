#include "lua.hpp"
#include "luaav.h"
#include "lua_glue.h"

/*
#include "allocore/protocol/al_Zeroconf.hpp"

using namespace al;
using namespace zero;

class LuaClient : public Client {
public:
	
	LuaClient(lua_State * L, const std::string& type = "_osc._udp.", const std::string& domain = "local.") : Client(type, domain), L(L) {}
	
	virtual ~LuaClient() {}

	///! called when a new service name is added:
	virtual void onServiceNew(const std::string& name) {
		printf("new service %s\n", name.c_str());
	}

	///! usually called after onServiceNew
	/// identifies the host/port/address(es) associated with the service name
	virtual void onServiceResolved(const std::string& name, const std::string& host_name, uint16_t port, const std::string& address) {
		printf("resolved service %s %s %d %s\n", name.c_str(), host_name.c_str(), port, address.c_str());
	}

	///! called when existing service name is removed:
	virtual void onServiceRemove(const std::string& name) {
		printf("removed service %s\n", name.c_str());
	}

protected:	
	lua_State * L;
};

template<> const char * Glue<Service>::usr_name() { return "Service"; }

template<> Service * Glue<Service>::usr_new(lua_State * L) { 
	return new Service(
		luaL_checkstring(L, 1), // name
		luaL_optint(L, 2, 8080), // port
		luaL_optstring(L, 3, "_osc._udp."), // type
		luaL_optstring(L, 4, "local.")
	); 
}

template<> void Glue<Service>::usr_gc(lua_State * L, Service * u) {
	delete u;
}

template<> const char * Glue<LuaClient>::usr_name() { return "Client"; }

template<> LuaClient * Glue<LuaClient>::usr_new(lua_State * L) { 
	return new LuaClient(
		L,
		luaL_optstring(L, 1, "_osc._udp."), // type
		luaL_optstring(L, 2, "local.")
	); 
}

static int client_poll(lua_State * L) {
	LuaClient * self = Glue<LuaClient>::checkto(L, 1);
	self->poll(luaL_optnumber(L, 2, 0.01));
	return 0;
}
template<> void Glue<LuaClient> :: usr_mt(lua_State * L) {
	lua_pushcfunction(L, client_poll); lua_setfield(L, -2, "poll");
	// etc. for each method
}

int hostname(lua_State * L) {
	lua_pushstring(L, Socket::hostName().c_str());
	return 1;
}

extern "C" int luaopen_zeroconf(lua_State *L) {
	const char * libname = luaL_optstring(L, 1, "zeroconf");
	
	static const luaL_reg lib[] = {
		{ "hostname", hostname },
		{ NULL, NULL}
	};
	luaL_register(L, libname, lib);
	
	Glue<Service>::define(L); Glue<Service>::register_ctor(L);
	Glue<LuaClient>::define(L); Glue<LuaClient>::register_ctor(L);
	
	return 1;
}

*/

#ifdef AL_LINUX
	#include "apr-1.0/apr_general.h"
	#include "apr-1.0/apr_errno.h"
	#include "apr-1.0/apr_pools.h"
	#include "apr-1.0/apr_network_io.h"
#else
	#include "apr-1/apr_general.h"
	#include "apr-1/apr_errno.h"
	#include "apr-1/apr_pools.h"
	#include "apr-1/apr_network_io.h"
#endif

#include "src/private/al_ImplAPR.h"

static apr_status_t check_apr(apr_status_t err) {
	char errstr[1024];
	if (err != APR_SUCCESS) {
		apr_strerror(err, errstr, sizeof(errstr));
		fprintf(stderr, "%s\n", errstr);
	}
	return err;
}

inline void initialize_apr() {
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		check_apr(apr_initialize());
		atexit(apr_terminate);	// FIXME - can we have multiple atexit calls?
	}
}

/*
	Base class for most APR wrapper objects
*/
class ImplAPR {
public:
	ImplAPR() {
		initialize_apr();
		check_apr(apr_pool_create(&mPool, NULL));
	}	

	~ImplAPR() {
		apr_pool_destroy(mPool);
	}
	
	apr_pool_t * pool() { return mPool; } 
	
protected:
	apr_pool_t * mPool;
};

int hostname(lua_State * L) {
	char buf[APRMAXHOSTLEN+1];
	al::ImplAPR apr;
	check_apr(apr_gethostname(buf, sizeof(buf), apr.pool()));
	lua_pushstring(L, buf);
	return 1;
}

extern "C" int luaopen_zeroconf(lua_State *L) {
	const char * libname = luaL_optstring(L, 1, "zeroconf");
	
	static const luaL_reg lib[] = {
		{ "hostname", hostname },
		{ NULL, NULL}
	};
	luaL_register(L, libname, lib);
	
	return 1;
}