local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local draw = require("draw")

local vec3 = require("space.vec3")
local C3 = require("ga.c3")

local pi = math.pi
local rad = math.rad
local deg = math.deg
local atan2 = math.atan2
local sqrt = math.sqrt
local cos = math.cos
local sin = math.sin
local random = math.random
local format = string.format
local abs = math.abs
local asin = math.asin

local error = error
local type = type
local print = print
local pairs = pairs
local ipairs = ipairs
local unpack = unpack

local no = C3.no()
local e1 = C3.e1()
local e2 = C3.e2()
local e3 = C3.e3()
local ni = C3.ni()

local e1ni = C3.op(e1, ni)
local e2ni = C3.op(e2, ni)
local e3ni = C3.op(e3, ni)
local noni = C3.op(no, ni)

local g1 = {no, e1, e2, e3, ni}
local g2 = {}
local g3 = {}
local g4 = {}
local g5 = {C3.op(C3.op(C3.op(C3.op(no, e1), e2), e3), ni)}

for i=1, 5 do
for j=i+1, 5 do
	local v1 = g1[i]
	local v2 = g1[j]
	g2[#g2+1] = C3.op(v1, v2)
end
end

for i=1, 5 do
for j=i+1, 5 do
for k=j+1, 5 do
	local v1 = g1[i]
	local v2 = g1[j]
	local v3 = g1[k]
	g3[#g3+1] = C3.op(C3.op(v1, v2), v3)
end
end
end

for i=1, 5 do
for j=i+1, 5 do
for k=j+1, 5 do
for l=k+1, 5 do
	local v1 = g1[i]
	local v2 = g1[j]
	local v3 = g1[k]
	local v4 = g1[l]
	g4[#g4+1] = C3.op(C3.op(C3.op(v1, v2), v3), v4)
end
end
end
end

local grade_tree = {g1, g2, g3, g4, g5}


local xaxis = C3.gp(no, e1)
local yaxis = C3.gp(no, e2)
local zaxis = C3.gp(no, e3)

local frame_palette = {
	primary = {1., 0.2, 0.2},
	secondary = {0., 0.63, 0.43},
	tertiary = {.22, .35, 0.66},
}

module(...)

local I3_inv = C3.vinv(C3.op(C3.op(e1, e2), e3))
function E3_dual(e3mv)
	return C3.lc(e3mv, I3_inv)
end

function pos(p)
	return {p.e1, p.e2, p.e3}
end

function point(v)
	local vv
	if(type(v) == "userdata") then
		vv = {v.e1, v.e2, v.e3}
	else
		vv = v
	end
	
	return C3.normalizedPoint(
		vv[1], vv[2], vv[3], 0.5*vec3.dot(vv, vv)
	)
end

function vector(v)
	local vv
	if(type(v) == "userdata") then
		vv = {v.e1, v.e2, v.e3}
	else
		vv = v
	end
	
	return C3.e3vector(vv[1], vv[2], vv[3])
end

function normalize_point(p)
	local alpha = C3.hip(ni, p).scalar
	return C3.gp(-1/alpha, p)
end

function Line(a, b)
	local L = C3.unit(C3.op(C3.op(
		point(a), 
		point(b)
	), ni))
	L.a = a
	L.b = b
	return L
end

function I(r)
	-- normalized grade 2 part of the rotor
	return C3.e3bivector(vec3.normalize{r.e1_e2, r.e2_e3, r.e3_e1})
end

function B(v1, v2)
	v1 = vec3.normalize(v1)
	v2 = vec3.normalize(v2)
	
	return C3.op(C3.e3vector(v1), C3.e3vector(v2))
end

function R(v1, v2)
	v1 = vec3.normalize(v1)
	v2 = vec3.normalize(v2)
	
	-- e^(L*phi/2)
	-- L* is the dual of the rotation axis
	-- phi is the rotation angle
	return C3.gp(C3.e3vector(v1), C3.e3vector(v2))
end

-- splits the translation component
-- into a translation along the screw axis (Tw)
-- and a commuting translation in the screw's 
-- rotation plane (Tv)
function split_translation(t, R)
	local I1 = I(R)
	print("R", R.description)
	print("1", I1.description)
	local w = perpendicular(t, I1)
	local R2 = C3.gp(R, R)
	print("w", w.description)
	print("wx", T(w).description)
--	print("R2", R2.description)
	local v = C3.gp(C3.vinv(C3.sub(1, R2)), project(t, I1))
	
	print("v", v.description)
	print("vx", T(v).description)
	
	--return T(w), T(v), T(C3.gp(-1, v))
	return w, v, C3.gp(v, -1)
end

function T(t)
	local tt
	if(type(t) == "table") then
		tt = C3.e3vector(t)
	else
		tt = t
	end
	
	return C3.mv(C3.exp(
		C3.freeVector(C3.gp(C3.op(tt, ni), -0.5))
	))
end

function Tb(t)
	local tt = C3.e3vector(t)
	return C3.freeVector(C3.gp(C3.op(tt, ni), -0.5))
end

function boost(S, t)
	local tt = T(t)
	return C3.gp(C3.gp(S, tt), S)
end

function RBM(t, R1)
	--print("t", t.description)
	local Tw, Tv, Tvinv = split_translation(t, R1)
	---[[
	print("Tw", Tw.description)
	print("Tv", Tv.description)
	print("TvR1", C3.gp(Tv, R1).description)
	--]]
	return C3.gp(Tw, C3.gp(C3.gp(Tv, R1), Tvinv))
end

function tangent(t)
	local nit = C3.hip(ni, t)
	local dir = C3.op(ni, nit)
	--[[
	print("nit: ", nit.description)
	print("dir: ", dir.description)
	print("1/nit: ", C3.vinv(nit).description)
	print("t:", t.description)
	print("hip", C3.hip(t, C3.vinv(nit)).description)
	print("op", C3.op(t, C3.vinv(nit)).description)
	--]]
	local pos = C3.gp(-1, C3.gp(t, C3.vinv(nit)))
	
	local p = {
		pos.e1,
		pos.e2,
		pos.e3
	}
	local d = {
		dir.e1_ni,
		dir.e2_ni,
		dir.e3_ni
	}

	return p, d
end

function dual_sphere(pos, r)
	return C3.applyVersor(T(pos), unit_sphere(r))
end

function dual_circle(pos, n, r)
	return C3.applyVersor(T(pos), unit_circle(n, r))
end

function unit_sphere(r)
	if(r < 0) then
		return C3.sub(
			no,
			C3.gp(
				-0.5*r*r, 
				ni
			)
		)
	else
		return C3.sub(
			no,
			C3.gp(
				0.5*r*r, 
				ni
			)
		)
	end
end

function unit_circle(n, r)
	local P = vector(n)
	if(r < 0) then
		return C3.op(
			C3.sub(
				no,
				C3.gp(
					-0.5*r*r, 
					ni
				)
			),
			P
		)
	else
		return C3.op(
			C3.sub(
				no,
				C3.gp(
					0.5*r*r, 
					ni
				)
			),
			P
		)
	end
end

function max_grade(gbmp)
	if((gbmp - 2^5) >= 0) then
		return 5
	elseif((gbmp - 2^4) >= 0) then
		return 4
	elseif((gbmp - 2^3) >= 0) then
		return 3
	elseif((gbmp - 2^2) >= 0) then
		return 2
	elseif((gbmp - 2^1) >= 0) then
		return 1
	else
		return 0
	end
end

function grade_to_bmp(g)
	return 2^g
end

function max_component(mv, eps)
	local g = max_grade(C3.gradeBitmap(mv, eps))
	local gn = grade_tree[g]
	
	local maxc
	local maxv = 0
	for i, c in ipairs(gn) do
		local v = abs(C3.hip(mv, c).scalar)
		if(v > maxv) then
			maxv = v
			maxc = c
		end
	end
	return maxc
end

function basis(E)
	local b = {}
	for i, c in ipairs(g1) do
		if(C3.gradeBitmap(C3.hip(c, E)) > 0) then
			b[#b+1] = c
		end
	end
	return b
end

function factor(B)
	local s = C3.norm(B)
	local E = max_component(B, 1e-5)
	local Ebasis = basis(E)
	local Bc = C3.gp(B, 1/s)
	
	local F = {}
	for i=1, #Ebasis-1 do
		local e = Ebasis[i]
		--[[
		print(i, e.description)
		print("Lc", C3.lc(e, Bc).description)
		print("1/Bc", C3.vinv(Bc).description)
		print("XXX", C3.gp(C3.lc(e, Bc), C3.vinv(Bc)).description)
		print(C3.norm(C3.gp(C3.lc(e, Bc), C3.vinv(Bc))))
		--]]
		local f = C3.unit(C3.gp(C3.lc(e, Bc), C3.vinv(Bc)))
		Bc = C3.lc(C3.vinv(f), Bc)
		F[i] = f
	end
	F[#F+1] = C3.unit(Bc)
	return F, s
end

function delta(A, B, eps)
	local AB = C3.gp(A, B)
	local g = max_grade(C3.gradeBitmap(AB, eps))
	return C3.extractGrade(AB, grade_to_bmp(g))
end

function meet(A, B, eps)
	eps = eps or 1e-5
	local Ainv = C3.vinv(A)
	local DAB = delta(A, B, eps)
	local ga = max_grade(C3.gradeBitmap(A, eps))
	local gb = max_grade(C3.gradeBitmap(B, eps))
	local gd = max_grade(C3.gradeBitmap(DAB, eps))
	local Mg = 0.5*(ga+gb-gd)
	
--	print("A", A.description)
--	print("B", B.description)
	local S = C3.dual(DAB)
	
--	print("xx", C3.gp(A, B).description)
--	print("S", S.description)
	
	if(max_grade(C3.gradeBitmap(C3.gp(A, B), eps)) == 0) then
		return A
	end
	
	if(max_grade(C3.gradeBitmap(S, eps)) <= 1) then
		return S
	end
	
	local ss = factor(S)
	local M = 1
	for i, s in ipairs(ss) do
--		print(i, s.description)
		local p = C3.lc(C3.lc(s, A), Ainv)
		if(C3.norm(p) > eps) then
			M = C3.op(M, p)
		end
	end
	return M
end

function point_pair(pp)
	local v = sqrt(C3.gp(pp, pp).scalar)
--	print(v)
	local fact = C3.vinv(C3.lc(C3.gp(-1, ni), pp))

	local p1 = C3.gp(C3.add(pp, v), fact)
	local p2 = C3.gp(C3.sub(pp, v), fact)
	return p1, p2
end

local round_for_grade = {
	[4] = "sphere",
	[3] = "circle",
	[2] = "dual circle",
	[1] = "dual sphere",
}

function round_category(round)
	return round_for_grade[max_grade(C3.gradeBitmap(round))]
end

function round_pos(round)
	local category = round_category(round)
	if(category == "dual sphere" or category == "dual circle") then
		round = C3.dual(round)
	end

	local ir = C3.hip(ni, round)
	local p = C3.gp(-1, C3.gp(round, C3.vinv(ir)))
	return point{p.e1, p.e2, p.e3}
end

function round_dir(round)
	local category = round_category(round)
	if(category == "dual sphere" or category == "dual circle") then
		round = C3.dual(round)
	end
	
	local ir = C3.hip(ni, round)
	local dir = C3.hip(no, C3.op(ni, ir))

	return {
		dir.e2_e3,
		dir.e3_e1,
		dir.e1_e2
	}
end

function flat_pos(flat)
	local ppos = C3.lc(C3.lc(no, flat), C3.vinv(flat))
	return C3.gp(ppos, -C3.sp(C3.mv(ni), ppos))
end

function flat_dir(flat)
	local dir = C3.hip(no, C3.hip(ni, flat))
	return {
		-dir.e2_e3,
		-dir.e3_e1,
		-dir.e1_e2
	}
end

function round_radius(round)
	local category = round_category(round)
	if(category == "dual sphere" or category == "dual circle") then
		round = C3.dual(round)
	end

	
	local R2 = C3.gp(
		C3.gp(round, C3.gradeInvolution(round)),
		C3.vinv(
			C3.gp(C3.hip(ni, round), C3.hip(ni, round))
		)
	).scalar

	if(R2 < 0) then
		return -sqrt(-R2)
	else
		return sqrt(R2)
	end
end

function grade2(r)
	return {r.e1_e2, r.e2_e3, r.e3_e1}
end

function quat(V)
	return {V.scalar, -V.e2_e3, -V.e3_e1, -V.e1_e2}
end

function axisangle(axis, angle)
	return C3.exp(C3.gp(E3_dual(vector(axis)), angle*0.5))
end

-- extracts the perpendicular component of t in relation to I
-- (t^I)/I
function perpendicular(t, I)
	return C3.gp(C3.op(t, I), C3.vinv(C3.mv(I)))
end

-- projects t onto I
-- (t|I)/I
function project(t, I)
	return C3.gp(C3.lc(t, I), C3.vinv(C3.mv(I)))
end

function line_bivector(L1, L2, phase)
	local V2 = C3.gp(L2, C3.vinv(L1))
	local V2_log, phi = log(V2, phase)
	return C3.gp(V2_log, 0.5), phi
end

function Bn(B, N)
	return C3.gp(B, 1/N)
end

function Bparts(B)
	local v = {
		B.e2_e3,
		B.e3_e1,
		B.e1_e2
	}
	
	local theta = asin(vec3.mag(v))
	return v, theta
end

function log(V, phase)
	phase = phase or 0
	local R = C3.lc(C3.gp(-1, no), C3.gp(V, ni))
	local t = C3.gp(-2, C3.gp(C3.lc(no, V), C3.vinv(R)))
	local g2mag = vec3.mag(grade2(R))
	
	if(g2mag < 0.0001) then
		if(R.scalar >= 0) then
			return C3.gp(C3.gp(t, ni), -0.5), 0
		else
			-- J is constructed from the Euclidean dual of t
			local J = C3.gp(C3.unit(C3.hip(C3.dual(t), C3.gp(no, ni))), pi)
			local T = C3.gp(-1, C3.gp(t, C3.gp(ni, 1/2)))
			return C3.sub(T, J), pi
		end
	else
		local I = C3.gp(C3.e3bivector(grade2(R)), 1/g2mag)
		local phi = -2*atan2(g2mag, R.scalar)+phase
		local w = perpendicular(t, I)
		local toutI = C3.gp(-1, w)
		local R2 = C3.gp(R, R)
		local tin1R2 = C3.gp(
			C3.vinv(C3.sub(1, R2)), 
			C3.gp(C3.lc(t, I), phi)
		)
		local tt = C3.gp(
			C3.gp(
				C3.add(toutI, tin1R2), 
				ni
			), 
			0.5
		)
		local ROT = C3.gp(I, phi*0.5)
		local LOG = C3.sub(tt, ROT)
		return LOG, phi
	end
end

function frame()
	return {
		xaxis = C3.gp(no, e1),
		yaxis = C3.gp(no, e2),
		zaxis = C3.gp(no, e3),
	}
end

function transform_frame(frame, V)
	local nframe = {}
	for k, v in pairs(frame) do
		nframe[k] = C3.applyVersor(V, v)
	end
	return nframe
end


function draw_frame(V)
	local X = C3.applyVersor(V, xaxis)
	local Y = C3.applyVersor(V, yaxis)
	local Z = C3.applyVersor(V, zaxis)
	
	local px, dx = tangent(X)
	local py, dy = tangent(Y)
	local pz, dz = tangent(Z)

	local s = 0.5
	gl.Begin(GL.LINES)
		gl.Color(frame_palette.primary)
		gl.Vertex(px)
		gl.Vertex(vec3.add(px, vec3.scale(dx, s)))
	gl.End()
	
	gl.Begin(GL.LINES)
		gl.Color(frame_palette.secondary)
		gl.Vertex(py)
		gl.Vertex(vec3.add(py, vec3.scale(dy, s)))
	gl.End()
	
	gl.Begin(GL.LINES)	
		gl.Color(frame_palette.tertiary)
		gl.Vertex(pz)
		gl.Vertex(vec3.add(pz, vec3.scale(dz, s)))
	gl.End()
	
	gl.Begin(GL.TRIANGLES)
		gl.Color(frame_palette.primary)
		draw.cone(
			vec3.add(px, vec3.scale(dx, s)), 
			vec3.scale(dx, -0.1), 
			0.025, 30
		)
	gl.End()
	
	gl.Begin(GL.TRIANGLES)
		gl.Color(frame_palette.secondary)
		draw.cone(
			vec3.add(py, vec3.scale(dy, s)), 
			vec3.scale(dy, -0.1), 
			0.025, 30
		)
	gl.End()
		
	gl.Begin(GL.TRIANGLES)
		gl.Color(frame_palette.tertiary)
		draw.cone(
			vec3.add(pz, vec3.scale(dz, s)), 
			vec3.scale(dz, -0.1), 
			0.025, 30
		)
	gl.End()
	
	return px
end

function draw_frame_surface(fs)
--	gl.Begin(GL.QUADS)
	for i=1, #fs-1 do
		gl.Begin(GL.LINE_STRIP)
		local frame_sequence = fs[i]
		for j=1, #frame_sequence-1 do
			local frame1 = frame_sequence[j]
			local frame2 = frame_sequence[j+1]
			local frame3 = fs[i+1][j+1]
			local frame4 = fs[i+1][j]
			
			local p1 = tangent(frame1.xaxis)
			local p2 = tangent(frame2.xaxis)
			local p3 = tangent(frame3.xaxis)
			local p4 = tangent(frame4.xaxis)
			
			gl.Vertex(p1)
			--[[
			gl.Vertex(p2)
			gl.Vertex(p3)
			gl.Vertex(p4)
			--]]
		end
		gl.End()
	end
	--gl.End()
end

function draw_frames(frame, palette)
	palette = palette or frame_palette
	local px, dx = tangent(frame.xaxis)
	local py, dy = tangent(frame.yaxis)
	local pz, dz = tangent(frame.zaxis)

	local s = 0.25
	gl.Begin(GL.LINES)
		gl.Color(palette.primary)
		gl.Vertex(px)
		gl.Vertex(vec3.add(px, vec3.scale(dx, s)))
	gl.End()
	
	gl.Begin(GL.LINES)
		gl.Color(palette.secondary)
		gl.Vertex(py)
		gl.Vertex(vec3.add(py, vec3.scale(dy, s)))
	gl.End()
	
	gl.Begin(GL.LINES)	
		gl.Color(palette.tertiary)
		gl.Vertex(pz)
		gl.Vertex(vec3.add(pz, vec3.scale(dz, s)))
	gl.End()
	
	---[[
	gl.Begin(GL.TRIANGLES)
		gl.Color(palette.primary)
		draw.cone(
			vec3.add(px, vec3.scale(dx, s)), 
			vec3.scale(dx, -0.1), 
			0.025, 20
		)
	gl.End()
	
	gl.Begin(GL.TRIANGLES)
		gl.Color(palette.secondary)
		draw.cone(
			vec3.add(py, vec3.scale(dy, s)), 
			vec3.scale(dy, -0.1), 
			0.025, 20
		)
	gl.End()
		
	gl.Begin(GL.TRIANGLES)
		gl.Color(palette.tertiary)
		draw.cone(
			vec3.add(pz, vec3.scale(dz, s)), 
			vec3.scale(dz, -0.1), 
			0.025, 20
		)
	gl.End()
	--]]
	
	return px
end

function transated_frame(t)
	local frame = {
		xaxis = C3.gp(no, e1),
		yaxis = C3.gp(no, e2),
		zaxis = C3.gp(no, e3),
	}
	for k, v in pairs(frame) do
		frame[k] = C3.applyVersor(t, v)
	end
	return frame
end

function cycle_product(C1, C2, p, N)
	N = N or 5
	local frame_surface = {}
	local frame = transated_frame(T(p))
	
	local frame_sequence1 = C1:frames2(frame, N)
	for i, f in ipairs(frame_sequence1) do
		local frame_sequence2 = C2:frames2(f, N)
		frame_surface[#frame_surface+1] = frame_sequence2
	end
	return frame_surface
end

function plane_pos(F)
	local dir = C3.gp(-1, C3.lc(ni, F))
	local _pos = C3.lc(C3.lc(no, F), C3.vinv(F))
	_pos = C3.gp(_pos, 1/-C3.sp(C3.mv(ni), C3.mv(_pos)))
	_pos = point(_pos)
	
	return _pos
end

function plane_dir(F)
	local dir = C3.gp(-1, C3.lc(noni, F))
	return E3_dual(dir)
end

function draw_line(L, lc, cp)
	local dir = C3.gp(-1, C3.lc(ni, L))
	local _pos = C3.lc(C3.lc(no, L), C3.vinv(L))
	_pos = C3.gp(_pos, 1/-C3.sp(C3.mv(ni), C3.mv(_pos)))
	_pos = point(_pos)
	dir = {
		dir.e1_ni,
		dir.e2_ni,
		dir.e3_ni,
	}
	
	if(lc) then
		gl.Color(lc)
	else
		gl.Color(0.6, 0.6, 0.6, 0.8)
	end
	gl.Begin(GL.LINES)
		gl.Vertex(
			vec3.add(pos(_pos), vec3.scale(dir, 30))
		)
		gl.Vertex(
			vec3.add(pos(_pos), vec3.scale(dir, -30))
		)
	gl.End()

	if(cp) then
		gl.Color(cp)
	else
		gl.Color(1, 0, 0.5, 1)
	end
	gl.Begin(GL.POINTS)
		gl.Vertex(pos(_pos))
	gl.End()
end

function line_dir(L)
	local dir = C3.gp(-1, C3.lc(ni, L))
	dir = {
		dir.e1_ni,
		dir.e2_ni,
		dir.e3_ni,
	}
	return dir
end

function draw_line2(L, amt)
	amt = amt or 30
	local dir = C3.gp(-1, C3.lc(ni, L))
	local _pos = C3.lc(C3.lc(no, L), C3.vinv(L))
	_pos = C3.gp(_pos, 1/-C3.sp(C3.mv(ni), C3.mv(_pos)))
	_pos = point(_pos)
	dir = {
		dir.e1_ni,
		dir.e2_ni,
		dir.e3_ni,
	}

	gl.Begin(GL.LINES)
		gl.Vertex(
			vec3.add(pos(_pos), vec3.scale(dir, amt))
		)
		gl.Vertex(
			vec3.add(pos(_pos), vec3.scale(dir, -amt))
		)
	gl.End()
end

function draw_band(L, amt, thick)
	amt = amt or 30
	thick = thick or 0.1
	
	local dir = C3.gp(-1, C3.lc(ni, L))
	local _pos = C3.lc(C3.lc(no, L), C3.vinv(L))
	_pos = C3.gp(_pos, 1/-C3.sp(C3.mv(ni), C3.mv(_pos)))
	_pos = point(_pos)
	
	dir = {
		dir.e1_ni,
		dir.e2_ni,
		dir.e3_ni,
	}
	
	local p = pos(_pos)
	local t = vec3.tangent(dir)
	local out = vec3.scale(t, thick*0.5)
	local p1 = vec3.add(p, out)
	local p2 = vec3.sub(p, out)
	
	gl.Begin(GL.QUAD_STRIP)
		gl.Vertex(
			vec3.add(p1, vec3.scale(dir, amt))
		)
		gl.Vertex(
			vec3.add(p2, vec3.scale(dir, amt))
		)
		gl.Vertex(
			vec3.add(p1, vec3.scale(dir, -amt*0.5))
		)
		gl.Vertex(
			vec3.add(p2, vec3.scale(dir, -amt))
		)
	gl.End()
end

local tpalette = {
	point = {1, 0, 0.5, 1},
	line = {0, 0.5, 1, 1},
}

function draw_tangent(t, cp, cl)
	cp = cp or tpalette.point
	cl = cl or tpalette.line

	local p, d = tangent(t)
	d = vec3.scale(d, 0.2)
	
	gl.Color(cl)
	gl.Begin(GL.LINES)
		gl.Vertex(p)
		gl.Vertex(vec3.add(p, d))
	gl.End()
	
	gl.Color(cp)
	gl.Begin(GL.POINTS)
		gl.Vertex(p)
	gl.End()
end

function draw_circle(C, N)
	local R = round_radius(C)
	if(R > 1e4) then
		if(max_grade(C3.gradeBitmap(C, 1e-5)) == 3) then
			draw_line2(C)
		else
			draw_line2(C3.dual(C), 3)
		end
	else
		if(R > 100) then
			gl.Begin(GL.LINE_LOOP)
			sketch.disc(
				pos(round_pos(C)),
				vec3.normalize(vec3.scale(round_dir(C), 10)),
				R,
				N
			)
			gl.End()
		else
			gl.Begin(GL.LINE_LOOP)
			sketch.disc(
				pos(round_pos(C)),
				round_dir(C),
				R,
				N
			)
			gl.End()
		end
	end
end

function draw_tube(C, N, thick)
	thick = thick or 0.1
	local R = round_radius(C)
	if(R > 1e4) then
		if(max_grade(C3.gradeBitmap(C, 1e-5)) == 3) then
			draw_band(C, 3, thick)
		else
			draw_band(C3.dual(C), 3, thick)
		end
		
	else
		if(R > 100) then
			gl.Begin(GL.QUAD_STRIP)
			sketch.disc(
				pos(round_pos(C)),
				vec3.normalize(vec3.scale(round_dir(C), 10)),
				R,
				N,
				thick
			)
			gl.End()
		else
			gl.Begin(GL.QUAD_STRIP)
			sketch.tube(
				pos(round_pos(C)),
				round_dir(C),
				R,
				N,
				thick
			)
			gl.End()
		end
	end
end

function draw_sphere(S, N, x)
	x = x or 6
	N = N or 40
	
	local p = pos(round_pos(S))
	local r = round_radius(S)
	
	if(r > 1e7) then
		--draw_line(C3.dual(C))
		--print(C3.dual(S).description)
		local DS = C3.dual(S)
		local p = pos(plane_pos(DS))
		local d = plane_dir(DS)
		
		local n = vec3.normalize(pos(d))
		local t1 = vec3.normalize(vec3.tangent(n))
		local t2 = vec3.normalize(vec3.cross(n, t1))
		
		local ds = 2*x/N
		for s=-x, x, ds do
			local v11 = vec3.scale(t1, s)
			local v12 = vec3.scale(t1, s+ds)
			
			gl.Begin(GL.QUAD_STRIP)
			for t=-x, x, ds do
				local v2 = vec3.scale(t2, t)
				local p1 = vec3.add(p, vec3.add(v2, v11))
				local p2 = vec3.add(p, vec3.add(v2, v12))
				
				gl.Vertex(p1)
				gl.Vertex(p2)
			end
			gl.End()
		end
	else
		---[[
		local dtheta = 2*pi/N
		for theta=0, 2*pi, dtheta do
		gl.Begin(GL.QUAD_STRIP)
		for phi=0, pi, 2*pi/N do
			gl.Vertex(
				r*cos(theta)*sin(phi)+p[1],
				r*sin(theta)*sin(phi)+p[2],
				r*cos(phi)+p[3]
			)
			gl.Vertex(
				r*cos(theta+dtheta)*sin(phi)+p[1],
				r*sin(theta+dtheta)*sin(phi)+p[2],
				r*cos(phi)+p[3]
			)
		end
		gl.End()
		end
		--]]
	end
end