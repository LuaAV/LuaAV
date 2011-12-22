local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local print = print
local unpack = unpack
-------------------------------------
local g = require("gui.draw")
local rect = require("gui.Rect")
-------------------------------------
--- A layer within the gui
-- Within a gui.context, there are several layers for 
-- distinguising between different classes of widgets.
-- Layers are typically overlaid on top of each other 
-- with the context testing them in order of priority for 
-- whether widgets in any particular layer receive an event.  
-- For example popup menus are usually high priority and 
-- would be the top-most layer with standard widgets 
-- getting events only if something in the popup layer doesn't
-- take it.
-- @name gui.Layer
module(...)

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

--- Constructor
-- @param init Constructor properties
-- @param init.context The layer's context
-- @name Layer
function C:__call(init)
	assert(init.context)
	local m = setmetatable(init, M)
	m.views = {}
	
	return m
end

--- Get the subview of a view at a particular point
-- @param view The view to examine
-- @param x x-coordinate
-- @param y y-coodinate
-- @ret The subview
function M:subview_at_point(v, x, y)
	for kk, vv in pairs(v.views) do
		if(vv.enabled and rect.within(vv.rect, x, y)) then
			return self:subview_at_point(vv, x, y)
		end
	end
	
	return v
end

--- Get the view at a particular point
-- @param x x-coordinate
-- @param y y-coodinate
-- @ret The view
function M:view_at_point(x, y)
	for k, v in pairs(self.views) do
		if(v.enabled and rect.within(v.rect, x, y)) then
			return self:subview_at_point(v, x, y)
		end
	end
end

--- Draw subviews of a view
-- @param vv The view
-- @param dim The dimensions of the context
-- @param wx The containing widget's width
-- @param wy The containing widget's height
function M:draw_subviews(vv, dim, wx, wy)
	for k, v in pairs(vv.views) do
		if(v.enabled and v.visible) then
			g.push()
				
				g.scissor(
					wx+v.rect.x, dim[2]-(wy+v.rect.y+v.rect.h), 
					v.rect.w, v.rect.h
				)
			
				local colors = v.colors or self.context.colors
				local r = v.rect
				g.translate(r.x+v.offset[1], r.y+v.offset[2], 0)
				
				if(v.border) then
					g.rect_color(
						colors.border_top, colors.border_bot,
						g.LineLoop, 0, 0, r.w, r.h
					)
				end
				if(v.background) then
					g.color(colors.background)
					g.rect(g.Quads, 0, 0, r.w, r.h)
				end
				v:draw()
				self:draw_subviews(v, dim, wx+v.rect.x, wy+v.rect.y)
				
			g.pop()
		end
	end
end

--- Draw the layer
function M:draw()
	g.enable(g.Scissor)
	
	local dim = self.context.dim
	for k, v in pairs(self.views) do
		if(v.enabled and v.visible) then
			g.push()
				
				g.scissor(
					v.rect.x, dim[2] - (v.rect.y+v.rect.h), 
					v.rect.w, v.rect.h
				)
			
				local colors = v.colors or self.context.colors
				local r = v.rect
				g.translate(r.x+v.offset[1], r.y+v.offset[2], 0)
				
				if(v.border) then
					g.rect_color(
						colors.border_top, colors.border_bot,
						g.LineLoop, 0, 0, r.w, r.h
					)
				end
				if(v.background) then
					g.color(colors.background)
					g.rect(g.Quads, 0, 0, r.w, r.h)
				end
				
				v:draw()
				self:draw_subviews(v, dim, v.rect.x, v.rect.y)
			g.pop()
		end
	end

	g.scissor(0, 0, self.context.dim[1], self.context.dim[2])
	g.disable(g.Scissor)
end

--- Add a view to the layer
-- @param v The view to add
function M:add_view(v)
	v.context = self.context
	self.views[#self.views+1] = v
	v:notify("add")
end
