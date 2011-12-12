#include "luaav_state.hpp"

#include "types/al_pq.hpp"
//#include "allocore/types/al_MsgQueue.hpp"
#include <math.h>
#include <queue>
#include <new>

#define ddebug(...) 
//#define ddebug(...) printf(__VA_ARGS__)

typedef std::queue<lua_State *> coroq;

static coroq * coroq_find(lua_State * L) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_EVENTS_LITERAL);
	lua_pushvalue(L, 1);
	lua_rawget(L, -2);
	if (!lua_isuserdata(L, -1)) {
		lua_pop(L, 1);
		new (lua_newuserdata(L, sizeof(coroq))) coroq();
		lua_pushvalue(L, 1);
		lua_pushvalue(L, -2);
		lua_rawset(L, -4);
	}
	// grab list
	coroq * q = (coroq *)lua_touserdata(L, -1);
	lua_pop(L, 2); 
	return q;
}

/*
	Routine for coroutines
 */
static int resume(al_sec t, lua_State * C) {
	// get parent state (for debug traceback)
	lua_State * P = luaav_getrootstate(C);
	lua_pushthread(C);
	lua_xmove(C, P, 1);
	
	
	int result = lua_status(C);
	int nargs = lua_gettop(C);
	
	// resume the coroutine?
	switch(result) {
		case 0:
			// if coroutine.status(C) == "created", then 
			// nargs must be > 1
			// since stack index 1 of C should be a function or callable object... 
			// if coroutine.status(C) == "dead" however,
			// then nargs will == 0
			//if (nargs > 0) 
			if (lua_isfunction(C, 1)) 
				result = lua_resume(C, nargs - 1);
			break;
		case LUA_YIELD:
			result = lua_resume(C, nargs); 
			break;
		default:
			break;
	}
	
	if (result != LUA_YIELD) {
		// handle errors:
		switch (result) {
			case 0: // coroutine completed or dead
				break;
			case LUA_ERRRUN:
			case LUA_ERRSYNTAX:
				lua_getfield(P, LUA_REGISTRYINDEX, LUAAV_DEBUG_TRACEBACK_LITERAL);
				lua_pushvalue(P, -2);
				lua_call(P, 1, 1);
				
				luaav_error_msg(
					"error %s\n\t%s", 
					lua_tostring(C, -1), 
					lua_tostring(P, -1) ? lua_tostring(P, -1) : ""
				);
				lua_pop(C, 1);
				break;
			case LUA_ERRMEM:
				luaav_error_msg("memory error %s\n", lua_tostring(C, -1));
				break;
			default:
				luaav_error_msg("unknown error %s\n", lua_tostring(C, -1));
		}	
		
		// unref the coroutine to allow __gc
		lua_getfield(P, LUA_REGISTRYINDEX, LUAAV_CACHE_LITERAL);
		lua_pushvalue(P, -2);
		lua_pushnil(P);		// cache[coro] = nil
		lua_rawset(P, -3);
		lua_pop(P, 1);
	}
	lua_pop(P, 1); // the ref to child
	
	return 0;
}

static int coro_abort(lua_State * L) {
	if (!lua_isthread(L, -1)) {
		lua_pushthread(L);
	}
	// leave 2 copies of the thread on the stack:
	lua_pushvalue(L, -1);
	
	// use first copy to retrieve associated clock:
	lua_rawget(L, lua_upvalueindex(1));	// LUAAV_CACHE_LITERAL
	pq * u = (pq *)lua_touserdata(L, -1);
	if (u) {
		al_pq_cancel_ptr(*u, lua_tothread(L, -2));
	}
	lua_pop(L, 1);
	
	// use 2nd copy to remove from registry (allow __gc)
	lua_pushnil(L);
	lua_rawset(L, lua_upvalueindex(1));	// LUAAV_CACHE_LITERAL
	
	return 0;
}

static int scheduler_tostring(lua_State * L) {
	lua_pushfstring(L, "scheduler (%p)", *(pq **)lua_touserdata(L, 1));
	return 1;
}

static int scheduler_gc(lua_State * L) {
	printf_gcdebug("scheduler\n");
	pq scheduler = *(pq *)lua_touserdata(L, 1);
	al_pq_free(&scheduler, 0);
	// ensure we can't double-free
	lua_pushnil(L);
	lua_setmetatable(L, -2);
	return 0;
}

static int scheduler_close(lua_State * L) {
	lua_pushvalue(L, lua_upvalueindex(1));
	return scheduler_gc(L);
}

