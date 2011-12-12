local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local video = require("video")
local VideoCam = video.VideoCamera

local ctx = "Video Camera Test"
win = Window(ctx, 0, 0, 720, 480)

local tex = Texture(ctx)
local cam = VideoCam()
cam:open(720, 480)

go(function()
	while(true) do
		tex:fromarray(cam:array())
		wait(1/33)
	end
end)

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:draw()
	gl.Color(1, 1, 1, 1)
	tex:bind()
	gl.Scale(tex.dim[1]/tex.dim[2], 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
end