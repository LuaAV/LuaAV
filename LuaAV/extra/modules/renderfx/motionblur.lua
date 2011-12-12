local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local ipairs = ipairs

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
	amt = 2.,
	angle = 45.,
}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('renderfx.motionblur', setconstructor)
----------------------------------------------------------
-- @param
--- ctx - render context name
function C:__call(init)
	init = ctxtable(init)
	
	-- setup fx as an extension of a Texture from a Slab
	local oslab = Slab{
		ctx = init.ctx,
		file = findfile("cf.motionblur.shl"),
	}
	
	local m = oslab:texture()
	m.oslab = oslab
	for k, v in pairs(init) do
		m[k] = v
	end
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	local amt = params.amt
	local scales = {
		1/(1+amt*0.5),
		1/(1+amt*0.4),
		1/(1+amt*0.3),
		1/(1+amt),
	}

	-- intermediate slabs
	local dim = m.dim
	m.slabs = {}
	for i, s in ipairs(scales) do
		local slab = Slab{
			ctx = ctx,
			file = findfile("cf.motionblur.shl"),
		}
		slab:texture().dim = {dim[1]*s, dim[2]*s}
		
		m.slabs[i] = slab
	end
	
	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:draw(tex)
	local amt = self.amt
	local angle = self.angle
	local slabs = self.slabs
	local oslab = self.oslab
	
	for i, s in ipairs(slabs) do
		s:param("amount", amt)
		s:param("angle", angle)
	end
	oslab:param("amount", amt)
	oslab:param("angle", angle)
	
	slabs[1]:draw(tex)
	slabs[2]:draw(slabs[1])
	slabs[3]:draw(slabs[2])
	slabs[4]:draw(slabs[3])
	oslab:draw(slabs[4])
end