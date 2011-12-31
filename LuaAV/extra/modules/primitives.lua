local assert = assert
local ipairs = ipairs
local pairs = pairs
local print = print

local table = table
local pi = math.pi
local cos = math.cos
local sin = math.sin
local floor = math.floor

local gl = require("opengl")
local GL = gl
local Mesh = require("opengl.Mesh")
local Array = require("Array")
local vec3 = require("space.vec3")

module(...)


function box(ctx)
	assert(ctx)

	local pts = {
		{-0.5, -0.5, -0.5},
		{-0.5, -0.5, 0.5},
		{-0.5, 0.5, -0.5},
		{-0.5, 0.5, 0.5},
		
		{0.5, -0.5, -0.5},
		{0.5, -0.5, 0.5},
		{0.5, 0.5, -0.5},
		{0.5, 0.5, 0.5},
	}
	local faces = {
		{2, 6, 8, 4},
		{6, 5, 7, 8},
		{5, 1, 3, 7},
		{1, 2, 4, 3},
		{4, 8, 7, 3},
		{1, 5, 6, 2},
	}
	
	local vertices = Array{
		components = 3, 
		type = Array.Float32, 
		dim = #faces*4
	}
	local normals = Array{
		components = 3, 
		type = Array.Float32, 
		dim = #faces*4
	}
	
	local idx = 0
	for _, f in ipairs(faces) do
		local n = vec3.normalize(vec3.normal(pts[ f[1] ], pts[ f[2] ], pts[ f[3] ]))
		for _, vidx in ipairs(f) do
			local v = pts[vidx]
			vertices:setcell(idx, v)
			normals:setcell(idx, n)
			idx = idx+1
		end
	end
	
	local mesh = Mesh{
		ctx = ctx,
		primitive = GL.QUADS,
	}
	mesh:vertex(vertices)
	mesh:normal(normals)

	return mesh
end

function grid(ctx)
	assert(ctx)

	local pts = {
		{-0.5, -0.5, -0.5},
		{-0.5, -0.5, 0.5},
		{-0.5, 0.5, -0.5},
		{-0.5, 0.5, 0.5},
		
		{0.5, -0.5, -0.5},
		{0.5, -0.5, 0.5},
		{0.5, 0.5, -0.5},
		{0.5, 0.5, 0.5},
	}
	local lines = {
		{1, 2}, {2, 4}, {4, 3}, {3, 1},
		{5, 6}, {6, 8}, {8, 7}, {7, 5},
		{1, 5}, {2, 6}, {3, 7}, {4, 8},
	}
	
	local vertices = Array{
		components = 3, 
		type = Array.Float32, 
		dim = #lines*2
	}
	for i, line in ipairs(lines) do
		vertices:setcell((i-1)*2, pts[ line[1] ])
		vertices:setcell((i-1)*2+1, pts[ line[2] ])
	end
	
	local mesh = Mesh{
		ctx = ctx,
		primitive = GL.LINES,
	}
	mesh:vertex(vertices)
	return mesh
end

function sphere(ctx, res)
	res = res or 20
	local hres = floor(res/2)
	local vertices = Array{
		components = 3, 
		type = Array.Float32, 
		dim = {res*(hres-1)+2}
	}
	
	local dtheta = 2*pi/(res)
	local dphi = pi/(hres)
	for j=1, hres-1 do
	for i=1, res do
		local idx = i-1+res*(j-1)
		vertices:setcell(idx, {
			cos(dtheta*i)*sin(dphi*j),
			sin(dtheta*i)*sin(dphi*j),
			cos(dphi*j),
		})
	end
	end
	vertices:setcell(vertices.dim[1]-2, {0, 0, 1})
	vertices:setcell(vertices.dim[1]-1, {0, 0, -1})
	
	local indices = Array{
		components = 1,
		type = Array.UInt32,
		dim = {4*res*hres}
	}
	indices:clear()
	indices:setcell(0, 0)
	indices:setcell(1, vertices.dim[1]-2)
	
	local idx = 0
	for j=1, hres-2 do
		for i=1, res do
			local idx1 = i-1+res*(j-1)
			local idx2 = idx1+1
			local idx3 = idx2+res
			local idx4 = idx1+res
			
			if(i==res) then
				idx2 = res*(j-1)
				idx3 = res*j
			end
		
			indices:setcell(idx, {idx1})
			indices:setcell(idx+1, {idx2})
			indices:setcell(idx+2, {idx3})
			indices:setcell(idx+3, {idx4})
			idx = idx+4
		end
	end

	local pole1 = vertices.dim[1]-2
	for i=1, res do
		local idx1 = i-1
		local idx2 = i
		
		if(i==res) then
			idx2 = 0
		end

		indices:setcell(idx, {pole1})
		indices:setcell(idx+1, {pole1})
		indices:setcell(idx+2, {idx1})
		indices:setcell(idx+3, {idx2})
		idx = idx+4
	end
	
	local pole2 = vertices.dim[1]-1
	for i=1, res do
		local idx1 = i-1 + res*(hres-2)
		local idx2 = i + res*(hres-2)
		
		if(i==res) then
			idx2 = res*(hres-2)
		end

		indices:setcell(idx, {idx1})
		indices:setcell(idx+1, {idx2})
		indices:setcell(idx+2, {pole2})
		indices:setcell(idx+3, {pole2})
		idx = idx+4
	end
	
	local mesh = Mesh{
		ctx = ctx,
		primitive = GL.QUADS,
	}
	mesh:vertex(vertices)
	mesh:normal(vertices)
	mesh:index(indices)

	return mesh
end