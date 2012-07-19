-- inspired by FastBreeder...

-- load in the Def module
local Def = require "audio.Def"
-- make its functions global:
Def.globalize()

-- randomize random generator
math.randomseed(os.time())

-- elements of the graph:
local env = Env{ 1 }
local pan = {}
local freq = {}
local osc = {}

local synth = { x=0.5, y=0.5, }

-- potential elements of a graph:
local binops = { 
	"Add", "Sub", "Mul",
	"Max", "Min", "Mean",
}
local unops = { 
	"SinOsc", "Buzz", "Phasor", "Saw", "Square",
	"Abs", "Sign", "Ceil", "Floor",
	"ATan", "Cos", "Sin", "Tanh"
}
local x, y = Port{ "x", 0.5 }, Port{ "y", 0.5 }
local constants = {
	1, 2, 3, 
	1/2, 2/3, 3/2,
	64, 256, 512, 1024,
	-3, -5, -9,
	1/16, 1/32,
	--env, 
	--env * env, 
	--1 - env, 
	noise.Pink{},
	x, y,
	x * x,
	y * y,
}	

-- create a random graph:
function makenode(depth, cache)
	local d = math.random(depth) - 1
	if d == 0 then
		if #cache > 0 then
			return table.remove(cache)
		else
			return constants[math.random(#constants)]
		end
	elseif d == 1 then
		local op = unops[math.random(#unops)]
		local node = Def[op]{makenode(depth-1, cache)}
		cache[#cache+1] = node
		return node
	else
		local op = binops[math.random(#binops)]
		local node = Def[op]{makenode(depth-1, cache), makenode(depth-1, cache)}
		cache[#cache+1] = node
		return node
	end
end

-- generate & play:
go(function()
	while true do
		local cache = { x, y }
		freq = makenode(5, cache)
		pan = makenode(5, cache)
		--osc = env * 0.1 * 
		osc = Pan2{ SinOsc{ 512 * freq }, pan }
		--
		--osc:dump()
		local def = Def{ osc, x = synth.x, y = synth.y }
		synth = def()
		
		wait("key")
	end
end)

---[[
local gl = require "opengl"
local GL = gl
local sketch = require("opengl.sketch")
local Label = require("label")

local ctx = "breeder"
local label = Label{
	ctx = ctx,
	alignment = Label.CENTER,
	size = 10
}
win = Window(ctx)

function line(x0, y0, x1, y1)
	gl.Begin(gl.LINES)
		gl.Vertex(x0, y0, 0)
		gl.Vertex(x1, y1, 0)
	gl.End()
end

function drawosc(o, x, y, w)
	local s = w * 0.9
	if type(o) == "table" then
		-- draw a box:
		gl.Color(1, 1, 1)
		label:draw({x, y, 0}, o.op)
		-- draw connections & inlets:
		local args = #o
		if args == 2 then
			gl.Color(0.3, 0.3, 0.3)
			line(x, y-15, x - s, y-30)
			line(x, y-15, x + s, y-30)
			drawosc(o[1], x - s, y-30, s)
			drawosc(o[2], x + s, y-30, s)
		elseif args == 1 then
			gl.Color(0.3, 0.3, 0.3)
			line(x, y-15, x, y-30)
			drawosc(o[1], x, y-30, s)
		end
	elseif type(o) == "number" then
		-- draw number
		gl.Color(0.8, 0.7, 0.6)
		label:draw({x, y, 0}, string.format("%.2f", o))
	else
		gl.Color(1., 0., 0.)
		label:draw({x, y, 0}, tostring(o))
	end
end

function win:mouse(e, k, x, y)
	synth.x = x / self.dim[1]
	synth.y = 1 - (y / self.dim[2])
end

function win:key(e, k)
	if e == "down" then
		event("key", e, k)
	end
end

function win:draw()
	sketch.enter_ortho(win.dim)

	local w, h = unpack(win.dim)
	
	-- walk osc and draw it:
	drawosc(osc, w/2, h, w/8)
	
	sketch.leave_ortho()
end
--]]
