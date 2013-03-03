#!av

local gl = require "gl"
local win = require "window"

win.title = "av"

function win:draw(w, h, dt)
	--print("fps", 1/dt)
	
	a = a or 0
	gl.Viewport(0, 0, self.width, self.height)
	gl.ClearColor(0, 0.1, 0.2, 0)
	gl.Clear(gl.COLOR_BUFFER_BIT)
	
	gl.MatrixMode(gl.PROJECTION)
	gl.LoadIdentity()
	
	gl.MatrixMode( gl.MODELVIEW )
	gl.LoadIdentity()
	
	gl.Begin(gl.LINE_STRIP)
	for i = 1, 30 do
		gl.Color3f(i/30, i/30, 0)
		gl.Vertex3f(math.sin((a+i) * 0.1), math.cos((a+i) * 0.1), 0)
	end
	gl.End() 
	a = a + 0.1

	
end

function win:key(e, k)
	
end