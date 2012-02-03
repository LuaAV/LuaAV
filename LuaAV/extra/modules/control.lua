local zeroconf = require "zeroconf"
local osc = require "osc"

local format = string.format

local
function makeJSON(def)
	local json = {}
	for k, v in pairs(def) do
	    if(type(v) == "string") then
    		json[#json+1] = string.format("'%s' : '%s' ", k, tostring(v))
    	elseif type(v) == "number" then
    	    json[#json+1] = string.format("'%s' : %f ", k, v)
    	end
    	
	end
	return string.format('{ %s }', table.concat(json, ", "))
end

-- how frequently to check for OSC:
local poll_wait = 0.01

-- figure out host ip:
local host_ip = "127.0.0.1"
for i, v in ipairs{ zeroconf.hostip() } do
    if v.name:match("en") then
        host_ip = v.address
    end
end
local host_port = 8081
local oscrecv = osc.Recv(host_port)

-- name of interface:
local interface_name = "LuaAV"
local orientation = "landscape"

local widget_types = {
	Button = {}, 
	MultiButton = { multi = true }, 
	Slider = {}, 
	MultiSlider = { multi = true }, 
	Knob = {},
	Label = {},
	MultiTouchXY = { multi = true },
	Accelerometer = { multi = true },
	Gyro = { multi = true },
	Compass = {},
	AudioVolume = {},
	AudioPitch = { multi = true },
}

-- a dictionary of devices
-- each is a dictionary of service names -> osc Send objects
local devices = {}

-- private storage of the list of active widgets:
local widgets = {}

local M = {}

for name, config in pairs(widget_types) do
	M[name] = function (def) 
		def.type = name 
		return M.Widget(def) 
	end	
end

-- modify the interface name / definition:
function M.init(def)
	interface_name = def.name or interface_name
	orientation = def.orientation or orientation
	poll_wait = def.poll_wait or poll_wait
	host_ip = def.host_ip or host_ip
	host_port = def.host_port or host_port
	
	-- close sockets
	oscrecv = nil
	collectgarbage()	
	collectgarbage()	 -- free port
	oscrecv = osc.Recv(host_port)
	
	-- clear interface:
	widgets = {}
	
	-- handshake:
	M.refresh()
end

function M.sendGUI(oscsend)  
	--print("sendGUI", interface_name, host_ip, host_port)
	-- handshake:
	oscsend:send("/control/createBlankInterface", 
		interface_name, orientation --, self.shouldDisplayMenu
	)
	oscsend:send("/control/pushDestination", 
		string.format("%s:%d", host_ip, host_port)
	)
	
	for name, widget in pairs(widgets) do
		if name:sub(1, 1) ~= "/" then
			--print("sending", name, widget)
	   	 	oscsend:send("/control/addWidget", widget.json) --, options)
	   	end
    end
end

function M.refresh()
    -- send GUI all known destinations
    for host, dev in pairs(devices) do
       for servicename, oscsend in pairs(dev) do
            M.sendGUI(oscsend)
       end 
    end
end

function M.send(oscaddress, ...)
    --print("sending ", oscaddress, ...)
    -- send to all known destinations
    for host, dev in pairs(devices) do
       for servicename, oscsend in pairs(dev) do
            print("sending to ", oscsend:ip(), servicename, host, oscaddress, ...)
            oscsend:send(oscaddress, ...)
       end 
    end
end

function M.Widget(def)
	assert(def and type(def) == "table", "widget constructor requires a table argument")
	local name = def.name or def[1]
	assert(name and type(name) == "string", "widget definition requires a name")
	
	local ty = def.type or "Slider"
	local min, max, value
	local config = assert(widget_types[ty], "unknown Widget type")
	if config.multi then
		value = def.value or { 0 }
		assert(type(value) == "table", "unexpeted Widget value type")
	else
		min = def.min or 0
		assert(type(min) == "number", "Widget min must be a number")
		max = def.max or 1
		assert(type(min) == "number", "Widget max must be a number")
		value = def.value or min
		assert(type(value) == "number", "unexpeted Widget value type")
	end
	
	local address = def.address or "/"..name
	assert(type(address) == "string", "Widget address must be a string")
	
	local w = {
		name = name,
		type = ty,
		address = address,
		value = value,
		min = min,
		max = max,
		-- TODO add other fields
	}
	
	local json = makeJSON(w)
	w.json = json
	
	M.send("/control/addWidget", json) --, options)
	
	widgets[w.name] = w
	widgets[w.address] = w
	
	if def.bind then
		M.bind(w.name, def.bind)
	end
end

local 
function setter(name, destination)
	return function(w)
		destination[name] = w.value
	end
end

function M.bind(name, destination)
	if type(name) == "table" or type(name) == "userdata" then
		-- global binding:
		for k in pairs(widgets) do
			if k:sub(1, 1) ~= "/" then
				M.bind(k, name)
			end
		end
	else
		local w = assert(widgets[name], format("no such widget: %s", name))
		if type(destination) == "table" or type(destination) == "userdata" then
			assert(type(w.value) == "number", "cannot bind a multi-valued widget to an object; use a function callback instead")
			-- install setter callback:
			w.callback = setter(name, destination)
			
			-- set value immediately
			destination[name] = w.value
		elseif type(destination) == "function" then
			-- install a callback
			w.callback = destination
		else
			error("unexpected type for bind destination: " .. type(destination))
		end
	end
end

-- read the widget value:
function M:__index(name)
	local w = widgets[name]
	if w then
		if type(w.value) == "table" then
			return unpack(w.value)
		else
			return w.value
		end
	else
		return rawget(M, name)
	end
end

-- update the widget value:
function M:__newindex(name, v)
	local w = widgets[name]
	if v == nil then
		if w then
			widgets[name] = nil
			widgets[w.address] = nil
		end
		-- send remove message:
		self.send("/control/removeWidget", name)
	else
		assert(w, format("no such widget: %s", name))
		if type(w.value) == "table" then
			assert(type(v) == "table", "multi-valued Widget requires a table value to set")
			self.send(w.address, unpack(v))
			
		elseif type(w.value) == "number" then
			assert(type(v) == "number", "single-valued Widget requires a number to set")
			self.send(w.address, v)
		end
	end
end

setmetatable(M, M)

-- initialize module receiver:
M.init{}

-- launch coroutine to receive OSC messages:
go(function()
	while M do
		for m in oscrecv:recv() do
		    --print(m.addr, m.types, unpack(m))
		    -- find widget in map:
		    local w = widgets[m.addr]
		    if w then
		    	if type(w.value) == "table" then	
		    		local lim = math.min(#w.value, #m)
		    		for i = 1, lim do
		    			w.value[i] = m[i]
		    		end
		    	else
		    		w.value = m[1]
		    	end
		        if w.callback then
		        	local ok, err = pcall(w.callback, w)
    		        if not ok then
    				    print(format("control error (%s): %s", m.addr, err))  
    	            end
    		    end
			end
		end
		wait(poll_wait)
	end
end)

-- initialize zeroconf listener to detect Control on mobile device:
local client = zeroconf.Client()

-- add callbacks to be notified when a service is added, resolved or removed:
client:added(function(name) end)

client:resolved(function(name, host, port, address) 
	local dev = devices[host] or {}
	if address ~= "0.0.0.0" then
		print(format("connected to service '%s' at %s:%d", name, address, port)) 
		local oscsend = osc.Send(address, port)
		M.sendGUI(oscsend)

		dev[name] = oscsend
		devices[host] = dev
	end
end)

client:removed(function(name) 
    print(format("lost connection to service '%s'", name))
    devices[name] = nil
end)

return M