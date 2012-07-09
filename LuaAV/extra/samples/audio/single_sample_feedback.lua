local Def = require "audio.Def"
Def.globalize()

local SR = audio.samplerate()
local twopi = math.pi * 2

local osc = Def{
	freq = 440,
	-- define a recursive accumulator:
	phase = P"phase" + P"freq" / SR,
	-- use it to create a sine oscillator:
	Sin{ P"phase" * twopi }
}

-- play it
local v = osc{ freq = 440 }

