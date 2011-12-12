local gl = require("opengl")
local GL = gl
local Mesh = require("opengl.Mesh")

local Array = require("Array")

local random = math.random
local cos = math.cos
local sin = math.sin

local ctx = "Test Mesh"
win = Window(ctx, 0, 0, 512, 512)

local vertex = Array{
	components = 3, 
	type = Array.Float32, 
	dim = {10, 10},
}

local index = Array{
	components = 1, 
	type = Array.UInt32, 
	dim = {8},
}

local mesh = Mesh{
	ctx = ctx,
	primitive = GL.TRIANGLES,
}
for i=0, vertex.dim[1]-1 do
for j=0, vertex.dim[2]-1 do
	vertex:setcell(i, j, {i/10, j/10, 0})
end
end
mesh:vertex(vertex)

index:setcell(0, 0)
index:setcell(1, 1)
index:setcell(2, vertex.dim[1])
index:setcell(3, vertex.dim[1]+1)
mesh:index(index)
for i=0, index.dim[1]-1 do
	print(i, index:getcell(i)[1])
end


function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	gl.Disable(GL.DEPTH_TEST)
	gl.Color(1, 1, 1, 0.65)
	mesh:draw()
end