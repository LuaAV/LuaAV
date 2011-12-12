local gl = require("opengl")
local GL = gl
----------------------------------------
local space = require("space")
local vec3 = require("space.vec3")
----------------------------------------
local cosm = require("cosm")
local Camera = cosm.Camera
local Space = cosm.Space
----------------------------------------
local ctx = "Cosm"
win = Window(ctx, 0, 0, 512, 512)
----------------------------------------


local
function random20()
	return math.random()*20-10
end

local
function random10()
	return math.random()*10-5
end

local cam = Camera{}

local space = Space{
	ctx = ctx,
	radius = 10
}

local pts = {}
for i=1, 300 do
	local pp = space:point{random20(), random20(), random20()}
	pp.collide = true
	pp.vel = {0, 0, 0.05}
	pp.turn = {5, 0, 0}
	
	pts[i] = pp
end

function win:key(event, key)
	cam:key(self, event, key)
	
	if(event == "down" and key == 27) then
		self.fullscreen = not self.fullscreen
	end
end


function win:draw()
	cam:update()
	cam:enter()
	
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
		gl.Color(1, 1, 1, 0.2)
		space:draw()
	
	gl.PointSize(4)
	for i, p in ipairs(pts) do
		p:update()
		p:draw()
		p.turn = {random10(), random10(), random10()}
	end
	
	cam:leave()
end