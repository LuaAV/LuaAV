local assert, pairs, tostring, type = assert, pairs, tostring, type
local print = print
local ipairs = ipairs
local setmetatable = setmetatable
local getfenv = getfenv
local unpack = unpack
----------------------------------------------------------
local gl = require("opengl")
local GL = gl
local Label = require("Label")
----------------------------------------------------------
local space = require("space")
local vec3 = require("space.vec3")
----------------------------------------------------------
local Canvas = require("Canvas")
local sketch = require("cgl.sketch")
----------------------------------------------------------
local table = require("table")
local math = require("math")
local string = require("string")
----------------------------------------------------------
local format = string.format
local cos = math.cos
local sin = math.sin
local pi = math.pi
local tan = math.tan
local rad = math.rad
local deg = math.deg
----------------------------------------------------------
local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

--- OpenGL immediate-mode-like interface for Cairo graphics
-- cgl implements an immediate-mode-like interface for the Cairo
-- vector graphics library.  It's intended to be be a one-to-one 
-- replacement for Lua OpenGL bindings such that a draw function using 
-- OpenGL can simply replace the environment for the function to 
-- make it use cgl instead.
-- @name cgl
module(..., setconstructor)
----------------------------------------------------------

local
function getextension(filename)
	return filename:match(".+%.(%w+)$")
end

local
function mat4_mat4(m1, m2)
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

local
function mat4_vec3(mat, v)
	return {
		mat[1]*v[1] + mat[5]*v[2] + mat[9]*v[3] + mat[13],
		mat[2]*v[1] + mat[6]*v[2] + mat[10]*v[3] + mat[14],
		mat[3]*v[1] + mat[7]*v[2] + mat[11]*v[3] + mat[15],
		1
	}
end

local
function mat4_vec4(mat, v)
	return {
		mat[1]*v[1] + mat[5]*v[2] + mat[9]*v[3] + mat[13]*v[4],
		mat[2]*v[1] + mat[6]*v[2] + mat[10]*v[3] + mat[14]*v[4],
		mat[3]*v[1] + mat[7]*v[2] + mat[11]*v[3] + mat[15]*v[4],
		mat[4]*v[1] + mat[8]*v[2] + mat[12]*v[3] + mat[16]*v[4],
	}
end

local
function window_coords(vp, ndc)
	return {
		vp[3]*0.5*ndc[1] + (vp[1]+vp[3]*0.5),
		vp[4] - (vp[4]*0.5*ndc[2] + (vp[2]+vp[4]*0.5)),
		-- not implemented
	}
end

local
function mat4_ortho(left, right, bot, top, near, far)
	return {
		2./(right-left), 0, 0, 0,
		0, 2./(top-bot), 0, 0,
		0, 0, -2./(far-near), 0,
		-(right+left)/(right-left), -(top+bot)/(top-bot), -(far+near)/(far-near), 1,
	}
end

local
function mat4_perspective(fovy, aspect, near, far)
	fovy = rad(fovy)
	local f = 1/tan(fovy*0.5)
	return {
		f/aspect, 0, 0, 0, 
		0, f, 0, 0, 
		0, 0, (far+near)/(near-far), -1,
		0, 0, 2*far*near/(near-far), 0
	}
end

local
function mat4_lookat(eye, center, up)
	local F = vec3.sub(center, eye)
	local f = vec3.normalize(F)
	local up_ = vec3.normalize(up)
	local s = vec3.cross(f, up_)
	local u = vec3.cross(s, f)
	
	return {
		s[1], u[1], -f[1], 0,
		s[2], u[2], -f[2], 0,
		s[3], u[3], -f[3], 0,
		-eye[1], -eye[2], -eye[3], 1.
	}
end

local
function mat4_translate(x, y, z)
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1.
	}
end

local
function mat4_rotate(a, x, y, z)
	a = rad(a)
	local c = cos(a)
	local s = sin(a)
	x, y, z = unpack(vec3.normalize{x, y, z})
	
	return {
		x*x*(1-c)+c, y*x*(1-c)+z*s, x*z*(1-c)-y*s, 0,
		x*y*(1-c)-z*s, y*y*(1-c)+c, y*z*(1-c)+x*s, 0,
		x*z*(1-c)+y*s, y*z*(1-c)-x*s, z*z*(1-c)+c, 0, 
		0, 0, 0, 1
	}
