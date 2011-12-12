local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")

local print = print

--- Common drawing state and routines
-- @name gui.draw
module(...)

-- primitives
Lines = GL.LINES
LineStrip = GL.LINE_STRIP
LineLoop = GL.LINE_LOOP
Triangles = GL.TRIANGLES
Quads = GL.QUADS

-- enable/disable state
Depth = GL.DEPTH_TEST
Blend = GL.BLEND
Scissor = GL.SCISSOR_TEST

-- blend functions
SourceAlpha = GL.SRC_ALPHA
One = GL.ONE



enable = gl.Enable
disable = gl.Disable
blend = gl.BlendFunc
push = gl.PushMatrix
pop = gl.PopMatrix
scissor = gl.Scissor
translate = gl.Translate

color = gl.Color

--- Enter 2D, orthographic drawing (one-to-one pixels)
-- @param dim The dimensions of the view
function enter2d(dim)
	sketch.enter_ortho(dim)
end

--- Leave 2D, orthographic drawing
function leave2d()
	sketch.leave_ortho()
end

--- Draw 2-tone rectangle
-- @param c1 Color1
-- @param c2 Color2
-- @param prim Primitive
-- @param x x-start
-- @param y y-start
-- @param w Width
-- @param h Height
function rect_color(c1, c2, prim, x, y, w, h)
	gl.Begin(prim)
		color(c1)
		gl.Vertex(x, y, 0)
		gl.Vertex(x+w, y, 0)
		color(c2)
		gl.Vertex(x+w, y+h, 0)
		gl.Vertex(x, y+h, 0)
	gl.End()
end

--- Draw a rectangle
-- @param prim Primitive
-- @param x x-start
-- @param y y-start
-- @param w Width
-- @param h Height
function rect(prim, x, y, w, h)
	gl.Begin(prim)
		gl.Vertex(x, y, 0)
		gl.Vertex(x+w, y, 0)
		gl.Vertex(x+w, y+h, 0)
		gl.Vertex(x, y+h, 0)
	gl.End()
end

function line(x1, y1, x2, y2)
	gl.Begin(GL.LINES)
		gl.Vertex(x1, y1)
		gl.Vertex(x2, y2)
	gl.End()
end