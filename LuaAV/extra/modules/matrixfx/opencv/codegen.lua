local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print
local ipairs = ipairs
local getmetatable = getmetatable
local tonumber = tonumber

local string = require 'string'
local math = require 'math'
local table = require 'table'

local cosmo = require("cosmo")
local compiler = require("codegen.compiler")
local Matrix = muro.Matrix

module('matrixfx.opencv.codegen')
-----------------------------------------------------

local template = [===[
#include "lua.h"
#include "matrix_c.h"
#include "matrix_opencv_c.h"
#include "stdlib.h"

$header

typedef struct _$name {
$do_params{$params}[[
	$type $name;
]]
} t_$name;
typedef t_$name* $name;

// LIB
int $name{}_new(lua_State *L);
int $name{}__index(lua_State *L);
int $name{}__tostring(lua_State *L);
int $name{}__gc(lua_State *L);
int $name{}_calc(lua_State *L);

$do_params{struct_name=$name,$params}[=[
$if{$default}[[
	int $struct_name{}_$name(lua_State *L);
]]
]=]
$do_methods{struct_name=$name,$methods}[[
	int $struct_name{}_$name(lua_State *L);
]]

#define LUA_METHOD(name)	{#name, $name{}_##name}
		
luaL_Reg $name{}_methods[] = {
	LUA_METHOD(new),
	{"__gc", $name{}__gc},
	{"__index", $name{}__index},
	{"__tostring", $name{}__tostring},
	LUA_METHOD(calc),
$do_params{$params}[=[
	$if{$default}[[
		LUA_METHOD($name),
	]]
]=]
$do_methods{$methods}[[
	LUA_METHOD($name),
]]
	{0, 0}
};
	
int $name{}_init(lua_State *L) {
	luaL_newmetatable(L, "$name");
	for(int i=0; $name{}_methods[i].name; i++) {
		lua_pushstring(L, $name{}_methods[i].name);
		lua_pushcfunction(L, $name{}_methods[i].func);
		lua_rawset(L, -3);
	}
	return 1;
}

	
$name $name{}_to_udata(lua_State *L, int idx) {
	$name *p_x = ($name *)luaL_checkudata(L, idx, "$name");
	return p_x ? *p_x : 0;
}
	
	
int $name{}_new(lua_State *L) {
	$name *p_x = ($name *)lua_newuserdata(L, sizeof($name));
	$name x = ($name)malloc(sizeof(t_$name));
	*p_x = x;

$do_params{$params}[=[
	$if{$init}[[$init]],[[x->$name = $default;]]
]=]

	luaL_getmetatable(L, "$name");
	lua_setmetatable(L, -2);

	return 1;
}
	
	
int $name{}__gc(lua_State *L) {
	$name x = $name{}_to_udata(L, 1);
	if(x) {
		$do_params{$params}[=[
			$if{$destroy}[[$destroy]]
		]=]
		free(x);
	}
	return 0;
}
	
int $name{}__index(lua_State *L) {
	$name x = $name{}_to_udata(L, 1);
	if(x) {
		lua_getmetatable(L, 1);
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		
		if(lua_isfunction(L, -1)) {
			lua_insert(L, -2);
			lua_pop(L, 1);
			return 1;
		}
		else {
			lua_pop(L, 2);
		}
	}
	return 0;
}
	
int $name{}__tostring(lua_State *L) {
	$name x = $name{}_to_udata(L, 1);
	lua_pushfstring(L, "$name %p", x);
	return 1;
}

$do_params{struct_name=$name,$params}[=[
$if{$default}[[

int $struct_name{}_$name(lua_State *L) {
	$struct_name x = $struct_name{}_to_udata(L, 1);
	if(x) {
		if($check_type{type=$type}(L, 2)) {
			x->$name = $to_value{type=$type}(L, 2);
			return 0;
		}
		else {
			$push_value{type=$type}(L, x->$name);
			return 1;
		}
	}
	else {
		luaL_error(L, "$struct_name{}.$name invalid object");
	}
	return 0;
}
]]
]=]

]===]

local
function push_scope(p, s)
	local pp = {}
	for k, v in pairs(p) do
		pp[k] = v
	end
	for k, v in pairs(s) do
		if(type(k) ~= "number") then
			pp[k] = v
		end
	end
	return pp
end

function generate(M, t)
	t.header = t.header or ""
	
	t.do_methods = function(t)
		if(t[1]) then
			for _, name in ipairs(t[1]) do
				cosmo.yield(push_scope({name = name}, t))
			end
		end
	end

	t.do_params = function(t)
		t.check_type = function(t)
			return ({
				float = "lua_isnumber",
				double = "lua_isnumber",
				char = "lua_isnumber",
				int = "lua_isnumber",
				bool = "lua_isboolean",
			})[t.type]
		end
		t.to_value = function(t)
			return ({
				float = "lua_tonumber",
				double = "lua_tonumber",
				char = "(char)lua_tonumber",
				int = "(int)lua_tonumber",
				bool = "lua_toboolean",
			})[t.type]
		end
		t.push_value = function(t)
			return ({
				float = "lua_pushnumber",
				double = "lua_pushnumber",
				char = "lua_pushnumber",
				int = "lua_pushnumber",
				bool = "lua_pushboolean",
			})[t.type]
		end
		
		t["if"] = function(v)
			if(v[1] == nil) then
				cosmo.yield{_template=2}
			else
				cosmo.yield{_template=1}
			end
		end
	
		for _, tt in ipairs(t) do
			for _, p in ipairs(tt) do
				cosmo.yield(push_scope(p, t))
			end
		end
	end


	local code = cosmo.f(template..t.calc)(t)
	print(code)

	-- compile C code and extract module init function
	local init = assert(
		compiler.compile(
			t.name.."_init", 
			code,
			t.header_paths
		),
		string.format("unable to compile %s module", t.name)
	)

	-- generate parameter setter/getter functions, 
	-- mapping struct into M
	for i, param in ipairs(t.params) do
		local k = param.name
		
		if(param.getter and param.setter) then
		
		elseif(param.getter) then
		
		elseif(param.setter) then
			local setter = param.setter
			M[k] = function(self, val)
				if(val) then
					val = setter(val)
					return self.struct[k](self.struct, val)
				else
					return self.struct[k](self.struct, val)
				end
			end
		else
			M[k] = function(self, val)
				return self.struct[k](self.struct, val)
			end
		end
	end
	
	-- return module table
	return init()
end