static int scheduler_go(lua_State * L) {
	pq scheduler = *(pq *)lua_touserdata(L, lua_upvalueindex(1));
	
	// create a new coro:
	lua_State * C = lua_newthread(L);
	
	/* figure out what we are waiting for */
	if (lua_isfunction(L, 1)) {
		/* no wait: insert to scheduler with delay = 0 */
		
		// ref with this scheduler: // cache[coro] = scheduler
		lua_pushvalue(L, -1);
		lua_pushvalue(L, lua_upvalueindex(1));		
		lua_rawset(L, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL 	
			
		// insert C into the clock schedule
		al::PQHelper queue(scheduler);
		if (!queue.send(0, 0, resume, C)) {
			luaL_error(L, "go: failed to allocate message");
		}
	
	} else if (lua_isnumber(L, 1)) {
		/* wait for a delay (and check for retry value also) */
		al_sec retry = 0;
		al_sec t = (al_sec)fabs(lua_tonumber(L, 1));
		lua_remove(L, 1);
		if (lua_isnumber(L, 1)) {
			retry = (al_sec)fabs(lua_tonumber(L, 1));
			lua_remove(L, 1);
		}
		
		// ref with this scheduler: // cache[coro] = scheduler
		lua_pushvalue(L, -1);
		lua_pushvalue(L, lua_upvalueindex(1));		
		lua_rawset(L, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL 	
			
		// insert C into the clock schedule at delay t
		al::PQHelper queue(scheduler);
		if (!queue.send(t, retry, resume, C)) {
			luaL_error(L, "go: failed to allocate message");
		}
		
	} else {
		/* wait for an event */
				
		// ref with this event: // cache[coro] = event
		lua_pushvalue(L, -1);	// coro
		lua_pushvalue(L, 1);	// event
		lua_rawset(L, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL			
		// queue on the string. find the queue or create it
		coroq * q = coroq_find(L);
		q->push(C);
		
		// done with event:
		lua_remove(L, 1);
	}

	/* put thread at stack index 1 */
	lua_insert(L, 1);
	
	// push the function and any arguments to the coroutine:
	lua_xmove(L, C, lua_gettop(L)-1);
	
	return 1;
}

static int scheduler_wait(lua_State * C) {

	if (lua_type(C, 1) == LUA_TNUMBER) {
		// get the wait period (seconds) & convert to ns:
		al_sec t = (al_sec)(fabs(luaL_optnumber(C, 1, 1.0)));
		//al_sec retry = (al_sec)(fabs(luaL_optnumber(C, 1, 0.0)));
		pq scheduler = *(pq *)lua_touserdata(C, lua_upvalueindex(1));
		
		al::PQHelper queue(scheduler);
		//if (!queue.send(t, retry, resume, C)) {
		if (!queue.send(t, 0.0, resume, C)) {
			luaL_error(C, "wait: failed to allocate message");
		}
		
		// ref with this scheduler: // cache[coro] = scheduler
		lua_pushthread(C);
		lua_pushvalue(C, lua_upvalueindex(1));	// this Scheduler
		lua_rawset(C, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL 

	} else if (lua_type(C, 1) == LUA_TTHREAD) {
		// todo: this would be scheduled to resume when the coro terminates...?
		luaL_error(C, "not yet implemented");
		
		// await on a coro
		// ensure coro is scheduled:
		lua_pushvalue(C, 1);
		lua_rawget(C, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL
		if (lua_isnoneornil(C, -1)) {
			luaL_error(C, "attempt to wait on unscheduled coroutine");
		}
		lua_pop(C, 1);
		
		
		// ref with this coro: // cache[coro] = src
		lua_pushthread(C);
		lua_pushvalue(C, 1);				// src
		lua_rawset(C, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL 
		
	} else {
		// await on an arbitrary event
		// ref with this string: // cache[coro] = string
		lua_pushthread(C);
		lua_pushvalue(C, 1);				// string
		lua_rawset(C, lua_upvalueindex(2));	// LUAAV_CACHE_LITERAL 	
		
		// queue on the string. find the queue or create it
		coroq * q = coroq_find(C);
		q->push(C);
	}
	
	
	// yield the coro, with all values except for the yield condition:
	return lua_yield(C, lua_gettop(C)-1);
}

static int scheduler_now(lua_State * L) {
	pq scheduler = *(pq *)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L, scheduler->now);
	return 1;
}

static int scheduler_update(lua_State * L) {
	al_sec until = (al_sec)(fabs(luaL_optnumber(L, 1, 1.)));
	int defer = lua_toboolean(L, 2);
	pq scheduler = *(pq *)lua_touserdata(L, lua_upvalueindex(1));
	al_pq_update(scheduler, until, defer);
	lua_pushnumber(L, scheduler->now);
	return 1;
}

static int scheduler_advance(lua_State * L) {
	al_sec period = (al_sec)(fabs(luaL_optnumber(L, 1, 1.)));
	int defer = lua_toboolean(L, 2);
	pq scheduler = *(pq *)lua_touserdata(L, lua_upvalueindex(1));
	al_pq_advance(scheduler, period, defer);
	lua_pushnumber(L, scheduler->now);
	return 1;
}

static int scheduler_event(lua_State * L) {
	coroq * q = coroq_find(L);
	
	/* 
		for each coro in the list, schedule it (and remove it from the list)
		check number waiting at this point, 
			since within resume() a coro may re-await on the same event
	*/
	int nargs = lua_gettop(L);
	int listeners = q->size();
	for (int i=0; i<listeners; i++) {
		lua_State * C = q->front(); q->pop();
		// push the event args:
		lua::xcopy(L, C, nargs);
		resume(0, C);
	}
	// what would the returns be?
	// collate all the returns of each coro?
	// would need a temporary stack to hold the args for that to work.
	return 0;
}

static int scheduler(lua_State * L) {	
	// TODO: grab size & birth args from stack?
	
	// uses same allocator as state:
	LuaAVState * S = luaav_fromstate(L);
	pq scheduler = al_pq_create(128, 0, luaav_arena_allocator, &S->main_pool);
	pq * u = (pq *)lua_newuserdata(L, sizeof(pq));
	*u = scheduler;
	
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	
	lua_pushcfunction(L, scheduler_tostring);
	lua_setfield(L, -2, "__tostring");
	
	lua_pushcfunction(L, scheduler_gc);
	lua_setfield(L, -2, "__gc");
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_close, 1);
	lua_setfield(L, -2, "close");
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_now, 1);
	lua_setfield(L, -2, "now");
	
	lua_pushvalue(L, -2);
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_CACHE_LITERAL);
	lua_pushcclosure(L, scheduler_go, 2);
	lua_setfield(L, -2, "go");
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_update, 1);
	lua_setfield(L, -2, "update");
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_advance, 1);
	lua_setfield(L, -2, "advance");
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_event, 1);
	lua_setfield(L, -2, "event");
	
	lua_pushvalue(L, -2);
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_CACHE_LITERAL);
	lua_pushcclosure(L, scheduler_wait, 2);
	lua_setfield(L, -2, "wait");
	
