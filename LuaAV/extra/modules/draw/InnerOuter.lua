local gl = require("opengl")
local GL = gl

local setmetatable = setmetatable
local getfenv = getfenv
local assert = assert
local unpack = unpack
local ipairs = ipairs
local pairs = pairs
local type = type
local print = print

local table = table

module(...)


local C = {}
local M = setmetatable(getfenv(), C)


function C:__call(init)
	init = init or {}
	local m = init
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = function(...)
				v(m, ...)
			end
		end
	end
	
	m.function_map = {
		Begin = m.Begin,
		Color = m.Color,
		Normal = m.Normal,
		Vertex = m.Vertex,
		End = m.End,
	}
	m.cached_functions = {}
	m.vertices = {}
	
	return m
end

local
function inverse_transpose(m)
	local det = m[1]*(m[6]*m[11]-m[7]*m[10])
				-m[5]*(m[2]*m[11]-m[10]*m[3])
				+m[9]*(m[2]*m[7]-m[6]*m[3])
	local invdet = 1/det
	local result = {}
	result[1] =  (m[6]*m[11]-m[7]*m[10])*invdet
	result[2] = -(m[5]*m[11]-m[9]*m[7])*invdet
	result[3] =  (m[5]*m[10]-m[9]*m[6])*invdet
	result[4] = -(m[2]*m[11]-m[10]*m[3])*invdet
	result[5] =  (m[1]*m[11]-m[9]*m[3])*invdet
	result[6] = -(m[1]*m[10]-m[2]*m[9])*invdet
	result[7] =  (m[2]*m[7]-m[3]*m[6])*invdet
	result[8] = -(m[1]*m[7]-m[3]*m[5])*invdet
	result[9] =  (m[1]*m[6]-m[2]*m[5])*invdet
	
	return result
end

local
function eye_norm(mv, n)
	return {
		mv[1]*n[1] + mv[4]*n[2] + mv[7]*n[3],
		mv[2]*n[1] + mv[5]*n[2] + mv[8]*n[3],
		mv[3]*n[1] + mv[6]*n[2] + mv[9]*n[3],
	}
end

local
function table_from_args(n, d, ...)
	local t = {...}
	if(type(t[1]) == "table") then
		t = {unpack(t[1])}
	end
	
	for i=#t+1, n do
		t[i] = d
	end
	
	return t
end

function M:Begin(prim)
	self.primitive = prim
	self.vertices = {}
end

function M:Color(...)
	self.color = table_from_args(4, 1, ...)
end

function M:Normal(...)
	self.normal = table_from_args(3, 0, ...)
end

function M:Vertex(...)
	self.vertex = table_from_args(3, 0, ...)
	self.vertex.color = {unpack(self.color)}
	self.vertex.normal = {unpack(self.normal)}
	self.vertices[#self.vertices+1] = self.vertex
end

function M:End()
	local itmv = inverse_transpose({gl.Get(GL.MODELVIEW_MATRIX)})

	-- sort buffer
	local vertices = {}
	if(self.primitive == GL.TRIANGLES) then
		for i, v in ipairs(self.vertices) do
			local normal = eye_norm(itmv, v.normal)
			if(normal[3] < 0) then
				v.color = {1, 0, 0.5, 1}
				table.insert(vertices, 1, v)
			else
				v.color = {0, 0.5, 1, 1}
				vertices[#vertices+1] = v
			end
		end
	elseif(self.primitive == GL.LINES) then
		for i, v in ipairs(self.vertices) do
			local normal = eye_norm(itmv, v.normal)
			if(normal[3] >= 0) then
				v.color = {0, 0, 0, 1}
				vertices[#vertices+1] = v
			end
		end
	elseif(self.primitive == GL.LINE_LOOP) then
		self.primitive = GL.LINES
	
		local normals = {}
		for i, v in ipairs(self.vertices) do
			normals[i] = eye_norm(itmv, v.normal)
		end
		
		for i=1, #normals-1 do
			local n1 = normals[i]
			local n2 = normals[i+1]
			
			if(n1[3] >= 0 and n2[3] >= 0) then
				local v1 = self.vertices[i]
				local v2 = self.vertices[i+1]
				v1.color = {0, 0, 0, 1}
				v2.color = {0, 0, 0, 1}
				vertices[#vertices+1] = v1
				vertices[#vertices+1] = v2
			end
		end
		
		local n1 = normals[#normals]
		local n2 = normals[1]
		
		if(n1[3] >= 0 and n2[3] >= 0) then
			local v1 = self.vertices[#self.vertices]
			local v2 = self.vertices[1]
			v1.color = {1, 1, 1, 1}
			v2.color = {1, 1, 1, 1}
			vertices[#vertices+1] = v1
			vertices[#vertices+1] = v2
		end
	end
	
	-- draw buffer
	local cached_functions = self.cached_functions
	cached_functions.Begin(self.primitive)
	for i, v in ipairs(vertices) do
		cached_functions.Color(v.color)
		cached_functions.Normal(v.normal)
		cached_functions.Vertex(v)
	end
	cached_functions.End(self.prim)
end

function M:enter()
	for k, v in pairs(self.function_map) do
		self.cached_functions[k] = gl[k]
		gl[k] = v
	end
end

function M:leave()
	for k, v in pairs(self.function_map) do
		gl[k] = self.cached_functions[k]
	end
end