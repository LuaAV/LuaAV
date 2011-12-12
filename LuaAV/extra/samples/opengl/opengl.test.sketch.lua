local gl = require("opengl")
local GL = gl
local Texture = require("opengl.Texture")
local sketch = require("opengl.sketch")

local Label = require("Label")

local video = require("video")
local VideoCamera = video.VideoCamera

local ctx = "Test OpenGL Sketch"
win = Window(ctx, 0, 0, 512, 512)

local cam = VideoCamera()
cam:open(720, 480)
local tex = Texture(ctx)

local label = Label{
	ctx = ctx,
	size = 100,
}

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

function win:draw()
	tex:fromarray(cam:array())
	
	---[[
	tex:bind()
	gl.Color(1, 1, 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad(0.5, 0.5)
	gl.End()
	tex:unbind()
	--]]
	
	gl.Color(0.5, 0.5, 0.5, 1)
	gl.Begin(GL.LINE_LOOP)
		sketch.quad(-0.5, -0.5, 1, 1)
	gl.End()
	
	sketch.enter_ortho()
		gl.Color(1, 0, 0.5, 1)
		gl.Begin(GL.LINES)
			gl.Vertex(-1, 0, 0)
			gl.Vertex(1, 1, 0)
		gl.End()
		
		--[[
		tex:bind()
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.QUADS)
			sketch.quad()
		gl.End()
		tex:unbind()
		--]]
	sketch.leave_ortho()
	
	local dim = self.dim
	sketch.enter_ortho(dim[1], dim[2])
		gl.Color(0, 0.5, 1, 1)
		gl.Begin(GL.LINES)
			gl.Vertex(0, 0, 0)
			gl.Vertex(dim[1], dim[2], 0)
		gl.End()
		
		---[[
		tex:bind()
		gl.Color(1, 1, 1, 1)
		gl.Begin(GL.QUADS)
			sketch.pixquad(dim[1]*0.5, dim[2])
		gl.End()
		tex:unbind()
		--]]
		
		gl.Disable(GL.DEPTH_TEST)
		label:draw({10, 100}, tostring(now()))
	sketch.leave_ortho()
end