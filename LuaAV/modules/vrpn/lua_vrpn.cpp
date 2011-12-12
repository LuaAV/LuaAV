#include "lua_vrpn.h"
#include "lua_utility.h"

#include "vrpn_Connection.h"
#include "vrpn_Tracker.h"

// This is the callback handler that will print the string from any
// incoming message (and also the locally-packed ones).
/*
typedef	struct {
	struct timeval	msg_time;	// Time of the report
	long		sensor;		// Which sensor is reporting
	double		pos[3];		// Position of the sensor
	double		quat[4];	// Orientation of the sensor
} vrpn_TRACKERCB;
*/

struct Tracker {
	vrpn_Tracker_Remote * tkr;
	vrpn_TRACKERCB data;
	char name[4096];

	static void tracker_handler(void * u, const vrpn_TRACKERCB d) {
		Tracker * tracker = (Tracker *)u;
		
		memcpy(&tracker->data, &d, sizeof(vrpn_TRACKERCB));
	}
	
	bool open(const char * address) {
		tkr = new vrpn_Tracker_Remote(address);	
		tkr->register_change_handler((void *)this, &tracker_handler);
		return true; // opportunity to fail
	}
	
	int poll(lua_State * L) {
		tkr->mainloop();
		return 0;
	}
	
	void close() {
		delete tkr;
	}
};

static int tracker_new(lua_State * L) {
	const char * body = luaL_checkstring(L, 1);
	const char * ip = luaL_checkstring(L, 2);
	int port = luaL_optint(L, 3, 3883);
	Tracker * u = (Tracker*)lua_newuserdata(L, sizeof(Tracker));
	sprintf(u->name, "%s@%s:%d", body, ip, port);
	
	if (u->open(u->name)) {
		luaL_getmetatable(L, "vrpn_Tracker_Remote");
		lua_setmetatable(L, -2);
		
		// create environment table to hold pos, quat etc.
		lua_createtable(L, 0, 3);
			lua_pushinteger(L, 0);	lua_setfield(L, -2, "sensor");
			lua_createtable(L, 3, 0);
				lua_pushnumber(L, 0);	lua_rawseti(L, -2, 1);
				lua_pushnumber(L, 0);	lua_rawseti(L, -2, 2);
				lua_pushnumber(L, 0);	lua_rawseti(L, -2, 3);
			lua_setfield(L, -2, "pos");
			lua_createtable(L, 4, 0);
				lua_pushnumber(L, 0);	lua_rawseti(L, -2, 1);
				lua_pushnumber(L, 0);	lua_rawseti(L, -2, 2);
				lua_pushnumber(L, 0);	lua_rawseti(L, -2, 3);
				lua_pushnumber(L, 1);	lua_rawseti(L, -2, 4);
			lua_setfield(L, -2, "quat");		
		lua_setfenv(L, -2);
		
		return 1;
	} 
	return 0;
}

static int tracker_gc(lua_State * L) {
	((Tracker *)(lua_touserdata(L, 1)))->close();
	return 0;
}

static int tracker_tostring(lua_State * L) {
	Tracker * u = ((Tracker *)(lua_touserdata(L, 1)));
	lua_pushfstring(L, "vrpn.tracker (%p: %s)", u, u->name);
	return 1;
}

static int tracker_name(lua_State * L) {
	lua_pushstring(L, ((Tracker*)(lua_touserdata(L, 1)))->name);
	return 1;
}

static int tracker_poll(lua_State * L) {
	Tracker * u = ((Tracker*)(lua_touserdata(L, 1)));	
	vrpn_Tracker_Remote * tkr = u->tkr;
	tkr->mainloop();
	lua_getfenv(L, 1);
	lua_pushinteger(L, u->data.sensor);	lua_setfield(L, -2, "sensor");
		lua_getfield(L, -2, "pos");
			lua_pushnumber(L, u->data.pos[0]);	lua_rawseti(L, -2, 1);
			lua_pushnumber(L, u->data.pos[1]);	lua_rawseti(L, -2, 2);
			lua_pushnumber(L, u->data.pos[2]);	lua_rawseti(L, -2, 3);
		lua_pop(L, 1);
		lua_getfield(L, -2, "quat");
			lua_pushnumber(L, u->data.quat[0]);	lua_rawseti(L, -2, 1);
			lua_pushnumber(L, u->data.quat[1]);	lua_rawseti(L, -2, 2);
			lua_pushnumber(L, u->data.quat[2]);	lua_rawseti(L, -2, 3);
			lua_pushnumber(L, u->data.quat[3]);	lua_rawseti(L, -2, 4);
		lua_pop(L, 1);
	
	return 1;
}

static int tracker_sensor(lua_State * L) {
	Tracker * u = ((Tracker*)(lua_touserdata(L, 1)));
	lua_pushnumber(L, u->data.sensor);
	return 1;
}

static int tracker_pos(lua_State * L) {
	Tracker * u = ((Tracker*)(lua_touserdata(L, 1)));
	lua_pushnumber(L, u->data.pos[0]);
	lua_pushnumber(L, u->data.pos[1]);
	lua_pushnumber(L, u->data.pos[2]);
	return 3;
}

static int tracker_quat(lua_State * L) {
	Tracker * u = ((Tracker*)(lua_touserdata(L, 1)));
	lua_pushnumber(L, u->data.quat[0]);
	lua_pushnumber(L, u->data.quat[1]);
	lua_pushnumber(L, u->data.quat[2]);
	lua_pushnumber(L, u->data.quat[3]);
	return 4;
}

static int tracker_test(lua_State * L) {
	lua_getfenv(L, 1);
	return 1;
}

static int tracker_index(lua_State * L) {
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
	if (lua_isnil(L, -1)) {
		lua_getmetatable(L, 1);
		lua_pushvalue(L, 2);
		lua_gettable(L, -2);
	}
	return 1;
}

static void tracker_initmetatable(lua_State * L) {
	luaL_newmetatable(L, "vrpn_Tracker_Remote");
	
	lua_pushcfunction(L, tracker_index);
	lua_setfield(L, -2, "__index");
	
	lua_pushcfunction(L, tracker_gc);
	lua_setfield(L, -2, "__gc");
	
	lua_pushcfunction(L, tracker_tostring);
	lua_setfield(L, -2, "__tostring");
	
	lua_pushcfunction(L, tracker_poll);
	lua_setfield(L, -2, "poll");
	
	lua_pushcfunction(L, tracker_name);
	lua_setfield(L, -2, "name");
	
//	lua_pushcfunction(L, tracker_pos);
//	lua_setfield(L, -2, "pos");
//	
//	lua_pushcfunction(L, tracker_quat);
//	lua_setfield(L, -2, "quat");
//	
//	lua_pushcfunction(L, tracker_sensor);
//	lua_setfield(L, -2, "sensor");
//	
//	lua_pushcfunction(L, tracker_test);
//	lua_setfield(L, -2, "test");
	
	lua_pop(L, 1);
}

int luaopen_vrpn(lua_State *L)
{
	tracker_initmetatable(L);
	
	static const luaL_reg lib[] = 
	{
		{"tracker", tracker_new },
		{NULL, NULL}
	};
	luaL_register(L, lua_tostring(L, -1), lib);
	
	// close connection on module unload:
	//lua::gc_sentinel(L, -1, unload);
	
	return 1;
}