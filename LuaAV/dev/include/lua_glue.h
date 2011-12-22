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

#ifndef LUA_GLUE_H
#define LUA_GLUE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "lua_utility.h"

/*
	Glue
	template annotation class to bind C++ types to Lua (via boxed pointer)
		(AKA Type Traits)
	includes (single-)inheritance and many helper functions
	
	NB: Glue does not preserve identity between userdata and their objects.
		That is, pushing pointer x twice results in two distinct userdata.	
		For this reason, the default behavior for __gc is to take no action.
		The usr_push and usr_gc methods can be used to trigger reqeust/release 
			behaviors for reference-counting, if desired.
		The usr_eq method can be used to query identity of objects in Lua.
		
		
	Example usage (for a class Foo defined below):
	
	class Base;
	class Foo : public Base {
	public:
		Foo(int x) : Base(), x(x), refs(0) {}
		~Foo() {}
		int x;
		int refs;
	}
	
	// Creating the binding requires a stage of definitions, followed by a registration call.
	
	
	// Definition
	
	// Required: define the binding name. It should be unique to the class.
	template<> const char * Glue<Foo>::usr_name() { return "Foo"; }
	
	// Optional: constructor. Defines the body of the lua_CFunction create() in Foo's metatable.
	template<> Foo * Glue<Foo>::usr_new(lua_State * L) { 
		int x = lua_checknumber(L, 1);
		return new Foo(x); 
	}
	
	
	
	
	// Optional: methods
	// Methods can be added to the Foo binding using usr_mt. 
	// First define methods as standard lua_CFunctions, using Glue<Foo>::checkto etc,
	// then define their names in the metatable using usr_mt:
	static int lua_foo_double(lua_State * L) {
		Foo * self = Glue<Foo>::checkto(L, 1);
		Foo * double = new Foo(self->x * 2);
		Glue<Foo>::push(L, double);
		return 1;
	}
	template<> void Glue<Foo> :: usr_mt(lua_State * L) {
		lua_pushcfunction(L, lua_foo_double); lua_setfield(L, -2, "double");
		// etc. for each method
	}
	
	
	
	// Optional: additional type-checking
	// Glue<> by default will check for a matching metatable and valid pointer 
	// on any call of Glue<Foo>::to()  or Glue<Foo>::checkto. 
	// Additional tests can be added with usr_to. 
	// The Lua userdata is at stack position idx.
	template<> Foo * Glue<Foo>::usr_to(lua_State * L, Foo * u, int idx) {
		// e.g. require that Foo->x is > 0
		return u->x > 0 ? u : NULL;
	}
	
	// Optional: Type co-ercion.
	// If Glue::checkto<Foo> or Glue::to<Foo> is called on a Lua stack value that is not a valid Foo, 
	// this function (if defined) may be called to co-erce the value in question into a Foo.
	// return NULL to signal failure to co-erce type, or a valid Foo pointer if coercion is possible.
	template<> Foo * Glue<Foo>::usr_reinterpret(lua_State * L, int idx) {
		// define coercion of number to Foo:
		if (lua_isnumber(L, idx)) {
			return new Foo(lua_tonumber(L, idx));
		}
		return NULL;
	}
	
	// Optional: tostring() handler
	// by default, Glue reports objecst as something like "Foo: 0x23238". 
	// this behavior can be overridden for more useful reporting:
	template<> int Glue<Foo>::usr_tostring(lua_State * L, Foo * u) {
		lua_pushfstring(L, "Foo(%d)", u->x);
		return 1; 
	}
	
	
	// Optional: Reference-counting, garbage collection etc.
	// by default, Glue<> does not delete userdata when garbage collected. 
	// to enable this, define usr_gc as follows:
	template<> Glue<Foo>::usr_gc(lua_State * L, Foo * u) {
		delete u;
	}
	// However, there is a problem; if the same Foo pointer was pushed twice, then it will be deleted on the first __gc call, and leave the second userdata as a dangling pointer.
	// one solution is to add reference counting to the Foo object itself; the binding can implement it as follows:
	template<> void Glue<Foo>::usr_push(lua_State * L, Foo * u) {
		u->refs++;
	}
	template<> void Glue<Foo>::usr_gc(lua_State * L, Foo * u) {
		if (--u->refs <= 0) delete u;
	}
	
	
	// Optional: Inheritance. The Foo binding can inherit the metatable of Base. 
	// WARNING: the Base metatable should be defined () before the Foo metatable 
	// (call Glue::define<Base>() before Glue::define<Foo>()).
	template<> const char * Glue<Foo>::usr_supername() { return Glue<Base>::usr_name(); }
	
	
	// Optional: Equality
	// Glue by default will test equality on the pointers in the userdata.
	// This behavior can be overridden with usr_eq.
	// however, note that the test will not made for super/base classes. 
	template<> bool Glue<Foo> :: usr_eq(lua_State * L, Foo * a, Foo * b) {
		// e.g. test on contents, rather than pointer:
		return a.x == b.x;
	}
	
	// Optional: Property Access
	// It's possible to distinguish between methods and properties, making a userdata 
	// look more like a table.  Methods are typically actions and properties are 
	// typically state values.  For example:
	// object:draw() v. object.radius = 1.2 or local radius = object.radius
	// Properties can defined as read-only, write-only, or read-write.
	
	// registering the methods and properties:
	template<> void Glue<Object>::usr_mt(lua_State * L) {
		static luaL_reg methods[] = {
			{ "draw", object_draw },
			{ NULL, NULL }
		};

		static luaL_reg getters[] = {
			{ "radius", object_radius },
			{ NULL, NULL }
		};

		static luaL_reg setters[] = {
			{ "radius", object_radius },
			{ NULL, NULL }
		};
		Glue<Foo>::usr_attr_mt(L, methods, getters, setters);
	}
	
	// __index meta-method
	template<> bool Glue<Object>::usr_has_index() { return true;}

	template<> void Glue<Object>::usr_index(lua_State * L, Object * u) {
		Glue<Object>::usr_attr_index(L, u);
	}

	
	// __newindex meta-method
	template<> void Glue<Object>::usr_newindex(lua_State * L, Object * u) {
		Glue<Object>::usr_attr_newindex(L, u);
	}

	// attaching the necessary env table to the userdata
	template<> void Glue<Object> :: usr_push(lua_State * L, Object * u) {
		Glue<Object>::usr_attr_push(L);	
	}
	
	
	// Registration
	
	// typically in the luaopen_x function of a module; after luaL_register(...)
	// the module is at stack index -1
	// first call Glue::define() for each bound class (in order of base class to sub class)
	// then call either register_table for a module.Foo table, or register_ctor for module.Foo function,
	// or simply call define() to register the type-binding without exposing a public user API in the module
	
	// Option 1: This will register the constructor as function Foo in the table at index -1
	Glue<Foo>::define(L);
	Glue<Foo>::register_ctor(L);
	
	// Option 2: This will register the metatable as table Foo in the table at index -1
	Glue<Foo>::define(L);
	Glue<Foo>::register_table(L);
	
	// Option 3: This will define the type, but not publish a constructor or metatable to the user:
	template<> void Glue<Foo>::define(L);
	
*/

