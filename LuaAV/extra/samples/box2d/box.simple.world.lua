local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")

local vec3 = require("space.vec3")
local box2d = require("box2d")
local World = require("box2d.World")

local ctx = "Box2D Simple World"
win = Window(ctx, 0, 0, 720, 405)

local dt = 1/33
world = World(0, -10)


box1 = world:body{
	type = box2d.staticBody,
	position = {0, -10}
}
box1:box{100, 20}

box2 = world:body{
	position = {0, 4.}
}
box2:box{
	size = {2, 2},
	density = 1.,
	friction = 0.3,
}

---[[
c1 = world:body{
	position = {10, 7.}
}
c1:circle{
	radius = 2.,
	density = 1.,
	friction = 0.3,
}
--]]

---[[
djoint = world:revoluteJoint(
	box2, c1, 
	box2.position, c1.position
)
--]]

function world:PostSolve(c)
	print(c.bodyA, c.bodyB)
end

go(function()
	while(true) do
		wait(dt)
		world:step(dt, 6, 2)
	end
end)

go(0.25, function()
	c1:applyForce({0, 1500}, c1.position)
end)

function win:draw()
	gl.Scale(0.05, 0.05, 0.05)
	world:draw()
end