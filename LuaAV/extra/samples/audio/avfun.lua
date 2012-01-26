local Def = require "audio.Def"
Def.globalize()

--script.live = true

local ctx = "CREATE"

local gl = require("opengl")
local GL = gl
local sketch = require "opengl.sketch"

require "color"
local HSLtoRGB = color.HSLtoRGB

win = win or Window(ctx, 0, 0, 720, 480)

simple = Def{
	freq = 100,
	amp = 0.3,
	dur = 1,
	pan = 0,
	hold = 1,
	
	Pan2{
		Env{ "dur", "hold" } 
		* Lag{ Tri{ "freq" }, 0.9 } 
		* Lag{ "amp" } 
		* 0.2,
		"pan",
	}
}

function y2freq(y)
	local f = (1 - (y/win.dim[2]))
	return 1000 * f * f
end

function x2pan(x)
	local f = 2*x/win.dim[1]
	return f-1
end

local function srandom() return math.random() * 2 - 1 end

function squiggle(x, y, dx, dy) 
	local h, s, l = 0, 0.5, 0.5
	local x0, y0 = x, y
	local x1, y1 = x, y
	local a = 0.2
	local synth = simple{
		amp = a,
		freq = y2freq(y0),
		pan = x2pan(x0),
	}
	while a > 0.01 do
		a = a * 0.97
		synth.amp = a
		synth.freq = y2freq(y0)
		synth.pan = x2pan(x0)
		
		gl.Color(HSLtoRGB(h, s, l, a*4))
		gl.Vertex(x0, y0, 0)
		gl.Vertex(x1, y1, 0)
		x0, y0 = x, y
		x1, y1 = x1+dx + srandom(), y1+dy + srandom()
		wait("draw")
	end
	synth.hold = 0
end

function squaggle(x, y, dx, dy) 
	local h, s, l = 0.5, 0.5, 0.5
	local x0, y0 = x, y
	local x1, y1 = x, y
	local a = 1
	local synth = simple{
		amp = a,
		freq = y2freq(y0*2),
		pan = x2pan(x0),
	}
	while a > 0.01 do
		a = a * 0.97
		h = h + dx*0.0001
		synth.amp = a
		synth.freq = y2freq(y1*2)
		synth.pan = x2pan(x0)
		
		gl.Color(HSLtoRGB(h, s, l, a*a*4))
		gl.Vertex(x0, y0, 0)
		gl.Vertex(x1, y1, 0)
		x0, y0 = x0+0.1*(x1-x0), y0+0.1*(y1-y0)
		x1, y1 = x1+dx*0.2+srandom()*1.5, y1+dy*0.2+srandom()*1.5
		wait("draw")
	end
	synth.hold = 0
end

local x1, y1 = win.dim[1]/2, win.dim[2]/2
local dx, dy = 0, 0
function win:mouse(e, b, x, y)
	if e=="drag" then
		if math.random() < 0.3  then
			go("draw", squaggle, x, y, dx + srandom(), dy + srandom())
		end
	elseif e=="up" or e=="down" then
		for i = 1, 15 do
			go("draw", squiggle, x, y, dx + srandom(), dy + srandom())
		end
	end
	dx, dy = x-x1, y-y1
	x1, y1 = x, y
	
	print(x, y)
end

function win:draw()
	sketch.enter_ortho(self.dim)
	gl.Enable(gl.BLEND)
	gl.BlendFunc(gl.SRC_ALPHA, gl.ONE)
	gl.Begin(gl.LINES)

	event("draw")
	
	gl.End()
	sketch.leave_ortho()
end

function win:key(e, k)
	if e == "down" and k == 27 then
		self.fullscreen = not self.fullscreen
	end
end
