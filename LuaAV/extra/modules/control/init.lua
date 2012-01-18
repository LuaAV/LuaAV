local osc = require "osc"

local format = string.format

-- some selfaults:
local remote_host_default = "localhost" --"192.168.1.124"
local remote_port_default = 8080
local myip = "localhost"
local myport = 8081

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
	remote_host = "localhost",
	remote_port = 8080,
	host = "localhost",
	port = 8081,
	shouldDisplayMenu = 1,
	orientation = "landscape",
	poll_interval = 0.01,
	handler = function() end,
}
Master_meta.__index = Master_meta

function Master_meta:send(address, ...)
	self.oscsend:send(address, ...)
end

function Master_meta:send(address, ...)
	self.oscsend:send(address, ...)
end

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
	self.shouldDisplayMenu = self.shouldDisplayMenu and (self.shouldDisplayMenu ~= 0) or 0
	
	-- close sockets:
	self.oscsend = nil
	self.oscrecv = nil
	collectgarbage()
	collectgarbage()
	
	-- re-create interface:
	self.oscsend = osc.Send(self.remote_host, self.remote_port)
	self.oscrecv = osc.Recv(self.port)
	
	-- handshake:
	self:send("/control/createBlankInterface", 
		self.name, self.orientation, self.shouldDisplayMenu
	)
	self:send("/control/pushDestination", 
		string.format("%s:%d", self.host, self.port)
	)
end

self.Widget = function(def)	
	def.type = def.type or "Slider"
	def.name = def.name or uid(def.type)
	local json = makeJSON(def)
	local options = [[{ \"min\" : 1, \"max" : 6, }]]
	--print(json)
	self:send("/control/addWidget", json) --, options)
	
	self._widgets[def.name] = def
end

self.set = function(name, ...)
	self:send(format("/%s", name), ...)
end

self.remove = function(name)
	self:send("/control/removeWidget", name)
	self._widgets[name] = nil
end

self.recv = function()
	return self.oscrecv:recv()
end

-- create default interface:
self.init{}

-- launch receiver coroutine:
go(function()
	while self do
		for m in self.oscrecv:recv() do
			
			-- delegate to user-handler:
			local ok, err = pcall(self.handler, m)
			if not ok then 
				print(format("control error (%s): %s", m.addr, err))
			end
		end
		wait(self.poll_interval or 0.1)
	end
end)

return self