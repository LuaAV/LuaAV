local Def = require "audio.Def"
Def.globalize()

-- create a Bus with which to share signal data between synths:
-- NOTE: the Bus system is still under active research;
-- the programming interface is quite likely to change in the future...
local foo = audio.Bus("foo", 2)

-- a modulable synth:
local sdef = Def{
	mod = { 1, 1 },
	amp = 0.3,
	SinOsc{ 1000 * P"mod" } * P"amp"
}

-- modulation source:
local mdef = Def{
	freq = { 1, 2 },
	SinOsc{ "freq" }
}

-- create a synth that reads from Bus 'foo':
local s = sdef{ mod = foo }

-- create a synth that writes to 'foo':
-- note: this only works because s is after m in the synth list. 
-- if they are created the other way around, it doesn't work.
local m = mdef{ 
	freq={ 1, 2 }, 
	out = foo 
}


