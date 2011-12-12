local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local Image = require("Image")


local ctx = "Test OpenGL Textures"
win = Window(ctx, 200, -1000, 1200, 800)

local img = Image(LuaAV.findfile("LuaAV.96.png"))
local tex = Texture(ctx)
tex:fromarray(img:array())
tex.bordercolor = {1, 0, 0, 1}


local
function show(tex, pos, scale)
	local aspect = tex.dim[1]/tex.dim[2]*scale[1]/scale[2]*win.dim[1]/win.dim[2]
	local ts = 4
	gl.PushMatrix()
		gl.Translate(pos)
		gl.Scale(scale)
		gl.Scale(0.95, 0.95, 0.95)
		tex:bind()
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.QUADS)
			gl.TexCoord(0, 0)			gl.Vertex(0, 1)
			gl.TexCoord(ts*aspect, 0)	gl.Vertex(1, 1)
			gl.TexCoord(ts*aspect, ts)	gl.Vertex(1, 0)
			gl.TexCoord(0, ts)			gl.Vertex(0, 0)	
		gl.End()
		tex:unbind()
	gl.PopMatrix()
end


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)
	
	sketch.enter_ortho()
		---[[
		tex.wrap = GL.REPEAT
		show(tex, {-1, 0, 0}, {1/2, 1, 1})
		
		tex.wrap = GL.MIRRORED_REPEAT
		show(tex, {-0.5, 0, 0}, {1/2, 1, 1})
		
		tex.wrap = GL.CLAMP
		show(tex, {0, 0, 0}, {1/2, 1, 1})
		
		tex.wrap = GL.CLAMP_TO_EDGE
		show(tex, {0.5, 0, 0}, {1/2, 1, 1})
		--]]
		
		tex.wrap = GL.CLAMP_TO_BORDER
		show(tex, {-1, -1, 0}, {1/2, 1, 1})
	sketch.leave_ortho()
end
