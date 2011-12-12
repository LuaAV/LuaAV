local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local Image = require("Image")

local ctx = "Image Load Test"
win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = {1, 0., 0.5, 1}

--local image = Image(LuaAV.findfile("glow.jpg"))
local image = Image(LuaAV.findfile("LuaAV.96.png"))
--local image = Image(LuaAV.findfile("hubble.jpg"))
local tex = Texture(ctx)
tex:fromarray(image:array())

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.Color(1, 1, 1, 1)
	gl.Scale(0.5, 0.5, 0.5)
	tex:bind()
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
end