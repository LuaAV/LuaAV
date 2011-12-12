local gl = require("opengl")
local GL = gl
local Tess = require("opengl.Tesselator")
local Mesh = require("opengl.Mesh")

local Array = require("Array")

local ctx = "OpenGL Tesselator"
win = Window(ctx, 0, 0, 512, 512)

local tess = Tess()
tess:BeginPolygon()

tess:BeginContour()
	tess:Vertex(0, 0)
	tess:Vertex(1, 0)
	tess:Vertex(1, 1)
	tess:Vertex(0.5, 1.5)
	tess:Vertex(0, 1)
tess:EndContour()

tess:BeginContour()
	tess:Vertex(0.25, 0.25)
	tess:Vertex(0.75, 0.25)
	tess:Vertex(0.75, 0.75)
	tess:Vertex(0.25, 0.75)
tess:EndContour()

tess:EndPolygon()

local vertex = Array()
local index = Array()

tess:geometry(vertex, index)

local mesh = Mesh(ctx)
mesh:vertex(vertex)
mesh:index(index)
mesh.primitive = GL.TRIANGLES

function win:draw()
	--gl.PolygonMode(GL.FRONT_AND_BACK, GL.LINE)
		mesh:draw()
end