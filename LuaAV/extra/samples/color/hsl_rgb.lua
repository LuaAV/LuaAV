local gl = require("opengl")
local GL = gl

local color = require("color")
local RGBtoHSL = color.RGBtoHSL
local HSLtoRGB = color.HSLtoRGB

local pi = math.pi
local cos = math.cos
local sin = math.sin

local ctx = "Test RGB/HSL"
win = Window(ctx, 0, 0, 512, 512)

local r1 = 0.3
local r2 = 0.8



function win:draw()
	gl.Enable(GL.BLEND)
	gl.BlendFunc(GL.SRC_ALPHA, GL.ONE)

	gl.PushMatrix()
	gl.Begin(GL.QUAD_STRIP)
	for theta=0, 2.001*pi, pi/40 do
		local h = theta/(2*pi)
		local alpha = cos(theta+now()*2)*0.5+0.51
		gl.Color(HSLtoRGB(h, 1, 0.5, alpha))
		gl.Vertex(r1*cos(theta), r1*sin(theta), 0)
		gl.Vertex(r2*cos(theta), r2*sin(theta), 0)
	end
	gl.End()
	gl.PopMatrix()
end

wait(1)
win= nil
