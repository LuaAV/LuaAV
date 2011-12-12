local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local print = print
local tostring = tostring

local min = math.min
local max = math.max
local floor = math.floor

local string = string
local go = go
local wait = wait
local event = event
local now = now

local co = coroutine
-------------------------------------
local g = require("gui.draw")
local Rect = require("gui.Rect")
local View = require("gui.View")
-------------------------------------
--- Display text in a gui
-- @name gui.Textfield
module(...)

local Superclass = View

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = function(t, k)
	return M[k] or Superclass[k]
end

--- Constructor
-- @param init Constructor properties
-- @param init.text The text label to show
-- @param init.size The font size (default 10)
-- @name Textfield
function C:__call(init)	
	init.text = init.text or ""
	init.size = init.size or 10
	init.margin = init.margin or {2, 2}
--	init.border = false
--	init.background = false
	
	local m = setmetatable(Superclass(init), M)
	m.focused = false
	m.cursor = m.text:len()
	m.selection = {
		m.cursor, m.cursor, 
		pivot = m.cursor,
		dragging = false,
		did_drag = false,
		was_extended = false
	}
	m.event_name = tostring(m)
	m:register("add", m.notify_add)
	return m
end

function M:set_text(v)
	self.text = v
	self:set_cursor(v:len())
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

function M:mouse_enter()
	self.context:set_cursor_style(self.context.IBEAM)
end

function M:mouse_exit()
	self.context:set_cursor_style(self.context.ARROW)
end

function M:gain_focus()
	self.focused = true
	go(self.event_name, function()
		while(self.focused) do
			local N = 16
			for i=1, N do
				self:draw_cursor(self.cursor)
				local _, reset = wait(self.event_name)
				if(reset) then
					N = 0
					break
				end
			end
			
			for i=1, N do
				local res, reset = wait(self.event_name)
				if(reset) then
					N = 0
					break
				end
			end
		end
	end)
end

function M:loose_focus()
	self.focused = false
end

function M:add_text_at_cursor(text)
	if(self:has_selection()) then
		self:backspace_selection()
	end

	self.text = self.text .. text
	self:move_cursor(text:len())
end

function M:backspace_selection()
	self.cursor = self.selection[2]
	self:backspace_text_at_cursor(self.selection[2]-self.selection[1])
end

function M:backspace_text_at_cursor(n)
	self.text = self.text:sub(0, self.cursor-n)..self.text:sub(self.cursor+1)
	self:move_cursor(-n)
end

function M:clamp_idx(idx)
	return max(0, min(idx, self.text:len()))
end

function M:shift_cursor(n)
	self.cursor = self:clamp_idx(self.cursor+n)
	event(self.event_name, true)
end

function M:place_cursor(idx)
	self.cursor = self:clamp_idx(idx)
	event(self.event_name, true)
end

function M:move_cursor(n)
	if(self:has_selection()) then
		if(n < 0) then
			self.cursor = self.selection[1]
		else
			self.cursor = self.selection[2]
		end
	else
		self.cursor = self:clamp_idx(self.cursor+n)
	end
	self:initialize_selection(self.cursor)
	event(self.event_name, true)
end

function M:set_cursor(idx)
	self.cursor = self:clamp_idx(idx)
	self:initialize_selection(self.cursor)
	event(self.event_name, true)
end

function M:char_at_loc(x)
	local text_pix = x-self.margin[1]
	return self:clamp_idx(floor(text_pix/self.size*2))
end

local key_map = {
	[127] = "backspace",
	[13] = "enter",
	[9] = "tab",
	
	[63232] = "up",
	[63233] = "down",
	[63234] = "left",
	[63235] = "right",
}

function M:key_down(k)
	local special_key = key_map[k]
	if(special_key) then
		if(special_key == "backspace") then
			if(self:has_selection()) then
				self:backspace_selection()
			else
				self:backspace_text_at_cursor(1)
			end
		elseif(special_key == "tab") then
			self:add_text_at_cursor(string.rep(" ", 3))
		elseif(special_key == "up" or special_key == "left") then
			if(self.context.keyboard_state.shift) then
				self:extend_selection_rel(-1)
			else
				self:move_cursor(-1)
			end
		elseif(special_key == "down" or special_key == "right") then
			if(self.context.keyboard_state.shift) then
				self:extend_selection_rel(1)
			else
				self:move_cursor(1)
			end
		elseif(special_key == "enter") then
			self:notify("value")
		end
	else
		self:add_text_at_cursor(string.char(k))
	end
end

function M:key_up(k)

end

function M:initialize_selection(idx)
	self.selection[1] = idx
	self.selection[2] = idx
	self.selection.pivot = idx
end

function M:initialize_selection_drag(idx)
	self.selection.dragging = true
end

