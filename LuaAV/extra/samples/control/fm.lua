local Def = require "audio.Def"
Def.globalize()

FM = Def{
	modFreq = 200, 
	depth = {0.1, 0.1}, 
	carFreq = 440,
	SinOsc{ 
		Lag{ P"carFreq" } * 
		(1 + SinOsc{ Lag{ P"modFreq" + P"depth" } } * Lag{ P"depth" }) 
	}
}

t = FM{ carFreq = 110, modFreq = 880, depth = {0.1,0.11} }



-- create an interface on the phone:
control = require "control"

control.Slider{ "carFreq", t, min=110, max=880 }
control.Slider{ "modFreq", t, min=10, max=1320 }
control.Accelerometer{ "depth", t, min=0, max=10 }

--print(control.carFreq)
--control.modFreq = 660

