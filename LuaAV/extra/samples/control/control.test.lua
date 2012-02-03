-- create a simple synth to control:
local Def = require "audio.Def"
Def.globalize()

local synthdef = Def{
	amp = 0.2,
	freq = 220,
	mod = 10,
	depth = 1,
	Lag{ P"amp", 0.997 } * SinOsc{ Lag{ P"freq", 0.999 } * (-1 + P"depth" * SinOsc{ P"mod" }) }
}

synth = synthdef()

-- load dependencies, 
-- handshake with Zeroconf, 
-- and create the interface:
local control = require "control"

-- declare some controls:
control.Slider{ "amp", value=0.1, bind=synth, }
control.Slider{ "mod", value=0.1, min=0, max=1000, bind=synth, }
control.Slider{ "freq", value=220, min=55, max=1760, }
control.Slider{ "depth", value=1, min=0, max=10, }
--control.Accelerometer{ "accel" }

-- get the value:
--print("amp", control.amp)

wait(1)

-- set the value
control.amp = 0.5

-- remove a control:
control.accel = nil

-- set up notifiers to update the synth from control:
---[[
control.bind("amp", synth)
control.bind("freq", synth)
control.bind("mod", synth)
control.bind("depth", synth)
--]]

--control.bind(synth)

-- this will be optional soon, once bonjour etc. is working:
--[[
control.init{
	--name = "LuaAV",
	--orientation = "portrait",
	
	--host = "127.0.0.1",
	--port = 8081,
	
	--remote_host = "127.0.0.1",
	--remote_port = 8080
}
--]]
--[[
local Widget = control.Widget

-- add widgets
wait(1)
Widget{} -- default type Slider
local button = Widget{ type="Button", min=1, max=2, }

function twiddle(widget)
    print(widget, widget.name, widget.value)
end

Widget{ type="Knob", name="Twiddler", callback=twiddle }

wait(1)

-- send a value:
button.set(2)

control.set("Twiddler", 0.5)

wait(1)

control.remove("Twiddler")
--]]