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
	width = 1.,
}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('renderfx.blurgauss', setconstructor)
----------------------------------------------------------
-- @param
--- ctx - render context name
function C:__call(init)
	init = ctxtable(init)
	init.type = init.type or GL.FLOAT
	
	-- setup fx as an extension of Texture
	local m = Texture(init)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end

	-- blur shader
	m.gauss = Shader{
		ctx = ctxstring(init),
		file = findfile("cf.gaussian.2p.shl"),
	}
	
	-- intermediate texture
	m.pass1 = Texture(init)
	
	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:draw(tex)
	local width = self.width
	local blur = self.gauss
	local pass1 = self.pass1
	
	gl.Color(1, 1, 1, 1)
	-- first blur pass
	pass1:startcapture()
	enter_ortho()
		tex:bind(0)
		blur:param("width", {width, 0})
		blur:bind()
		gl.Color(1, 1, 0, 1)
		gl.Begin(GL.QUADS)
			quad()
		gl.End()
		blur:unbind()
		tex:unbind(0)
	leave_ortho()
	pass1:endcapture()
	
	-- second blur pass
	self:startcapture()
	enter_ortho()
		pass1:bind(0)
		blur:param("width", {0, width})
		blur:bind()
		gl.Begin(GL.QUADS)
			quad()
		gl.End()
		blur:unbind()
		pass1:unbind(0)
	leave_ortho()
	self:endcapture()
end