template <typename T>
class Glue {
public:
	/*	
		required hook to define metatable name
	*/
	static const char * usr_name();
	/*	
		optional hook to define metatable superclass name
	*/
	static const char * usr_supername();
	
	/*	
		optional hook to define a create function (default returns NULL)
		arguments at stack indices 1+
	*/
	static T * usr_new(lua_State * L);
	
	/*	
		optional hook to specify __gc method (default is no action) 
		NB: multiple calls to push() the same object will result in an equal 
			number of calls to gc() the same object (i.e. some form of reference 
			counting may be appropriate)
	*/
	static void usr_gc(lua_State * L, T * u);
	
	/*	
		optional hook to apply additional behavior when pushing a T into Lua 
			(e.g. reference count increment, create userdata environment, etc.)
		userdata is at stack index -1
	*/
	static void usr_push(lua_State * L, T * u);
	
	
	/*
		optional hook to signal the use of usr_index.  If defined, 
		the __index metamethod will us usr_index instead of directly 
		using the metatable
	*/
	static bool usr_has_index();	
	
	/*	
		optional hook to specify __index method (default is to use the metatable) 
		key will be at stack index 2
		(userdata itself is at index 1)
		
		NOTE: usr_has_index() must return true if this is used
	*/
	static void usr_index(lua_State * L, T * u);
	
