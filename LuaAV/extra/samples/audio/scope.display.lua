local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")

local Label = require("Label")

local Scope = require("audio.Scope")
local Def = require "audio.Def"
Def.globalize()

local format = string.format
local cos = math.cos
local pi = math.pi
local random = math.random


beep = Def{
	freq = 150,
	jump = 300,
	dur = 0.25,
	amp = 0.25,
	
	Env{ "dur" } * Square{
		freq = P"freq"*(
			0.5+0.5*P"jump"*
			SinOsc{
				phase = 0.25, --0.25,
				freq = 1/P"dur",
			}
			--]]
		)
	}*P"amp"
}

local F = 10 --49.333333333333 --34.666666666667 --10

go(0.2, function()
	---[[
	while(now() < 60) do
		--print(format("%.4f", now()).." "..cos(now()*10*pi)*0.5+0.5)
		beep{
			amp = 0.8,
			freq = 10-8*cos(now()*F*pi),
			jump = 100,
			dur = 0.05,
		}
		wait(0.15)
	end
end)


local ctx = "Scope"
win = Window(ctx, 0, -800, 1920, 1000)
win.sync = true
win.fps = 40

local label = Label{
	ctx = ctx,
	size = 20,
	alignment = Label.RIGHT
}

local scope = Scope{ctx = ctx}
scope:enable(true)
scope:settime(0.1)
--scope:primitive(GL.LINE_STRIP)
--print(scope.scale)

local
function grid(x1, x2, y1, y2, nx, ny)
	gl.Begin(GL.LINES)
	local dx = (x2-x1)/nx
	for i=0, nx do
		local x = dx*i+x1
		gl.Vertex(x, y1)
		gl.Vertex(x, y2)
	end
	
	local dy = (y2-y1)/ny
	for i=0, nx do
		local y = dy*i+y1
		gl.Vertex(x1, y)
		gl.Vertex(x2, y)
	end
	gl.End()
end

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	gl.Disable(GL.DEPTH_TEST)
	gl.PointSize(1)
	

	local aspect = self.dim[1]/self.dim[2]
	sketch.enter_ortho()
	gl.Scale(1/aspect, 1, 1)
		gl.Color(1, 1, 1, 0.1)
		grid(-1, 1, -1, 1, 10, 10)
	
		gl.Color(1, 1, 1, 1)
		label:draw_3d(self.dim, {-1.2, 0.2, 0}, format("samplerate: %d", audio.samplerate()))
		label:draw_3d(self.dim, {-1.2, 0.15, 0}, format("nsamples: %d", scope:nsamples()))
		label:draw_3d(self.dim, {-1.2, 0.1, 0}, format("time: %f", scope:time()))
		label:draw_3d(self.dim, {-1.2, 0.05, 0}, format("scope: %d", audio.scope()))
		label:draw_3d(self.dim, {-1.2, 0, 0}, format("now: %.4f", now()))
		
		
		gl.Enable(GL.BLEND)
		scope:draw{1, 1, 1, 0.95}
	sketch.leave_ortho()
end