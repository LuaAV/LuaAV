local Def = require "audio.Def"
Def.globalize()





-- a simple synth to demonstrate with:
local mono = Def{
	SinOsc{ freq=440 } * Env{ dur=1 }
}
mono()
wait(1)

-- notice that the frequency input is now a list:
local stereo = Def{
	SinOsc{ freq={440, 550} } * Env{ dur=1 }
}
stereo()
wait(1)



-- selecting an individual channel from a multi-channel stream:
local osc = SinOsc{ freq={440, 550} }	-- stereo
local right = Channels{ osc, 2 }		-- mono
local swapped = Channels{ osc, 2, 1 }	-- stereo swapped

-- mixing a multi-channel signal to mono:
local mono = Def{
	Mono{ SinOsc{ freq={440, 550, 660, 770} } * Env{ dur=1 } * 0.25 }
}
mono()
wait(1)

-- mixing a multi-channel signal to stereo:
local stereo = Def{
	Stereo{ SinOsc{ freq={440, 550, 660, 770} } * Env{ dur=1 } * 0.25 }
}
stereo()
wait(1)



-- parameters can also be multi-channel. 
-- notice how this sounds the same as the previous
local pstereo = Def{
	dur = 1,
	freq = { 440, 550 }, 
	SinOsc{ freq=P"freq" } * Env{ dur=P"dur" }
}
pstereo()
wait(1)

-- can assign to multi-channel parameters with a list:
local voice = pstereo()
for i = 1, 10 do
	voice.freq = { i * 110, (10-i) * 110 }
	wait(0.1)
end
-- any nils in the list are treated as 'leave parameter-channel as-is':
local voice = pstereo{ dur=2 }
for i = 1, 10 do
	voice.freq = { nil, (10-i) * 110 }
	wait(0.1)
	voice.freq = { i * 110, nil }
	wait(0.1)
end

-- and single values still work (sets all channels):
local voice = pstereo()
voice.freq = 550
wait(1)

print("panners")

-- using panners and spatializers
local panning = Def{
	freq = 440, 
	Pan2{ 
		SinOsc{ "freq" } * Env{ dur=1 },
		pan = SinOsc{ 4 }
	}
}
--[[
 

panning()
wait(1)
--]]