	/*	
		optional hook to specify __newindex method (default is to signal an error) 
		key will be at stack index 2, value will be at stack index 3
		(userdata itself is at index 1)
	*/
	static void usr_newindex(lua_State * L, T * u);
	
	/*	
		optional hook to specify __eq method 
			(default tests equality of pointers a and b)
		NB: Will only apply for objects of the same T (not superclasses)
			The Lua manual states that the __eq metamethod "only is selected 
			when both objects being compared have the same type and the same 
			metamethod for the selected operation."
	*/
	static bool usr_eq(lua_State * L, T * a, T * b);
	
	/*	
		optional hook to convert non-userdata value at stack index idx to a T 
		e.g. convert a number into a T object...
	*/
	static T * usr_reinterpret(lua_State * L, int idx);
	
	/*	
		optional extra hook when retrieving a T from Lua (invoked by Glue::to(), Glue::checkto() etc.)
		this is an opportunity to insert extra conditions on verifying the userdata as of valid type
			(e.g. checking for a magic number within u)
		return u if the condition is met; return NULL if not.
		(Lua userdata is at stack index idx)
	*/
	static T * usr_to(lua_State * L, T * u, int idx);
	
	/*	
		optional hook to override the default __tostring method 
	*/
	static int usr_tostring(lua_State * L, T * u);
	
	/*	
		optional hook to add additional fields to metatable
		metatable is at stack index -1
	*/
	static void usr_mt(lua_State * L);

	/*	create the metatable 
		if superclass != NULL, metatable will inherit from the superclass metatable 
			(which must already be published)
		call this e.g. in luaopen_xxx
	*/
	static void define(lua_State * L);
	
	/*	register either
			the constructor function (create; usr_new must be defined) 
			or the metatable itself 
		to the table at stack index -1, under the name usr_name
		call this e.g. in luaopen_xxx
	*/
	static void register_ctor(lua_State * L);
	static void register_table(lua_State * L);
	
	/*
		make the metatable callable with __call
	*/
	static void register_class(lua_State * L);
	
	/*
		Register the metatable as a module with name libname.
		This puts the metatable in package.loaded[libname] and leaves 
		the table on the top of the stack.
	*/
	static void register_module(lua_State * L, const char *libname);
	
	/*	Install additional methods to metatable via a luaL_Reg array */
	static void usr_lib(lua_State * L, const luaL_Reg * lib);
	
	/*	
		push a T pointer to the Lua space (also calls usr_push if defined) 
		NB: pushing the same object will create a new userdatum for each push
	*/
	static int push(lua_State * L, T * u);
	/*	if index idx is a T (checks metatable key), returns it, else return NULL */
	static T * to(lua_State * L, int idx = 1);
	/*	as above but throws error if not found */
	static T * checkto(lua_State * L, int idx = 1);
	/*	Lua bound constructor (usr_new must be defined) */
	static int create(lua_State * L);
	/*	zero the pointer in the userdata */
	static void erase(lua_State * L, int idx);
	
	
	/*
		Intended for use in a custom usr_mt function for registering userdata 
		functions that operate like table fields as opposed to function calls.  
		Additionally adds functionality for per-userdata custom fields with __newindex.
	*/
	static void usr_attr_mt(
		lua_State *L, 
		luaL_reg *methods, 
		luaL_reg *getters = NULL, 
		luaL_reg *setters = NULL
	);

	/*
		Intended for use in a custom usr_push.  It adds an environment table to the 
		userdata where per-userdata custom fields are stored.  An environment table is 
		required if usr_attr_index and usr_attr_newindex are also used.
	*/
	static void usr_attr_push(lua_State * L);

