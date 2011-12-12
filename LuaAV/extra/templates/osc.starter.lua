local osc = require("osc")

-- configure OSC send/receive address & ports
local sendto_ipaddress = '192.168.10.5'
local sendto_port = 7777
local receiveat_port = 7778

local oscout = osc.Send(sendto_ipaddress, sendto_port) 
local oscin  = osc.Recv(receiveat_port)

-- retrieve and handle incoming OSC:
function get_osc()
	for msg in oscin:recv() do	
		print(msg.addr, msg.types, unpack(msg))
		-- add message handling here
	end
end

-- window setup
local ctx = "OSC Template"
win = Window(ctx, 0, 0, 250, 250)

-- for example, sending messages based on mouse/keyboard interaction:
function win:mouse(e, btn, x, y)
	oscout:send('/mouse', e, btn, x, y)
end
function win:key(e, key)
	oscout:send('/key', e, key)
end


go(function()
	while(true) do
		get_osc()
		wait(1/40)
	end
end)