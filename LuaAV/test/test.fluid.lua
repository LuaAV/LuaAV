local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local Slab = require("opengl.Slab")
local sketch = require("opengl.sketch")

local lattice = require("lattice")
local Lattice = lattice.Lattice

local fluid = require("fluid")


local random = math.random
local max = math.max
local min = math.min
local sqrt = math.sqrt

local ctx = "Fluid and Structured"
win = Window(ctx, 200, -1000, 1200, 800)

local dim = {64, 64}
local field1 = Lattice(3, lattice.Float32, dim)
local field2 = Lattice(3, lattice.Float32, dim)
local tex = Texture(ctx)
tex.minfilter = GL.NEAREST
tex.magfilter = GL.NEAREST


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

go(function()
	while(true) do
		-- field1 is current (src)
	
		-- do something to it
		local v = field1:getcell(16, 16)
		v[1] = v[1] + 0.5
		field1:setcell(16, 16, v)
		-- process field1 with result in field2
		fluid.diffuse(field1, field2, 0.1, 4)
		-- make field2 current
		field1, field2 = field2, field1
		
		tex:frommatrix(field1)
		wait(1/10)
	end
end)

function win:draw()
	sketch.enter_ortho()
		tex:bind()
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex:unbind()
	sketch.leave_ortho()
end