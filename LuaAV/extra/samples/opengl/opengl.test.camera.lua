local gl = require("opengl")
local GL = gl

local Camera = require("glutils.navcam")

local ctx = "Test Camera"
win = Window(ctx, 0, 0, 512, 512)

local cam = Camera()

function win:key(e, k)
	cam:key(self, e, k)
	
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:resize()
	cam:resize(self)
end

function win:draw()
	cam:step()
	cam:enter()
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.LINES)
		for i=-10, 10 do
			gl.Vertex(i, -10, 0)
			gl.Vertex(i, 10, 0)
			
			gl.Vertex(-10, i, 0)
			gl.Vertex(10, i, 0)
			
			gl.Vertex(0, -10, i)
			gl.Vertex(0, 10, i)
			
			gl.Vertex(0, i, -10)
			gl.Vertex(0, i, 10)
		end
		gl.End()
	cam:leave()
end