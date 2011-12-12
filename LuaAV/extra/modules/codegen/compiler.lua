local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local ipairs = ipairs
local pairs = pairs
local print = print
local getfenv = getfenv
local unpack = unpack
-----------------------------------------------------
local math = require 'math'
local table = require 'table'
local string = require 'string'
-----------------------------------------------------
local clang = require("clang")
local llvm = require("clang.llvm")
-----------------------------------------------------
local apppath = LuaAV.apppath
-----------------------------------------------------
local app = require("app")
local platform = app.platform
local utils = require("app.utilities")
local findfileinpath = utils.findfileinpath
local stripfilename = utils.stripfilename
-----------------------------------------------------

module(..., package.seeall)
-----------------------------------------------------

local compiler_header = ({
	OSX = [[
	]],

	LINUX = [[
	]],
})[platform]



local
function find_header(filename)
	local h = findfileinpath(app.apppath, filename, true)
	if(not h) then
		h = findfileinpath("/usr/local/include", filename, true)
	end

	if(not h) then
		h = findfileinpath("/usr/include", filename, true)
	end
	return h
end

-- required header search paths
local lua_h = find_header("lua.h")
lua_h = stripfilename(lua_h)

local muro_h = find_header("matrix_c.h")
muro_h = stripfilename(muro_h)

local includes = {
	{path = lua_h},
	{path = muro_h},
}

local cc = clang.Compiler()

for i, v in ipairs(includes) do
	cc:include(v.path)
end

if(app.platform == "LINUX") then
	cc:include(app.apppath.."/../../modules/luaclang/llvm-2.6/dev_linux/clang/1.0/include")
	cc:include("/usr/include/lua5.1")
else
	--=cc:include("/Developer/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/i686-apple-darwin9/4.0.1/include")
	cc:include(app.apppath.."/../../modules/luaclang/llvm-2.6/lib_osx/clang/1.0/include")
end


function compile(fname, code, paths)
	code = compiler_header .. code
	paths = paths or {}
	for i, p in ipairs(paths) do
		cc:include(p)
	end
	
	
	local res = {cc:compile(code)}
	if(not res[1]) then
		print(" *** ERROR *** ")
		for k, v in pairs(res) do
			print(k, v)
		end
		print(" *** ERROR *** ")
	else
		local m = res[1]
		_G[m] = true


		local m = res[1]
		if(type(fname) == "table") then
			local f = {}
			for i, name in ipairs(fname) do
				f[i] = assert(cc:getluafunction(name))
			end
			return f
		else
			return assert(cc:getluafunction(fname))
		end
	end
	
	return nil
end