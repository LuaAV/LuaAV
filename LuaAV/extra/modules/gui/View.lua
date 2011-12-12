local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local print = print
local type = type
-------------------------------------
local g = require("gui.draw")
-------------------------------------
--- View base class for all widgets
-- @name gui.View
module(...)

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

local
function default_bool(default, v)
	if(type(v) == "boolean") then
		return v
	else
		return default
	end
end

--- Constructor
-- @param init Constructor properties
-- @param init.offset Initial offset (default {0, 0})
-- @param init.enabled Enabled flag (default true)
-- @param init.visible Visible flag (default true)
-- @param init.border Draw border (default true)
-- @param init.background Draw Background (default true)
-- @name View
function C:__call(init)
	assert(init.rect)
	init.offset = {0, 0}
	init.enabled = default_bool(true, init.enabled)
	init.visible = default_bool(true, init.visible)
	init.border = default_bool(true, init.border)
	init.background = default_bool(true, init.background)
	
	local m = setmetatable(init, M)
	m.views = {}
	
	m.notifications = {}
	
	return m
end

function M:draw(dim)
end

function M:resize()
end

--- Notify any registered listeners of an event
-- @param msg The message
-- @param ... The arguments to pass to the listener
function M:notify(msg, ...)
	local events = self.notifications[msg]
	if(events) then
		for k, e in pairs(events) do
			e.handler(self, ...)
		end
	end
end

--- Register for event notifications
-- @param msg The message
-- @param f The handler
function M:register(msg, f)
	if(not self.notifications[msg]) then
		self.notifications[msg] = {}
	end
	local n = self.notifications[msg]
	local event = {
		handler = f,
	}
	
	n[1] = event
	
	return event
end

--- Add a subview
-- @param v The subview
function M:add_view(v)
	if(self.context) then
		v.context = self.context
		self.views[#self.views+1] = v
		v:notify("add")
	else
		self.views[#self.views+1] = v
	end
end