end

local
function mat4_scale(x, y, z)
	return {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1.
	}
end

local
function mat4_identity()
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1.
	}
end

local
function mat4_print(mat)
	print(format("%.4f %.4f %.4f %.4f", mat[1], mat[5], mat[9], mat[13]))
	print(format("%.4f %.4f %.4f %.4f", mat[2], mat[6], mat[10], mat[14]))
	print(format("%.4f %.4f %.4f %.4f", mat[3], mat[7], mat[11], mat[15]))
	print(format("%.4f %.4f %.4f %.4f", mat[4], mat[8], mat[12], mat[16]))
end

local
function vec4_print(v)
	print(format("%.4f %.4f %.4f %.4f", v[1], v[2], v[3], v[4]))
end

local
function mat4_copy(mat)
	return {unpack(mat)}
end


local CGL = M
for k, v in pairs(GL) do
	-- grab enums from GL
	if(type(v) == "number") then
		CGL[k] = v
	end
end

local matrix_for_mode = {
	[CGL.PROJECTION] = "projection",
	[CGL.MODELVIEW] = "modelview",
}

--- Constructor
-- @param fmt A surface format (e.g. CAIRO.FORMAT_RGB24) or PDF filename
-- @param w Width
-- @param h Height
-- @name cgl
function C:__call(fmt, w, h)
	local aspect = w/h
	local canvas
	local use_pdf = false
	if(type(fmt) == "number") then
		--cs = cairo.image_surface_create(fmt, w, h)
		error("non-PDF surfaces not supported with cgl.canvas")
	elseif(type(fmt) == "string" and getextension(fmt) == "pdf") then
		use_pdf = true
		canvas = Canvas(fmt, {0, 0, w, h})
	else
		error("cgl: first argument must be a surface format or PDF filename")
	end
	
	local m = {
		-- color
		backg_color = {0, 0, 0, 1},
		fill_color = {1, 1, 1, 1},
		stroke_color = {1, 1, 1, 1},
		
		-- matrices
		matrixmode = CGL.MODELVIEW,
		viewport = {0, 0, w, h},
		modelview = mat4_lookat({0, 0, 4}, {0, 0, 0}, {0, 1, 0}),
		projection = mat4_perspective(45, aspect, 0.1, 100),
		modelview_stack = {},
		projection_stack = {},
		
		-- drawing data
		colors = {},
		vertices = {},
		drawmode = CGL.LINES,
		drawing = false,
	--	polystyle = CGL.FILL,
		thickness = 1.,
		point_size = 1.,
		
		-- Canvas
		canvas = canvas,
		
		-- sub-modules
		sketch = sketch,
	}
	
	m.stacks = {
		[CGL.PROJECTION] = m.projection_stack,
		[CGL.MODELVIEW] = m.modelview_stack,
	}
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			-- wrap OO functions to look like OpenGL
			m[k] = function(...)
				return v(m, ...)
			end
		end
	end
	
	return m
end

--- Draw a label
-- @param pos Position
-- @param text Label text
-- @param font Font name (default Monaco)
-- @param sz Size (default 8)
-- @param slant Font slant (default CAIRO.FONT_SLANT_NORMAL)
-- @param weight Font weight (default CAIRO.FONT_WEIGHT_NORMAL)
function M:label(pos, text, font, sz, slant, weight)
	font = font or "Monaco"
	sz = sz or 8
	slant = slant or CAIRO.FONT_SLANT_NORMAL
	weight = weight or CAIRO.FONT_WEIGHT_NORMAL
	
	local cr = self.cr
	local te = cairo.TextExtents.new()
	
	cr:set_source_rgba(unpack(self.fill_color))
	cr:select_font_face(font, slant, weight)
	cr:set_font_size(sz)
	cr:text_extents(text, te)
	
	local vv = self.Project(pos)

	cr:move_to(vv[1], vv[2])

	cr:show_text(text)
end

