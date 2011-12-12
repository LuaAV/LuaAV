local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable

local print = print

local math = require 'math'
local table = require 'table'

require 'opengl'
require 'muro'

local tan = math.tan
local pi = math.pi

local gl = gl
local GL = gl

local Shader = muro.Shader
local Slab = muro.Slab
local Texture = muro.Texture
local sketch = muro.Sketch
local quad = sketch.quad
local enter_ortho = sketch.enter_ortho
local leave_ortho = sketch.leave_ortho

local findfile = LuaAV.findfile

local
function ctxstring(v)
	if(type(v) == "table")
		then return v.ctx
		else return v
	end
end


local
function ctxtable(v)
	if(type(v) == "string")
		then return {ctx = v}
		else return v
	end
end

local 
function totable(v)
	if(type(v) ~= "table") then
		return {}
	else
		return v
	end
end


local C = {}
local M

local params = {
}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('renderfx.absdiff', setconstructor)
----------------------------------------------------------
-- @param
--- ctx - render context name
function C:__call(init)
	init = ctxtable(init)
	
	-- blur shader
	local absdiff = Slab{
		ctx = ctxstring(init),
		file = findfile("op.absdiff.shl"),
	}
	
	-- setup fx as an extension of Texture within a slab
	local m = absdiff:texture()
	for k, v in pairs(init) do
		m[k] = v
	end
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.absdiff = absdiff
	m.prev = Texture(init)
	
	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:draw(tex)
	local prev = self.prev
	local op = self.absdiff
	
	-- adapt dimension
	if(tex ~= self.last) then
		prev.dim = tex.dim
	end
	self.last = tex
	
	op:draw(tex, prev)
	prev:copy(tex)
end