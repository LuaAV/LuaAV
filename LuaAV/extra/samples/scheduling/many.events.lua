local gl = require("opengl")
local GL = gl

local color = require("color")
local HSLtoRGB = color.HSLtoRGB
HSLtoRGB(1, 1, 1)
local cos = math.cos
local sin = math.sin
local random = math.random

local ctx = "Many Events"
win = nil
win = win or Window(ctx, 0, 0, 512, 512)
win.clearcolor = {0.1, 0.1, 0.1, 1}
script.live = true

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:mouse(e, btn, x, y)
	if(e == "drag") then
		go("draw", function()
			local dur = 9.5
			local t = now()
			while(now() < t+dur) do
				gl.LineWidth(random(5)+1)
				gl.Color(HSLtoRGB(t*0.01+y, 0.5, 0.5, 0.12))
				gl.Begin(GL.LINES)
					gl.Vertex(-16/9, cos(t+y*0.1), 0)
					gl.Vertex(16/9, sin(now()+t+y+math.pi), 0)
				gl.End()
				wait("draw")
			end
		end)
	end
end

function win:create()
	---[[
	go("draw", function()
		while(true) do
			gl.Color(1, 0, 0.5, 1)
			gl.Begin(GL.LINES)
				gl.Vertex(-1, 0, 0)
				gl.Vertex(1, 0, 0)
			gl.End()
			wait("draw")
		end
	end)
	
	go("draw", function()
		while(true) do
			gl.Color(0, 0.5, 1, 1)
			gl.Begin(GL.LINES)
				gl.Vertex(0, -1, 0)
				gl.Vertex(0, 1, 0)
			gl.End()
			wait("draw")
		end
	end)
	--]]
end

function win:draw()
	gl.Disable(GL.DEPTH_TEST)
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA)
	event("draw")
end