local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local Mesh = require("opengl.Mesh")
local Shader = require("opengl.Shader")

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

local text = "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*() Hi There!"

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		else
			local c = font:char(k)
			text = text:sub(2)..string.char(k)
		end
	end
end

local vertex = Array{
	components = 3, 
	type = Array.Float32, 
	dim = {text:len()*4},
}

local texcoord = Array{
	components = 2, 
	type = Array.Float32, 
	dim = {text:len()*4},
}

local mesh = Mesh{
	ctx = ctx,
	primitive = GL.QUADS,
}

local viz = Shader{
	ctx = ctx,
	file = LuaAV.findfile("viz.texcoords.shl")
}

function win:draw()
	local dim = self.dim
	
	gl.MatrixMode(GL.PROJECTION)
		gl.LoadIdentity()
		gl.Ortho(0, dim[1], dim[2], 0, -100, 100)
	gl.MatrixMode(GL.MODELVIEW)

	gl.Disable(GL.DEPTH_TEST)
	
	font:render(text, vertex, texcoord)
	mesh:texcoord(texcoord)
	mesh:vertex(vertex)

	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.Color(1, 1, 1, 1)
	gl.Translate(10, 10, 0)
	tex:bind()
		mesh:draw()
	tex:unbind()
end