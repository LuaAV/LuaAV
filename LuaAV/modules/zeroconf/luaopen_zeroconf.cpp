#include "lua.hpp"
#include "luaav.h"
#include "lua_glue.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>


#include "allocore/protocol/al_Zeroconf.hpp"

using namespace al;
using namespace zero;

class LuaClient : public Client {
public:
	
	LuaClient(lua_State * L, const std::string& type = "_osc._udp.", const std::string& domain = "local.") : Client(type, domain), L(luaav_getrootstate(L)) {}
	
	virtual ~LuaClient() {}

	///! called when a new service name is added:
	virtual void onServiceNew(const std::string& name);

	///! usually called after onServiceNew
	/// identifies the host/port/address(es) associated with the service name
	virtual void onServiceResolved(const std::string& name, const std::string& host_name, uint16_t port, const std::string& address);

	///! called when existing service name is removed:
	virtual void onServiceRemove(const std::string& name);

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
	//printf("gc service\n");
	delete u;
}

template<> const char * Glue<LuaClient>::usr_name() { return "Client"; }

template<> LuaClient * Glue<LuaClient>::usr_new(lua_State * L) { 
	// first arg must be a function to specify the callback
	return new LuaClient(
		L,
		luaL_optstring(L, 1, "_osc._udp."), // type
		luaL_optstring(L, 2, "local.")
	); 
}

template<> void Glue<LuaClient>::usr_gc(lua_State * L, LuaClient * u) {
	//printf("gc Client\n");
	delete u;
}

// push reference back to self:
template<> void Glue<LuaClient>::usr_push(lua_State * L, LuaClient * u) {
	Glue<LuaClient>::usr_attr_push(L);
	lua_getfenv(L, -1);
	//lua::dump(L, "push fenv");
	usr_set_reference_map(L, u);
	lua_pop(L, 1);
	//lua::dump(L, "push");
}

static int client_added(lua_State * L) {
	LuaClient * self = Glue<LuaClient>::checkto(L, 1);
	if (!lua_isfunction(L, 2)) luaL_error(L, "missing function argument (callback to notify service changes)");
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	//lua::dump(L, "adding");
	lua_setfield(L, -2, "added");
	//lua::dump(L, "added");
	return 0;
}

static int client_resolved(lua_State * L) {
	LuaClient * self = Glue<LuaClient>::checkto(L, 1);
	if (!lua_isfunction(L, 2)) luaL_error(L, "missing function argument (callback to notify service changes)");
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	//lua::dump(L, "adding");
	lua_setfield(L, -2, "resolved");
	//lua::dump(L, "added");
	return 0;
}

static int client_removed(lua_State * L) {
	LuaClient * self = Glue<LuaClient>::checkto(L, 1);
	if (!lua_isfunction(L, 2)) luaL_error(L, "missing function argument (callback to notify service changes)");
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	//lua::dump(L, "adding");
	lua_setfield(L, -2, "removed");
	//lua::dump(L, "added");
	return 0;
}

//static int client_callback(lua_State * L) {
//	LuaClient * self = Glue<LuaClient>::checkto(L, 1);
//	if (!lua_isfunction(L, 2)) luaL_error(L, "missing function argument (callback to notify service changes)");
//	Glue<LuaClient>::usr_set_reference_map(L, self);
//	return 0;
//}

static int client_poll(lua_State * L) {
	LuaClient * self = Glue<LuaClient>::checkto(L, 1);
	self->poll(luaL_optnumber(L, 2, 0.01));
	return 0;
}
template<> void Glue<LuaClient> :: usr_mt(lua_State * L) {
	lua_pushcfunction(L, client_poll); lua_setfield(L, -2, "poll");
	lua_pushcfunction(L, client_added); lua_setfield(L, -2, "added");
	lua_pushcfunction(L, client_removed); lua_setfield(L, -2, "removed");
	lua_pushcfunction(L, client_resolved); lua_setfield(L, -2, "resolved");
	// etc. for each method
}

///! called when a new service name is added:
void LuaClient :: onServiceNew(const std::string& name) {
	//printf("new service %s (%p)\n", name.c_str(), this);
	Glue<LuaClient>::usr_get_reference_map(L, this);
	if (lua_istable(L, -1)) {
		// get callback:
		lua_getfield(L, -1, "added");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, name.c_str());
			if (lua_pcall(L, 1, 0, 0)) {
				fprintf(stdout, "error: %s\n", lua_tostring(L, -1));
			}
		}
		lua_settop(L, 0);
	}
}

///! usually called after onServiceNew
/// identifies the host/port/address(es) associated with the service name
void LuaClient :: onServiceResolved(const std::string& name, const std::string& host_name, uint16_t port, const std::string& address) {
	//printf("resolved service %s %s %d %s\n", name.c_str(), host_name.c_str(), port, address.c_str());
	Glue<LuaClient>::usr_get_reference_map(L, this);
	if (lua_istable(L, -1)) {
		// get callback:
		lua_getfield(L, -1, "resolved");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, name.c_str());
			lua_pushstring(L, host_name.c_str());
			lua_pushinteger(L, port);
			lua_pushstring(L, address.c_str());
			if (lua_pcall(L, 4, 0, 0)) {
				fprintf(stdout, "error: %s\n", lua_tostring(L, -1));
			}
		}
		lua_settop(L, 0);
	}
}

///! called when existing service name is removed:
void LuaClient :: onServiceRemove(const std::string& name) {
	//printf("removed service %s\n", name.c_str());
	Glue<LuaClient>::usr_get_reference_map(L, this);
	if (lua_istable(L, -1)) {
		// get callback:
		lua_getfield(L, -1, "removed");
		if (lua_isfunction(L, -1)) {
			lua_pushstring(L, name.c_str());
			if (lua_pcall(L, 1, 0, 0)) {
				fprintf(stdout, "error: %s\n", lua_tostring(L, -1));
			}
		}
		lua_settop(L, 0);
	}
}

int hostname(lua_State * L) {
	lua_pushstring(L, Socket::hostName().c_str());
	return 1;
}

int hostip(lua_State * L) {

    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    int results = 0;
    
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                
                lua_newtable(L);
                lua_pushstring(L, temp_addr->ifa_name);
                lua_setfield(L, -2, "name");
                lua_pushstring(L, inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr));
                lua_setfield(L, -2, "address");
                results++;
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    // Free memory
    freeifaddrs(interfaces);

	//lua_pushstring(L, Socket::hostIP().c_str());
	return results;
}

extern "C" int luaopen_zeroconf(lua_State *L) {
	const char * libname = luaL_optstring(L, 1, "zeroconf");
	
	static const luaL_reg lib[] = {
		{ "hostname", hostname },
        { "hostip", hostip },
		{ NULL, NULL}
	};
	luaL_register(L, libname, lib);
	
	Glue<Service>::define(L); Glue<Service>::register_ctor(L);
	Glue<LuaClient>::define(L); Glue<LuaClient>::register_ctor(L);
	
	return 1;
}