local Def = require "audio.Def"
Def.globalize()



-- a Def to click:
local env = Lag{ Env{ dur=P"dur"}, 0.95 }
local s = Def{
	dur = 0.1, 
	amp = 0.3,
	Pan2{ 
		SinOsc{ 1024 * env * env } * env * P"amp",
		pan=SinOsc{ -1/P"dur" }
	}
}

-- Def using a built-in reverb unit:
local Mixer = Def{
	dry = 1,
	wet = 0.25,

	P"input" + 
	{ 
		Delay{ P"wet" * P"input", delay = 1/4 + SinOsc{ 3/2 }/16, feedback = 0.9 },
		Delay{ P"wet" * P"input", delay = 1/8 + SinOsc{ 4 }/(2^(6*(2+SinOsc{ -1/8 }))), feedback = 0.95 }
	}
}

-- a bus to route audio signals to the mixer:
local bus = audio.Bus("bus")
local mixer = Mixer{ input = bus }


while true do
	-- pulsing clicks:
	s{ dur = 0.5, amp = 0.3, out = bus, }
	
	-- alternate wet/dry emphasis:
	--mixer.dry, mixer.wet = mixer.wet, mixer.dry
	wait(1/2)
end	