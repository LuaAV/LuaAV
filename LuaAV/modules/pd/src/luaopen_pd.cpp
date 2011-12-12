extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	extern int luaopen_pd(lua_State * L);
	
	#include "z_libpd.h"
}

#include "luaav.h"

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

int lua_clear_search_path(lua_State * L) {
	libpd_clear_search_path();
	return 0;
}

int lua_add_to_search_path(lua_State * L) {
	libpd_add_to_search_path(luaL_checkstring(L, 1));
	return 0;
}
	

/* require "pd" */
int luaopen_pd(lua_State * L) {
	
	/* make sure that both LuaAV and CppSound are initialized */
	if (!initialized) {
		libpd_init();
		initialized = 1;
	}
	//
//	luaL_newmetatable(L, "CppSoundSynth_metatable");
//	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
//	lua_pushstring(L, "Csound");	lua_setfield(L, -2, "name");
//	
//	luaL_Reg metalib[] = {
//		//{ "output", luaav3_Synth_setoutput },
////		{ "setoutput", luaav3_Synth_setoutput },
////		{ "amp", luaav3_Synth_setamp },
////		{ "setamp", luaav3_Synth_setamp },
////		{ "fadeout", luaav3_Synth_fadeout },
////
////		{ "verbose", CppSoundSynth_verbose },
////		{ "nchnls", CppSoundSynth_nchnls },
////		{ "ksmps", CppSoundSynth_ksmps },
////		{ "kr", CppSoundSynth_kr },
////		{ "sr", CppSoundSynth_sr },
////		{ "0dbfs", CppSoundSynth_0dbfs },
////		{ "scoretime", CppSoundSynth_scoretime },
////		{ "scorepending", CppSoundSynth_scorepending },
////		{ "scorerewind", CppSoundSynth_scorerewind },
////		{ "scoreevent", CppSoundSynth_scoreevent },
////		{ "messagelevel", CppSoundSynth_messagelevel },
////		{ "message", CppSoundSynth_message },
////		{ "listopcodes", CppSoundSynth_listopcodes },
////
////		{ "getinstrumentcount", CppSoundSynth_getInstrumentCount },
////		{ "ins", CppSoundSynth_getSpin },
//	
//		{ NULL, NULL }
//	};
//	luaL_register(L, NULL, metalib);

	// sentinel on unload 
	//gc_sentinel(L, -1, unload);
	
	// define the module table 
	luaL_Reg lib[] = {
		{ "clear_search_path", lua_clear_search_path },
		{ "add_to_search_path", lua_add_to_search_path },
		{ NULL, NULL }
	};
	luaL_register(L, "pd", lib);
//	lua_pushinteger(L, csoundGetVersion());
//	lua_setfield(L, -2, "version");
	
	return 1;
}
