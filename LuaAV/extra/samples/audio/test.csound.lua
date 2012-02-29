local csound = require "audio.csound"

local orc = csound.parse[[
instr synth1
	asig	oscil 0.5, p4
	asqr = asig * asig
			out asqr
endin

instr synth2
	asig	phasor p4
	afilt	lowpass2 asig, 400, 10
			out (afilt * 0.5)
endin
]]

orc.synth1{ p3 = 0.5, p4 = 440 }
orc.synth2{ p3 = 0.75, p4 = 660 }

wait(1)

orc.synth2{ p3 = 1., p4 = 440 }
wait(0.25)
orc.synth1{ p3 = 0.75, p4 = 660 }
