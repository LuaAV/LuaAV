local cgl_util = require("cgl.util")

gl = require("opengl")
GL = gl

local ctx = "Test Cairo"
win = Window(ctx, 140, -1000, 800, 800)
win.clearcolor = {1, 1, 1, 0}

local
function save_to_file()
	cgl_util.opengl_to_cgl(
		script.path.."/renders/test.pdf",
		win,
		win.draw,
		_G
	)
end

function win:key(e, k)
	if(e == "down") then
		if(k == 32) then
			save_to_file()
		end
	end
end


function win:draw()
	gl.LineWidth(5)
	gl.Begin(GL.LINES)
		gl.Color(1, 0, 0.5, 0)
		gl.Vertex(-1, 0, 0)
		gl.Color(0, 0.5, 1, 1)
		gl.Vertex(1, 0, 0)
	gl.End()
	
	gl.Begin(GL.LINE_STRIP)
		gl.Color(0, 1, 0.5, 0)
		gl.Vertex(-1, 0.3, 0)
		gl.Color(1, 0., 0.5, 1)
		gl.Vertex(1, 0.3, 0)
		gl.Color(0, 0.5, 1, 1)
		gl.Vertex(0, 1, 0)
	gl.End()
end