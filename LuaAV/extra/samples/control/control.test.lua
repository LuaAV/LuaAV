

local control = require "control"
local Widget = control.Widget

-- this will be optional soon, once bonjour etc. is working:
control.init{
	name = "LuaAV",
	orientation = "portrait",
	
	host = "10.0.2.4",
	port = 8081,
	
	remote_host = "10.0.2.1",
	remote_port = 8080
}

-- add widgets
Widget{} -- default type Slider
Widget{ type="Button", name="Bang", min=1, max=2, }
Widget{ type="Knob", name="Twiddler", }
Widget{ type="Accelerometer" }

wait(1)

-- send a value:
control.set("Bang", 2)

-- remove a widget:
control.remove("Twiddler")


-- install a generic handler:
control.handler = function(m)
	print(m.addr, m.types, unpack(m))
end