function M:extend_selection_rel(n)
	if(n < 0) then
		if(self.selection[2] == self.selection.pivot) then
			self.selection[1] = self:clamp_idx(self.selection[1]-1)
		else
			self.selection[2] = self:clamp_idx(self.selection[2]-1)
		end
	else
		if(self.selection[1] == self.selection.pivot) then
			self.selection[2] = self:clamp_idx(self.selection[2]+1)
		else
			self.selection[1] = self:clamp_idx(self.selection[1]+1)
		end	
	end
	self.selection.was_extended = true
	self:shift_cursor(n)
end

function M:cut_selection_and_paste_at(idx, text)
	local paste_idx
	if(idx <= self.selection[1]) then
		paste_idx = idx
	else
		paste_idx = idx - (self.selection[2] - self.selection[1])
	end
	if(text) then
		self:cut_selection()
	else
		text = self:cut_selection()
	end
	self:place_cursor(paste_idx)
	self:initialize_selection(idx)
	self:paste(text)
	self.selection[1] = paste_idx
	self.selection[2] = paste_idx+text:len()
end

function M:cut_selection()
	local text = self.text:sub(self.selection[1]+1, self.selection[2])
	local dual_text = self.text:sub(0, self.selection[1])..self.text:sub(self.selection[2]+1)
	self.text = dual_text
	return text
end

function M:split_at_cursor()
	return self.text:sub(0, self.cursor), self.text:sub(self.cursor+1)
end

function M:copy()
	return self.text:sub(self.selection[1]+1, self.selection[2])
end

function M:cut()
	local text = self:cut_selection()
	self:set_cursor(self.selection[1])
	return text
end

function M:paste(text)
	if(self:has_selection()) then
		self:cut_selection_and_paste_at(self.selection[1], text)
	else
		local t1, t2 = self:split_at_cursor()
		self.text = t1..text..t2
	end
end

function M:selectall()
	self.selection[1] = 0
	self.selection[2] = self.text:len()
end

function M:extend_selection(idx)
	if(idx < self.selection.pivot) then
		self.selection[1] = idx
	else
		self.selection[2] = idx
	end
	self.selection.was_extended = true
	self:place_cursor(idx)
end

function M:has_selection()
	return self.selection[1] ~= self.selection[2]
end

function M:within_selection(idx)
	return	idx >= self.selection[1] and 
			idx <= self.selection[2]
end

function M:mouse_down(btn, x, y)
	local idx = self:char_at_loc(x)
	self.selection.was_extended = false
	if(
		self:has_selection() and 
		self:within_selection(idx)
	) then
		self.context:set_cursor_style(self.context.ARROW)
		self:initialize_selection_drag(idx)
	else
		self:initialize_selection(idx)
	end
end

function M:mouse_drag(btn, x, y)
	local idx = self:char_at_loc(x)
	if(self.selection.dragging) then
		self.selection.did_drag = true
		if(self:within_selection(idx)) then
			self.drag_cursor = nil
		else
			self.drag_cursor = idx
		end
	else
		self:extend_selection(idx)
	end
end

function M:mouse_up(btn, x, y)
	local idx = self:char_at_loc(x)
	if(self.selection.did_drag) then
		if(self.drag_cursor) then
			self:cut_selection_and_paste_at(self.drag_cursor)
		end
		self.context:set_cursor_style(self.context.IBEAM)
		self:place_cursor(idx)
	elseif(self.selection.was_extended) then
		self:place_cursor(idx)
	else
		self:set_cursor(idx)
	end
	
	self.selection.dragging = false
	self.selection.did_drag = false
	self.selection.was_extended = false
end

function M:pix_to_char(idx)
	local text_to_cursor = self.text:sub(0, idx)
	return self.margin[1] + self.font.font:width(text_to_cursor)
end

function M:draw_cursor(idx)
	local x = self:pix_to_char(idx)
	g.line(
		x, 
		self.margin[2], 
		x, 
		self.rect.h-self.margin[2]
	)
end

--- Draw the label
function M:draw()
	if(self:has_selection()) then
		g.color(0.6, 0.6, 0.6, 0.5)
		local x, y = self:pix_to_char(self.selection[1]), self.margin[2]
		local x2, y2 = self:pix_to_char(self.selection[2]), self.rect.h-self.margin[2]
		g.rect(
			g.Quads, 
			x, y,
			x2-x, y2-y
		)
		
		if(self.selection.dragging and self.drag_cursor) then
			g.color(1, 1, 1, 1)
			self:draw_cursor(self.drag_cursor)
		end
	else
		g.color(1, 1, 1, 1)
		event(self.event_name)
	end

	g.color(1, 1, 1, 1)
	self.font:draw({self.margin[1], self.rect.h*0.5}, self.text)
end