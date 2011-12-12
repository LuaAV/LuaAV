local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local video = require("video")
local Video = video.Video
local Rec = video.VideoRecorder

local random = math.random

local ctx = "Video Player Test"
win = Window(ctx, 0, 0, 720, 480)

local tex = Texture(ctx)
local vid = Video()
vid:open(LuaAV.findfile("slowbird.m4v"))
vid:play()

local rec = Rec()
rec:open(script.path.."/test.mov", vid.dim)

print("speed:", vid.speed)
print("dur:", vid.dur)
print("nframes:", vid.nframes)
print("dim:", unpack(vid.dim))

vid.speed = 1

go(function()
	while(true) do
		tex:fromarray(vid:array())
	--	print("frame:", vid.frame)
	--	print("pos:", vid.pos)
		wait(1/33)
	end
end)

go(1, function()
	while(true) do
		vid.pos = random()
		wait(2)
	end
end)

go(function()
	while(now() < 5) do
		rec:fromarray(vid:array())
		wait(1/30)
	end
	print("DONE")
	rec:close()
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