--[[
	Utility data structure for managing a project configuration

	config{
		install_path = installation path,
		[cflags] = optional compilation flags,
		[ldflags] = optional linker flags,
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

local OS = _OSNAME

local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('config', setconstructor)
----------------------------------------------------------
function C.__call(M, init)
	assert(init.install_path, "project.install_path: no path given")
	
	local m = {}
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.cflags = init.cflags or {}
	m.ldflags = init.ldflags or {}
	m.install_path = init.install_path
	
	return m
end