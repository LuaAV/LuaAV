local gl = require("opengl")
local GL = gl
local glu = require("opengl.glu")
-------------------------------------
local Texture = require("opengl.Texture")
local Mesh = require("opengl.Mesh")
-------------------------------------
local font = require("font")
local Font = require("font.Font")
-------------------------------------
local Array = require("Array")
-------------------------------------
local min = math.min
local floor = math.floor
local ceil = math.ceil
local sub = string.sub
local find = string.find
-------------------------------------
local setmetatable = setmetatable
local getfenv = getfenv
local assert = assert
local print = print
local type = type
-------------------------------------
local default_fontfile = LuaAV.findfile("VeraMono.ttf")
local drawbg = false

--- Label drawing in an arbitrary scene
-- @name Label
module(...)

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M


M.CENTER = "Center"
M.LEFT = "Left"
M.RIGHT = "Right"


--- Constructor
-- @param init Constructor properties
-- @param init.ctx The label's context name
-- @param init.fontfile The file to load the font from
-- @param init.size The font size (default 8)
-- @param init.margin The x/y margin around the font in pixels (default {0, 0})
-- @param init.alignment The text alignment (Label.LEFT, Label.CENTER (default), Label.RIGHT)
-- @param init.bg draws a background (default not draw)
-- @param init.bgcolor sets background color (default is black)
-- @param init.color sets color (default is white)
-- @name Label

function C:__call(init)
	if(type(init) == "string") then
		init = {
			ctx = init
		}
	end

	assert(init.ctx)
	init.fontfile = init.fontfile or default_fontfile
	init.size = init.size or 8
	init.margin = init.margin or {0, 0}
	init.alignment = init.alignment or M.CENTER
	init.bg = init.bg or drawbg
	init.bgcolor = init.bgcolor or {0.0, 0.0, 0.0}
	init.color = init.color or {1.0, 1.0, 1.0}
	init.maxwidth = init.maxwidth or 0
	init.numlines = 1
	
	local m = setmetatable(init, M)
	if(not m.font) then
		m.font = assert(Font(init.fontfile, init.size))
	end
	m.tex = Texture(init.ctx)
	m.tex:fromarray(m.font:ascii_chars())
	m.mesh = Mesh{
		ctx = init.ctx,
		primitive = GL.QUADS,
	}
	m.vertex = Array{
		components = 3,
		type = Array.Float32,
		dim = {12}
	}
	m.texcoord = Array{
		components = 2,
		type = Array.Float32,
		dim = {12}
	}
	
	return m
end

--- Draw some text
-- @param pos The position to draw the text
-- @param text The text to draw
function M:draw(pos, text)
	local len = text:len()
	self.vertex.dim = {len*4}
	self.texcoord.dim = {len*4}
	
	self.font:render(text, self.vertex, self.texcoord)
	self.mesh:vertex(self.vertex)
	self.mesh:texcoord(self.texcoord)
	
	local npos = {
		pos[1], 
		pos[2]+self.margin[2]-self.size*0.5
	}
	
	--do multiline text stuff here
	local eff_width = self.font:width(text)
	--print(eff_width)
	if(self.maxwidth ~= 0) then 
		eff_width = max_width 
	end
	
	
	if(self.alignment == M.CENTER) then
		npos[1] = floor(npos[1]-eff_width*0.5)
	elseif(self.alignment == M.LEFT) then
		npos[1] = floor(npos[1]+self.margin[1])
	elseif(self.alignment == M.RIGHT) then
		npos[1] = floor(npos[1]-self.margin[1]-eff_width)
	else
		npos[1] = floor(npos[1])
	end
	npos[2] = floor(npos[2])
	
	
    
	if(self.bg) then
	    gl.Color(self.bgcolor)
		gl.PushMatrix()
		gl.Translate(npos)
		
	    if(self.maxwidth > 0) then
			gl.Begin(GL.POLYGON)
				gl.Vertex(0.0, -self.size, 0.0)
				gl.Vertex(0.0, 0.0, 0.0)
				gl.Vertex(self.maxwidth, 0.0, 0.0)
				gl.Vertex(self.maxwidth, -self.size, 0.0)	
			gl.End()
		else 
			gl.Begin(GL.POLYGON)
				gl.Vertex(0.0, -self.size, 0.0)
				gl.Vertex(0.0, 0.0, 0.0)
				gl.Vertex(self.font:width(text), 0.0, 0.0)
				gl.Vertex(self.font:width(text), -self.size, 0.0)	
			gl.End()
		end
		gl.PopMatrix()
		gl.Color(self.color)
	end
	
    
	local dt = gl.IsEnabled(GL.DEPTH_TEST)
	local bl = gl.IsEnabled(GL.BLEND)
	
	gl.Disable(GL.DEPTH_TEST)
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	
	gl.PushMatrix()
		
		gl.Translate(npos)
		self.tex:bind()
			self.mesh:draw()
		self.tex:unbind()
	gl.PopMatrix()
	
	if(dt == 1) then
		gl.Enable(GL.DEPTH_TEST)
	end
	if(bl == 0) then
		gl.Disable(GL.BLEND)
	end
end

function M:draw_ortho(dim, pos, text)
	gl.MatrixMode(GL.PROJECTION)
		gl.PushMatrix()
		gl.LoadIdentity()
		gl.Ortho(0, dim[1], dim[2], 0, -100, 100)
	
	gl.MatrixMode(GL.MODELVIEW)
		gl.PushMatrix()
		gl.LoadIdentity()

		self:draw(pos, text)
		
	
	gl.MatrixMode(GL.PROJECTION)
		gl.PopMatrix()
	
	gl.MatrixMode(GL.MODELVIEW)
		gl.PopMatrix()
		
end

function M:draw_3d(dim, pos, text)
	local ortho_pos = glu.Project(pos)
	ortho_pos[1] = (ortho_pos[1])
	ortho_pos[2] = (dim[2]-ortho_pos[2])
	self:draw_ortho(dim, ortho_pos, text)
end


function M:draw_3d_multi(dim, pos, text)
	local ortho_pos = glu.Project(pos)
	ortho_pos[1] = (ortho_pos[1] + 5)
	ortho_pos[2] = (dim[2]-ortho_pos[2])
	
	local unitwid = self.font:width("a")
	local unitheight = unitwid*1.6
	
	local linelen = floor(self.maxwidth / unitwid)
    local numlines = ceil (text:len() / linelen)
    
    --print("unit, linelen: ", unitwid, linelen, numlines)
     
	
	--for p=1, numlines do
	while text:len() > 0 do
		local linetext = sub(text, 1, linelen)
		local lastspace = find(linetext, "%s", 1)
		local nxspace = lastspace
		
		local linebreak = find(linetext, "\n", 1)
		
		local remainder
		
		if(linebreak) then
			remainder = sub(linetext, linebreak+1)
			linetext = sub(linetext, 1, linebreak-1)
		    text = remainder .. sub(text, linelen+1)
		    
		else
			while nxspace ~= nil do 
				nxspace = find(linetext, "%s", lastspace+1)
				if(nxspace ~= nil) then lastspace = nxspace end 
			end
			
			remainder = sub(linetext, lastspace+1)
			linetext = sub(linetext, 1, lastspace)
		    text = remainder .. sub(text, linelen+1)
		end
		
		
		
		
		self:draw_ortho(dim, ortho_pos, linetext)
		ortho_pos[2] = ortho_pos[2] + unitheight
	end
	
end