/*
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/
/*

Some extra routines to ease life with Lua & C++

*/

#ifndef LUAAV_UTILITY_H
#define LUAAV_UTILITY_H 1

#include <string>
#include <vector>
#include <list>
#include <cstring>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define printf_gcdebug(...) 
#ifndef printf_gcdebug
	#define printf_gcdebug(...) printf("DEBUG __gc: "); printf(__VA_ARGS__)
#endif

#define lua_define_enum(L, idx, ENUM) lua_pushinteger(L, ENUM); lua_setfield(L, idx, #ENUM );

#ifdef __cplusplus
extern "C" {
#endif

extern const char * unsafe_globals[];
extern const char * unsafe_os_fields[];

#ifdef __cplusplus
}
#endif

namespace lua {

static void dump(lua_State * L, const char * msg) {
	printf("DUMP lua_State (%p) %s\n", L, msg);
	int top = lua_gettop(L);
	for (int i=1; i<=top; i++) {
		switch(lua_type(L, i)) {
			case LUA_TNIL:
				printf("%i (-%i): nil\n", i, top+1-i); break;
			case LUA_TBOOLEAN:
				printf("%i (-%i): boolean (%s)\n", i, top+1-i, lua_toboolean(L, i) ? "true" : "false"); break;
			case LUA_TLIGHTUSERDATA:
				printf("%i (-%i): lightuserdata (%p)\n", i, top+1-i, lua_topointer(L, i)); break;
			case LUA_TNUMBER:
				printf("%i (-%i): number (%f)\n", i, top+1-i, lua_tonumber(L, i)); break;
			case LUA_TSTRING:
				printf("%i (-%i): string (%s)\n", i, top+1-i, lua_tostring(L, i)); break;
			case LUA_TUSERDATA:
//				printf("%i (-%i): userdata (%p)\n", i, top+1-i, lua_topointer(L, i)); break;
				lua_getglobal(L, "tostring");
				lua_pushvalue(L, i);
				lua_call(L, 1, 1);
				printf("%i (-%i): %s\n", i, top+1-i, lua_tostring(L, -1));
				lua_pop(L, 1);
				break;
			default:{
				printf("%i (-%i): %s (%p)\n", i, top+1-i, lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			}
		}
	}
}

static void debug(lua_State * L, const char * msg) {
	struct lua_Debug ar;
	int top = lua_gettop(L);
	//printf("DEBUG lua_State (%p) %s\n", L, msg);
	for (int i=1; i<=top; i++) {
		lua_getstack(L, i, &ar);
        if (lua_getinfo(L, ">n", &ar)) {
			printf("%i: what=%s, name=%s namewhat=%s, source=%s short_src=%s, currentline=%i linedefined=%i lastlinedefined=%i nups=%i\n",
				i,
				ar.what,
				ar.name ? ar.name : "",
				ar.namewhat,
				ar.source, ar.short_src,
				ar.currentline, ar.linedefined, ar.lastlinedefined,
				ar.nups
			);
        }
		else {
			printf("level %i not valid to debug\n", i);
		}
	}
}

/*
	Pushing various C++ types to the stack:
*/

template<typename T> inline void push(lua_State * L, T t); // specialized implementations:

template<typename T> inline void push(lua_State * L, T * v) { lua_pushlightuserdata(L, (void *)v); }

template<> inline void push(lua_State * L, double v) { lua_pushnumber(L, (lua_Number)v); }
template<> inline void push(lua_State * L, float v) { lua_pushnumber(L, (lua_Number)v); }
template<> inline void push(lua_State * L, signed int v) { lua_pushinteger(L, (lua_Integer)v); }
template<> inline void push(lua_State * L, signed long v) { lua_pushnumber(L, (lua_Integer)v); }
template<> inline void push(lua_State * L, signed long long v) { lua_pushnumber(L, (lua_Integer)v); }
template<> inline void push(lua_State * L, unsigned int v) { lua_pushinteger(L, (lua_Integer)v); }
template<> inline void push(lua_State * L, unsigned long v) { lua_pushnumber(L, (lua_Integer)v); }
template<> inline void push(lua_State * L, unsigned long long v) { lua_pushnumber(L, (lua_Integer)v); }
template<> inline void push(lua_State * L, const char * v) { lua_pushstring(L, (const char *)v); }
template<> inline void push(lua_State * L, char * v) { lua_pushstring(L, (const char *)v); }
template<> inline void push(lua_State * L, char v) { lua_pushlstring(L, (const char *)(&v), 1); }
template<> inline void push(lua_State * L, std::string v) { lua_pushstring(L, (const char *)(v.c_str())); }
template<> inline void push(lua_State * L, bool v) { lua_pushboolean(L, (int)v); }
template<> inline void push(lua_State * L, void * v) { lua_pushlightuserdata(L, (void *)v); }
template<> inline void push(lua_State * L, lua_CFunction v) { lua_pushcclosure(L, v, 0); }
template<> inline void push(lua_State * L, lua_State * v) { lua_pushthread(v); lua_xmove(v, L, 1); }

template<typename T>
inline void push(lua_State * L, std::vector<T> * v) {
	lua_createtable(L, v->size(), 0);
	for (int i=0; i<v->size(); i++) {
		push(v[i]); lua_rawseti(L, -2, i+1);
	}
}

template<typename T>
inline void push(lua_State * L, std::vector<T> & v) {
	lua_createtable(L, v.size(), 0);
	for (int i=0; i<v.size(); i++) {
		push(v[i]); lua_rawseti(L, -2, i+1);
	}
}

template<typename T>
inline void push(lua_State * L, std::list<T> * v) {
	lua_createtable(L, v->size(), 0);
	for (int i=0; i<v->size(); i++) {
		push(v[i]); lua_rawseti(L, -2, i+1);
	}
}

template<typename T>
inline void push(lua_State * L, std::list<T> & v) {
	lua_createtable(L, v.size(), 0);
	for (int i=0; i<v.size(); i++) {
		push(v[i]); lua_rawseti(L, -2, i+1);
	}
}

/*
	Reading various C++ types from the stack:
*/

template<typename T> inline bool is(lua_State * L, int idx); // specialized implementations:
template<> inline bool is<double>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<float>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<unsigned int>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<unsigned long>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<unsigned long long>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<signed int>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<signed long>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<signed long long>(lua_State * L, int idx) { return lua_isnumber(L, idx); }
template<> inline bool is<bool>(lua_State * L, int idx) { return lua_isboolean(L, idx); }
template<> inline bool is<char *>(lua_State * L, int idx) { return lua_type(L, idx) == LUA_TSTRING; }
template<> inline bool is<std::string>(lua_State * L, int idx) { return lua_type(L, idx) == LUA_TSTRING; }
template<lua_CFunction> inline bool is(lua_State * L, int idx) { return lua_iscfunction(L, idx); }


template<typename T> inline T to(lua_State * L, int idx); // specialized implementations:
template<typename T> inline void to(lua_State * L, int idx, T * v) { return (T *)lua_touserdata(L, idx); }
template<> inline double to(lua_State * L, int idx) { return lua_tonumber(L, idx); }
template<> inline float to(lua_State * L, int idx) { return (float)lua_tonumber(L, idx); }
template<> inline unsigned int to(lua_State * L, int idx) { return (unsigned int)lua_tointeger(L, idx); }
template<> inline unsigned long to(lua_State * L, int idx) { return (unsigned long)lua_tointeger(L, idx); }
template<> inline unsigned long long to(lua_State * L, int idx) { return (unsigned long long)lua_tointeger(L, idx); }
template<> inline signed int to(lua_State * L, int idx) { return (signed int)lua_tointeger(L, idx); }
template<> inline signed long to(lua_State * L, int idx) { return (signed long)lua_tointeger(L, idx); }
template<> inline signed long long to(lua_State * L, int idx) { return (signed long long)lua_tointeger(L, idx); }
template<> inline bool to(lua_State * L, int idx) { return lua_toboolean(L, idx) ? true : false; }
template<> inline char * to(lua_State * L, int idx) { return (char *)lua_tostring(L, idx); }
template<> inline std::string to(lua_State * L, int idx) { return lua_tostring(L, idx); }
template<> inline lua_CFunction to(lua_State * L, int idx) { return lua_tocfunction(L, idx); }

template<typename T> inline T opt(lua_State * L, int idx, T); // specialized implementations:

template<typename T> inline const T * opt(lua_State * L, int idx, T * v) {
	return lua_isuserdata(L, idx) ? (T *)lua_touserdata(L, idx) : v;
}
template<> inline double opt(lua_State * L, int idx, double v) {
	return luaL_optnumber(L, idx, v);
}
template<> inline float opt(lua_State * L, int idx, float v) {
	return (float)luaL_optnumber(L, idx, v);
}
template<> inline unsigned short opt(lua_State * L, int idx, unsigned short v) {
	return (unsigned short)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline unsigned int opt(lua_State * L, int idx, unsigned int v) {
	return (unsigned int)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline unsigned long opt(lua_State * L, int idx, unsigned long v) {
	return (unsigned long)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline unsigned long long opt(lua_State * L, int idx, unsigned long long v) {
	return (unsigned long long)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline signed int opt(lua_State * L, int idx, signed int v) {
	return (signed int)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline signed long opt(lua_State * L, int idx, signed long v) {
	return (signed long)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline signed long long opt(lua_State * L, int idx, signed long long v) {
	return (signed long long)luaL_optinteger(L, idx, (lua_Integer)v);
}
template<> inline bool opt(lua_State * L, int idx, bool v) {
	return lua_isboolean(L, idx) ? lua_toboolean(L, idx) : v;
}
template<> inline const char * opt(lua_State * L, int idx, const char * v) {
	return (const char *)luaL_optstring(L, idx, v);
}
template<> inline std::string opt(lua_State * L, int idx, std::string v) {
	return std::string(luaL_optstring(L, idx, v.c_str()));
}
template<> inline lua_CFunction opt(lua_State * L, int idx, lua_CFunction v) {
	return lua_iscfunction(L, idx) ? lua_tocfunction(L, idx) : v;
}

template<typename T>
static T optfield(lua_State * L, int idx, T def, const char * name) {
	T t;
	lua_getfield(L, idx, name);
	t = (T)opt(L, -1, def);
	lua_pop(L, 1);
	return t;
}

/*
	Calling global functions:
*/

inline bool checkglobal(lua_State * L, const char * fname, int type = LUA_TFUNCTION) {
	lua_getglobal(L, fname);
	if (lua_type(L, -1) != type) {
		printf("error: %s not found or is not a %s\n", fname, lua_typename(L, type));
		lua_pop(L, 1); return false;
	}
	return true;
}

inline int lerror(lua_State * L, int err) {
	if (err) {
		printf("error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	return err;
}

inline int pcall(lua_State * L, int nargs) {
	int debug_idx = -nargs-3;
	// put debug.traceback just below the function & args
	{
		lua_getglobal(L, "debug");
		lua_pushliteral(L, "traceback");
		lua_gettable(L, -2);
		lua_insert(L, debug_idx);
		lua_pop(L, 1); // pop debug table
	}
	int top = lua_gettop(L);
	int res = lerror(L, lua_pcall(L, nargs, LUA_MULTRET, -nargs-2));
	int nres = lua_gettop(L) - top; 
//	int nres = lua_gettop(L) - top + nargs + 1;
	lua_remove(L, -(nres+nargs+2)); // remove debug function from stack
	return res;
}

inline int dostring(lua_State * L, const char * str) {
	int err = lerror(L, luaL_loadstring(L, str));
	int top = lua_gettop(L);
	return err ? err : pcall(L, top-1);
}

inline int call(lua_State * L, const char * fname) {
	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		return pcall(L, 0);
	}
	return 1;
}

template<typename A1>
inline int call(lua_State * L, const char * fname, A1 a1) {
	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		push(L, a1);
		return pcall(L, 1);
	}
	return 1;
}

template<typename A1, typename A2>
inline int call(lua_State  * L, const char * fname, A1 a1, A2 a2) {

	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		push(L, a1);
		push(L, a2);
		return pcall(L, 2);
	}
	return 1;
}

template<typename A1, typename A2, typename A3>
inline int call(lua_State  * L, const char * fname, A1 a1, A2 a2, A3 a3) {

	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		push(L, a1);
		push(L, a2);
		push(L, a3);
		return pcall(L, 3);
	}
	return 1;
}

template<typename A1, typename A2, typename A3, typename A4>
inline int call(lua_State  * L, const char * fname, A1 a1, A2 a2, A3 a3, A4 a4) {

	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		push(L, a1);
		push(L, a2);
		push(L, a3);
		push(L, a4);
		return pcall(L, 4);
	}
	return 1;
}

template<typename A1, typename A2, typename A3, typename A4, typename A5>
inline int call(lua_State  * L, const char * fname, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {

	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		push(L, a1);
		push(L, a2);
		push(L, a3);
		push(L, a4);
		push(L, a5);
		return pcall(L, 5);
	}
	return 1;
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline int call(lua_State * L, const char * fname, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {

	if (checkglobal(L, fname, LUA_TFUNCTION)) {
		push(L, a1);
		push(L, a2);
		push(L, a3);
		push(L, a4);
		push(L, a5);
		push(L, a6);
		return pcall(L, 6);
	}
	return 1;
}

inline int call(lua_State * L, lua_CFunction func, int nargs = 0) {

	push(L, func);
	lua_insert(L, -(nargs+1));
	return pcall(L, nargs);
}

template<typename A1>
inline int call(lua_State * L, lua_CFunction func, A1 a1) {
	push(L, func);
	push(L, a1);
	return pcall(L, 1);
}

template<typename A1, typename A2>
inline int call(lua_State  * L, lua_CFunction func, A1 a1, A2 a2) {

	push(L, func);
	push(L, a1);
	push(L, a2);
	return pcall(L, 2);
}

template<typename A1, typename A2, typename A3>
inline int call(lua_State  * L, lua_CFunction func, A1 a1, A2 a2, A3 a3) {

	push(L, func);
	push(L, a1);
	push(L, a2);
	push(L, a3);
	return pcall(L, 3);
}

template<typename A1, typename A2, typename A3, typename A4>
inline int call(lua_State  * L, lua_CFunction func, A1 a1, A2 a2, A3 a3, A4 a4) {

	push(L, func);
	push(L, a1);
	push(L, a2);
	push(L, a3);
	push(L, a4);
	return pcall(L, 4);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5>
inline int call(lua_State  * L, lua_CFunction func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
	push(L, func);
	push(L, a1);
	push(L, a2);
	push(L, a3);
	push(L, a4);
	push(L, a5);
	return pcall(L, 5);
}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline int call(lua_State * L, lua_CFunction func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {

	push(L, func);
	push(L, a1);
	push(L, a2);
	push(L, a3);
	push(L, a4);
	push(L, a5);
	push(L, a6);
	return pcall(L, 6);
}

/*
	Clears out all values of a table on the stack
*/
static void clear_table(lua_State * L, int idx) {
	lua_pushvalue(L, idx);
	int t = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, -2)) { // -2 is the table
		lua_pushnil(L);			// new value
		lua_settable(L, t);		// t[k] = nil
	}
	lua_pop(L, 1);				// table
}

/* 
	(shallow) copy fields from one table to another 
	Could be used e.g. to exend a metatable with another
*/
static void copy_fields(lua_State * L, int srcidx, int dstidx) {
	lua_pushvalue(L, dstidx);
	int dst = lua_gettop(L);
	lua_pushvalue(L, srcidx);
	int src = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, src)) { // -2 is the table
		lua_pushvalue(L, -2); // key
		lua_pushvalue(L, -2); // value
		lua_settable(L, dst);
		lua_pop(L, 1); // value
	}
	lua_pop(L, 2);				// src/dst copies
}

/*
** Copies values from State src to State dst (which may be entirely independent)
** idx is the stack index of dst to begin copying from
** returns number of items copied
*/
static int copy_values(lua_State *src, lua_State *dst, int idx) {
	int top = lua_gettop(src);
	int nargs = top-idx+1;
	lua_checkstack(dst, nargs);
	for (; idx <= top; idx++) {
		switch (lua_type (src, idx)) {
			case LUA_TNUMBER:
				lua_pushnumber (dst, lua_tonumber (src, idx));
				break;
			case LUA_TBOOLEAN:
				lua_pushboolean (dst, lua_toboolean (src, idx));
				break;
			case LUA_TSTRING: {
				const char *string = lua_tostring (src, idx);
				size_t length = lua_strlen (src, idx);
				lua_pushlstring (dst, string, length);
				break;
			}
			case LUA_TLIGHTUSERDATA: {
				lua_pushlightuserdata (dst, lua_touserdata (src, idx));
				break;
			}
			case LUA_TNIL:
			default:
				luaL_error(src, "cannot push argument of type %s", lua_typename(src, lua_type(src, idx)));
				break;
		}
	}
	return nargs;
}

static void loadlib(lua_State * L, const char * name, lua_CFunction func) {
	lua_pushcfunction(L, func);
	lua_pushstring(L, name);
	if (lua_pcall(L, 1, 1, 0))
		printf("loadlib error: %s", lua_tostring(L, -1));
	lua_settop(L, 0);
}

static void preloadlib(lua_State * L, const char * name, lua_CFunction func) {
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, func);
	lua_setfield(L, -2, name);
	lua_settop(L, 0);
}

/*
	load a luaL_Reg * of modules:
*/
static void loadlibs(lua_State * L, const luaL_Reg * lib) {
	for (; lib->func; lib++) {
		loadlib(L, lib->name, lib->func);
	}
	lua_settop(L, 0);
}

static void preloadlibs(lua_State * L, const luaL_Reg * lib) {
	for (; lib->func; lib++) {
		preloadlib(L, lib->name, lib->func);
	}
	lua_settop(L, 0);
}

/*
	Struct & function to read in Lua source line by line from C string array:
*/
typedef struct {
	const char ** listing;
	int line;
} ListingReaderData;
static const char * ListingReader (lua_State *L, void *data, size_t *size) {
	ListingReaderData * rdata = (ListingReaderData *)data;
	int i = rdata->line++;
	*size = strlen(rdata->listing[i]);
	return rdata->listing[i];
}
static int loadlib_fromlisting(lua_State * L, const char * libname, const char ** listing) {
	ListingReaderData rdata = { listing, 0 };
	if(lua_load(L, ListingReader, &rdata, libname)) {
		luaL_error(L, "error loading module %s: %s", libname, lua_tostring(L, -1));
	}
	lua_pushstring(L, libname);	// module name
	lua_call(L, 1, 1);			// execute module
	return 1;
}

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

// idx = index of userdata
static int gc_sentinel_value(lua_State *L, int idx) {
	lua_getfield(L, LUA_REGISTRYINDEX, "LuaAV.gc_sentinels");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_pushnil(L);	// error
	}
	else {
		lua_pushvalue(L, idx);
		lua_gettable(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	
	return 1;
}

static const char * typestring(int type) {
	switch(type) {
		case LUA_TNIL:				return "nil";
		case LUA_TBOOLEAN:			return "boolean";
		case LUA_TNUMBER:			return "number";
		case LUA_TSTRING:			return "string";
		case LUA_TTHREAD:			return "thread";
		case LUA_TFUNCTION:			return "function";
		case LUA_TLIGHTUSERDATA:
		case LUA_TUSERDATA:			return "userdata";
		default:					return "invalid";
	}
}


static int findmodule(lua_State *L, const char *name) {
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_getfield(L, -1, name);
	lua_insert(L, -3);
	lua_pop(L, 2);

	return 1;
}


static int notinsandboxed(lua_State * L) {
	return luaL_error(L, "not available in sandboxed state");
}

// see: http://lua-users.org/wiki/SandBoxes
inline void openlibs_sandboxed(lua_State * L) {
	luaL_openlibs(L);

	lua_pushcfunction(L, notinsandboxed);
	int nisb = lua_gettop(L);

	// nil out unsafe os module fields
	const char * unsafe_os_fields[] = {
		"date",
		"execute",
		"exit",
		"getenv",
		"remove",
		"rename",
		"setlocale",
		"tmpname",
		NULL
	};
	lua_getglobal(L, "os");
	for(int i=0; unsafe_os_fields[i]; i++) {
		//lua_pushnil(L);
		lua_pushvalue(L, nisb);
		lua_setfield(L, -2, unsafe_os_fields[i]);
	}
	lua_pop(L, 1); // os

	// nil out unsafe globals
	const char * unsafe_globals[] = {
		"collectgarbage",
		"dofile",
		"_G",
		"getfenv",
		"getmetatable",
		"load",
		"loadfile",
		"loadstring",
		"rawequal",
		"rawget",
		"rawset",
		"setfenv",
		"setmetatable",
		"module",
		"require",
		"package",
		"debug",
		NULL
	};
	for(int i=0; unsafe_globals[i]; i++) {
		//lua_pushnil(L);
		lua_pushvalue(L, nisb);
		lua_setglobal(L, unsafe_globals[i]);
	}

	lua_pop(L, 1); // notinsandboxed
	//return L;
}

/*
	Force the deletion of a typed pointer when a particular value in Lua is collected.
	Usage example:
		// assumes some collectible value, e.g. a module table, is at stack index -1:
		Foo * myfoo = new Foo();
		AutoGCPtr<Foo>::create(L, -1, myfoo);
*/
template<typename T>
class AutoGCPtr {
typedef AutoGCPtr<T> Self;
	T * data;
	static int gc(lua_State * L) {
		Self * self = (Self *)lua_touserdata(L, lua_upvalueindex(1));
		delete self->data;
		return 0;
	}
public:
	static void create(lua_State * L, T * t) {
		int idx = lua_gettop(L);
		Self * self = (Self *)lua_newuserdata(L, sizeof(Self));
		self->data = t;
		lua_newtable(L);
		lua::dump(L, "gccreate");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_pushvalue(L, -2);
		lua_pushcclosure(L, gc, 1);
		lua::dump(L, "gccreate gc");
		lua_setfield(L, -2, "__gc");
		lua_setmetatable(L, -2);
		lua_pushvalue(L, idx);
		lua_settable(L, LUA_REGISTRYINDEX);
		lua::dump(L, "gccreate done");
	}
};

} // lua::

#endif	//_LUA_UTILITY_H_
