#ifndef _LUAAV_UDATA_H_
#define _LUAAV_UDATA_H_ 1

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

#include "luaav.h"
#include "lua_utility.h"

#define LUAAV_MODULE_NAME				"LuaAV"
#define LUAAV_UDATA_LIB_META			"LUAAV_lib_meta"
//#define LUAAV_UDATA_INSTANCES_META		"LUAAV_instances_meta"
#define LUAAV_UDATA_INSTANCES_METAFIELD	"__instances"

#define REF_TABLE_FIELD		"__UDATA_REF_TABLE_FIELD"
#define REF_FIELD			"__UDATA_REF_FIELD"

namespace luaav {

struct LuaMethod {
public:
	enum MethodType {
		METHOD = 0,
		GET,
		SET,
		GETSET,
	};

	const char *name;
	lua_CFunction func;
	MethodType type;
};


// Base userdata class in the Curiously Recurring Template Pattern
// http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern
template <class T>
class Udata {
protected:
	typedef T *TID;

public:
	Udata()
	: mL(0), mFinalizer(0)
	{}

	virtual ~Udata() {};

	// utility functions
	static void * check_udata(lua_State *L, int ud);
	static int check_equality(lua_State *L);
	static TID to_udata(lua_State *L, int index);
	static const char * classname(lua_State *L, int index);
	static TID * udata_push(lua_State *L, T *obj);
	static int register_udata(lua_State *L);
	static int setup_functions(lua_State *L);
	static int inherit(lua_State *L);
	static bool is_prototype(lua_State *L);

	// reference counting
	static int inc_ref(lua_State *L);
	static int dec_ref(lua_State *L);

	// metamethods
	static int udata__prototype(lua_State *L, T *x);
	static int udata__index(lua_State *L);
	static int udata__newindex(lua_State *L);
	static int udata__gc(lua_State *L);
	static int udata__tostring(lua_State *L);
	static int udata__call(lua_State *L);

	// boxed ptr management
	static int null_udata(lua_State *L, T *o);
	static int instance_to_userdata(lua_State *L, T *o);

	// lua_State management
	virtual void setState(lua_State * L) {
		if (L == 0) {
			mL = 0;
		} else {
			mL = luaav_getrootstate(L);
		}
	}
	virtual lua_State * getState() {return mL;}

	virtual void finalizer(lua_CFunction v) {mFinalizer = v;}
	virtual lua_CFunction finalizer() {return mFinalizer;}

protected:
	static const luaL_reg udata_lib_meta[];

	lua_State *mL;
	lua_CFunction mFinalizer;
};

template <class T>
void * Udata<T> :: check_udata(lua_State *L, int ud) {
	void *p = lua_touserdata(L, ud);
	if (p != NULL && lua_getmetatable(L, ud)) {
		lua_pushlightuserdata(L, (void *)T::name);
		lua_rawget(L, -2);

		if(!lua_isnil(L, -1)) {
			lua_pop(L, 2);
			return p;
		}

		lua_pop(L, 2);
	}

	return NULL;
}

// -1 is the constant
// -2 changes based on __super
template <class T>
int Udata<T> :: check_equality(lua_State *L) {
	// quick check of equality
	int equal = 0;

	lua_pushlightuserdata(L, (void *)T::name);
	lua_rawget(L, -2);

	if(!lua_isnil(L, -1)) {
		equal = 1;
	}
	lua_pop(L, 1);

	return equal;
}

template <class T>
typename Udata<T>::TID Udata<T> :: to_udata(lua_State *L, int index) {
	TID *p_obj = (TID *)check_udata(L, index);
	if(p_obj)	return *p_obj;
	else		return NULL;
}

template <class T>
const char * Udata<T> :: classname(lua_State *L, int index) {
	if(lua_getmetatable(L, index)) {
		lua_getfield(L, -1, "__name");
		const char *name = (lua_type(L, -1) == LUA_TSTRING) ? lua_tostring(L, -1) : NULL;
		lua_pop(L, 2);
		return name;
	}
	else {
		return NULL;
	}
}

template <class T>
typename Udata<T>::TID * Udata<T> :: udata_push(lua_State *L, T *obj) {
	if(obj) {
		obj->setState(L);

		luaL_getmetatable(L, LUAAV_UDATA_LIB_META);

		// check instances field in metatable
		lua_pushstring(L, LUAAV_UDATA_INSTANCES_METAFIELD);
		lua_rawget(L, -2);					// get instances table
		lua_pushinteger(L, (LUA_INTEGER)obj);
		lua_rawget(L, -2);					// get this instance's field

		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);		// pop nil from stack
			lua_pushinteger(L, (LUA_INTEGER)obj);

			T **p_obj = (T **)lua_newuserdata(L, sizeof(TID));
			*p_obj = obj;

			lua_pushvalue(L, -1);
			lua_insert(L, -4);
			lua_rawset(L, -3);

			lua_pop(L, 1);					// pop instances from stack
			lua_remove(L, -2);

			luaL_getmetatable(L, T::name);
			lua_setmetatable(L, -2);

			lua_newtable(L);
			lua_setfenv(L, -2);

			return p_obj;
		}
		else {
			lua_insert(L, -3);
			lua_pop(L, 2);
			int top = lua_gettop(L);

			return (T **)luaL_checkudata(L, top, T::name);
		}
	}
	else {
		lua_pushnil(L);
		return 0;
	}
}