local
function table_for_args(n, ...)
	local args = {...}
	if(type(args[1]) == "table") then
		args = args[1]
	end
	for i=#args+1, n do
		args[i] = 0
	end
	return args
end

local
function table_for_args_color(n, ...)
	local args = {...}
	if(type(args[1]) == "table") then
		args = args[1]
	end
	for i=#args+1, n do
		if(i == 4) then
			args[i] = 1
		else
			args[i] = 0
		end
	end
	return args
end

function M:label_3d(label, dim, pos, text)
	local font = "Monaco"
	local sz = label.size

	local canvas = self.canvas
	
	canvas.fill_color = self.fill_color
	canvas:set_font(font, sz)
	
	local text_size = canvas:text_size(text)

	local vpos = table_for_args(3, unpack(pos))
	local vv = self.Project(vpos)
	vv[1] = label.margin[1]+vv[1]
	vv[2] = label.margin[2]+vv[2]
	
	if(label.alignment == Label.CENTER) then
		vv[1] = vv[1] - 0.5*text_size[1]
	end

	canvas:show_text(vv, text)
end

--- Gets the active matrix stack
-- @ret Matrix stack
function M:active_stack()
	return self.stacks[self.matrixmode]
end

--- Sets the matrix mode
-- @param mode The mode (CGL.PROJECTION or CGL.MODELVIEW)
function M:MatrixMode(mode)
	self.matrixmode = mode
end

--- Load the identity matrix in the current stack
function M:LoadIdentity()
	if(self.matrixmode == CGL.MODELVIEW) then
		self.modelview = mat4_identity()
	else
		self.projection = mat4_identity()
	end
end