	/*
		Intended for use in usr_index.  First checks the enviroment table and then the 
		metatable.
	*/
	static int usr_attr_index(lua_State *L);

	/*
		Intended for use in usr_newindex.  First checks the metatable and then uses the 
		environment table.
	*/
	static int usr_attr_newindex(lua_State *L);
	
	/*
		For userdata with attributes, prototype constructors (table constructors) can 
		be used to automatically set settable attributes.  This function checks to see 
		if the argument passed to usr_new is a prototype argument.
	*/
	static bool usr_attr_is_prototype(lua_State *L, int idx=1);
	
	/*
		Uses a prototype constructor to set a userdata's attributes.  Intended to be used 
		in usr_attr_push where index 1 is the prototype table and index -1 is the userdata.
		usr_attr_prototype will call usr_attr_is_prototype to check if it can be used.
	*/
	static int usr_attr_prototype(lua_State *L);
	
	
	/*
		Utility functions for mapping pointers to userdata values.  Used most often in callback 
		situations when you need to go from a pointer to the actual userdata value in Lua 
		and don't want to create a totally new userdata.
	*/
	static int usr_set_reference_map(lua_State *L, T *u);
	static int usr_get_reference_map(lua_State *L, T *u);

	// internal methods:
	static const char * mt_name(lua_State * L);
	static int gc(lua_State * L);
	static int tostring(lua_State * L);
	static int newindex(lua_State * L);
	static int index(lua_State * L);
	static int eq(lua_State * L);
private:
	// internal methods:
	static int class_create(lua_State *L);
	// object wrapper:
	struct Box { T * ptr; };
};

/*
	Inline implementation
*/
#pragma mark Inline Implementation

template <typename T> const char * Glue<T> :: usr_supername() { return NULL; }
template <typename T> T * Glue<T> :: usr_new(lua_State * L) { printf("NULL USER_NEW\n"); return 0; }
template <typename T> T * Glue<T> :: usr_reinterpret(lua_State * L, int idx) { return 0; } // if idx isn't a userdata
template <typename T> void Glue<T> :: usr_mt(lua_State * L) {}
template <typename T> void Glue<T> :: usr_lib(lua_State * L, const luaL_Reg * lib) {
	luaL_getmetatable(L, mt_name(L));
	while (lib->name) {
		lua_pushcclosure(L, lib->func, 0);
		lua_setfield(L, -2, lib->name);
		lib++;
	}
	lua_pop(L, 1);
}
template <typename T> void Glue<T> :: usr_gc(lua_State * L, T * u) {}

template <typename T> bool Glue<T> :: usr_has_index() { return false; }

template <typename T> void Glue<T> :: usr_index(lua_State * L, T * u) {
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);
}

template <typename T> void Glue<T> :: usr_newindex(lua_State * L, T * u) {
	luaL_error(L, "cannot assign keys/values to %s", Glue<T>::usr_name());
}
template <typename T> int Glue<T> :: usr_tostring(lua_State * L, T * u) { 
	lua_pushfstring(L, "%s: %p", Glue<T>::usr_name(), u); 
	return 1; 
}
template <typename T> bool Glue<T> :: usr_eq(lua_State * L, T * a, T * b) {
	return a == b;
}
template <typename T> void Glue<T> :: usr_push(lua_State * L, T * u) {}

template <typename T> T * Glue<T> :: usr_to(lua_State * L, T * u, int idx) { return u; }

template <typename T> int Glue<T> :: push(lua_State * L, T * u) {
	if (u==0)
		return luaL_error(L, "Cannot create null %s", usr_name());
	Box * udata = (Box *)lua_newuserdata(L, sizeof(Box));
	luaL_getmetatable(L, mt_name(L));
	lua_setmetatable(L, -2);
	udata->ptr = u;
	usr_push(L, u);	
	return 1;
}

