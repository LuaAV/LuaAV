local zeroconf = require "zeroconf"
local osc = require "osc"

local format = string.format

local uid = (function()
	local id = 0
	return function(name)
		id = id + 1
		return string.format("%s%d", name or "anon", id)
	end
end)()

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

-- figure out host ip:
local host_ip = "127.0.0.1"
for i, v in ipairs{ zeroconf.hostip() } do
    if v.name:match("en") then
        host_ip = v.address
    end
end

--[[

A widget has an internal name, which is used for the OSC addressing

For each parameter of a widget (e.g. multislider, accelerometer)

--]]

-- how frequently to check for OSC:
local poll_wait = 0.01

local host_port = 8081
local oscrecv = osc.Recv(host_port)

-- name of interface:
local interface_name = "LuaAV"
local orientation = "landscape"

-- a dictionary of devices
-- each is a dictionary of service names -> osc Send objects
local devices = {}

-- private storage of the list of active widgets:
local widgets = {}

-- map from OSC address to widget:
local addr2widget = {}

local M = {}

local widget = {}
widget.__index = widget

function widget:message(m, count)
	-- store new values:
	for i, v in ipairs(self.value) do
		if i <= count then
			self.value[i] = m[i]
		end
	end
	
	-- call handlers:
	local handlers = self.handlers
	for _, handler in ipairs(handlers) do
		-- catching errors:
		local ok, err = pcall(handler, unpack(self.value))
		if not ok then
			print(format("control error (%s): %s", m.addr, err))  
		end
	end
end

function widget:addmap(name, dst)
	--print("addmap", name, dst)
	local h = dst	
	-- create handler for objects:
	if type(dst) ~= "function" then
		h = function(...)
			--print("debug", dst, name, ...)
			dst[name] = {...}
		end
	end
	local handlers = self.handlers
	handlers[#handlers+1] = h
end

function widget:setmap(name, dst)
	self:clearmap()
	self:addmap(name, dst)
end

function widget:clearmap()
	self.handlers = {}
end

function widget:register(addr)	
	if self.address then self:unregister() end
	self.address = addr
	addr2widget[addr] = self
	--print(addr, addr2widget[addr])
end

function widget:unregister()
	addr2widget[self.address] = nil
end

function make_widget(def, config)
	def.type = def.type or "Slider"
	
	def.min = def.min or 0
	assert(type(def.min) == "number", "Widget min must be a number")
	def.max = def.max or 1
	assert(type(def.min) == "number", "Widget max must be a number")
	local default = def.value or def.min
	assert(type(default) == "number", "unexpeted Widget value type")
	
	if def.name == nil then
		def.name = def[1]
		if type(def.name) ~= "string" then
			def.name = uid("control")
		end
	end
	
	local w = setmetatable({
		type = def.type,
		name = def.name,
		
		min = def.min,
		max = def.max,
		
		label = def.label,
		page = def.page,
		color = def.color,
		-- etc.
		
		value = {},
		handlers = {},
		
		updateRate = 100, -- Hz
	}, widget)
	
	if config.count then
		for i = 1, config.count do
			w.value[i] = default
		end
	else
		error("TODO: derive slider count for multislider")
	end
	
	w:register(def.address or format("/%s", def.name))
	
	local i = 1
	while i < #def do	
		local names = {}
		local idx = 1
		local v = def[i]
		while type(v) == 'string' do	
			-- lazy name:
			names[#names+1] = { idx=idx, name=v }
			idx = idx + 1
			i = i + 1
			v = def[i]
		end
		if #names == 0 then
			-- generate a random name
			names[1] = uid("control")
		end
		while v and type(v) ~= 'string' do
			for _, p in ipairs(names) do
				w:addmap(p.name, v)
			end
			i = i + 1
			v = def[i]
		end
	end
	
	local json = makeJSON(w)
	--print(json)
	w.json = json
	M.send("/control/addWidget", json) --, options)
	
	widgets[w.name] = w
	
	return w
end


local widget_types = {
	MultiButton = {}, 
	MultiSlider = {}, 
	Label = { count=0 },
	Button = { count=1 }, 
	Slider = { count=1 }, 
	Knob = { count=1 },
	AudioVolume = { count=1 },
	AudioPitch = { count=1 },
	--MultiTouchXY = { count=2 },
	Accelerometer = { count=3 },
	Gyro = { count=6 },
	--Compass = { count=1 },
}

for name, config in pairs(widget_types) do
	M[name] = function(def) 
		def.type = name
		return make_widget(def, config)
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
	addr2widget = {}
	
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
            --print("sending to ", oscsend:ip(), servicename, host, oscaddress, ...)
            oscsend:send(oscaddress, ...)
       end 
    end
end

--[[
function M.Widget(def)

	assert(def and type(def) == "table", "widget constructor requires a table argument")
	local name = def.name or table.remove(def, 1) or uid("control")
	assert(name and type(name) == "string", "widget definition requires a name (string)")
	
	local map = def.map or table.remove(def)
	
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
		label = def.label,
		page = def.page,
		color = def.color,
		-- TODO add other fields
		
		handlers = {},
	}
	
	local json = makeJSON(w)
	w.json = json
	
	widgets[w.name] = w
	widgets[w.address] = w
	
	if map then
		M.map(w.name, map)
	end
	
	M.send("/control/addWidget", json) --, options)
	
	return name
end

function M.map(name, destination)
	if type(name) == "table" or type(name) == "userdata" then
		-- global maping:
		for k in pairs(widgets) do
			if k:sub(1, 1) ~= "/" then
				M.map(k, name)
			end
		end
	else
		local w = assert(widgets[name], format("no such widget: %s", name))
		if type(destination) == "table" or type(destination) == "userdata" then
			assert(type(w.value) == "number", "cannot map a multi-valued widget to an object; use a function callback instead")
			-- install setter callback:
			w.callback = setter(name, destination)
			
			-- set value immediately
			destination[name] = w.value
		elseif type(destination) == "function" then
			-- install a callback
			w.callback = destination
		else
			error("unexpected type for map destination: " .. type(destination))
		end
	end
end
--]]

-- read the widget value:
function M:__index(name)
	local w = widgets[name]
	if w then
		return unpack(w.value)
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
		w:unregister()
	else
		assert(w, format("no such widget: %s", name))
		if type(v) == "table" then
			w.value[i] = v[i] or w.value[i]
		else
			for i = 1, #w.value do
				w.value[i] = v
			end
		end
	end
end

local function handlemessage(m)
	-- find widget in map:
	local w = addr2widget[m.addr]
	if w then
		w:message(m, #m.types)
	end
end
M.handlemessage = handlemessage

setmetatable(M, M)

-- initialize module receiver:
M.init{}


-- launch coroutine to receive OSC messages:
go(function()
	while M do
		for m in oscrecv:recv() do
			handlemessage(m)
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