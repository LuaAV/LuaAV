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
		json[#json+1] = string.format("'%s' : '%s', ", k, tostring(v))
	end
	return string.format('{ %s }', table.concat(json))
end

local Master_meta = {
	name = uid("LuaAV"),
	
	-- defaults:
	remote_host = "127.0.0.1",
	remote_port = 8080,
	host = "127.0.0.1",
	port = 8081,
	shouldDisplayMenu = 1,
	orientation = "landscape",
	poll_interval = 0.01,
	handler = function() end,
}
Master_meta.__index = Master_meta

local self = {
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
	self.shouldDisplayMenu = self.shouldDisplayMenu and (self.shouldDisplayMenu ~= 0 and 1 or 0) or 0
	
	-- close sockets:
	self.oscsend = nil
	self.oscrecv = nil
	collectgarbage()
	collectgarbage()
	
	-- re-create interface:
	self.oscsend = osc.Send(self.remote_host, self.remote_port)
	self.oscrecv = osc.Recv(self.port)
	
	-- handshake:
	self.send("/control/createBlankInterface", 
		self.name, self.orientation, self.shouldDisplayMenu
	)
	self.send("/control/pushDestination", 
		string.format("%s:%d", self.host, self.port)
	)
end


function self.send(address, ...)
	self.oscsend:send(address, ...)
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