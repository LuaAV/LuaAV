local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local video = require("video")
local VidCam = video.VideoCamera

local ctx = "Video Starter"
win = Window(ctx, 0, 0, 512, 512)

local cam = VidCam()
cam:open(640, 480)

local vidtex = Texture(ctx)

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:draw()
	vidtex:fromarray(cam:array())
	local dim = vidtex.dim
	local aspect = dim[1]/dim[2]
	
	gl.Color(1, 1, 1, 1)
	vidtex:bind()
	gl.Scale(aspect, 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	vidtex:unbind()
end