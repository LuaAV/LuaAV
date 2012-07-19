require "audio.util"
local mtof = audio.util.mtof

local Def = require "audio.Def"
Def.globalize()
audio.clear() 

math.randomseed(os.time())

mary = {
       { 64, 1/2 },
       { 62, 1/2 },
       { 60, 1/2 },
       { 62, 1/2 },

       { 64, 1/2 },
       { 64, 1/2 },
       { 64, 1 },

       { 62, 1/2 },
       { 62, 1/2 },
       { 62, 1 },

       { 64, 1/2 },
       { 67, 1/2 },
       { 67, 1 },

       { 64, 1/2 },
       { 62, 1/2 },
       { 60, 1/2 },
       { 62, 1/2 },

       { 64, 1/2 },
       { 64, 1/2 },
       { 64, 1/2 },
       { 64, 1/2 },

       { 62, 1/2 },
       { 62, 1/2 },
       { 64, 1/2 },
       { 62, 1/2 },

       { 60, 2 },
}

env = Env{ "dur", hold="gate" }
freq = Lag{"freq", "slide", mtof(64) }
am = SinOsc{1/P"dur"}^3
fm_env = Def{
   amp = 1, dur = 1, pan=0, gate = 0,
   freq = 400, cm = 1.5, mod_depth = 0.5,
   slide = 0.7,
   Pan2{
	   Lag{ P"amp" * env, "slide" } * SinOsc{
			freq * (1 + P"mod_depth" * (env^3) * SinOsc{ P"cm" * freq * (1+0.3*(P"mod_depth"*am^6)) })
	   } * am,
	   Lag{ "pan", "slide" }
   }
}

local i = 1
function playlong()
   local longnote = fm_env{
	   gate = 1,
	   amp = 0,
	   cm = 14/2.05,
	   mod_depth = 8/3,
	   slide = 0.99,
	   dur = 1/3,
   }
   while true do
	   i = (i % #mary) + 1
	   local note = mary[math.random(#mary)]
	   longnote.amp = 0.6/(10+math.random(20))
	   longnote.freq = mtof(104 - note[1]) * 2^math.random(3)
	   longnote.pan = math.random() - 0.5
	   wait(3)
	   --longnote.freq = mtof(104 - note[1]) * 2^math.random(3)
	   longnote.amp = 0
		  wait(5)
   end
end

for i = 1, 5 do
       go(i/32, playlong)
end

function playshort(i)
       local note_data = mary[i]
       local freq = 2 * mtof(128 - note_data[1])               -- convert MIDI note number to Hz
       local dur = note_data[2]
       fm_env{
               freq=freq,
               dur=2*(dur^3),
               amp = 0.1,
               cm = 2 + 2*(16*(math.sin(now()/16)) % 1),
               pan = 2 * (math.random() - 0.5),
               mod_depth = 2*((now()/8) % 2)/dur,
       }
       wait(dur / 4)
end

while true do
       for i = #mary, 1, -1 do         -- retrograde
               playshort(i)
       end
       for i = 1, #mary do
               playshort(i)
       end
end