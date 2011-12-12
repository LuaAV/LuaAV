local gl = require("opengl")
local GL = gl

local Label = require("label")
local format = string.format

local sketch = require("opengl.sketch")

local ctx = "Test Label"
win = Window(ctx, 0, 0, 512, 512)

local label = Label{
	ctx = ctx,
	size = 20
}

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:draw()
	gl.PointSize(4)
	gl.Begin(GL.POINTS)
		gl.Vertex(0, 0, 0)
	gl.End()
	
	gl.Color(1, 0, 0.5, 1)
	gl.Begin(GL.LINES)
		gl.Vertex(-2, 0, 0)
		gl.Vertex(2, 0, 0)
	gl.End()
	
	gl.Color(1, 1, 1, 1)
	label:draw_3d(win.dim, {0, 0, 0}, format("now(): %f", now()))
end