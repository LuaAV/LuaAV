local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")

local font = require("font")
local Font = require("font.Font")

local Array = require("Array")

local floor = math.floor

local ctx = "Test Font"
win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = {0.8, 0.2, 0.2, 1}


local font = Font(LuaAV.findfile("VeraMono.ttf"), 10)
local tex = Texture(ctx)
tex:fromarray(font:ascii_chars())


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		else
			local c = font:char(k)
			print(k, c.width, c.y_offset)
		end
	end
end

local text = "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*() Hi There!"


function draw_char(pos, idx)
--	idx = 256 - idx
	local xidx = idx % 16
	local yidx = floor(idx/16)
	local tc_x = (xidx*12+1)/(12*16)
	local tc_y = (yidx*12+1)/(12*16)
	local tc_d = 10/(12*16)
	
--	print(idx)
	local c = font:char(idx)

	gl.PushMatrix()
	gl.Translate(pos)
	gl.Color(1, 1, 1, 1)
	tex:bind()
	gl.Begin(GL.QUADS)
		gl.TexCoord(tc_x, tc_y) gl.Vertex(0, 10, 0)
		gl.TexCoord(tc_x+tc_d, tc_y) gl.Vertex(10, 10, 0)
		gl.TexCoord(tc_x+tc_d, tc_y+tc_d) gl.Vertex(10, 0, 0)
		gl.TexCoord(tc_x, tc_y+tc_d) gl.Vertex(0, 0, 0)
	gl.End()
	tex:unbind()
	gl.PopMatrix()
	
	return c.width
end

function draw_char(pos, idx)
	local c = font:char(idx)
	local s = 1
	local w = 12*s
	local h = (c.y_offset+2)*s
	local ww = 12*16
	local hh = 12*16
--	local idx = 97
	local xidx = idx % 16
	local yidx = floor(idx/16)
	local tc_x = (xidx*12)/ww
	local tc_y = (yidx*12)/hh
	local tc_w = (c.width+2)/ww
	local tc_h = (c.y_offset+2)/hh
	
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.PushMatrix()
	gl.Translate(pos)
	tex:bind()
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		gl.TexCoord(tc_x, tc_y) gl.Vertex(0, h, 0)
		gl.TexCoord(tc_x+tc_w, tc_y) gl.Vertex((2+c.width)*s, h, 0)
		gl.TexCoord(tc_x+tc_w, tc_y+tc_h) gl.Vertex((2+c.width)*s, 0, 0)
		gl.TexCoord(tc_x, tc_y+tc_h) gl.Vertex(0, 0, 0)
	gl.End()
	tex:unbind()
	gl.PopMatrix()
	
	return c.width
end

function draw_char(pos, idx)
	local c = font:char(idx)
	local s = 1
	local margin = 1
	local size = 10
	local w = (size+2*margin)*s
	local h = (size+2*margin)*s
	local ww = (size+2*margin)*16
	local hh = (size+2*margin)*16
	local xidx = idx % 16
	local yidx = floor(idx/16)
	local tc_x = (xidx*(size+2*margin))/ww
	local tc_y = (yidx*(size+2*margin))/hh
	local tc_w = (size+2*margin)/ww
	local tc_h = (size+2*margin)/hh
	local yy = c.y_offset
	
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.PushMatrix()
	gl.Translate(pos)
	tex:bind()
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		gl.TexCoord(tc_x, tc_y) gl.Vertex(0, h+yy, 0)
		gl.TexCoord(tc_x+tc_w, tc_y) gl.Vertex(w*s, h+yy, 0)
		gl.TexCoord(tc_x+tc_w, tc_y+tc_h) gl.Vertex(w*s, yy, 0)
		gl.TexCoord(tc_x, tc_y+tc_h) gl.Vertex(0, yy, 0)
	gl.End()
	tex:unbind()
	gl.PopMatrix()
	
	return c.width
end

function win:draw()
	local dim = self.dim
	
	gl.MatrixMode(GL.PROJECTION)
		gl.LoadIdentity()
		gl.Ortho(0, dim[1], 0, dim[2], -100, 100)
	gl.MatrixMode(GL.MODELVIEW)

	gl.Disable(GL.DEPTH_TEST)
	local pen = {10+0., 10}
	for i=1, text:len() do
		pen[1] = pen[1] + draw_char(pen, text:sub(i, i):byte())
	end
	
--	print(tex.dim[1], 12*16)

	--[[
	local i = 4
	local s = 2
	local idx = 103 --text:sub(i, i):byte()
	local c = font:char(idx)
	local w = 12*s
	local h = 12*s
	local ww = 12*16
	local hh = 12*16
	local xidx = idx % 16
	local yidx = floor(idx/16)
	local tc_x = (xidx*12)/ww
	local tc_y = (yidx*12)/hh
	local tc_w = (c.width+2)/ww
	local tc_h = (12)/hh
	local yy = -c.y_offset
	
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.PushMatrix()
	gl.Translate(250, 200)
	tex:bind()
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		gl.TexCoord(tc_x, tc_y) gl.Vertex(0, h, 0)
		gl.TexCoord(tc_x+tc_w, tc_y) gl.Vertex((2+c.width)*s, h, 0)
		gl.TexCoord(tc_x+tc_w, tc_y+tc_h) gl.Vertex((2+c.width)*s, yy, 0)
		gl.TexCoord(tc_x, tc_y+tc_h) gl.Vertex(0, yy, 0)
	gl.End()
	tex:unbind()
	gl.PopMatrix()
	--]]
	--[=[
	local s = 1
	gl.PushMatrix()
	gl.Translate(10, 0, 0)
	gl.Disable(GL.DEPTH_TEST)
	tex:bind()
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		gl.TexCoord(0, 0) gl.Vertex(0, 12*16*s, 0)
		gl.TexCoord(1, 0) gl.Vertex(12*16*s, 12*16*s, 0)
		gl.TexCoord(1, 1) gl.Vertex(12*16*s, 0, 0)
		gl.TexCoord(0, 1) gl.Vertex(0, 0, 0)
	gl.End()
	tex:unbind()
	
	--[[
	gl.Color(1, 0, 0, 1)
	gl.Begin(GL.LINES)
	for xidx=0, 15 do
		local tc_x = (xidx*12+1)*s
		gl.Vertex(tc_x, 0, 0)
		gl.Vertex(tc_x, dim[2], 0)
	end
	gl.End()
	--]]
	gl.PopMatrix()
	--]=]
end