template <class T>
bool Udata<T> :: is_prototype(lua_State *L) {
	return lua_type(L, (1)) == LUA_TTABLE;
}

template <class T>
int Udata<T> :: inc_ref(lua_State *L) {
	if(lua_isuserdata(L, -3) && lua_isuserdata(L, -2) && lua_type(L, -1) == LUA_TTABLE) {
		// lua_getfenv(L, -2);
		lua_pushstring(L, REF_TABLE_FIELD);
		lua_rawget(L, -2);
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);

			lua_pushstring(L, REF_TABLE_FIELD);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);
		}

		lua_pushvalue(L, -3);	// push userdata
		lua_rawget(L, -2);		// check if in ref table
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);

			lua_pushvalue(L, -3);	// push userdata
			lua_pushinteger(L, 1);	// one ref
			lua_rawset(L, -3);
		}
		else {
			int ref = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
			ref++;

			lua_pushvalue(L, -3);
			lua_pushinteger(L, ref);	// one ref
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);	// pop env and ref table
	}

	return 0;
}

template <class T>
int Udata<T> :: dec_ref(lua_State *L) {
	if(lua_isuserdata(L, -3) && lua_isuserdata(L, -2) && lua_type(L, -1) == LUA_TTABLE) {
		// lua_getfenv(L, -2);
		lua_pushstring(L, REF_TABLE_FIELD);
		lua_rawget(L, -2);

		lua_pushvalue(L, -3);	// push userdata
		lua_rawget(L, -2);
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
		}
		else {
			int ref = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
			ref--;

			if(ref <= 0) {
				lua_pushvalue(L, -3);	// push userdata
				lua_pushnil(L);
				lua_rawset(L, -3);
			}
			else {
				lua_pushvalue(L, -3);	// push userdata
				lua_pushinteger(L, ref);
				lua_rawset(L, -3);
			}
		}
		lua_pop(L, 1);	// pop metatable and ref table
	}

	return 0;
}

template <class T>
int Udata<T> :: udata__prototype(lua_State *L, T *x) {
	if(lua_type(L, 1) == LUA_TTABLE) {
		udata_push(L, x);

		// we have a prototype table, iterator through it and set values
		lua_getmetatable(L, -1);
			lua_pushstring(L, "setters");
			lua_rawget(L, -2);

		lua_pushstring(L, "__newindex"); lua_rawget(L, -3);
		//lua_getglobal(L, "print");
		int t = lua_gettop(L);

		// table is in the stack at index 't'
		lua_pushnil(L);  // first key
		while (lua_next(L, 1) != 0) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			lua_pushvalue(L, t);
			lua_pushvalue(L, 2);
			lua_pushvalue(L, -4);
			lua_pushvalue(L, -4);
			lua_call(L, 3, 0);

			// removes 'value'; keeps 'key' for next iteration
			lua_pop(L, 1);
		}

		lua_pop(L, 3);
		return 1;
	}
	// or userdata with __type

	return 0;
}


