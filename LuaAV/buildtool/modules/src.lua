--[[
	A utility structure for manaing src files during compilation.
	
	src{
		src = src file path,
		build_dir = directory to put compiled object file,
	}
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

local buildutils = require("buildutils")
local stripfilename = buildutils.stripfilename
local strippath = buildutils.strippath
local stripextension = buildutils.stripextension

local OS = _OSNAME

local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('src', setconstructor)
----------------------------------------------------------
function C.__call(M, init)
	assert(init.src, "src.src: no source file path given")
	assert(init.build_dir, "src.build_dir: not build directory given")

	local fname = strippath(init.src)
	local stem = stripextension(fname)

	local m = {}
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.src = init.src
	m.obj = string.format("%s/%s.o", init.build_dir, stem)
	
	return m
end