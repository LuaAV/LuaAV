local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local print = print
local unpack = unpack

require("math")

local abs = math.abs
local acos = math.acos
local asin = math.asin
local sqrt = math.sqrt
local atan2 = math.atan2
local pi = math.pi

local gl = require("opengl")
local GL = gl

local Mesh = require("opengl.Mesh")

local Array = require("Array")


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

module('glutils.cubegrid', setconstructor)
----------------------------------------------------------
local
function buildgrid(ncells, gsz)	
	local vgrid = Array{
		components = 3, 
		type = Array.Float32,
		dim = {2, 12*(ncells+1)}
	}
	local idx = 0
	for i=0, ncells do
		local ii = i/(ncells)*2*gsz-gsz
		vgrid:setcell(0, idx+0, {ii, gsz, gsz})
		vgrid:setcell(1, idx+0, {ii, -gsz, gsz})
		vgrid:setcell(0, idx+1, {ii, gsz, -gsz})
		vgrid:setcell(1, idx+1, {ii, -gsz, -gsz})
		
		vgrid:setcell(0, idx+2, {ii, gsz, gsz})
		vgrid:setcell(1, idx+2, {ii, gsz, -gsz})
		vgrid:setcell(0, idx+3, {ii, -gsz, gsz})
		vgrid:setcell(1, idx+3, {ii, -gsz, -gsz})
		
		vgrid:setcell(0, idx+4, {gsz, ii, gsz})
		vgrid:setcell(1, idx+4, {-gsz, ii, gsz})
		vgrid:setcell(0, idx+5, {gsz, ii, -gsz})
		vgrid:setcell(1, idx+5, {-gsz, ii, -gsz})
		
		vgrid:setcell(0, idx+6, {gsz, ii, gsz})
		vgrid:setcell(1, idx+6, {gsz, ii, -gsz})
		vgrid:setcell(0, idx+7, {-gsz, ii, gsz})
		vgrid:setcell(1, idx+7, {-gsz, ii, -gsz})
		
		vgrid:setcell(0, idx+8, {gsz, gsz, ii})
		vgrid:setcell(1, idx+8, {-gsz, gsz, ii})
		vgrid:setcell(0, idx+9, {gsz, -gsz, ii})
		vgrid:setcell(1, idx+9, {-gsz, -gsz, ii})
		
		vgrid:setcell(0, idx+10, {gsz, gsz, ii})
		vgrid:setcell(1, idx+10, {gsz, -gsz, ii})
		vgrid:setcell(0, idx+11, {-gsz, gsz, ii})
		vgrid:setcell(1, idx+11, {-gsz, -gsz, ii})
		
		idx = idx + 6*2
	end
	
	return vgrid
end

function C:__call(init)
	init = ctxtable(init or {})
	
	init.primitive = GL.LINES
	init.cells = init.cells or 5
	init.size = init.size or 20

	local m = Mesh(init)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m:vertex( buildgrid(init.cells, init.size*0.5) )
	return m
end