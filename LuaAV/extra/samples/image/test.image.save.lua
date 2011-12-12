local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local Image = require("Image")
local Array = require("Array")

local ctx = "Image Load Test"
win = Window(ctx, 0, 0, 512, 512)

--local image = Image(LuaAV.findfile("glow.jpg"))
local image = Image()
local tex = Texture{
	ctx = ctx,
	dim = {512, 512}
}

go("save", function()

	tex:startcapture()
		gl.LineWidth(4)
		gl.Begin(GL.LINES)
			gl.Color(1, 0, 0, 1)
			gl.Vertex(-1, -1)
			gl.Vertex(1, 1)
			
			gl.Color(0, 0.5, 1, 1)
			gl.Vertex(-1, 1)
			gl.Vertex(1, -1)
		gl.End()
	tex:endcapture()
	
	tex:toarray()
	
	image:fromarray(tex:array())
	image:save(script.path.."/frame.jpg")
end)

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	
	gl.Rotate(20, 0, 0, 1)
	
	event("save")
	
	sketch.enter_ortho()
		gl.Color(1, 1, 1, 1)
		tex:bind()
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex:unbind()
	sketch.leave_ortho()
end