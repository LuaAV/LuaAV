local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local next = next
local print = print
local setmetatable = setmetatable

local table = require 'table'
local string = require 'string'
local os = require 'os'
local io = require 'io'
local print = print


local buildutils = require("buildutils")
local makepath = buildutils.makepath
local compilefile = buildutils.compilefile
local linkobjects = buildutils.linkobjects
local ensurepath = buildutils.ensurepath
local copyfile = buildutils.copyfile
local removefile = buildutils.removefile

module('autoconf')
----------------------------------------------------------

local test_endianness_code = [[
#include <stdio.h>

int main(int argc, const char *argv[]) {
	int x = 1;
	if(*(char *)&x == 1) {
		printf("ENDIANNESS:LITTLE\n");
	}
	else {
		printf("ENDIANNESS:BIG\n");
	}
	return 0;
}
]]

local test_os_code = [[
#include <stdio.h>

#if defined(WIN32) || defined(__WINDOWS_MM__)
	#define LUAAV_WIN32
	const char *OSNAME = "WIN32";
	int OSBIT = 32;
#elif defined( __APPLE__ ) && defined( __MACH__ )
	#define LUAAV_OSX
	const char *OSNAME = "OSX";
	int OSBIT = 32;
#else
	#define LUAAV_LINUX
	const char *OSNAME = "LINUX";

	#if defined(__LP64__) || defined(_LP64)
        int OSBIT = 64;
    #else
        int OSBIT = 32;
    #endif
#endif

int main(int argc, const char *argv[]) {
	printf("_OSNAME:%s\n", OSNAME);
	printf("_OSBIT:%d\n", OSBIT);
	return 0;
}
]]

function test_os(reporoot)
	local confs = buildutils.configure(reporoot, test_os_code)
	return confs
end
