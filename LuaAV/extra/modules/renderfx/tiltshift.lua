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
local enter_ortho = sketch.enter_ortho
local leave_ortho = sketch.leave_ortho

local PlaneRamp = require("renderfx.planeramp")
local Depthbuffer = require("renderfx.depthbuffer")
local Framebuffer = require("renderfx.framebuffer")

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


local blurfile = "cf.gaussian.2p.shl"
--local blurfile = "cam.blur.shl"
local C = {}
local M

local params = {
	slope = 0.5,
	scale = 1.,
	falloff = 0.5,
}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('renderfx.tiltshift', setconstructor)
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
	
	-- construct filters for filter chain
	m.ramp = PlaneRamp(init)
	m.depthbuffer = Depthbuffer(init)
	m.framebuffer = Framebuffer(init)
	
	m.blurx = Slab{
		ctx = init.ctx,
		dim = init.dim,
		file = findfile(blurfile)
	}
	m.blury = Slab{
		ctx = init.ctx,
		dim = init.dim,
		file = findfile(blurfile)
	}
	
	--"cam.lens.shl"
	
	m.tiltshift = Shader{
		ctx = ctxtable(init),
		--file = LuaAV.findfile(""),
	}
	
	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:draw(f)
	self.ramp.slope = self.slope
	self.blurx:param("width", {self.scale, 0.})
	self.blury:param("width", {0., self.scale})
	
	self.ramp:draw()
	self.depthbuffer:draw(f)
	self.framebuffer:draw(f)
	
	self.blurx:draw(self.framebuffer)
	self.blury:draw(self.blurx)
	
	local tex1 = self.blury:texture() --self.framebuffer
	
	self:startcapture()
		enter_ortho()
		gl.Color(1, 1, 1, 1)
		self.tiltshift:bind()
			tex1:bind(0)
			gl.Begin(GL.QUADS)	
				sketch.quad()	
			gl.End()
			tex1:unbind(0)
		self.tiltshift:unbind()
		leave_ortho()
	self:endcapture()
end