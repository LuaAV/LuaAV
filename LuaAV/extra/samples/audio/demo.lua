local Def = require "audio.Def"
Def.globalize()

math.randomseed(os.time())


local example = Def{
	freq = 400, dur = 1, amp = 0.5,
	Pan2{ 
		Lag{ P"amp" * Env{ "dur" }, 0.9 } 
			* SinOsc{ Lag{ "freq", 0.9 } }, 
		pan=SinOsc{ freq=8 } 
	},
}

local synth = example{ amp = 0.2, dur = 2, freq = 800 }

while now() < 2 do
	wait(0.1)
	synth.freq = math.random(10) * 100
end
