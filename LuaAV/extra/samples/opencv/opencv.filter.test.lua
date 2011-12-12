local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")
local Slab = require("opengl.Slab")

local video = require("video")
local VidCam = video.VideoCamera

local Array = require("Array")

local opencv = require("opencv")


local ctx = "OpenCV Test"
win = Window(ctx, 0, 0, 512, 512)

local cam = VidCam()
cam:open(320, 240)

local tex1 = Texture(ctx)
local tex2 = Texture(ctx)

local fx = Slab{
	ctx = ctx,
	file = LuaAV.findfile("cf.bilateral.shl")
}
--fx:texture().minfilter = GL.NEAREST
--fx:texture().magfilter = GL.NEAREST

local bilat = Array(3, Array.UInt8, {320, 240})
local gray = Array(1, Array.UInt8, {320, 240})
local dist = Array(1, Array.Float32, {320, 240})
local small = Array(3, Array.UInt8, {320/4, 240/4})
local big = Array(3, Array.UInt8, {320*4, 240*4})

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

local blur = {5, 5}

go(function()
	for i=10, 1, -1 do
		blur = {i, i}
		wait(1)
	end
end)

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	--opencv.bilateral(cam:array(), bilat, -8, 100, 2)
	--opencv.blur(cam:array(), bilat, blur)
	--opencv.box_filter(cam:array(), bilat, {7, 7})
	--opencv.dilate(cam:array(), bilat)
	--opencv.erode(cam:array(), bilat)
	--opencv.median_blur(cam:array(), bilat, 7)
	--opencv.morphology_ex(cam:array(), bilat, opencv.MORPH_OPEN, nil, 3)
	--opencv.morphology_ex(cam:array(), bilat, opencv.MORPH_CLOSE, nil, 3)
	--opencv.morphology_ex(cam:array(), bilat, opencv.MORPH_GRADIENT, nil, 3)
	--opencv.morphology_ex(cam:array(), bilat, opencv.MORPH_TOPHAT, nil, 3)
	--opencv.morphology_ex(cam:array(), bilat, opencv.MORPH_BLACKHAT, nil, 3)
	--opencv.laplacian(cam:array(), bilat, 3, 2)
	--opencv.sobel(cam:array(), bilat, 2, 2)
	--opencv.scharr(cam:array(), bilat, 0, 1)
	--opencv.resize(cam:array(), small, opencv.INTER_NEAREST)
	--opencv.resize(cam:array(), small, opencv.INTER_LINEAR)
	--opencv.resize(cam:array(), small, opencv.INTER_AREA)
	--opencv.resize(cam:array(), small, opencv.INTER_CUBIC)
	--opencv.resize(cam:array(), small, opencv.INTER_LANCZOS4)
	--opencv.resize(cam:array(), big, opencv.INTER_NEAREST)
	--opencv.resize(cam:array(), big, opencv.INTER_LINEAR)
	--opencv.resize(cam:array(), big, opencv.INTER_AREA)
	--opencv.resize(cam:array(), big, opencv.INTER_CUBIC)
	--opencv.resize(cam:array(), big, opencv.INTER_LANCZOS4)
	--opencv.adaptive_threshold(cam:array(), bilat, 255, opencv.ADAPTIVE_THRESH_MEAN_C, opencv.THRESH_BINARY, 3, 128)
	opencv.cvt_color(cam:array(), gray, opencv.CV_RGB2GRAY)
	opencv.threshold(gray, gray, 80, 255, opencv.THRESH_BINARY)
	opencv.distance_transform(gray, dist, opencv.CV_DIST_L2, opencv.CV_DIST_MASK_PRECISE)
--	opencv.adaptive_threshold(gray, gray, 128, opencv.ADAPTIVE_THRESH_MEAN_C, opencv.THRESH_BINARY, 3, 1)
	tex1:fromarray(cam:array())
	tex2:fromarray(dist)
	--tex2:fromarray(gray)
	--tex2:fromarray(bilat)
	--tex2:fromarray(small)
	--tex2:fromarray(big)

	--[[
	fx:draw(tex1)
	
	local tex3 = fx:texture()
	
	gl.Color(1, 1, 1, 1)
	gl.PushMatrix()
		gl.Translate(-1, 0, 0)
		tex2:bind()
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex2:unbind()
	gl.PopMatrix()
	gl.PushMatrix()
		gl.Translate(1, 0, 0)
		tex3:bind()
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex3:unbind()
	gl.PopMatrix()
	--]]
	
	
	gl.PushMatrix()
		gl.Translate(-1, 0, 0)
		tex1:bind()
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex1:unbind()
	gl.PopMatrix()
	
	gl.PushMatrix()
		gl.Translate(1, 0, 0)
		tex2:bind()
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex2:unbind()
	gl.PopMatrix()
end