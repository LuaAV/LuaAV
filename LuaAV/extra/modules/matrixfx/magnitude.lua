local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable

local print = print

local math = require 'math'
local table = require 'table'

require 'muro'

local Matrix = muro.Matrix
local Magnitude = muro.Magnitude

local C = {}
local M = {
	outmat,	-- output matrix (adapts to input)
	mag,	-- magnitude filter
}

--[[
	{
		basic = {
			userdata = function(self, inmat),
			table = function(self, inmats), just the first matrix comparison
			__index = error,
		},
		
		adapt_input = {
			userdata = function(self, inmat),
			table = function(self, inmats),
			__index = error,
		},
	}
--]]


local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('matrixfx.magnitude', setconstructor)

--[[------------------------------------------------------
	A lightweight wrapper around muro.Magnitude that adapts 
	to input matrix dimensions and type.
--]]

----------------------------------------------------------
function C:__call()
	-- setup an instance of the fx
	local m = {}
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	-- create fx members
	m.outmat = Matrix()
	m.mag = Magnitude()

	return m
end

function M:calc(inmat)
	-- adapt to input, restricted to 1-plane matrices
	local outmat = self.outmat
	outmat:adapt(1, inmat.type, inmat.dim)
	
	-- calculate
	self.mag:calc(inmat, outmat)
	
	-- return result
	return outmat
end