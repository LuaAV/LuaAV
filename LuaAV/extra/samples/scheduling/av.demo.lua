local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")

local Def = require "audio.Def"
local P, SinOsc, Env, Lag, Pan2 = Def.P, Def.SinOsc, Def.Env, Def.Lag, Def.Pan2

local pi = math.pi
local cos = math.cos
local sin = math.sin
local pow = math.pow
local sqrt = math.sqrt
local hypot = function(a, b)
	return sqrt(a*a+b*b)
end

local ctx = "AV Canvas"
win = Window(ctx, 0, 0, 512, 512)


local ss = Def{
	freq1 = 400, 
	freq2 = 10,
	amp = 0.5,
	dur = 0.5,
	
	Pan2{ 
		Env{ "dur" } * Lag{ P"amp", 0.9 } * SinOsc{ P"freq1" * SinOsc{ P"freq2"} },
		pan=SinOsc{ freq=10 } 
	}, 

}


function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

local
function canvas_pt(x, y, dim)
	local dim = dim
	local aspect = dim[1]/dim[2]
	
	return aspect*(x/dim[1]*2-1), 1-y/dim[2]*2
end

function win:mouse(e, btn, x, y)
	x, y = canvas_pt(x, y, self.dim)
	
	if(e == "down") then
	
	elseif(e == "drag") then
		go("draw", function()
			local t = now()
			local big = self.shift
			while(now() < t+2) do
				gl.PointSize(big and 10 or 1)
				gl.Color(1, 1, 1, 1)
				gl.Begin(GL.POINTS)
					local xx = pow((now()-t)/1.5, 4)+x
					gl.Vertex(xx, y)
				gl.End()
				
				ss{
					freq1 = (big and 440 or 220)*2^(2*(xx+1.2)),
					freq2 = (y+1)*100,
					amp = big and 0.1 or 0.03,
					dur = 0.02,
				}	
				wait("draw")
			end
		end)
	elseif(e == "up") then
	
	end
end

function win:draw()
	local dim = self.dim
	local aspect = dim[1]/dim[2]
	sketch.enter_ortho(-aspect, -1, 2*aspect, 2)
		event("draw")
	sketch.leave_ortho()
end