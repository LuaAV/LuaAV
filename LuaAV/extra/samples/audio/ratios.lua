local Def = require "audio.Def"
Def.globalize()

--------------------------------------------------------------------------------
-- Test
--------------------------------------------------------------------------------

-- build up a synthdef from sub-expressions
-- core oscillator:
local osc = Min{ 
	Abs{ SinOsc{"freq"} }, 
	Abs{ SinOsc{ P"freq" * "ratio" } }, 
}
-- strange envelope:
local env = Sin{ math.pi * P"ratio" * Env{ "dur", 0 } }

-- define synthdef:
local Zow = Def{
	-- parameters of this synthdef (Ports):
	amp = 0.1,
	freq = 440,
	dur = 1,
	ratio = 7/6,
	pan = 0,
	-- combine sub-expressions into graph:
	Pan2{ "amp" * env * osc, "pan" }
}

-- use the synthdef in an algorithmic composition:
local du = 1/4
local ratio = 1
local playing = true

go(function()
	while playing do
		for i = 1, 8 do
			print(string.format("ratio: %i/%i", i+1, i))
			ratio = ((i+1)/i)
			wait(du*8)
		end
	end
end)

go(function()
	while playing do
		for i = 0, 256 do
			-- create (schedule) a synth:
			Zow{ 
				amp = 0.25, 
				dur = du, 
				freq = 200 * (1+(-i % 16)), 
				ratio = ratio, 
				pan = math.random()-0.5 
			}
			wait(du/8)
		end
		-- stop the other coroutine:
		playing = false
	end
end)
