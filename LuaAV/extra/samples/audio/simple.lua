local Def = require "audio.Def"
Def.globalize()

local simpledef = Def{
	freq = { 440, 660 },
	dur = 1,
	amp = 0.5,
	P"amp" * Env{ "dur" } * SinOsc{ P"freq" }
}

local s = simpledef{ dur = 2.5, amp = 0.5 }
for i = 1, 10 do
	s.freq = { i * 100, i * 150 }
	wait(0.1)
end

