local gl = require("opengl")
local GL = gl

local pi = math.pi
local cos = math.cos
local sin = math.sin
local print = print

local unpack = unpack

local vec3 = require("space.vec3")

module(...)

function cone(pos, dir, r, n)
	r = r or 1
	n = n or 20
	
--	print("pos", unpack(pos))
--	print("dir", unpack(dir))

	local normal = vec3.normalize(dir)
	local tangent = vec3.normalize(vec3.tangent(normal))
	local bitangent = vec3.normalize(vec3.cross(normal, tangent))

--	print("normal", unpack(normal))
--	print("tangent", unpack(tangent))

	local dtheta = 2.*pi/n
	local theta = 0
	
	local pwprev, pw1
	local pprev, p, p1
	local norm
	for i=0, n-1 do
		local v1 = vec3.scale(tangent, r*cos(theta));
		local v2 = vec3.scale(bitangent, r*sin(theta));
		p = vec3.add(v1, v2);
		p = vec3.add(p, dir);
		
		local pw = vec3.add(p, pos)
		
		if(i==0) then
			p1 = {unpack(p)}
			pw1 = {unpack(pw)}
		else
			norm = vec3.cross(p, pprev)
			norm = vec3.normalize(norm)
		
		--	gl.Normal(norm)
			gl.Vertex(pos)
			gl.Vertex(pw)
			gl.Vertex(pwprev)
		end
		theta = theta+dtheta
		
		pprev = {unpack(p)}
		pwprev = {unpack(pw)}
	end
	
	norm = vec3.cross(p1, pprev)
	norm = vec3.normalize(norm)
	
--	gl.Normal(norm)
	gl.Vertex(pos)
	gl.Vertex(pw1)
	gl.Vertex(pwprev)
end

function disc(pos, dir, r, n)
	r = r or 1
	n = n or 20

	local normal = vec3.normalize(dir)
	local tangent = vec3.normalize(vec3.tangent(dir))
	local bitangent = vec3.cross(normal, tangent)
	
	local dtheta = 2.*pi/n
	local theta = 0
	for i=0, n-1 do
		local v1 = vec3.scale(tangent, r*cos(theta))
		local v2 = vec3.scale(bitangent, r*sin(theta))
		local v = vec3.add(v1, v2)
		local p = vec3.add(v, pos)
		
		gl.Normal(v)
		gl.Vertex(p)
		theta = theta+dtheta
	end
end

--[[
function viewalign()
	local mv = gl.Get(GL_MODELVIEW_MATRIX)
	
	local va = {}
	va[1] = mv[1]
	va[2] = mv[5]
	va[3] = mv[9]
	va[4] = 0.
	
	va[5] = mv[2]
	va[6] = mv[6]
	va[7] = mv[10]
	va[8] = 0.
	
	va[9] = mv[3]
	va[10] = mv[7]
	va[11] = mv[11]
	va[12] = 0.
	
	va[13] = 0.
	va[14] = 0.
	va[15] = 0.
	va[16] = 1.
	
	gl.MultMatrix(va)
end
--]]