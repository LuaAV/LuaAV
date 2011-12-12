local assert, pairs, tostring, type = assert, pairs, tostring, type
local print = print
local getmetatable = getmetatable
local setmetatable = setmetatable
local getfenv = getfenv
----------------------------------------------------------
local string = require 'string'
----------------------------------------------------------
local gl = require("opengl")
local GL = gl
local glu = require("opengl.glu")
local sketch = require("opengl.sketch")
local enter_ortho = sketch.enter_ortho
local leave_ortho = sketch.leave_ortho
local quad = sketch.quad
----------------------------------------------------------
local space = require("space")
local vec3 = require("space.vec3")
local Space = require("space.Space")
local Nav = require("space.Nav")
----------------------------------------------------------
local Navcam = require("glutils.navcam")
local Cubegrid = require("glutils.cubegrid")
----------------------------------------------------------
module 'cosm'
----------------------------------------------------------

local M = getfenv()

M.Camera = Navcam
M.Boundary = Cubegrid


-----------------------------------------------------------
-- Nav
M.Nav = Nav
function Nav:draw(c)
	local p = self.pos
	local z = self.z

	gl.Color(1, 0, 0, 1)
	gl.Begin(GL.POINTS)
		gl.Vertex(p)
	gl.End()
	
	gl.Color(c or {0, 1, 1, 1})
	gl.Begin(GL.LINES)
		gl.Vertex(p)
		gl.Vertex(vec3.add(p, vec3.scale(z, 0.15)))
	gl.End()
end

-----------------------------------------------------------
-- Space

M.Space = Space

local Space_meta = getmetatable(Space)
local space_constructor = Space_meta.__call

function Space_meta:__call(init)
	assert(init.ctx, "Space.ctx is nil")
	init.radius = init.radius or 10
	local m = space_constructor(self, init.radius)
	
	m.boundary = Cubegrid{
		ctx = init.ctx,
		size = 2*init.radius,
	}
	
	return m
end

function Space:draw()
	self.boundary:draw()
end

-----------------------------------------------------------