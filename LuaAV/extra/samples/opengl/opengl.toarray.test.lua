local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Texture = require("opengl.Texture")

local ctx = "Test Texture Copy"
win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = {0.2, 0.2, 0.2, 1}


local source = Texture(ctx)
local dest = Texture(ctx)

go("draw", function()
	
end)

function win:draw()
	source:startcapture()
		gl.LineWidth(10)
		gl.Begin(GL.LINES)
			gl.Color(1, 0, 0, 1)
			gl.Vertex(-1, 1, 0)
			gl.Vertex(1, -1, 0)
			
			gl.Color(0, 1, 1, 1)
			gl.Vertex(1, 1, 0)
			gl.Vertex(-1, -1, 0)
		gl.End()
	source:endcapture()

	source:toarray()
	
	dest:fromarray( source:array() )
	event("draw")
	
	sketch.enter_ortho()
		gl.PushMatrix()
			gl.Translate(-0.5, 0, 0)
			gl.Scale(0.5, 0.5, 0.5)
			
			gl.Color(1, 1, 1, 1)
			source:bind()
			gl.Begin(GL.QUADS)
				sketch.quad()
			gl.End()
			source:unbind()
		gl.PopMatrix()
		
		gl.PushMatrix()
			gl.Translate(0.5, 0, 0)
			gl.Scale(0.5, 0.5, 0.5)
			
			gl.Color(1, 1, 1, 1)
			dest:bind()
			gl.Begin(GL.QUADS)
				sketch.quad()
			gl.End()
			dest:unbind()
		gl.PopMatrix()
	sketch.leave_ortho()
end