local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local Mesh = require("opengl.Mesh")
local model = require("opengl.model")
local Image = require("Image")

local ctx = "Test Model"

win = Window(ctx, 0, 0, 512, 512)
win.clearcolor = {0.2, 0.2, 0.2, 1}

local groups, materials = model.read(ctx, LuaAV.findfile("starfish.obj"))

-- supports multiple groups:
for i, group in ipairs(groups) do
	print(i, group.mesh, group.material)
end
-- materials are available too:
for k, mtl in pairs(materials) do
	print("material name:", k)
	print("diffuse:", unpack(mtl.diffuse))
	print("ambient:", unpack(mtl.ambient))
	print("specular:", unpack(mtl.specular))
	print("shininess:", mtl.shininess)
end

local starfish = groups[1].mesh

local img = Image(LuaAV.findfile("glow.jpg"))
local tex = Texture(ctx)
tex:fromarray(img:array())

function win:draw()
	gl.PushMatrix()
		local s = 0.05
		gl.Scale(s, s, s)
		gl.Rotate(now()*10, 1, 0, 0)
		tex:bind()
		starfish:draw()
		tex:unbind()
	gl.PopMatrix()
end