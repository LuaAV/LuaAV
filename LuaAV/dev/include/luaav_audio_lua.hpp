#ifndef INCLUDE_LUAAV_AUDIO_JIT_H
#define INCLUDE_LUAAV_AUDIO_JIT_H 1

//#include "luaav_audio.hpp"
//#include "lua_utility.h"
//
//typedef sample number;
//
//
//
//template<typename T>
//class LNode {
//public: 
//	struct Box { T * ptr; };
//	
//	static T * to(lua_State * L, int idx) {
//		T * u = 0;
//		lua_pushvalue(L, idx);
//		if (lua_isuserdata(L, -1)) {
//			if (lua_getmetatable(L, -1)) {
//				// TODO: how to check for matching metatable?
//				u = ((Box *)lua_touserdata(L, -2))->ptr;
//			}
//			lua_pop(L, 1);	// metatable
//		}
//		lua_pop(L, 1);
//		return u;
//	}
//	
//	static T * checkto(lua_State * L, int idx) {
//		T * u = to(L, idx);
//		if (u == 0) luaL_error(L, "Synth not found (index %d)", idx);
//		return u;
//	}
//	
//	static int stop(lua_State * L) {
//		T * self = checkto(L, 1);
//		//printf("Synth stop\n");
//		self->stop();
//		return 0;
//	}
//	
//	static int gc(lua_State * L) {
//		T * self = checkto(L, 1);
//		//lua::dump(L, "Synth gc");
//		if (self->release()) {
//			if (self->stateA() == Node::STOPPED) {
//				delete self;	// node is dead to Lua and to Audio
//			} else {
//				self->stateL() = Node::DEAD;	// this node is dead to Lua
//			}
//		} 
//		return 0;
//	}
//	
//	static int prototype(lua_State * L) {
//		lua_getfenv(L, 1);
//		return 1;
//	}
//	
//	static int tostring(lua_State * L) {
//		lua_pushfstring(L, "Synth: %p", to(L, 1));
//		return 1;
//	}
//	
//	static int eq(lua_State * L) {
//		lua_pushboolean(L, to(L, 1) == to(L, 2));
//		return 1;
//	}
//	
//	static int index(lua_State *L) {
//		T * self = to(L, 1);
//		// first check metatable:
//		lua_getmetatable(L, 1);
//		lua_pushvalue(L, 2);
//		lua_rawget(L, -2);
//		if(lua_isnil(L, -1)) {
//			lua_pop(L, 1);	// bad result; but leave metatable there... 
//			// next try __getters:
//			if (self->valid()) {
//				lua_pushstring(L, "__getters");
//				lua_rawget(L, -2);
//				lua_pushvalue(L, 2);
//				lua_rawget(L, -2);
//				if(lua_type(L, -1) == LUA_TFUNCTION) {
//					lua_pushvalue(L, 1);
//					int err = lua_pcall(L, 1, 1, 0);
//					if(err) {
//						luaL_error(L, "error indexing %s.%s: %s\n",
//												lua_tostring(L, 1),
//												lua_tostring(L, 2),
//												lua_tostring(L, -1));
//					}
//				} else {
//					lua_pop(L, 3); // non-getter, __getters, metatable
//					// try fenv
//					lua_getfenv(L, 1);
//					lua_pushvalue(L, 2);
//					lua_rawget(L, -2);
//				}
//			}
//		}
//		return 1;
//	}
//	
//	static int newindex(lua_State * L) {
//		T * self = to(L, 1);
//		if (self->valid()) {
//			// find setter:
//			lua_getmetatable(L, 1);
//			lua_pushstring(L, "__setters");
//			lua_rawget(L, -2);
//				lua_pushvalue(L, 2);
//				lua_rawget(L, -2);
//			
//			// call setter:
//			if(lua_type(L, -1) == LUA_TFUNCTION) {
//				lua_pushvalue(L, 1);
//				lua_pushvalue(L, 2);
//				lua_pushvalue(L, 3);
//				int err = lua_pcall(L, 3, 0, 0);
//				if(err) {
//					luaL_error(L, "error indexing %s.%s: %s\n",
//												lua_tostring(L, 1),
//												lua_tostring(L, 2),
//												lua_tostring(L, -1));
//				}
//			} else {
//				lua_pop(L, 1); // non-setter
//			}
//			lua_pop(L, 2); // __setters, metatable
//		
//			// store the value in the fenv:
//			lua_getfenv(L, 1);
//			lua_pushvalue(L, 2);
//			lua_pushvalue(L, 3);
//			lua_rawset(L, -3);
//			lua_pop(L, 1);	// fenv
//		} else {
//			luaL_error(L, "synth in invalid state");
//		}
//		return 0;
//	}
//	
//	static int create(lua_State * L) {
//		luaL_checktype(L, 1, LUA_TTABLE);		// parameters
//		
//		static const int metaIdx = lua_upvalueindex(1);
//		static const int ctxIdx = lua_upvalueindex(2);
//		//static const int jitIdx = lua_upvalueindex(3);
//		
//		// make sure context is at stack index 2
//		Context * C = Glue<Context>::to(L, 2);
//		if (C == NULL) { 
//			lua_settop(L, 1);
//			lua_pushvalue(L, ctxIdx);	
//			C = Glue<Context>::to(L, 2);
//		}
//		
//		//JIT * jit = Glue<JIT>::to(L, jitIdx);
//		
//		Box * udata = (Box *)lua_newuserdata(L, sizeof(Box));
//		int udIdx = lua_gettop(L);
//		lua_pushvalue(L, metaIdx);
//		lua_setmetatable(L, udIdx);
//		
//		// give the userdata a fenv table
//		lua_newtable(L);	
//		lua_setfenv(L, udIdx);
//	
//		//T * u = new T(C, jit);
//		T * u = new T(C);
//		udata->ptr = u;
//		u->retain(); 	// one reference in Lua
//		
//		// apply prototype:
//		lua_getfield(L, metaIdx, "__newindex");
//		int newindexIdx = lua_gettop(L);
//		lua_pushnil(L);  // first key
//		// iterate through the prototype table
//		while (lua_next(L, 1) != 0) {
//			// uses 'key' (at index -2) and 'value' (at index -1)
//			lua_pushvalue(L, newindexIdx);	// ud.__newindex function
//			lua_pushvalue(L, udIdx);		// ud
//			lua_pushvalue(L, -4);			// prototype key
//			lua_pushvalue(L, -4);			// prototype value
//			lua_pcall(L, 3, 0, 0);				// ud.__newindex(ud, key, value)
//			// removes 'value'; keeps 'key' for next iteration
//			lua_pop(L, 1);
//		}
//		lua_pop(L, 1); // __newindex
//		
//		u->init(L);
//		u->play();
//		return 1;
//	}
//	
//	static int createJIT(lua_State * L) {
//		luaL_checktype(L, 1, LUA_TTABLE);		// parameters
//		
//		static const int metaIdx = lua_upvalueindex(1);
//		static const int ctxIdx = lua_upvalueindex(2);
//		static const int jitIdx = lua_upvalueindex(3);
//		
//		// make sure context is at stack index 2
//		Context * C = Glue<Context>::to(L, 2);
//		if (C == NULL) { 
//			lua_settop(L, 1);
//			lua_pushvalue(L, ctxIdx);	
//			C = Glue<Context>::to(L, 2);
//		}
//		
//		JIT * jit = Glue<JIT>::to(L, jitIdx);
//		
//		Box * udata = (Box *)lua_newuserdata(L, sizeof(Box));
//		int udIdx = lua_gettop(L);
//		lua_pushvalue(L, metaIdx);
//		lua_setmetatable(L, udIdx);
//		
//		// give the userdata a fenv table
//		lua_newtable(L);	
//		lua_setfenv(L, udIdx);
//	
//		T * u = new T(C, jit);
//		udata->ptr = u;
//		u->retain(); 	// one reference in Lua
//		
//		// apply prototype:
//		lua_getfield(L, metaIdx, "__newindex");
//		int newindexIdx = lua_gettop(L);
//		lua_pushnil(L);  // first key
//		// iterate through the prototype table
//		while (lua_next(L, 1) != 0) {
//			// uses 'key' (at index -2) and 'value' (at index -1)
//			lua_pushvalue(L, newindexIdx);	// ud.__newindex function
//			lua_pushvalue(L, udIdx);		// ud
//			lua_pushvalue(L, -4);			// prototype key
//			lua_pushvalue(L, -4);			// prototype value
//			lua_pcall(L, 3, 0, 0);				// ud.__newindex(ud, key, value)
//			// removes 'value'; keeps 'key' for next iteration
//			lua_pop(L, 1);
//		}
//		lua_pop(L, 1); // __newindex
//		
//		u->init(L);
//		u->play();
//		return 1;
//	}
//	
//	static int defineJIT(lua_State * L, luaL_reg lib[], luaL_reg getters[], luaL_reg setters[]) {
//		Glue<Context>::checkto(L, 1);
//		Glue<JIT>::checkto(L, 2);
//		
//		// define metatable:
//		lua_newtable(L);
//		luaL_register(L, NULL, lib);
//		static luaL_reg lib_standard[] = {
//			{ "stop", stop },
//			{ "prototype", prototype },
//			{ "__gc", gc },
//			{ "__eq", eq },
//			{ "__tostring", tostring },
//			{ "__index", index },
//			{ "__newindex", newindex },
//			{ NULL, NULL }
//		};
//		luaL_register(L, NULL, lib_standard);
//		
//		lua_newtable(L);
//		luaL_register(L, NULL, getters);
//		lua_setfield(L, -2, "__getters");
//		
//		lua_newtable(L);
//		luaL_register(L, NULL, setters);
//		lua_setfield(L, -2, "__setters");	
//		
//		lua_insert(L, 1);
//		//lua::dump(L, "meta, ctx, jit");
//		lua_pushcclosure(L, createJIT, 3);
//		return 1;
//	}
//	
//	static int define(lua_State * L, luaL_reg lib[], luaL_reg getters[], luaL_reg setters[]) {
//		lua_getfield(L, LUA_REGISTRYINDEX, LUAAV_AUDIO_ROOT);
//		//Glue<Context>::push(L, (Context *)luaav_fromstate(L));
//		//Glue<JIT>::checkto(L, 2);
//		
//		// define metatable:
//		lua_newtable(L);
//		luaL_register(L, NULL, lib);
//		static luaL_reg lib_standard[] = {
//			{ "stop", stop },
//			{ "prototype", prototype },
//			{ "__gc", gc },
//			{ "__eq", eq },
//			{ "__tostring", tostring },
//			{ "__index", index },
//			{ "__newindex", newindex },
//			{ NULL, NULL }
//		};
//		luaL_register(L, NULL, lib_standard);
//		
//		lua_newtable(L);
//		luaL_register(L, NULL, getters);
//		lua_setfield(L, -2, "__getters");
//		
//		lua_newtable(L);
//		luaL_register(L, NULL, setters);
//		lua_setfield(L, -2, "__setters");	
//		
//		lua_insert(L, -2);
//		//lua::dump(L, "meta, ctx");
//		lua_pushcclosure(L, create, 2);
//		return 1;
//	}
//	
//	// constructs a member-setter method:
//	template<typename R, R T::* member>
//	static int set(lua_State * L) {
//		T * self = checkto(L, 1);
//		if (self->valid()) {
//			R val = lua::to<R>(L, 3);
//			self->context()->inbox().send(self->context()->now(), msg<R, member>, self, val);
//		}
//		return 0;
//	}
//	
//	// constructs a member(pointer)-setter method:
//	template<typename R, R * T::* member>
//	static int set(lua_State * L) {
//		T * self = checkto(L, 1);
//		if (self->valid()) {
//			R * val = lua::to<R>(L, 3);
//			self->context()->inbox().send(self->context()->now(), msg<R, member>, self, val);
//		}
//		return 0;
//	}
//	
//	template<typename R, R T::* member>
//	static void msg(al_sec t, T * self, R val) { if (self->stateA() <= Node::STOPPED) self->*member = val; }
//	
//	template<typename R, R * T::* member>
//	static void msg(al_sec t, T * self, R * val) { if (self->stateA() <= Node::STOPPED) self->*member = val; }
//	
//	template<Port T::*member>
//	static int port_set(lua_State * L) {
//		//lua::dump(L, "port_set");
//		T * self = checkto(L, 1);
//		if (self->valid()) {
//			al::MsgTube& inbox = self->context()->inbox();
//			al_sec t = self->context()->now();
//			int chans = (self->*member).channels();
//			switch(lua_type(L, 3)) {
//				case LUA_TNUMBER: {
//					inbox.send(t, Port::msg<T, member>, self, 0, chans, lua_tonumber(L, 3));
//				}
//				break;
//				case LUA_TUSERDATA: {
//					inbox.send(t, Port::msg<T, member>, self, 0, chans, Glue<Signal>::checkto(L, 3));
//				}
//				break;
//				case LUA_TTABLE:
//					for (int i=0; i<chans; i++) {
//						lua_rawgeti(L, 3, i+1);
//						switch (lua_type(L, -1)) {
//							case LUA_TNUMBER: {
//									inbox.send(t, Port::msg<T, member>, self, i, 1, lua_tonumber(L, -1));
//								}
//								break;
//							case LUA_TUSERDATA: {
//									inbox.send(t, Port::msg<T, member>, self, i, 1, Glue<Signal>::checkto(L, -1));
//								}
//								break;
//							default:
//								break;
//						}
//						lua_pop(L, 1);
//					}	
//				default:
//				break;
//			}
//		}
//		return 0;
//	}
//};
//
//inline int port_init(lua_State * L, const char * name, int udIdx, int ctxIdx) {
//	lua_getfield(L, udIdx, name);
//	if (lua_isnoneornil(L, -1)) {
//		lua_getfield(L, ctxIdx, name);
//		lua_setfield(L, udIdx, name);
//	}
//	lua_pop(L, 1);
//}

#endif
