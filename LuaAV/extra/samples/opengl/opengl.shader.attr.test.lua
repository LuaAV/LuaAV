local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Texture = require("opengl.Texture")
local Shader = require("opengl.Shader")

local Image = require("Image")

local ctx = "Shader Attr Test"
win = Window(ctx, 0, 0, 512, 512)

local img = Image(LuaAV.findfile("glow.jpg"))
local tex = Texture(ctx)
tex:fromarray(img:array())

local shader = Shader{
	ctx = ctx,
	file = LuaAV.findfile("vd.billboard.shl")
}

local offset = shader:attr("offset")

function win:draw()
	shader:bind()
	gl.Rotate(now()*60, 1, 1, 0)
		gl.Color(1, 1, 1, 1)
		tex:bind()
		gl.Begin(GL.QUADS)
			offset:submit(-1, 1)
			gl.TexCoord(0, 0)
			gl.Vertex(0, 0, 0)
			
			offset:submit(1, 1)
			gl.TexCoord(1, 0)
			gl.Vertex(0, 0, 0)
			
			offset:submit(1, -1)
			gl.TexCoord(1, 1)
			gl.Vertex(0, 0, 0)
			
			offset:submit(-1, -1)
			gl.TexCoord(0, 1)
			gl.Vertex(0, 0, 0)
		gl.End()
		tex:unbind()
	shader:unbind()
end