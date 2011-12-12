#include "lua.h"
#include "lauxlib.h"
#include <stdio.h>


int test_c(lua_State *L) {
	printf("This is a C function test_c\n");
	return 0;
}