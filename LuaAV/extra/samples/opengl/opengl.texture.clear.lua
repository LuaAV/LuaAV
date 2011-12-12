local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local Image = require("Image")


local ctx = "OpenGL Texture Clear"
win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = {0.2, 0.2, 0.2, 1}

local img = Image(LuaAV.findfile("LuaAV.96.png"))
local tex = Texture(ctx)
tex:fromarray(img:array())


function win:draw()
	--[[
	tex:startcapture()
		gl.Color(1, 0, 0, 1)
		gl.Begin(GL.LINES)
			gl.Vertex(-1, 0, 0)
			gl.Vertex(1, 0, 0)
		gl.End()
	tex:endcapture()
	--]]
	
	tex:clear()
	
	tex:bind()
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
end