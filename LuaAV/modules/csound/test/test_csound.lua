
local csound = require("csound")

print("csound.version", csound.version)

for k, v in pairs(csound) do 
	print("csound", k, v)
end

local example = { 
	orchestra = [[
sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

instr 1
	a1 oscil 1, p4, 1
	outs a1, a1
endin

instr 100
	ain in
	outs ain, ain
endin
	]],
	score = [[
f1 0 4096 10 1;
i1 0 1
i100 0 3600
	]],
}

local example = { 
	orchestra = [[

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

gavb init 0

; this table simply a test to check API wrapper
gitest ftgen 20, 0, 2048, 10, 1

instr 1
	k_amp chnget "amp"
	k_cps chnget "cps"
  	a1	linen	 p4, 0.01, p3, p3/2	; amp, attack, dur, release
  	aout oscil a1, p5, 1			; amp, freq, wavetable
  	gavb = gavb + aout
	printk2 k_amp
	printk2 k_cps
	outs aout, aout
endin

instr 200
	aoutL, aoutR reverbsc gavb, gavb, 0.85, 6000
	outs aoutL, aoutR
	gavb = 0 						; Clear to prevent audio build-up.
endin
	]],
	score = [[
f1 0 4096 10 1;
i200 0 3600
	]],
}

--local c = csound.create{ filename = filename }
local c = csound.create(example)
print(c, c:getinstrumentcount(), "instruments")

local amp = 0.1

-- i p1 (instr) p2 (start) p3 (dur), p4...
local param = { instr = 1, start = 0, dur = .1, amp = amp, freq = 440 }

--[[
for i = 1, 10 do
	param.freq = 110 * math.random(20)
	c:scoreevent("i", param.instr, now() + param.start, param.dur, param.amp, param.freq )
	wait(.1)
end 

c:scoreevent("i", 1, now(), .3, amp, 880)
c:scoreevent("i", 1, now()+2, .3, amp, 440)
c:scoreevent("i", 1, 0, 1, amp, 330)
--]]


wait(5)
c:stop()

print('ok')
