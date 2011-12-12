local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")

local sketch = require("opengl.sketch")

local ctx = "Texture RTT Test"
win = Window(ctx, 0, 0, 512, 512)

local tex = Texture(ctx)

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)

	tex:startcapture()
		gl.LineWidth(3)
		gl.Begin(GL.LINES)
		for i=-1, 1, 0.1 do
			gl.Vertex(-2, now()%2-1, 0)
			gl.Vertex(1, i, 0)
		end
		gl.End()
	tex:endcapture()

	gl.Color(1, 1, 1, 1)
	for i=-2, 1 do
	for j=-2, 1 do
		tex:bind()	
		gl.Begin(GL.QUADS)
			sketch.quad(i, j, 1, 1)
		gl.End()
		tex:unbind()
	end
	end
end