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
	slope = 0.5,
}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('renderfx.planeramp', setconstructor)
----------------------------------------------------------
-- @param
--- ctx - render context name
--- slope - plane slope [0, 1]
function C:__call(init)
	init = init or {}
	
	-- setup fx as an extension of Texture
	local m = Texture(init)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end

	-- texcoord visualizer shader
	m.viztc = Shader{
		ctx = ctxstring(init),
		file = findfile("viz.texcoords.shl"),
	}
	
	-- parameter defaults
	local vals = totable(init)
	for k, v in pairs(params) do
		m[k] = vals[k] or v
	end
	
	return m
end

function M:draw()
	local eps = 0.001
	local lensplane_angle = self.slope*pi*0.5
	local x = 1./( tan(lensplane_angle)+eps )

	self:startcapture()
--------		
		gl.ShadeModel(GL.SMOOTH)
		enter_ortho()

		gl.Color(1, 1, 1, 1)
		self.viztc:bind()
		gl.Begin(GL.QUAD_STRIP)
			gl.TexCoord(x, x)	gl.Vertex(-1, 1, 0)
			gl.TexCoord(x, x)	gl.Vertex(1, 1, 0)
			
			gl.TexCoord(0, 0)	gl.Vertex(-1, 0, 0)
			gl.TexCoord(0, 0)	gl.Vertex(1, 0, 0)
			
			gl.TexCoord(x, x)	gl.Vertex(-1, -1, 0)
			gl.TexCoord(x, x)	gl.Vertex(1, -1, 0)
		gl.End()
		self.viztc:unbind()
		leave_ortho()	
--------	
	self:endcapture()
end