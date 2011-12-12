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
module(...)

local Superclass = View

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = function(t, k)
	return M[k] or Superclass[k]
end

function C:__call(init)		
	local m = setmetatable(Superclass(init), M)
	return m
end


function M:draw()
end
