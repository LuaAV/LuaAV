<CsoundSynthesizer>
<CsOptions>
-d
</CsOptions>
<CsInstruments>


sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

zakinit 2, 1 ; Create 2 a-rate zak channels and 1 k-rate zak channel.

gavb init 0

instr 1
  	k1	linen	 p4, 0.1, p3, p3/2	; amp, attack, dur, release
  	aout oscil k1, p5, 1		; amp, freq, wavetable
  	gavb = gavb + aout

	outs aout, aout
endin

instr 200
	aoutL, aoutR reverbsc gavb, gavb, 0.9, sr/2 ;kfblvl, kfco[, israte[, ipitchm[, iskip]]] 
	;a1, a2 babo ain, 1, 0.2, 0.2, 4, 5, 6, 1 ; input, pos(x, y, z), roomdim(x, y, z), diffusion
  	outs aoutL, aoutR

	gavb = 0

	zacl 0, 2  ; Clear audio channels to prevent audio build-up.
endin

</CsInstruments>

<CsScore>

f1 0 1024 10 1

</CsScore>

</CsoundSynthesizer>
<MacOptions>
Version: 3
Render: Real
Ask: Yes
Functions: ioObject
Listing: Window
WindowBounds: 969 453 553 240
CurrentView: io
IOViewEdit: On
Options:
</MacOptions>
<MacGUI>
ioView nobackground {59624, 59624, 59624}
ioSlider {5, 5} {20, 100} 0.000000 1.000000 0.000000 slider1
ioSlider {45, 5} {20, 100} 0.000000 1.000000 0.000000 slider2
ioSlider {85, 5} {20, 100} 0.000000 1.000000 0.000000 slider3
ioSlider {125, 5} {20, 100} 0.000000 1.000000 0.000000 slider4
ioSlider {165, 5} {20, 100} 0.000000 1.000000 0.000000 slider5
</MacGUI>

