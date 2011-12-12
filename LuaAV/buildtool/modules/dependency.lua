--[[
	dependency{
		name = name of dependency,
		path = path of dependency on disk,
		type = dependency type,
	}
	
	dependency.type must be one of:
		+ "object"
		+ "shared"
		+ "static"
		+ "framework"
		
	A dependency maintains methods for producing compiler and linker flags when 
	it is included in a build step. 
--]]
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

local OS = _OSNAME

local buildutils = require("buildutils")
local strippath = buildutils.strippath

local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

local types = {
	"object",
	"shared",
	"static",
	"framework",
}
for i, v in ipairs(types) do
	types[v] = true
end

local search_flags = {
	object = function(self)
		return ""
	end,

	static = function(self)
		--return string.format("-L%s", self.path)
		return ""
	end,
	
	shared = function(self)
		return string.format("-L%s", self.path)
	end,
	
	framework = function(self)
		return string.format("-F%s", self.path)
	end,
}

local type_flags = {
	object = function(self)
		return string.format("%s/%s", self.path, self.name)
	end,

	static = function(self)
		if type(self.path) == "table" then
			print(self.path, unpack(self.path))
		end
		--return string.format("-l%s", self.name)
		return string.format("%s/lib%s.a", self.path, self.name)
	end,
	
	shared = function(self)
		return string.format("-l%s", self.name)
	end,
	
	framework = function(self)
		return string.format("-framework %s", self.name)
	end,
}

module('dependency', setconstructor)
----------------------------------------------------------
function C.__call(M, init)
	assert(init.name, "dependency.name: no name given")
	assert(init.path, "dependency.path: no path given")
	assert(init.type, "dependency.type: no type given")
	init.type = init.type:lower()
	assert(types[init.type], "dependency.type: invalid, must be " .. table.concat(types, ", "))

	local m = {}
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.name = init.name
	m.path = init.path
	m.type = init.type
	
	return m
end

function M:search_flag()
	return search_flags[self.type](self)
end

function M:linker_flag()
	return string.format("%s %s", self:search_flag(), type_flags[self.type](self))
end
