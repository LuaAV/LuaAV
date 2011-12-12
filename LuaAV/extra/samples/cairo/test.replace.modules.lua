local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Label = require("label")

local cgl_util = require("cgl.util")

local ctx = "Replace Module"
win = Window(ctx, 30, -1000, 900, 600)
win.clearcolor = {0.93, 0.93, 0.93, 0}

function win:key(e, k)
	if(e == "down" and k == 32) then
		cgl_util.opengl_to_cgl_replace(
			script.path.."/test.pdf",
			self,
			win.draw
		)
	end
end

local label = Label{
	ctx = ctx,
}

function gl.label_3d(self, dim, pos, text)
	self:draw_3d(dim, pos, text)
end

function win:draw()
	gl.Scale(0.5, 0.5, 0.5)
	
	gl.Color(0, 0, 0, 1)
	gl.Begin(GL.LINES)
		gl.Vertex(-1, 0.1, 0)
		gl.Vertex(1, 0.1, 0)
		
		gl.Vertex(0, -1, 0)
		gl.Vertex(0, 1, 0)
	gl.End()
	
	gl.Color(1, 0, 1, 1)
	gl.Begin(GL.LINE_LOOP)
		sketch.circle(50)
	gl.End()
	
	
	
	gl.Color(0, 0, 0, 1)
	gl.label_3d(label, self.dim, {0, 0}, "HHHH")
end