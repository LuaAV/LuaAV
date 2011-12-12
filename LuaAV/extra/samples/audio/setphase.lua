local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Mesh = require("opengl.Mesh")
local Texture = require("opengl.Texture")
local Slab = require("opengl.Slab")
local Array = require("Array")

local floor = math.floor

local Def = require "audio.Def"
Def.globalize()

audio.scope(true)

-- phase modulation:

beep = Def{
	--Env{ 20.5 } * SinOsc{ phase={Phasor{200}, Phasor{100}} } * 0.4
	Env{ .5 } * SinOsc{freq=SinOsc{500}*800+200 } * 0.4
}

beep2 = Def{
	Env{ 200.5 } * SinOsc{freq = 1000}
}

go(function()
	for i=1, 10 do
		beep{ }
		wait(0.4)
	end
end)

local ch0 = audio.scope(0)
local x = Array(ch0.header)
for i=0, x.dim[1]-1 do
	x:setcell(i, {i/(x.dim[1]-1)})
end

local ctx = "Scope"
win = Window(ctx, 0, -800, 1920, 1000)
win.sync = true
win.fps = 40


local mesh = Mesh(ctx)
--mesh.primitive = GL.POINTS
mesh.primitive = GL.LINE_STRIP

local tex = Texture(ctx)
tex.single_channel = GL.LUMINANCE
tex.minfilter = GL.NEAREST
tex.magfilter = GL.NEAREST
local av = Slab{
	ctx = ctx,
	file = LuaAV.findfile("audio.scope.vertices.shl"),
	texture = {
		type = GL.FLOAT,
		format = GL.RGB,
		dim = ch0.dim,
		minfilter = GL.NEAREST,
		magfilter = GL.NEAREST,
	}
}
av:param("texdim", ch0.dim)

local W, H = ch0.dim[1], ch0.dim[2]
local nsamples = ch0.dim[1]*ch0.dim[2]
print(ch0.dim[1]*ch0.dim[2]/44100)
print(nsamples)

local buffer1 = Array{
	components = 1,
	type = Array.Float32,
	dim = {5, 5},
}
local w = buffer1.dim[1]
local N = w*buffer1.dim[2]
for j=0, buffer1.dim[2]-1 do
for i=0, buffer1.dim[1]-1 do
	local v = (i+j*w)/N
	buffer1:setcell(i, j, {v})
end
end
--av:param("texdim", buffer1.dim)

--[[
local ch0 = Array{
	components = 1,
	type = Array.Float32,
	dim = {W, H},
}
local w = ch0.dim[1]
local N = w*ch0.dim[2]
for j=0, ch0.dim[2]-1 do
for i=0, ch0.dim[1]-1 do
	local v = (i+j*w)/N
	v = (v*4)%1
	ch0:setcell(i, j, {v})
end
end
--]]

local buffer2 = Array()

local recordings = {}

go(function()
	wait("draw")
	recordings[#recordings+1] = {
		pos = audio.scope(),
		buffer = buffer1,
	}
	buffer1:copy(ch0)
	
	wait("draw")
	recordings[#recordings+1] = {
		pos = audio.scope(),
		buffer = buffer2,
	}
	buffer2:copy(ch0)
	
	print("DONE")
end)

local
function draw_buffer(buffer, pos)
	local w = buffer.dim[1]
	local h = buffer.dim[2]
	local nsamples = w*h
	
	gl.Color(1, 1, 1, 0.7)
	gl.Begin(GL.POINTS)
	for j=0, h-1 do
	for i=0, w-1 do
		local spos = (i+j*w)
		local cpos = (spos+pos)%nsamples
		local ci = cpos%w
		local cj = floor(cpos/w)
		local x = spos/nsamples*4-2
		gl.Vertex(x, buffer:getcell(ci, cj)[1])
	end
	end
	gl.End()
	
	gl.Color(1, 0, 0.5, 1)
	gl.Begin(GL.LINES)
		gl.Vertex(pos/nsamples*4-2, 1)
		gl.Vertex(pos/nsamples*4-2, -1)
	gl.End()
end

go("check", function()
	local arr = av:texture():array()
	
	for j=0, arr.dim[2]-1 do
	for i=0, arr.dim[1]-1 do
		local v = arr:getcell(i, j)
		print(string.format("(%d,%d): %f %f", i, j, v[1], v[2]))
	end
	end
end)

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	gl.Color(1, 1, 1, 0.7)
	gl.Scale(1, 1, 1)
	gl.PointSize(1)
	gl.Disable(GL.DEPTH_TEST)
	--[[
	gl.Begin(GL.POINTS)
	local lim = ch0.dim[1]
	--for i=0, lim, 1 do
	local spos = audio.scope()
	local jlim = spos+lim-1
	for j=spos, jlim, 1 do
		local i = j%lim
		local idx = j-spos
		gl.Vertex(4*idx/lim-2, ch0:getcell(i)[1])
	end
	gl.End()
	--]]
	
	---[[
	if(recordings[1]) then
		--draw_buffer(recordings[1].buffer, recordings[1].pos)
		--draw_buffer(recordings[1].buffer, 3)
		
		--recordings[1].pos = audio.scope()
		--recordings[1].buffer:copy(ch0)
	end
	--]]
	--[[
	gl.PushMatrix()
	if(recordings[2]) then
		gl.Translate(0, 0.5, 0)
		draw_buffer(recordings[2].buffer, recordings[2].pos)
	end
	gl.PopMatrix()
	--]]
	
	
	--print(audio.scope())
	tex:fromarray(ch0)
	--tex:fromarray(buffer1)
	--av:param("sample", recordings[1] and recordings[1].pos or 0) --audio.scope())
	av:param("sample", audio.scope())
	--av:param("sample", 4000)
	av:param("scale", 2)
	av:draw(tex)
	event("draw")
	
	--[[
	gl.Color(1, 1, 1, 1)
	tex:bind()
	gl.Begin(GL.QUADS)
		sketch.quad(-1, 0, 1, 1)
	gl.End()
	tex:unbind()
	--]]
	---[[
	local tex = av:texture()
	gl.Color(1, 1, 1, 1)
	tex:bind()
	gl.Begin(GL.QUADS)
		sketch.quad(0, 0, 1, 1)
	gl.End()
	tex:unbind()
	--]]
	
	---[=[
	local tex = av:texture()
	tex:toarray()
--	event"check"
	mesh:vertex(tex:array())
	
	gl.Scale(-2, 1, 1)
	gl.Color(1, 1, 1, 0.75)
	mesh:draw()
	--]=]
end--