template <typename T> int Glue<T> :: create(lua_State * L) {
	T * u = usr_new(L);
	if (!u) 
		luaL_error(L, "Error creating %s", usr_name());
	push(L, u);
	return 1;
}
template <typename T> void Glue<T> :: erase(lua_State * L, int idx) {
	if (checkto(L, idx)) {
		Box * udata = (Box *)lua_touserdata(L, idx);
		udata->ptr = NULL;
	}
}
template <typename T> T * Glue<T> :: to(lua_State * L, int idx) {
	T * u = 0;
	lua_pushvalue(L, idx);
	if (lua_isuserdata(L, -1)) {
		if (lua_getmetatable(L, -1)) {
			lua_getfield(L, -1, Glue<T>::usr_name());
			if (!lua_isnoneornil(L, -1)) {
				u = ((Box *)lua_touserdata(L, -3))->ptr;
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	} else if (!lua_isnoneornil(L, idx)) {
		u = usr_reinterpret(L, lua_gettop(L));
	}
	lua_pop(L, 1);
	return u != NULL ? usr_to(L, u, idx) : NULL;
}
template <typename T> T * Glue<T> :: checkto(lua_State * L, int idx) {
	T * u = to(L, idx);
	if (u == 0) luaL_error(L, "%s not found (index %d)", usr_name(), idx);
	return u;
}
template <typename T> void Glue<T> :: define(lua_State * L) {
	luaL_newmetatable(L, mt_name(L));
	lua_pushstring(L, Glue<T>::usr_name());
	lua_pushboolean(L, true);
	lua_settable(L, -3);

	if (usr_has_index()) {
		lua_pushcfunction(L, index);
		lua_setfield(L, -2, "__index");
	}
	else {
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}

	lua_pushcfunction(L, newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pushcfunction(L, gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, gc);
	lua_setfield(L, -2, "close");	/* equivalent to __gc but manually called */
	lua_pushcfunction(L, create);
	lua_setfield(L, -2, "create");

	int u_mt = lua_gettop(L);
	usr_mt(L);
	
	if (usr_supername() != NULL) {
		const char * supername = lua_pushfstring(L, "meta_%s", usr_supername()); lua_pop(L, 1);
		luaL_getmetatable(L, supername);
		lua_setmetatable(L, -2);
	}
	
	lua_settop(L, u_mt-1);
}

template <typename T> void Glue<T> :: register_table(lua_State * L) {
	luaL_getmetatable(L, mt_name(L));
	lua_setfield(L, -2, usr_name());
}
template <typename T> void Glue<T> :: register_ctor(lua_State * L) {
	lua_pushcfunction(L, Glue<T>::create);
	lua_setfield(L, -2, usr_name());
}

template <typename T> int Glue<T> :: class_create(lua_State *L) {
	lua_remove(L, 1);
	return create(L);
}

template <typename T> void Glue<T> :: register_class(lua_State * L) {
	luaL_getmetatable(L, mt_name(L));
	lua_newtable(L);
		lua_pushstring(L, "__call");
		lua_pushcfunction(L, Glue<T>::class_create);
		lua_rawset(L, -3);
	lua_setmetatable(L, -2);
	lua_pop(L, 1);
}

template <typename T> void Glue<T> :: register_module(lua_State * L, const char *libname) {
	lua_getglobal(L, "package");
		lua_getfield(L, -1, "loaded");
			luaL_getmetatable(L, mt_name(L));
			lua_setfield(L, -2, libname);
	lua_pop(L, 2);
	
	luaL_getmetatable(L, mt_name(L));
}


template <typename T> int Glue<T> :: gc(lua_State * L) { 
	T * u = to(L, 1); 
	if (u) { 
		Glue<T>::usr_gc(L, u); 
		// is this really necessary?
		lua_pushnil(L);
		lua_setmetatable(L, 1);
	} 
	return 0; 
}

template <typename T> int Glue<T> :: index(lua_State * L) { 
	T * u = to(L, 1); 
	if (u) { 
		Glue<T>::usr_index(L, u); 
	} 
	return 1; 
}

template <typename T> int Glue<T> :: newindex(lua_State * L) {
	T * u = to(L, 1); 
	if (u) { 
		Glue<T>::usr_newindex(L, u); 
	} 
	return 0; 
}
template <typename T> int Glue<T> :: eq(lua_State * L) {
	T * a = to(L, 1);
	T * b = to(L, 2);
	lua_pushboolean(L, usr_eq(L, a, b) == true);
	return 1;
}
template <typename T> int Glue<T> :: tostring(lua_State * L) {
	T * u = to(L, 1);
	if (u)
		Glue<T>::usr_tostring(L, u);
	else
		lua_pushfstring(L, "%s: nil", Glue<T>::usr_name());
	return 1;
}
template <typename T> const char * Glue<T> :: mt_name(lua_State * L) {
	const char * t = lua_pushfstring(L, "meta_%s", usr_name()); lua_pop(L, 1); return t;
}

template <typename T> void Glue<T>::usr_attr_mt(
	lua_State *L, 
	luaL_reg *methods, 
	luaL_reg *getters, 
	luaL_reg *setters
) {
	for(int i=0; methods[i].name; i++) {
		lua_pushcfunction(L, methods[i].func);
		lua_setfield(L, -2, methods[i].name);
	}
	
	lua_newtable(L);
	if(getters) {
		for(int i=0; getters[i].name; i++) {
			lua_pushcfunction(L, getters[i].func);
			lua_setfield(L, -2, getters[i].name);
		}
	}
	lua_setfield(L, -2, "__getters");
	

	lua_newtable(L);
	if(setters) {
		for(int i=0; setters[i].name; i++) {
			lua_pushcfunction(L, setters[i].func);
			lua_setfield(L, -2, setters[i].name);
		}
	}
	lua_setfield(L, -2, "__setters");
}

template <typename T> void Glue<T>::usr_attr_push(lua_State * L) {
	lua_newtable(L);
	lua_setfenv(L, -2);
}

template <typename T> int Glue<T>::usr_attr_index(lua_State *L) {
	// first check metatable:
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);	// bad result; but leave metatable there... 
		// next try __getters:
		lua_pushstring(L, "__getters");
		lua_rawget(L, -2);
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if(lua_type(L, -1) == LUA_TFUNCTION) {
			lua_pushvalue(L, 1);
			int err = lua_pcall(L, 1, 1, 0);
			if(err) {
				luaL_error(L, "error indexing %s.%s: %s\n",
										lua_tostring(L, 1),
										lua_tostring(L, 2),
										lua_tostring(L, -1));
			}
		} else {
			lua_pop(L, 3); // non-getter, __getters, metatable
			// try fenv
			lua_getfenv(L, 1);
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
		}
	}
	return 1;
	
//	lua_getfenv(L, 1);
//	lua_pushvalue(L, 2);
//	lua_rawget(L, -2);
//
//	if(lua_isnil(L, -1)) {
//		lua_pop(L, 2);
//		lua_getmetatable(L, 1);
//
//		lua_pushvalue(L, 2);
//		lua_rawget(L, -2);
//		
//		if(lua_isnil(L, -1)) {
//			lua_pop(L, 1);
//			lua_pushstring(L, "__getters");
//			lua_rawget(L, -2);
//			
//			lua_pushvalue(L, 2);
//			lua_rawget(L, -2);
//			
//			if(lua_type(L, -1) == LUA_TFUNCTION) {
//				lua_pushvalue(L, 1);
//				int err = lua_pcall(L, 1, 1, 0);
//				if(err) {
//					luaL_error(L, "error indexing %s.%s: %s\n",
//											lua_tostring(L, 1),
//											lua_tostring(L, 2),
//											lua_tostring(L, -1));
//					lua_pop(L, 1);
//					return 0;
//				}
//			}
//		}
//	}
//	
//	return 1;
}

template <typename T> int Glue<T>::usr_attr_newindex(lua_State *L) {

	// find setter:
	lua_getmetatable(L, 1);
	lua_pushstring(L, "__setters");
	lua_rawget(L, -2);
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
	// call setter:
	if(lua_type(L, -1) == LUA_TFUNCTION) {
		lua_pushvalue(L, 1);
		lua_pushvalue(L, 3);
		int err = lua_pcall(L, 2, 0, 0);
		if(err) {
			luaL_error(L, "error indexing %s.%s: %s\n",
										lua_tostring(L, 1),
										lua_tostring(L, 2),
										lua_tostring(L, -1));
		}
	} else {
		lua_pop(L, 1); // non-setter
	}
	lua_pop(L, 2); // __setters, metatable

	// store the value in the fenv:
	lua_getfenv(L, 1);
	lua_pushvalue(L, 2);
	lua_pushvalue(L, 3);
	lua_rawset(L, -3);
	lua_pop(L, 1);	// fenv
	return 0;

//	lua_getmetatable(L, 1);
//
//	lua_pushstring(L, "__setters");
//	lua_rawget(L, -2);
//		lua_pushvalue(L, 2);
//		lua_rawget(L, -2);
//		
//	if(lua_type(L, -1) == LUA_TFUNCTION) {
//		lua_pushvalue(L, 1);
//		lua_pushvalue(L, 3);
//		
//		int err = lua_pcall(L, 2, 0, 0);
//		if(err) {
//			luaL_error(L, "error indexing %s.%s: %s\n",
//										lua_tostring(L, 1),
//										lua_tostring(L, 2),
//										lua_tostring(L, -1));
//			lua_pop(L, 1);
//		}
//		lua_pop(L, 3);
//	}
//	else {
//		lua_pop(L, 3);
//		
//		lua_getfenv(L, 1);
//		lua_pushvalue(L, 2);
//		lua_pushvalue(L, 3);
//		lua_rawset(L, -3);
//
//		lua_pop(L, 1);
//	}
//	return 0;
}

template <typename T> bool Glue<T>::usr_attr_is_prototype(lua_State *L, int idx) {
	return lua_istable(L, idx) == 1;
}

template <typename T> int Glue<T>::usr_attr_prototype(lua_State *L) {
	int ud_idx = lua_gettop(L);
	int pidx = ud_idx-1;
	if(pidx > 1) {
	//	pidx = 1;
	}
	if(usr_attr_is_prototype(L, pidx)) {
		// iterate through the prototype table
		lua_getmetatable(L, -1);
		lua_pushstring(L, "__newindex");
		lua_rawget(L, -2);
		int newindex_idx = lua_gettop(L);

		// table is in the stack at index 't'
		lua_pushnil(L);  // first key
		while (lua_next(L, pidx) != 0) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			lua_pushvalue(L, newindex_idx);	// ud.__newindex function
			lua_pushvalue(L, ud_idx);		// ud
			lua_pushvalue(L, -4);			// prototype key
			lua_pushvalue(L, -4);			// prototype value
			lua_call(L, 3, 0);				// ud.__newindex(ud, key, value)

			// removes 'value'; keeps 'key' for next iteration
			lua_pop(L, 1);
		}
		lua_pop(L, 2);
	}
	return 0;
}


