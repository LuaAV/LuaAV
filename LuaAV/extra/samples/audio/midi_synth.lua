require "audio.driver"
local Def = require "audio.Def"
Def.globalize()

--script.live = true


local example = Def{
	freq = 400, 
	amp = 0.5,
	
	Pan2{ 
		Lag{ P"amp", 0.99 } * SinOsc{ P"freq" }, 
		pan=SinOsc{ freq=12 } 
	}, 
}

require "audio.util"
local mtof = audio.util.mtof

require "midi"
for i, v in ipairs(midi.inports()) do print("in port", i, v) end
for i, v in ipairs(midi.outports()) do print("out port", i, v) end

local voices = {}
local fadein = 0.005
local fadeout = .1
local ampmax = 0.3
local bendrange = 2

function startnote(channel, note, velocity)
	local freq = mtof(note)
	local amp = (velocity/128) * ampmax
	
	-- create new note
	return example{
		freq = mtof(note),
		amp = (velocity/128) * ampmax,
	}		
end

function endnote(synth)
	synth.amp = 0
	wait(1)
	synth:stop()
end

local input = midi.Input()
input:ignoretypes(false, false, false)

local dt = 1/100
while true do
	for m in input:poll() do
		local event = m.event
		if event == "noteon" then
			local channel = m[1]
			local note = m[2]
			local velocity = m[3]
			
			-- voice stealing:
			local synth = voices[note]
			if synth then
				go(endnote, synth)
			end
			voices[note] = nil
			
			-- create new note:
			voices[note] = example{
				freq = mtof(note),
				amp = (velocity/128) * ampmax,			
			}
			
		elseif event == "noteoff" then
			local channel = m[1]
			local note = m[2]
			local velocity = m[3]
			
			-- schedule note to fade out:
			local synth = voices[note]
			if synth then
				go(endnote, synth)
			end
			voices[note] = nil
			
		elseif event == "pressure" then
			local press = m[2]/128
		elseif event == "wheel" then
			local channel = m[1]
			local bend = bendrange * ((m[2] / 8192) - 1)
			print("wheel", bend)
		else
			print(event, unpack(m))
		end
	end
	wait(dt)
end