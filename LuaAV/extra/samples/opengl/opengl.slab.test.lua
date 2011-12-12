local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local Slab = require("opengl.Slab")

local sketch = require("opengl.sketch")

local video = require("video")
local VideoCam = video.VideoCamera

local ctx = "Video Camera Test"
win = Window(ctx, 0, 0, 720, 480)

print(LuaAV.findfile("cf.gaussian.2p.shl"))
local blur = Slab{
	ctx = ctx,
	file = LuaAV.findfile("cf.gaussian.2p.shl"),
	param = {
		width = {0.01, 0}
	}
}
local tex = Texture(ctx)
local cam = VideoCam()
cam:open()

go(function()
	while(true) do
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

go(function()
	while(true) do
		wait("blur")
		
		local cond = true
		local bb = 0
		go(function()
			while(cond) do
				bb = bb+0.0015
				blur:param("width", {bb, 0})
				wait(1/33)
			end
		end)
		wait("sharpen")
		cond = false
		
		go(function()
			local cc = true
			go("blur", function()
				cc = false
			end)
		
			while(bb > 0.001 and cc) do
				bb = bb*0.97
				blur:param("width", {bb, 0})
				wait(1/33)
			end
		end)
	end
end)

function win:mouse(e, btn, x, y)
--	local v = x/self.dim[1]*2-1
	if(e == "down") then
		event("blur")
	elseif(e == "up") then
		event("sharpen")
	end
end

function win:draw()
	gl.Color(1, 1, 1, 1)
	
	tex:fromarray(cam:array())
	
	blur:draw(tex)
	
	local t = blur:texture()
	t:bind()	
	gl.Scale(tex.dim[1]/tex.dim[2], 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	t:unbind()
end