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

local Framebuffer = require("renderfx.framebuffer")
local MotionBlur = require("renderfx.motionblur2")

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

module('renderfx.framebuffer.motionblur', setconstructor)
----------------------------------------------------------
-- @param
--- ctx - render context name
function C:__call(init)
	init = ctxtable(init)
	
	local m = MotionBlur(init)
	m.mbdraw = m.draw
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.fb = Framebuffer(init)
	m.fbvel = Framebuffer(init)
	m.fbvel.type = GL.FLOAT
	m.fbvel.clearcolor = {0, 0, 0, 0}

	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:drawvel(f, ...)
	self.fbvel:draw(f, ...)
end

function M:draw(f, ...)
	self.fb:draw(f, ...)
	self:mbdraw(self.fb, self.fbvel)
end