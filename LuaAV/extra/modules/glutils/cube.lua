local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local print = print
local ipairs = ipairs

require 'opengl'
require 'muro'
require 'math'

local abs = math.abs
local acos = math.acos
local asin = math.asin
local sqrt = math.sqrt
local atan2 = math.atan2
local pi = math.pi

local gl = gl
local GL = gl

local Matrix = muro.Matrix
local Mesh = muro.Mesh
local vec3 = muro.Vec3


local
function ctxtable(v)
	if(type(v) == "string")
		then return {ctx = v}
		else return v
	end
end


local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('glutils.cube', setconstructor)
----------------------------------------------------------
local verts = {
	{-1, -1, -1},
	{1, -1, -1},
	{1, 1, -1},
	{-1, 1, -1},
	
	{-1, -1, 1},
	{1, -1, 1},
	{1, 1, 1},
	{-1, 1, 1},
}

local norms = {
	{0, 0, -1},
	{1, 0, 0},
	{0, 0, 1},
	{-1, 0, 0},
	{0, -1, 0},
	{0, 1, 0},
}

local faces = {
	{4, 3, 2, 1},
	{2, 3, 7, 6},
	{6, 7, 8, 5},
	{5, 8, 4, 1},
	{6, 5, 1, 2},
	{8, 7, 3, 4},
}

local
function buildcube(dim)
	local nverts = #faces*#faces[1]
	local vmat = Matrix(3, "float32", nverts, 1)
	local nmat = Matrix(3, "float32", nverts, 1)
	
	local idx = 0
	for i, face in ipairs(faces) do
		for _, ii in ipairs(face) do
			local v = verts[ii]
			local n = norms[i]
			vmat:setcell(idx, 0, v)
			nmat:setcell(idx, 0, n)
			idx = idx + 1
		end
	end
	
	return vmat, nmat
end

function C:__call(init)
	init = ctxtable(init or {})
	
	init.dim = init.dim or {1, 1, 1}
	init.drawmode = GL.QUADS
	init.mode = Mesh.VBO_MODE

	local m = Mesh(init)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	local vv, nn = buildcube(init.dim)
	m:vertex(vv)
	m:normal(nn)
	return m
end