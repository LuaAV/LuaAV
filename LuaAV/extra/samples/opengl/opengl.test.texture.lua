local gl = require("opengl")
local GL = gl

local sketch = require("opengl.sketch")
local Texture = require("opengl.Texture")

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

local img = Image(LuaAV.findfile("LuaAV.96.png"))
local tex = Texture(ctx)
tex:fromarray(img:array())

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

	gl.Rotate(now()*20, 1, cos(now()), 0)
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