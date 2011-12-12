local gl = require "opengl"
local Texture = require "opengl.Texture"
local sketch = require "opengl.sketch"

local ctx = "special K"
local win = Window(ctx)
local tex = Texture(ctx)


require("kinect")

for k, v in pairs(kinect) do print("kinect", k, v) end

function win:draw()
	--tex:fromarray(kinect:rgb())
	tex:bind()
	gl.Begin(gl.QUADS)
		sketch.quad()
	gl.End()
	tex:unbind()

end

print(kinect.rgb())
