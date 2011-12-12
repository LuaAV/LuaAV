local clang = require("clang")
compiler = clang.Compiler()

local header_path = app.resourcepath.."/Headers"
compiler:include(header_path)

compiler:header("remapped.h", [=[

int external(lua_State * L) {
	printf("external\n");
	return 0;
}

]=])

assert(compiler:compile[=[

#include "lua.hpp"

#include "remapped.h"

extern "C" int test(lua_State * L) {
	printf("test\n");
	external(L);
	return 0;
}

]=])

local jit = assert(compiler:jit())
local test = jit:pushcfunction("test")

print(test())