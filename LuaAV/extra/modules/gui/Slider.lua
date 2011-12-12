local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local type = type
local print = print

local min = math.min
local max = math.max

local co = coroutine
-------------------------------------
local g = require("gui.draw")
local Rect = require("gui.rect")
local View = require("gui.view")
-------------------------------------
--- A slider for ranged values
-- @name gui.slider
module(...)

local Superclass = View

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = function(t, k)
	return M[k] or Superclass[k]
end

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
-- @param init.value Starting value (default 0.5)
-- @param init.range Starting range (default {0, 1})
-- @param init.clickset Click-to-set flag (default true)
-- @name slider
function C:__call(init)	
	init.value = init.value or 0.5
	init.range = init.range or {0, 1}
	init.clickset = default_bool(true, init.clickset)
	
	local m = setmetatable(Superclass(init), M)
	m.color = {1,1,1,1}
	m.mouse_handler = m:create_mouse_handler()

	return m
end

--- Set the value of the slider
-- @param v The value
function M:set_value(v)
	self.value = max(self.range[1], min(self.range[2], v))
	self:notify("value")
end

function M:mouse_down(btn, x, y, nclks)
	self.mouse_handler("down", btn, x, y, nclks)
end

function M:mouse_drag(x, y)
	self.mouse_handler("drag", x, y)
end

function M:mouse_up(x, y)
	self.mouse_handler("up", x, y)
end

local
function pix2val(self, v)
	--return self.range[1] + (self.range[2] - self.range[1])*v/self.rect.w
	return self.range[1] + (self.range[2] - self.range[1])*v/self.rect.h
end

function M:create_mouse_handler()
	local f = co.wrap(function()
		while(true) do
			local e, btn, x, y, nclks = co.yield()
			if(e == "down") then
				if(self.clickset) then
					self:set_value(
						pix2val(self, y)
					)
				end
				
				local start_v = self.value
				local start_x = x
				local start_y = y
				repeat
					e, x, y = co.yield()

					local dx = x-start_x
					local dy = y-start_y
					self:set_value(
						--start_v + (self.range[2] - self.range[1])*dx/self.rect.w
						start_v + (self.range[2] - self.range[1])*dy/self.rect.h
					)
				until(e ~= "drag")
			end
		end
	end)
	f()
	return f
end

--- Draw the slider
function M:draw()
	local r = self.rect
	local dv = self.range[2] - self.range[1]
	--g.color(1, 1, 1, 1)
	g.color(self.color)
	--g.rect(g.Quads, 0, -r.h + (self.value+self.range[1])/dv*r.h, r.w, r.h)
	g.rect(g.Quads, 0, r.h + (self.value * r.h), r.w, -r.h)	
end