#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_compiler.hpp"
#include "lua_utility.h"

#include <string>

using al::Compiler;
using al::JIT;
using std::string;


int lua_compiler_compile(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	
	if(lua::is<const char *>(L, 2)) {
//		lua::push<bool>(
//			L, 
//			
//		);
		if (!s->compile(string(lua::to<const char *>(L, 2)), string(luaL_optstring(L, 3, "untitled")))) {
			luaL_error(L, "Compiler.compiler: compilation errors");
		}
	}
	else {
		luaL_error(L, "Compiler.compiler: invalid arguments");
	}
	return 1;
}

int lua_compiler_jit(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	JIT * jit = s->jit();
	if(jit) {
		jit->retain(); // one reference for lua
		if(! Glue<JIT>::usr_get_reference_map(L, jit)) {
			Glue<JIT>::push(L, jit);
		}
		return 1;
	}

	return 0;
}

int lua_compiler_define(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		s->options.defines.push_back(
			string(lua::to<const char *>(L, 2))
		);
	}
	else {
		luaL_error(L, "Compiler.define: invalid arguments");
	}
	return 0;
}

int lua_compiler_include(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		s->options.system_includes.push_back(
			string(lua::to<const char *>(L, 2))
		);
	}
	else {
		luaL_error(L, "Compiler.include: invalid arguments");
	}
	return 0;
}

int lua_compiler_header(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	if(lua::is<const char *>(L, 2) && lua::is<const char *>(L, 3)) {
		s->header(
			string(lua::to<const char *>(L, 2)), 
			string(lua::to<const char *>(L, 3))
		);
	}
	else {
		luaL_error(L, "Compiler.header: invalid arguments");
	}
	return 0;
}

int lua_compiler_dump(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	s->dump();
	return 0;
}

//static int lua_compiler_functions(lua_State * L) {
//	Compiler * c = Glue<Compiler>::checkto(L, 1);
//	lua_newtable(L);
//	int n = 1;
//	for (Module::iterator i = c->module->begin(), e = c->module->end(); i != e; ++i) {
//		lua_pushstring(L, i->getName().data());
//		lua_rawseti(L, -2, n++);
//	}
//	return 1;
//}

int lua_compiler_optimize(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	const char *olevel = lua::opt<const char *>(L, 2, "02");
	s->optimize(string(olevel));
	return 0;
}

int lua_compiler_cpp(lua_State *L) {
	Compiler *s = Glue<Compiler>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->options.CPlusPlus = lua::to<bool>(L, 2) ? 1 : 0;
	}
	else {
		lua::push<bool>(L, (s->options.CPlusPlus == 1) ? true : false);
		return 1;
	}
	return 0;
}

static int lua_compiler_writebitcode(lua_State * L) {
	Compiler * c = Glue<Compiler>::checkto(L, 1);
	const char * filename = luaL_checkstring(L, 2);
	lua_pushboolean(L, c->writebitcode(filename));
	return 1;
}

static int lua_compiler_readbitcode(lua_State * L) {
	Compiler * c = Glue<Compiler>::checkto(L, 1);
	const char * filename = luaL_checkstring(L, 2);
	lua_pushboolean(L, c->readbitcode(filename));
	return 1;
}

static int lua_compiler_datalayout(lua_State * L) {
	Compiler * c = Glue<Compiler>::checkto(L, 1);
	lua_pushstring(L, c->getDataLayout().data());
	return 1;
}

static int lua_compiler_targettriple(lua_State * L) {
	Compiler * c = Glue<Compiler>::checkto(L, 1);
	lua_pushstring(L, c->getTargetTriple().data());
	return 1;
}
//static int lua_compiler_module(lua_State * L) {
//	luaL_dostring(L, "require 'clang.llvm'");
//	Compiler * c = Glue<Compiler>::checkto(L, 1);
//	if (c->module && c->emp) {
//		Glue<ModuleProvider>::push(L, c->emp);
//		return 1;
//	}
//	return 0;
//}


template<> void Glue<Compiler>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "compile", lua_compiler_compile },
		{ "jit", lua_compiler_jit },
		{ "include", lua_compiler_include },
		{ "define", lua_compiler_define },
		{ "header", lua_compiler_header },
		{ "dump", lua_compiler_dump },
		{ "optimize", lua_compiler_optimize },
		{ "writebitcode", lua_compiler_writebitcode },
		{ "readbitcode", lua_compiler_readbitcode },
		{ "datalayout", lua_compiler_datalayout },
		{ "targettriple", lua_compiler_targettriple },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "cpp", lua_compiler_cpp },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "cpp", lua_compiler_cpp },
		{ NULL, NULL}
	};
	Glue<Compiler>::usr_attr_mt(L, methods, getters, setters);
}


int lua_jit_getfunctionptr(lua_State *L) {
	JIT *s = Glue<JIT>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		void * f = s->getfunctionptr(
			string(lua::to<const char *>(L, 2))
		);
		if(f) {
			lua_pushlightuserdata(L, f);
			return 1;
		}
	}
	return 0;
}

int lua_jit_retain(lua_State *L) {
	JIT *s = Glue<JIT>::checkto(L, 1);
	s->retain();
	return 0;
}

int lua_jit_release(lua_State *L) {
	JIT *s = Glue<JIT>::checkto(L, 1);
	s->release();
	return 0;
}


int lua_jit_pushcfunction(lua_State *L) {
	JIT *s = Glue<JIT>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		lua_CFunction f = (lua_CFunction)s->getfunctionptr(
			string(lua::to<const char *>(L, 2))
		);
		if(f) {
			// push JIT as upvalue;
			// prevents garbage collection of JIT while the function f is in use
			// also makes it available at lua_upvalueindex(1) in the JITted code
			//printf("JIT %p\n", s);
			lua_pushvalue(L, 1);
			lua_pushcclosure(L, f, 1);
			return 1;
		}
	}
	return 0;
}

// crash prone
//int lua_jit_dump(lua_State *L) {
//	JIT *s = Glue<JIT>::checkto(L, 1);
//	s->dump(); 
//	return 0;
//}

template<> void Glue<JIT>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		//{ "dump", lua_jit_dump },
		{ "pushcfunction", lua_jit_pushcfunction },
		{ "getfunctionptr", lua_jit_getfunctionptr },
		{ "retain", lua_jit_retain },
		{ "release", lua_jit_release },
		{ NULL, NULL}
	};
	Glue<JIT>::usr_attr_mt(L, methods);
}

static int lua_sweep(lua_State * L) {
	JIT::sweep();
	return 0;
}

static int lua_pointer(lua_State * L) {
	if (lua_toboolean(L, 1)) {
		lua_pushvalue(L, lua_upvalueindex(1));
	} else {
		lua_pushlightuserdata(L, L);
	}
	return 1;
}


extern "C" int luaopen_clang(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	
	struct luaL_reg lib[] = {
		{ "sweep", lua_sweep },
		{ NULL, NULL }
	};
	luaL_register(L, libname, lib);
	
	lua_pushlightuserdata(L, L);
	lua_pushcclosure(L, lua_pointer, 1);
	lua_setfield(L, -2, "lua_pointer");
	
	
	Glue<Compiler>::define(L);
	Glue<Compiler>::register_ctor(L);
	Glue<JIT>::define(L);
	
	//JIT::verbose();
	JIT::sweep();

	return 1;
}