#define USERDATA_REF_MAP_FIELD	"__USERDATA_REF_MAP_FIELD"
template <typename T> int Glue<T>::usr_set_reference_map(lua_State *L, T *u) {
	// add ref to metatable
	luaL_getmetatable(L, mt_name(L));
	lua_getfield(L, -1, USERDATA_REF_MAP_FIELD);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_newtable(L);
		
		// make weak values
		lua_newtable(L);
		lua_pushstring(L, "v");
		lua_setfield(L, -2, "__mode");
		lua_setmetatable(L, -2);
		
		lua_setfield(L, -2, USERDATA_REF_MAP_FIELD);
		lua_getfield(L, -1, USERDATA_REF_MAP_FIELD);
	}
	lua_pushinteger(L, (lua_Integer)u);
	lua_pushvalue(L, -4);
	lua_rawset(L, -3);
	lua_pop(L, 2);
	return 0;
}

template <typename T> int Glue<T>::usr_get_reference_map(lua_State *L, T *u) {
	luaL_getmetatable(L, mt_name(L));
	lua_getfield(L, -1, USERDATA_REF_MAP_FIELD);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2);
		return 0;
	}
	lua_pushinteger(L, (lua_Integer)u);
	lua_rawget(L, -2);
	lua_insert(L, -3);
	lua_pop(L, 2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	return 1;
}

#endif /* include guard */

