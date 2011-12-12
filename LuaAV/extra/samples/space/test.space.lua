local space = require("space")

local vec3 = require("space.vec3")


local v1 = {1, 0, 0}
local v2 = {0, 1, 0}
print(unpack(vec3.add(v1, v2)))


local quat = require("space.quat")

local q1 = {1, 0, 0, 0}
local q2 = quat.from_axis_angle(45, 0, 1, 0)
print(unpack(q2))


local Space = require("space.Space")
local space = Space()
print(space)
local nav = space:point{0, 0, 0}
print(nav)
print(nav.pos)
nav.radius = 10
print(nav.radius)

