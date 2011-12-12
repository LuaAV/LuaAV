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
local Texture = muro.Texture
local sketch = muro.Sketch
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

local params = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('renderfx.thinlens', setconstructor)
----------------------------------------------------------
-- @param
--- ctx - render context name
function C:__call(init)
	init = ctxtable(init)
	init.type = GL.FLOAT
	
	-- setup fx as an extension of Texture
	local m = Texture(init)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end

	-- texcoord visualizer shader
	m.lens = Shader{
		ctx = ctxstring(init),
		file = findfile("dof.thinlens.shl"),
	}
	
	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:param(...)
	self.lens:param(...)
end

function M:draw(f)
	self:startcapture()
		self.lens:bind()		
			f()	
		self.lens:unbind()
	self:endcapture()
end