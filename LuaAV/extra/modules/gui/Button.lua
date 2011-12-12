local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local print = print
local type = type

-------------------------------------
local g = require("gui.draw")
local Rect = require("gui.Rect")
local View = require("gui.View")
-------------------------------------
--- Binary state (true/false) UI element 
-- Buttons can act as toggles where succcessive mouse clicks 
-- toggle state or as press-and-release switches where 
-- state toggles when pressed but immediately release after 
-- the press ends.
-- @name gui.Button
module(...)

local Superclass = View

local
function default_bool(default, v)
	if(type(v) == "boolean") then
		return v
	else
		return default
	end
end

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = function(t, k)
	return M[k] or Superclass[k]
end

--- Constructor
-- @param init Constructor properties
-- @param init.value Initial value (default false)
-- @param init.toggles Whether button toggles (default true)
-- @name Button
function C:__call(init)	
	init.value = default_bool(false, init.value)
	init.toggles = default_bool(true, init.toggles)
	
	local m = setmetatable(Superclass(init), M)
	
	return m
end

--- Set the value of the button
-- @param v True/False
function M:set_value(v)
	self.value = v
	self:notify("value")
end

function M:mouse_down(btn, x, y, nclks)
	self:set_value(not self.value)
end

function M:mouse_up(btn, x, y)
	if(not self.toggles) then
		self:set_value(not self.value)
	end
end

--- Draw the button
function M:draw()
	if(self.value) then
		local r = self.rect
		g.color(1, 1, 1, 1)
		g.rect(g.Quads, 2, 2, r.w-4, r.h-4)
	end
end
