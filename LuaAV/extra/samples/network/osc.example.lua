local osc = require("osc")
local gl = require "opengl"
local GL = gl

local send_address = '127.0.0.1'	-- or another IP address, or hostname such as 'localhost'
local send_port = 16447
local receive_port = 16448

local oscout = osc.Send(send_address, send_port) 
local oscin  = osc.Recv(receive_port)   

local ctx = "OSC Template"
win = Window(ctx, 0, 0, 200, 200)  

local x, y = 0, 0

function getOSC() 
	for msg in oscin:recv() do	
		print("received:", msg.addr, unpack(msg))
		
		if msg.addr == "/xy" then
			x, y = unpack(msg)
		end
	end
end

--for k, v in pairs(gl) do print(k, v) end

function win:draw()
	getOSC() 
	--gl.Ortho(0, win.dim[1], 0, win.dim[2], -100, 100)
	gl.Scale(0.01, 0.01, 1)
	gl.Translate(-100, -100, 0)
	gl.Begin(gl.LINES)
		gl.Vertex(100, 100)
		gl.Vertex(x, y)
	gl.End()
end

function win:mouse(event, button, x, y)
	oscout:send("/mouse", event, button, x, y)
	
	-- example using bundles:
	oscout:send{
		{ addr="/mousebun1", event, button, x, y },
		{ addr="/mousebun2", event, button, x, y }
	}
	print("sent:", "/mouse", event, button, x, y)
end