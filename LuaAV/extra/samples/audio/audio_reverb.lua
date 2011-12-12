local Def = require "audio.Def"
Def.globalize()


-- a Def to click:
local env = Env{ dur=P"dur"}
local s = Def{
	dur = 0.1, 
	amp = 0.3,
	Pan2{ 
		SinOsc{ 800 * env } * env * P"amp",
		pan=SinOsc{ -1/P"dur" }
	}
}

-- Def using a built-in reverb unit:
local Mixer = Def{
	dry = 1,
	wet = 0.1,
	decay = 0.85,
	bandwidth = 0.9995,
	damping = 0.4,

	P"dry" * P"input" + 
	P"wet" * Reverb{ 
		Mono{ P"input", },
		decay="decay",
		bandwidth="bandwidth",
		damping="damping",
		--decay=1-Phasor{1}, --(1+SinOsc{-1/16})*0.5, 
		--bandwidth=(1+SinOsc{1/12})*0.5,
		--damping=(1+SinOsc{1/8})*0.5,
	}
}

-- a bus to route audio signals to the mixer:
local bus = audio.Bus("reverbmix", 2)
local mixer = Mixer{ input = bus }


while true do
	-- pulsing clicks:
	s{ dur = 0.5, amp = 0.3, out = bus, }
	
	-- alternate wet/dry emphasis:
	--mixer.dry, mixer.wet = mixer.wet, mixer.dry
	wait(1/2)
end	