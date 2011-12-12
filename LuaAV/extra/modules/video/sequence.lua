local setmetatable = setmetatable
local getfenv = getfenv
local assert = assert
local tostring = tostring
local print = print
local string = string
local format = string.format

local app_utils = require("app.utilities")
local Image = require("Image")

module(...)


local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

function C:__call(init)
	assert(init.path)
	assert(init.name)
	local m = setmetatable(init, M)
	return m
end


function M:startrecord()
	self.abspath = format("%s/%s", self.path, self.name:gsub("%s", "."))
	app_utils.ensurepath(self.abspath)
	self.idx = 1
end

local
function frame_number(f)
	local n = tostring(f)
	if(n:len() < 5) then
		return string.rep("0", 5-n:len())..n
	else
		return n
	end
end

function M:frame(tex)
	local filename = format("%s/frame%s.png", self.abspath, frame_number(self.idx))
	self.idx = self.idx+1

	tex:toarray()
	local img = Image()
	img:fromarray(tex:array())
	img:save(filename)
end