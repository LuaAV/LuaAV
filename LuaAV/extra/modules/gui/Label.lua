local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local print = print

local min = math.min
local max = math.max

local co = coroutine
-------------------------------------
local g = require("gui.draw")
local Rect = require("gui.Rect")
local View = require("gui.View")
-------------------------------------
--- Display text in a gui
-- @name gui.Label
module(...)

local Superclass = View

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = function(t, k)
	return M[k] or Superclass[k]
end

M.CENTER = "Center"
M.LEFT = "Left"
M.RIGHT = "Right"
M.TOP = "Top"
M.BOTTOM = "Bottom"
M.MIDDLE = "Middle"

--- Constructor
-- @param init Constructor properties
-- @param init.label The text label to show
-- @param init.size The font size (default 10)
-- @name Label
function C:__call(init)	
	init.label = init.label or ""
	init.size = init.size or 10
	init.margin = init.margin or {0, 0}
	init.border = init.border or false
	init.background = init.background or false
	init.horizontal_align = init.horizontal_align or M.CENTER
	init.vertical_align = init.vertical_align or M.MIDDLE
	init.text_color = init.text_color or {1,1,1,1}

	local m = setmetatable(Superclass(init), M)
	m:register("add", m.notify_add)
	return m
end

--- Set the font size
-- @param v The size
function M:set_size(v)
	self.size = v
	self.font = self.context:get_font(self.size)
end

function M:notify_add()
	self.font = self.context:get_font(self.size)
end

--- Draw the label
function M:draw()
	local dpos = {0,0}

	if (self.horizontal_align == M.CENTER) then
		dpos[1] = self.rect.w*0.5
	elseif (self.horizontal_align == M.LEFT) then
		dpos[1] = self.margin[1]
	elseif (self.horizontal_align == M.RIGHT) then
		dpos[1] = self.rect.w - self.margin[1]
	end

	if (self.vertical_align == M.TOP) then
		dpos[2] = self.margin[2] + self.size*1.5
	elseif (self.vertical_align == M.BOTTOM) then
		dpos[2] = self.rect.h + self.size*0.5 - self.margin[2]
	elseif (self.vertical_align == M.MIDDLE) then
		dpos[2] = self.rect.h*0.5+self.size
	end

	self.font.alignment = self.horizontal_align
	g.color(self.text_color)
	self.font:draw(dpos, self.label)
end