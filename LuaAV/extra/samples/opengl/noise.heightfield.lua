local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Texture = require("opengl.Texture")
local Mesh = require("opengl.Mesh")
local Slab = require("opengl.Slab")

local ctx  = "Noise Field"
win = Window(ctx, 0, 0, 720, 480)
win.clearcolor = {0.91, 0.91, 0.91, 0}


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

local pnoise2d = Slab{
	ctx = ctx,
	file = LuaAV.findfile("gn.perlin.noise.2d.shl")
}
pnoise2d:texture().type = GL.FLOAT
pnoise2d:texture().dim = {700, 700}

local heightfield = Slab{
	ctx = ctx,
	file = LuaAV.findfile("gn.heightmap.shl"),
	param = {
		scale = {4., 4.},
		offset = {-2., -2.},
	}
}
heightfield:texture().format = GL.RGB
heightfield:texture().type = pnoise2d:texture().type
heightfield:texture().dim = pnoise2d:texture().dim

local mesh = Mesh{
	ctx = ctx,
	primitive = GL.LINES,
}


function win:draw()
	gl.Enable(GL.BLEND)
	gl.Disable(GL.DEPTH_TEST)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	gl.PointSize(1)
	
	pnoise2d:param("offset", now()*2, 0)
	pnoise2d:param("scale", now()*10, 4)
	--pnoise2d:param("scale", now()*10, now()*10)
	pnoise2d:draw()
	heightfield:draw(pnoise2d)
	
	heightfield:texture():toarray()
	mesh:vertex(heightfield:texture():array())
	
	--[[
	local tex = heightfield:texture()
	gl.Color(1, 1, 1, 1)
	tex:bind()
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
	--]]
	
	gl.Color(0, 0, 0, 0.8)
	mesh:draw()
end