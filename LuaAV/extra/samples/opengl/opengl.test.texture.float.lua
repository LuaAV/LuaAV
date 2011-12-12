local gl = require("opengl")
local GL = gl

local sketch = require("opengl.sketch")
local Texture = require("opengl.Texture")

local Array = require("Array")
local Image = require("Image")

local cos = math.cos

local ctx = "OpenGL CTX Test"
win = Window(ctx, 0, 0, 512, 512)


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

local img = Array{
	components = 1,
	type = Array.Float32,
	dim = {4, 4},
}
for i=0, 3 do
for j=0, 3 do
	img:setcell(i, j, ((i+j)%2))
end
end
local tex = Texture(ctx)
tex:fromarray(img)

print("GL.TEXTURE_2D", GL.TEXTURE_2D)
print(tex.target)
print(tex.type)
print(unpack(tex.dim))
print(tex.format)
go(0.1, function()
	print(tex.texid)
end)

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)

	gl.Color(1, 1, 1, 1)
	tex:bind()
	--[[
	print(
		gl.GetTexLevelParameter(GL.TEXTURE_2D, 0, GL.TEXTURE_WIDTH),
		gl.GetTexLevelParameter(GL.TEXTURE_2D, 0, GL.TEXTURE_HEIGHT)
	)
	--]]
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()
end