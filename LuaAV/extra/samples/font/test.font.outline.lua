local gl = require("opengl")
local GL = gl
local Mesh = require("opengl.Mesh")

local font = require("font")
local Font = require("font.Font")

local Array = require("Array")

local ctx = "Test Font Outline"
win = Window(ctx, 0, 0, 256, 256)


local default_fontfile = LuaAV.findfile("VeraMono.ttf")
local f = Font(default_fontfile, 10)
print(f)

local vertex = Array()
local index = Array()


local mesh = Mesh(ctx)
mesh.primitive = GL.TRIANGLES

f:outline(195, vertex, index)
mesh:vertex(vertex)
mesh:index(index)

--[[
print(index.dim[1])
for i=0, index.dim[1]-1 do
	print("index: "..i, index:getcell(i, 0)[1])
end
--]]

---[[
go(function()
	for i=195, 240 do
		f:outline(i, vertex, index)
		mesh:vertex(vertex)
		mesh:index(index)
		wait(0.5)
	end
end)
--]]

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

local verts = {
	{192.000000, 256.000000},
	{64.000000, 256.000000},
	{64.000000, 320.000000},
	{256.000000, 320.000000},
	{256.000000, 0.000000},
	{253.875000, -28.750000},
	{247.500000, -54.000000},
	{236.875000, -75.750000},
	{222.000000, -94.000000},
	{203.687500, -108.875000},
	{181.750000, -119.500000},
	{156.187500, -125.875000},
	{127.000000, -128.000000},
	{64.000000, -128.000000},
	{64.000000, -64.000000},
	{119.000000, -64.000000},
	{135.875000, -63.000000},
	{150.500000, -60.000000},
	{162.875000, -55.000000},
	{173.000000, -48.000000},
	{181.312500, -39.000000},
	{187.250000, -28.000000},
	{190.812500, -15.000000},
	{192.000000, 0.000000},
	
	{256.000000, 512.000000},
	{256.000000, 448.000000},
	{192.000000, 448.000000},
	{192.000000, 512.000000},
}

local
function do_vertex(idx)
	local s = 1
	gl.Vertex(verts[idx+1][1]*s, verts[idx+1][2]*s)
end

local shapes = {
	{
		prim = GL.TRIANGLE_FAN,
		{256.000000, 0.000000},
		{253.875000, -28.750000},
		{187.250000, -28.000000},
		{190.812500, -15.000000},
		{192.000000, 0.000000},
		{192.000000, 256.000000},
		{64.000000, 256.000000},
	},
	{
		prim = GL.TRIANGLE_FAN,
		{253.875000, -28.750000},
		{0.000000, 0.000000},
		{173.000000, -48.000000},
		{181.312500, -39.000000},
		{187.250000, -28.000000},
	},
	{
		prim = GL.TRIANGLE_STRIP,
		{135.875000, -63.000000},
		{0.000000, 0.000000},
		{150.500000, -60.000000},
		{247.500000, -54.000000},
		{162.875000, -55.000000},
		{0.000000, 0.000000},
	},
	{
		prim = GL.TRIANGLES,
		{256.000000, 512.000000},
		{0.000000, 0.000000},
		{192.000000, 512.000000},
		{236.875000, -75.750000},
		{222.000000, -94.000000},
		{203.687500, -108.875000},
		{64.000000, 320.000000},
		{64.000000, 256.000000},
		{256.000000, 320.000000},
	}
}

local shapes = {
	{
		prim = GL.TRIANGLE_FAN,
		{0.000000, 0.000000},
		{1.000000, 0.000000},
		{1.000000, 1.000000},
		{0.000000, 1.000000},
	}
}

for i=0, vertex.dim[1]-1 do
--	print(unpack(vertex:getcell(i, 0)))
	if(i==7) then
		break
	end
end


mesh.primitive = GL.TRIANGLES
function win:draw()
	local s = 1 --0.002
	gl.Scale(s, s, s)
	--gl.PolygonMode(GL.FRONT_AND_BACK, GL.LINE)
	
		mesh:draw()
--	gl.PolygonMode(GL.FRONT_AND_BACK, GL.FILL)
	
	gl.PushMatrix()
	gl.Translate(-300, 0, 0)
		gl.Begin(GL.TRIANGLES)
		for i=0, vertex.dim[1]-1 do
			if(i>=31 and i<34) then
				gl.Vertex(vertex:getcell(i, 0))
			end
		end
		gl.End()
	gl.End()
	
	--[[
	for i, s in ipairs(shapes) do
		gl.Begin(s.prim)
		for i, v in ipairs(s) do
			gl.Vertex(v)
		end
		gl.End()
	end
	--]]
	
	local begin = 0
	local count = #verts
	--[[
	gl.Scale(2, 2, 2)
	gl.PushMatrix()
	gl.Translate(-300, 0, 0)
	gl.Begin(GL.TRIANGLE_FAN)
	for i=begin+1, count-1 do
		do_vertex(begin)
		do_vertex(i)
		if(i+1 >= count) then
			do_vertex(begin+1)
		else
			do_vertex(i+1)
		end
	end
	gl.End()
	gl.PopMatrix()
	
	gl.PushMatrix()
	gl.Begin(GL.TRIANGLE_FAN)
	for i, v in ipairs(verts) do
		gl.Vertex(v)
	end
	gl.End()
	gl.PopMatrix()
	--]]
end