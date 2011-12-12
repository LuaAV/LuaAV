local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local Slab = require("opengl.Slab")
local sketch = require("opengl.sketch")

local video = require("video")
local VideoCamera = video.VideoCamera

local ctx = "OpenGL Texture Clear"
win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = {0.2, 0.2, 0.2, 1}

local cam = VideoCamera()
cam:open(720, 480)
local tex = Texture(ctx)
local tex_fb = Texture(ctx)

local accum = Slab{
	ctx = ctx,
	file = LuaAV.findfile("co.accum.shl")
}

accum:param("erase_color", {0, 0, 0, 0.25})

function win:draw()
	tex:fromarray(cam:array())
	accum:draw(tex, tex_fb)
	tex_fb:copy(accum:texture())
	
	local t = accum:texture()
	t:bind(0)
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	t:unbind()
end