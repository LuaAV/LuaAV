local cos = math.cos
local sin = math.sin
local rad = math.rad
local _vec3 = require("space.vec3")

local unpack = unpack

module(...)

function mat4(m1, m2)
	return {
		m1[1]*m2[1] + m1[5]*m2[2] + m1[9]*m2[3] + m1[13]*m2[4],
		m1[2]*m2[1] + m1[6]*m2[2] + m1[10]*m2[3] + m1[14]*m2[4],
		m1[3]*m2[1] + m1[7]*m2[2] + m1[11]*m2[3] + m1[15]*m2[4],
		m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3] + m1[16]*m2[4],
		
		m1[1]*m2[5] + m1[5]*m2[6] + m1[9]*m2[7] + m1[13]*m2[8],
		m1[2]*m2[5] + m1[6]*m2[6] + m1[10]*m2[7] + m1[14]*m2[8],
		m1[3]*m2[5] + m1[7]*m2[6] + m1[11]*m2[7] + m1[15]*m2[8],
		m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7] + m1[16]*m2[8],
		
		m1[1]*m2[9] + m1[5]*m2[10] + m1[9]*m2[11] + m1[13]*m2[12],
		m1[2]*m2[9] + m1[6]*m2[10] + m1[10]*m2[11] + m1[14]*m2[12],
		m1[3]*m2[9] + m1[7]*m2[10] + m1[11]*m2[11] + m1[15]*m2[12],
		m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11] + m1[16]*m2[12],
		
		m1[1]*m2[13] + m1[5]*m2[14] + m1[9]*m2[15] + m1[13]*m2[16],
		m1[2]*m2[13] + m1[6]*m2[14] + m1[10]*m2[15] + m1[14]*m2[16],
		m1[3]*m2[13] + m1[7]*m2[14] + m1[11]*m2[15] + m1[15]*m2[16],
		m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15] + m1[16]*m2[16],
	}
end

function vec3(mat, v)
	return {
		mat[1]*v[1] + mat[5]*v[2] + mat[9]*v[3] + mat[13],
		mat[2]*v[1] + mat[6]*v[2] + mat[10]*v[3] + mat[14],
		mat[3]*v[1] + mat[7]*v[2] + mat[11]*v[3] + mat[15],
		1
	}
end

function vec4(mat, v)
	return {
		mat[1]*v[1] + mat[5]*v[2] + mat[9]*v[3] + mat[13]*v[4],
		mat[2]*v[1] + mat[6]*v[2] + mat[10]*v[3] + mat[14]*v[4],
		mat[3]*v[1] + mat[7]*v[2] + mat[11]*v[3] + mat[15]*v[4],
		mat[4]*v[1] + mat[8]*v[2] + mat[12]*v[3] + mat[16]*v[4],
	}
end

function ortho(left, right, bot, top, near, far)
	return {
		2./(right-left), 0, 0, 0,
		0, 2./(top-bot), 0, 0,
		0, 0, -2./(far-near), 0,
		-(right+left)/(right-left), -(top+bot)/(top-bot), -(far+near)/(far-near), 1,
	}
end

function perspective(fovy, aspect, near, far)
	fovy = rad(fovy)
	local f = 1/tan(fovy*0.5)
	return {
		f/aspect, 0, 0, 0, 
		0, f, 0, 0, 
		0, 0, (far+near)/(near-far), -1,
		0, 0, 2*far*near/(near-far), 0
	}
end

function lookat(eye, center, up)
	local F = _vec3.sub(center, eye)
	local f = _vec3.normalize(F)
	local up_ = _vec3.normalize(up)
	local s = _vec3.cross(f, up_)
	local u = _vec3.cross(s, f)
	
	return {
		s[1], u[1], -f[1], 0,
		s[2], u[2], -f[2], 0,
		s[3], u[3], -f[3], 0,
		-eye[1], -eye[2], -eye[3], 1.
	}
end

function translate(x, y, z)
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1.
	}
end

function rotate(a, x, y, z)
	a = rad(a)
	local c = cos(a)
	local s = sin(a)
	x, y, z = unpack(_vec3.normalize{x, y, z})

	return {
		x*x*(1-c)+c, y*x*(1-c)-z*s, x*z*(1-c)+y*s, 0,
		x*y*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s, 0,
		x*z*(1-c)-y*s, y*z*(1-c)+x*s, z*z*(1-c)+c, 0, 
		0, 0, 0, 1
	}
end

function scale(x, y, z)
	return {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1.
	}
end

function identity()
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1.
	}
end

function col(m, idx)
	return {
		m[(idx-1)*4+1],
		m[(idx-1)*4+2],
		m[(idx-1)*4+3],
	}
end