//	lua_pushvalue(L, -2);
//	lua_pushcclosure(L, scheduler_clear, 1);
//	lua_setfield(L, -2, "clear");
	
	lua_setmetatable(L, -2);
	return 1;
}

static int cpu(lua_State * L) {
	lua_pushnumber(L, al_time());
	return 1;
}

static int latency(lua_State * L) {
	luaav_state * S = luaav_fromstate(L);
	S->audio_latency = luaL_optnumber(L, 1, S->audio_latency);
	lua_pushnumber(L, S->audio_latency);
	return 1;
}

extern "C" int luaopen_luaav_time(lua_State * L) {
	const char * libname = lua_tostring(L, -1);
		
	LuaAVState * S = luaav_fromstate(L);
	
	/* 
		create weak-valued Scheduler lookup table:
	*/
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setmetatable(L, -2);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushstring(L, "v");
	lua_setfield(L, -2, "__mode");
	lua_setfield(L, LUA_REGISTRYINDEX, LUAAV_CACHE_LITERAL);
	
	/*
		create events table
	*/
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, LUAAV_EVENTS_LITERAL);
	
	/*
	 create the time module table
	 */
	struct luaL_reg lib[] = {
		{"cpu", cpu },
		{"latency", latency },
		{"scheduler", scheduler },
		
		//{"cache", cache },
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);
	
	/*
	 extend it with closures on the main scheduler userdata
	 */
	pq * u = (pq *)lua_newuserdata(L, sizeof(pq));
	// TODO: store this as singleton instance, or use existing singleton?
	*u = S->mainpq; 
	
	lua_pushvalue(L, -2); // the module table also becomes the metatable of the main scheduler
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_now, 1); 
	lua_pushvalue(L, -1);
	lua_setglobal(L, "now"); 
	lua_setfield(L, -2, "now");
	
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, scheduler_event, 1);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "event"); 
	lua_setfield(L, -2, "event");
	
	lua_pushvalue(L, -2);
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_CACHE_LITERAL);
	lua_pushcclosure(L, scheduler_go, 2);
	lua_pushvalue(L, -1);
	lua_setfield(L, LUA_REGISTRYINDEX, LUAAV_GO_LITERAL);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "go"); 
	lua_setfield(L, -2, "go");
	
	lua_pushvalue(L, -2);
	lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_CACHE_LITERAL);
	lua_pushcclosure(L, scheduler_wait, 2);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "wait"); 
	lua_setfield(L, -2, "wait");
	
//	lua_pushvalue(L, -2);
//	lua_pushcclosure(L, scheduler_clear, 1);
//	lua_setfield(L, -2, "clear");	

	lua_setmetatable(L, -2); // the module table also becomes the metatable of the main scheduler
	
	return 1;
}