--- Push the matrix on the current stack
function M:PushMatrix()
	local stack = self.active_stack()
	stack[#stack+1] = mat4_copy(self[ matrix_for_mode[self.matrixmode] ])
end

--- Pop the matrix on the current stack
function M:PopMatrix()
	local stack = self.active_stack()
	self[ matrix_for_mode[self.matrixmode] ] = stack[#stack]
	stack[#stack] = nil
end

--- Set an orthographic matrix on the current stack
-- @param left Left coordinate
-- @param right Right coordinate
-- @param bot Bottom coordinate
-- @param top Top coordinate
-- @param near Near clip plane distance
-- @param far Far clip plane distance
function M:Ortho(left, right, bot, top, near, far)
	local matname = matrix_for_mode[self.matrixmode]
	self[matname] = mat4_mat4(self[matname] , mat4_ortho(left, right, bot, top, near, far))
end

--- Set a perspective matrix on the current stack
-- @param fovy Vertical field of view in degrees
-- @param aspect Aspect ratio
-- @param near Near clip plane distance
-- @param far Far clip plane distance
function M:Perspective(fovy, aspect, near, far)
	local matname = matrix_for_mode[self.matrixmode]
	self[matname] = mat4_mat4(self[matname] , mat4_perspective(fovy, aspect, near, far))
end

--- Set a coordinate system on the current stack
-- @param eye Viewing eye location
-- @param center Center point of the field of view
-- @param up The up direction
function M:LookAt(eye, center, up)
	local matname = matrix_for_mode[self.matrixmode]
	self[matname] = mat4_mat4(self[matname] , mat4_lookat(eye, center, up))
end

--- Translate the coordinate system on the current stack
-- @param ... Either xyz values or a table of such values
function M:Translate(...)
	local v = table_for_args(3, ...)
	local matname = matrix_for_mode[self.matrixmode]
	self[matname] = mat4_mat4(self[matname] , mat4_translate(v[1], v[2], v[3]))
end

--- Rotate the coordinate system on the current stack
-- @param a Angle to rotate in degrees
-- @param x X-coordinate of the axis
-- @param y Y-coordinate of the axis
-- @param z Z-coordinate of the axis
function M:Rotate(a, x, y, z)
	local matname = matrix_for_mode[self.matrixmode]
	self[matname] = mat4_mat4(self[matname] , mat4_rotate(a, x, y, z))
end

--- Scale the coordinate system on the current stack
-- @param x X-coordinate
-- @param y Y-coordinate
-- @param z Z-coordinate
function M:Scale(x, y, z)
	local matname = matrix_for_mode[self.matrixmode]
	self[matname] = mat4_mat4(self[matname] , mat4_scale(x, y, z))
end

--- Project a point through the current coordinate system
-- @param v The point 
function M:Project(v)
	-- modelview/projection
	local vv = mat4_vec3(self.modelview, v)
	vv = mat4_vec4(self.projection, vv)

	-- normalized device coordinates
	vv[1] = vv[1]/vv[4]
	vv[2] = vv[2]/vv[4]
	vv[3] = vv[3]/vv[4]
	vv[4] = vv[4]/vv[4]

	-- screen coordinates
	return window_coords(self.viewport, vv)
end

function M:Enable()
end

function M:Disable()
end

function M:BlendFunc()
end

--- Set the clear color
-- @param ... Either RGB(A) values or a table of such values
function M:ClearColor(...)
	self.backg_color = table_for_args_color(4, ...)
end

--- Set the stroke color
-- @param ... Either RGB(A) values or a table of such values
function M:StrokeColor(...)
	self.stroke_color = table_for_args_color(4, ...)
end

--- Set the fill color
-- @param ... Either RGB(A) values or a table of such values
function M:FillColor(...)
	self.fill_color = table_for_args_color(4, ...)
end

--- Set the point size
-- @param v The size
function M:PointSize(v)
	self.point_size = v
end

--- Set the line width
-- @param v The width
function M:LineWidth(v)
	self.thickness = v
end

--- Set the color (fill and stroke)
-- @param ... Either RGB(A) values or a table of such values
function M:Color(...)
	local v = table_for_args_color(4, ...)
	self.fill_color = v
	self.stroke_color = v
	if(self.drawing) then
		self.colors[#self.colors+1] = v
	end
end

function M:Normal()
end

--- Clear the surface using the clear color
function M:Clear()
	self.canvas:clear(self.backg_color)
end

--- Start drawing a shape
-- @param prim The primitive shape
function M:Begin(prim)
	self.drawmode = prim
	self.drawing = true
end

--- Add a vertex to the current shape
-- @param ... Either XYZ coorindates or a table of such values
function M:Vertex(...)
	self.vertices[#self.vertices+1] = table_for_args(3, ...)
end

function M:TexCoord(...)
	-- dummy function for now
end

local
function expand_colors(self, default)
	local ncolors = #self.colors
	for i=ncolors+1, #self.vertices do
		self.colors[i] = self.colors[ncolors] or default
	end
end

--- End drawing a shape
function M:End()
	local canvas = self.canvas
	local vertices = self.vertices
	
	if(self.drawmode == CGL.POINTS) then
		canvas.fill_color = self.fill_color
		for i, v in ipairs(vertices) do
			local vv = self.Project(v)
			canvas:oval(
				vv[1]-self.point_size*0.5, 
				vv[2]-self.point_size*0.5,
				self.point_size,
				self.point_size
			)
		end
		canvas:fill()
		
	elseif(self.drawmode == CGL.LINES) then
		canvas.stroke_color = self.stroke_color
		canvas.line_width = self.thickness
		
		local vv_prev
		for i, v in ipairs(vertices) do
			local vv = self.Project(v)
			if(i%2 == 1) then
				vv_prev = vv
				canvas:move_to(vv[1], vv[2])
			else
				--[[
				local patt = cairo.pattern_create_linear(
					vv_prev[1], vv_prev[2],
					vv[1], vv[2]
				)
				local c1 = self.colors[i-1]
				local c2 = self.colors[i]
				cairo.pattern_add_color_stop_rgba(patt, 0, c1[1], c1[2], c1[3], c1[4])
				cairo.pattern_add_color_stop_rgba(patt, 1, c2[1], c2[2], c2[3], c2[4])
				cr:set_source(patt)
				--]]
				
				canvas:line_to(vv[1], vv[2])
				canvas:stroke()
			end
		end

	elseif(self.drawmode == CGL.LINE_STRIP) then
		canvas.stroke_color = self.stroke_color
		canvas.line_width = self.thickness
		
		local v = vertices[1]
		if(v) then
			local vv = self.Project(v)
			canvas:move_to(vv[1], vv[2])
			
			local vv_prev = vv
			
			for i=2, #vertices do
				vv = self.Project(vertices[i])
				canvas:line_to(vv[1], vv[2])
				vv_prev = vv
			end
			canvas:stroke()
		end
		
	elseif(self.drawmode == CGL.LINE_LOOP) then
		canvas.stroke_color = self.stroke_color
		canvas.line_width = self.thickness
		
		local v = vertices[#vertices]
		if(v) then
			local vv = self.Project(v)
			canvas:move_to(vv[1], vv[2])
			for i, v in ipairs(vertices) do
				vv = self.Project(v)
				canvas:line_to(vv[1], vv[2])
			end
			canvas:stroke()
		end
		
	elseif(self.drawmode == CGL.TRIANGLES) then
		expand_colors(self, self.fill_color)
		canvas.fill_color = self.fill_color
		for i=1, #vertices, 3 do
			canvas.fill_color = self.colors[i]
			local vv1 = self.Project(vertices[i])
			local vv2 = self.Project(vertices[i+1])
			local vv3 = self.Project(vertices[i+2])
			canvas:move_to(vv1[1], vv1[2])
			canvas:line_to(vv2[1], vv2[2])
			canvas:line_to(vv3[1], vv3[2])
			canvas:line_to(vv1[1], vv1[2])
			canvas:fill()
		end
		
		
	elseif(self.drawmode == CGL.TRIANGLE_FAN) then
		canvas.fill_color = self.fill_color
		local v = vertices[1]
		local vv = self.Project(v)
		canvas:move_to(vv[1], vv[2])
		for i=2, #vertices-1 do
			local bv1 = vertices[i]
			local bv2 = vertices[i+1]
			
			local bvv1 = self.Project(bv1)
			local bvv2 = self.Project(bv2)
			
			canvas:line_to(bvv1[1], bvv1[2])
			canvas:line_to(bvv2[1], bvv2[2])
			canvas:line_to(vv[1], vv[2])
		end
		canvas:fill()
		
	elseif(self.drawmode == CGL.QUADS) then
		canvas.fill_color = self.fill_color
		for i=1, #vertices, 4 do
			local vv1 = self.Project(vertices[i])
			local vv2 = self.Project(vertices[i+1])
			local vv3 = self.Project(vertices[i+2])
			local vv4 = self.Project(vertices[i+3])
			canvas:move_to(vv1[1], vv1[2])
			canvas:line_to(vv2[1], vv2[2])
			canvas:line_to(vv3[1], vv3[2])
			canvas:line_to(vv4[1], vv4[2])
			canvas:line_to(vv1[1], vv1[2])
			canvas:fill()
		end
		
	elseif(self.drawmode == CGL.QUAD_STRIP) then
		canvas.fill_color = self.fill_color
--		canvas.stroke_color = self.fill_color
		for i=1, #vertices-2, 2 do
			local vv1 = self.Project(vertices[i])
			local vv2 = self.Project(vertices[i+1])
			local vv3 = self.Project(vertices[i+3])
			local vv4 = self.Project(vertices[i+2])
			canvas:move_to(vv1[1], vv1[2])
			canvas:line_to(vv2[1], vv2[2])
			canvas:line_to(vv3[1], vv3[2])
			canvas:line_to(vv4[1], vv4[2])
			canvas:line_to(vv1[1], vv1[2])
			canvas:fill()
		end
		
	elseif(self.drawmode == CGL.POLYGON and #vertices > 0) then
		canvas.fill_color = self.fill_color
		local vv = self.Project(vertices[1])
		canvas:move_to(vv[1], vv[2])
		for i=2, #vertices do
			local vv1 = self.Project(vertices[i])
			canvas:line_to(vv1[1], vv1[2])
		end
		canvas:fill()
	end
	self.vertices = {}
	self.colors = {}
	self.drawing = false
end

function M:PolygonMode()
end

--- Save the current surface to a file
-- @param filename (usually a PDF or a PNG file)
function M:Save(filename)
	self.canvas:close()
end