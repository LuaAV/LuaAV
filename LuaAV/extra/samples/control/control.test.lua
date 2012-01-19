local control = require "control"
local Widget = control.Widget

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