template <class T>
int Udata<T> :: udata__index(lua_State *L) {
	lua_getfenv(L, 1);				// get userdata environment
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);				// check environment table for field

	if(lua_isnil(L, -1)) {
		lua_getmetatable(L, 1);		// get userdata metatable
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);			// check metatable for field

		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_pushstring(L, "getters");
			lua_rawget(L, -2);

			lua_pushvalue(L, 2);
			lua_rawget(L, -2);		// check metatable for field

			// if nil, check in env table
			if (lua_type(L, -1) == LUA_TFUNCTION) { //if(lua_iscfunction(L, -1)) {
				lua_pushvalue(L, 1);
				int err = lua_pcall(L, 1, 1, 0);
				if(err) {
					luaL_error(L, "error indexing %s.%s: %s\n",
											T::name,
											lua_tostring(L, 2),
											lua_tostring(L, -1));
					lua_pop(L, 1);
					return 0;
				}
			}
		}
	}

	return 1;
}

template <class T>
int Udata<T> :: udata__newindex(lua_State * L) {
	lua_getmetatable(L, 1);		// get userdata metatable
	lua_getfield(L, -1, "setters");
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
	
	int func = lua_gettop(L);
	if(lua_type(L, func) == LUA_TFUNCTION) {
		lua_pushvalue(L, 1);
		lua_pushvalue(L, 3);
		int err = lua_pcall(L, 2, 0, 0);
		if(err) {
			luaL_error(L, "error indexing %s.%s: %s\n",
										T::name,
										lua_tostring(L, 2),
										lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		lua_pop(L, 3);
	} 
	else {
		lua_pop(L, 3);
	
		lua_getfenv(L, 1);		// get userdata environment
		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
		lua_rawset(L, -3);

		lua_pop(L, 1);
	}

	return 0;
}

template <class T>
int Udata<T> :: udata__gc(lua_State *L) {
	printf_gcdebug("Udata<T> %s\n", T::name);
	TID o = to_udata(L, 1);
	if(o) {
		if(o->finalizer()) {
			o->finalizer()(L);
		}
		o->setState(0);
		delete o;
	}

	return 0;
}

template <class T>
int Udata<T> :: udata__tostring(lua_State *L) {
	TID o = to_udata(L, 1);
	lua_pushfstring(L, "%p %s", o, T::name);

	return 1;
}

template <class T>
int Udata<T> :: udata__call(lua_State *L) {
	lua_getfield(L, 1, "new");
	lua_replace(L, 1);		// put constructor at index 1
	lua_call(L, lua_gettop(L)-1, 1);

	return 1;
}

template <class T>
int Udata<T> :: null_udata(lua_State *L, T *o) {
	luaL_getmetatable(L, LUAAV_UDATA_LIB_META);
	lua_pushstring(L, LUAAV_UDATA_INSTANCES_METAFIELD);
	lua_rawget(L, -2);					// get instances table
	lua_pushinteger(L, (LUA_INTEGER)o);
	lua_rawget(L, -2);					// get this instance's field

	if(!lua_isnil(L, -1)) {
		// have to null out the userdata value
		TID *p_obj = (TID *)luaL_checkudata(L, -1, T::name);
		*p_obj = NULL;
	}

	return 0;
}

// might be better ro rearchitect instances to be module-wide
// clearing stack properly?
template <class T>
int Udata<T> :: instance_to_userdata(lua_State *L, T *o) {
	luaL_getmetatable(L, LUAAV_UDATA_LIB_META);
	lua_pushstring(L, LUAAV_UDATA_INSTANCES_METAFIELD);
	lua_rawget(L, -2);					// get instances table
	lua_pushinteger(L, (LUA_INTEGER)o);
	lua_rawget(L, -2);					// get this instance's field

	lua_remove(L, -2);
	lua_remove(L, -2);

	return 1;
}

template <class T>
const luaL_reg Udata<T>::udata_lib_meta[] = {
	{"__index", Udata<T>::udata__index},
	{"__newindex", Udata<T>::udata__newindex},
	{"__gc", Udata<T>::udata__gc},
	{"__tostring", Udata<T>::udata__tostring},
	{NULL, NULL}
};

template <class T>
int Udata<T> :: register_udata(lua_State *L) {
	luaL_newmetatable(L, T::name);

	/*
		Shouldn't trample globals like this
	*/
//    // set as global
//    lua_pushvalue(L, -1);
//    lua_setglobal(L, T::name);

	// setup constructor
	lua_newtable(L);
		lua_pushstring(L, "__call");
		lua_pushcfunction(L, Udata<T>::udata__call);
		lua_rawset(L, -3);
	lua_setmetatable(L, -2);	// set as metatable for __call

	// setup introspection
	lua_pushstring(L, "__name");
	lua_pushstring(L, T::name);
	lua_rawset(L, -3);

	setup_functions(L);			// udata generic functions and class functions
	inherit(L);					// superclass functions

	// setup instance referencing
	if(luaL_newmetatable(L, LUAAV_UDATA_LIB_META)) {
		lua_pushstring(L, LUAAV_UDATA_INSTANCES_METAFIELD);
		lua_newtable(L);
			// make table weak-valued so udata get collected
			lua_pushstring(L, "v");
			lua_setfield(L, -2, "__mode");

			lua_pushvalue(L, -1);
			lua_setmetatable(L, -2);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);	// pop instance referencing table
	
	return 1;
}

template <class T>
int Udata<T> :: setup_functions(lua_State *L) {
	luaL_register(L, 0, udata_lib_meta);

	// add attrs table (return values, not functions)
	lua_pushstring(L, "getters");
	lua_newtable(L);
	lua_rawset(L, -3);

	lua_pushstring(L, "setters");
	lua_newtable(L);
	lua_rawset(L, -3);

	const LuaMethod *class_lib = T::getLuaMethods();
	int i=0;

	while(class_lib[i].name) {
		switch(class_lib[i].type)
		{
			// returns a function to be operated on by the paren operator ()
			case LuaMethod::METHOD:
				lua_pushstring(L, class_lib[i].name);
				lua_pushcfunction(L, class_lib[i].func);
				lua_rawset(L, -3);
				break;

			// returns a value only
			case LuaMethod::GET:
				lua_getfield(L, -1, "getters");
					lua_pushstring(L, class_lib[i].name);
					lua_pushcfunction(L, class_lib[i].func);
					lua_rawset(L, -3);
				lua_pop(L, 1);
				break;

			// sets a value only
			case LuaMethod::SET:
				lua_getfield(L, -1, "setters");
					lua_pushstring(L, class_lib[i].name);
					lua_pushcfunction(L, class_lib[i].func);
					lua_rawset(L, -3);
				lua_pop(L, 1);
				break;

			// returns or sets a value
			case LuaMethod::GETSET:
				// make both functions from Lua's perspective the same
				// if both are set using lua_pushcfunction, they will be different
				// so we must set them as the same "Lua" variable
				lua_pushcfunction(L, class_lib[i].func);
					lua_getfield(L, -2, "getters");
						lua_pushstring(L, class_lib[i].name);
						lua_pushvalue(L, -3);
						lua_rawset(L, -3);
					lua_pop(L, 1);

					lua_getfield(L, -2, "setters");
						lua_pushstring(L, class_lib[i].name);
						lua_pushvalue(L, -3);
						lua_rawset(L, -3);
					lua_pop(L, 1);
				lua_pop(L, 1);	// pop function
				break;
		}
		i++;
	}

	return 0;
}

template <class T>
int Udata<T> :: inherit(lua_State *L) {
	lua_pushlightuserdata(L, (void *)T::name);
	lua_pushboolean(L, true);
	lua_rawset(L, -3);

	int k=0;
	const char **superTable = T::getSuperclassTable();
	while(superTable[k]) {
		lua_pushlightuserdata(L, (void *)superTable[k]);
		lua_pushboolean(L, true);
		lua_rawset(L, -3);

		// put methods of metatable in as well
		luaL_getmetatable(L, superTable[k]);
		if(!lua_isnil(L, -1)) {
			// table is in the stack at index 't'
			lua_pushnil(L);  // first key
			while (lua_next(L, -2) != 0) {
				// uses 'key' (at index -2) and 'value' (at index -1)
				if(lua_isfunction(L, -1)) {		// want to transfer superclass tables
					if(lua_isstring(L, -2)) {	// get string value of key
						const char *name = lua_tostring(L, -2);
						if(name[0] != '_' && name[1] != '_')  {		// not a reserved __meta field
							luaL_getmetatable(L, T::name);
							lua_pushvalue(L, -3);
							lua_rawget(L, -2);

							if(lua_isnil(L, -1)) {	// doesn't already exist in table
								lua_pop(L, 1);		// pop nil
								lua_pushvalue(L, -3);
								lua_pushvalue(L, -3);
								lua_rawset(L, -3);
								lua_pop(L, 1);
							}
							else {
								lua_pop(L, 2);
							}
						}
					}
				}

				// removes 'value'; keeps 'key' for next iteration
				lua_pop(L, 1);
			}

		}
		lua_pop(L, 1);

		k++;
	}

	return 0;
}

}	// luaav::

#endif	// _LUAAV_UDATA_H_
