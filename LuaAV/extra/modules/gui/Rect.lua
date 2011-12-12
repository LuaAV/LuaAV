local assert = assert
local getfenv = getfenv
local setmetatable = setmetatable
local pairs = pairs
local print = print
local type = type
-------------------------------------
--- Rectangles for managing widget extents
-- @name gui.Rect
module(...)

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

--- Constructor
-- @param x x-coordinate
-- @param y y-coordinate
-- @param w Width
-- @param h Height
-- @name Rect
function C:__call(x, y, w, h)
	if(type(x) == "table") then
		if(x[1]) then
			return {
				x = x[1],
				y = x[2],
				w = x[3],
				h = x[4]
			}
		else
			return {
				x = x.x,
				y = x.y,
				w = x.w,
				h = x.h
			}
		end
	else
		return {
			x = x,
			y = y,
			w = w,
			h = h
		}
	end
end

--- Check if a point is within a rectangle
-- @param r The rectangle
-- @param x x-coodinate
-- @param y y-coodinate
-- @ret True/False result
function within(r, x, y)
	return	r.x <= x and 
			r.x+r.w > x and
			r.y <= y and 
			r.y+r.h > y
end