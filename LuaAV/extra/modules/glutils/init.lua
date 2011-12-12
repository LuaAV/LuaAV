local assert, pairs, tostring, type = assert, pairs, tostring, type
local print = print
local setmetatable = setmetatable

local string = require 'string'

require("opengl")
local gl = gl
local GL = gl
local glu = glu

require("muro")
local sketch = muro.Sketch
local enter_ortho = sketch.enter_ortho
local leave_ortho = sketch.leave_ortho
local quad = sketch.quad

module 'glutils'
----------------------------------------------------------

function pickray(p)
	local p1 = { glu.UnProject(p[1], p[2], 0.) }
	local p2 = { glu.UnProject(p[1], p[2], 1.) }
	
	return p1, p2
end

local defoutline = {0.25, 0.25, 0.25, 1.}
function drawoutline(f, c)
	local c = c or defoutline
	
	gl.Enable(GL.POLYGON_OFFSET)
	gl.Enable(GL.POLYGON_OFFSET_LINE)
	gl.PolygonOffset(-1, -2)
	
	gl.Color(c)
	gl.PolygonMode(GL.FRONT_AND_BACK, GL.LINE)
		f()
	gl.PolygonOffset(0, 0)
	
	gl.PolygonMode(GL.FRONT_AND_BACK, GL.FILL)
	gl.Disable(GL.POLYGON_OFFSET)
end

--[[
Polygon offset, as its name implies, only works with polygonal primitives. It affects only the filled primitives: GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON. Polygon offset will work when you render them with glPolygonMode set to GL_FILL, GL_LINE, or GL_POINT.

Polygon offset doesn't affect non-polygonal primitives. The GL_POINTS, GL_LINES, GL_LINE_STRIP, and GL_LINE_LOOP primitives can't be offset with glPolygonOffset().
--]]
function enter_offset()
	gl.Enable(GL.POLYGON_OFFSET)
	gl.Enable(GL.POLYGON_OFFSET_LINE)
	gl.PolygonOffset(1, 3)
end

function leave_offset()
	gl.PolygonOffset(0, 0)
	gl.Disable(GL.POLYGON_OFFSET)
end

function show(tex, x, y, w, h)
	enter_ortho(x, y, w, h)
		gl.Scale(1, -1, 1)
		tex:bind(0)
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.QUADS)
			quad()
		gl.End()
		tex:unbind(0)
	leave_ortho()
end