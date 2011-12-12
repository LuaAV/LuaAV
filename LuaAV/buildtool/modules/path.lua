--[[
	path{
		[1] = path string,
		relative = option flag specifying a relative path
	}
	
	If the path is relative, the resolve method can be used to 
	concatenate the contained relative path to a root path.
--]]
local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local next = next
local print = print
local unpack = unpack
local setmetatable = setmetatable

local table = require 'table'
local string = require 'string'
local os = require 'os'
local io = require 'io'
local print = print

local OS = _OSNAME

local buildutils = require("buildutils")
local makepath = buildutils.makepath

local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('path', setconstructor)
----------------------------------------------------------
function C.__call(M, init)
	assert(init[1], "path[1]: no path given")

	local m = {}
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m[1] = makepath(unpack(init))
	m.relative = init.relative
	
	return m
end

function M:resolve(root)
	if(self.relative) then
		self[1] = makepath(root, self[1])
	end
end
