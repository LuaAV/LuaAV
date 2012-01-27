local osc = require "osc"
local zeroconf = require "zeroconf"

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
local hostip = "127.0.0.1"
for i, v in ipairs{ zeroconf.hostip() } do
    --print(i, v.name, v.address)
    if v.name:match("en") then
        hostip = v.address
    end
end

local client = zeroconf.Client()

-- a dictionary of devices
-- each is a dictionary of service names -> osc Send objects
local devices = {}

local Master_meta = {
	name = uid("LuaAV"),
	
	-- defaults:
--	remote_host = "127.0.0.1",
--	remote_port = 8080,
	host = hostip,
	port = 8081,
--	shouldDisplayMenu = "true",
	orientation = "landscape",
	poll_interval = 0.01,
	handler = function() end,
}
Master_meta.__index = Master_meta

-- the singleton object
local self = {
    -- the GUI
	_widgets = {}
}
setmetatable(self, Master_meta)


--- use this function to 
self.init = function(def)
	self.name = def.name or self.name
	
	-- copy def:
	for k, v in pairs(def) do
		self[k] = v
	end
	
	-- fix parameter types:
	-- FIXME: many more tests here
	--self.shouldDisplayMenu = self.shouldDisplayMenu and "true" or nil
	
	-- close sockets:
	self.oscrecv = nil
	self.oscrecv = osc.Recv(self.port)
	
	self._widgets = {}
	
	-- handshake:
	self.refresh()
end


function self.sendGUI(oscsend)
    
    -- handshake:
	oscsend:send("/control/createBlankInterface", 
		self.name, self.orientation --, self.shouldDisplayMenu
	)
	oscsend:send("/control/pushDestination", 
		string.format("%s:%d", self.host, self.port)
	)
	
	for name, widget in pairs(self._widgets) do
	    oscsend:send("/control/addWidget", widget.json) --, options)
    end
     
end

function self.refresh()
    -- send to all known destinations
    for host, dev in pairs(devices) do
       for servicename, oscsend in pairs(dev) do
            self.sendGUI(oscsend)
       end 
    end
--	self.oscsend:send(address, ...)
end

function self.send(oscaddress, ...)
    -- send to all known destinations
    for host, dev in pairs(devices) do
       for servicename, oscsend in pairs(dev) do
            print("sending to ", oscsend:ip(), servicename, host, oscaddress, ...)
            oscsend:send(oscaddress, ...)
       end 
    end
--	self.oscsend:send(address, ...)
end

local function printer(w) print("asdasdasd", w.name, w.value) end

self.Widget = function(def)	
	def.type = def.type or "Slider"
	def.name = def.name or uid(def.type)
	def.callback = def.callback or printer
	def.set = function(v)
	    def.value = v
	    self.send("/"..def.name, v)
    end
	
	local json = makeJSON(def)
	local options = [[{ \"min\" : 1, \"max" : 6, }]]
	--print(json)
	self.send("/control/addWidget", json) --, options)
	
	def.json = json
	
	self._widgets["/"..def.name] = def
	return def -- should return def???
end

self.remove = function(name)
	self.send("/control/removeWidget", name)
	self._widgets["/"..name] = nil
end

self.recv = function()
	return self.oscrecv:recv()
end

self.set = function(addr, v)
    local w = self._widgets["/"..addr]
    if w then 
	    w.set(v)
	end
end

-- add callbacks to be notified when a service is added, resolved or removed:
client:added(function(name) 
    print("added service", name) 
end)

client:resolved(function(name, host, port, address) 
    print("resolved service", name, host, port, address) 
    local dev = devices[host] or {}
    if address ~= "0.0.0.0" then
    
        local oscsend = osc.Send(address, port)
    
        self.sendGUI(oscsend)
    
        dev[name] = oscsend
        devices[host] = dev
    end
end)

client:removed(function(name) 
    print("removed service", name) 
    devices[name] = nil
end)

-- create default interface:
self.init{}


-- launch receiver coroutine:
go(function()
	while self do
		for m in self.oscrecv:recv() do
		    print(m.addr, m.types, unpack(m))
		    -- find widget in map:
		    local w = self._widgets[m.addr]
		    if w then
		        w.value = m[1]
		        w.values = m
		        if w.callback then
		        local ok, err = pcall(w.callback, w)
    		        if not ok then
    				    print(format("control error (%s): %s", m.addr, err))  
    	            end
    		    end
			end
			-- delegate to user-handler:
			--[[
			local ok, err = pcall(self.handler, m)
			if not ok then 
				print(format("control error (%s): %s", m.addr, err))
			end
			--]]
		end
		wait(self.poll_interval or 0.1)
	end
end)

return self