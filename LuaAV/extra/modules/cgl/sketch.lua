local gl = require("opengl")
local GL = gl
local glu = require("opengl.glu")
local vec3 = require("space.vec3")


local cos = math.cos
local sin = math.sin
local pi = math.pi

local print = print
local ipairs = ipairs

module(...)

function enter_ortho(...)
	local v = {...}
	local vp = {-1, -1, 2, 2}
	for i, vv in ipairs(v) do
		vp[i] = vv
	end

	if(#v > 2) then
		if(#v == 3) then
			vp[4] = vp[3]
		end
		
		gl.MatrixMode(GL.PROJECTION)
		gl.PushMatrix()
			gl.LoadIdentity()
			gl.Ortho(vp[1], vp[1]+vp[3], vp[2], vp[2]+vp[4], -100, 100)

		gl.MatrixMode(GL.MODELVIEW)
		gl.PushMatrix()
			gl.LoadIdentity()
			
	elseif(#v > 0) then
		if(#v == 1) then
			vp[2] = vp[1]
		end
		
		gl.MatrixMode(GL.PROJECTION)
		gl.PushMatrix()
			gl.LoadIdentity()
			gl.Ortho(0, vp[1], vp[2], 0, -100, 100)

		gl.MatrixMode(GL.MODELVIEW)
		gl.PushMatrix()
			gl.LoadIdentity()

	else
		gl.MatrixMode(GL.PROJECTION)
		gl.PushMatrix()
			gl.LoadIdentity()
			gl.Ortho(-1, 1, -1, 1, -100, 100)

		gl.MatrixMode(GL.MODELVIEW)
		gl.PushMatrix()
			gl.LoadIdentity()
	end
end

function leave_ortho()
	gl.MatrixMode(GL.PROJECTION)
	gl.PopMatrix()

	gl.MatrixMode(GL.MODELVIEW)
	gl.PopMatrix()
end

function circle(n)
	n = n or 20
	
	local dtheta = 2.*pi/n
	local theta = 0
	for i=0, n-1 do
		gl.Vertex(cos(theta), sin(theta), 0)
		theta = theta + dtheta
	end
end

function quad(...)
	local q = {...}
	local quad = {-1, -1, 2, 2}
	for i, v in ipairs(q) do
		quad[i] = v
	end
	
	if(#q > 2) then
		if(#q == 3) then
			quad[4] = quad[3]
		end
	elseif(#q > 0) then
		if(#q == 1) then
			quad[2] = quad[1]
		end
		
		quad[4] = quad[2]
		quad[3] = quad[1]
		quad[2] = -quad[4]/2
		quad[1] = -quad[3]/2
	end
	
	gl.TexCoord(0, 1) gl.Vertex(quad[1], quad[2], 0)
	gl.TexCoord(1, 1) gl.Vertex(quad[1]+quad[3], quad[2], 0)
	gl.TexCoord(1, 0) gl.Vertex(quad[1]+quad[3], quad[2]+quad[4], 0)
	gl.TexCoord(0, 0) gl.Vertex(quad[1], quad[2]+quad[4], 0)
end

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