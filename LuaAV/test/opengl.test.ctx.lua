local gl = require("opengl")
local GL = gl

local ctx = "OpenGL CTX Test"
win = Window(ctx, 0, 0, 512, 512)


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end


function win:draw()
	gl.Rotate(now()*20, 0, 0, 1)
	gl.Begin(GL.LINES)
		gl.Vertex(-1, 0, 0)
		gl.Vertex(1, 0, 0)
	gl.End()
end