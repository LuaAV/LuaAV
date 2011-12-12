local gl = require("opengl")
local GL = gl
local Shader = require("opengl.Shader")

local cos = math.cos

local ctx = "Shader Test"
win = Window(ctx, 0, 0, 512, 512)

local shader = Shader{
	ctx = ctx,
	file = LuaAV.findfile("viz.texcoords.shl"),
	param = {
		scale = 2,
	}
}

print(shader.file)
for p in shader:params() do
	print(p.name, p.type, unpack(p.default))
end

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)

	gl.Color(1, 1, 1, 1)
	--shader:param("scale", cos(now())%1)
	shader:bind()
	gl.Begin(GL.QUADS)
		gl.TexCoord(0, 0)
		gl.Vertex(-1, 1, 0)
		gl.TexCoord(1, 0)
		gl.Vertex(1, 1, 0)
		gl.TexCoord(1, 1)
		gl.Vertex(1, -1, 0)
		gl.TexCoord(0, 1)
		gl.Vertex(-1, -1, 0)
	gl.End()
	shader:unbind()
end