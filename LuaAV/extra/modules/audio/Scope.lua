local gl = require("opengl")
local GL = gl
local Mesh = require("opengl.Mesh")
local Texture = require("opengl.Texture")
local Slab = require("opengl.Slab")
local Array = require("Array")

local audio = require("audio")
local scopeshaderfile = LuaAV.findfile("audio.scope.vertices.shl")

local setmetatable = setmetatable
local assert = assert
local getfenv = getfenv
local print = print

local math = math

module(...)


local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

function C:__call(init)
	assert(init.ctx)
	local m = setmetatable(init, M)
	
	m.ch0 = audio.scope(0)
	m.ch1 = audio.scope(1)
	
	m.tex = Texture{
		ctx = m.ctx,
		single_channel = GL.LUMINANCE,
		minfilter = GL.NEAREST,
		magfilter = GL.NEAREST,
	}
	
	m.av = Slab{
		ctx = m.ctx,
		file = scopeshaderfile,
		texture = {
			type = GL.FLOAT,
			format = GL.RGB,
			dim = m.ch0.dim,
			minfilter = GL.NEAREST,
			magfilter = GL.NEAREST,
		}
	}
	m.av:param("texdim", m.ch0.dim)
	m.scale = 1.
	
	mesh = Mesh{
		ctx = m.ctx,
		primitive = GL.POINTS,
	}
	
	return m
end

function M:nsamples()
	return self.ch0.dim[1]*self.ch0.dim[2]
end

function M:enable(v)
	audio.scope(v)
end

function M:primitive(prim)
	self.mesh.primitive = prim
end

function M:settime(dur)
	local maxdur = self:nsamples()/audio.samplerate()
	dur = math.min(dur, maxdur)
	self.scale = maxdur/dur
	self.av:param("scale", self.scale)
end

function M:time()
	return self:nsamples()/audio.samplerate()/self.scale
end

function M:draw()
	self.av:param("sample", audio.scope())
	
	self.tex:fromarray(self.ch0)
	self.av:draw(self.tex)
	
	local tex = self.av:texture()
	tex:toarray()
	self.mesh:vertex(tex:array())

	self.mesh:draw()
end