local Highlighter = require("codepad.highlighter")
local Debugger = require("codepad.debugger")
local CodePad = require("app.CodePad")

local pairs = pairs
local ipairs = ipairs
local type = type
local print = print
local loadstring = loadstring
local pcall = pcall
local tonumber = tonumber

module(..., package.seeall)

local C = {}
local M = setmetatable(getfenv(), C)

local highlight_rules = {
	{
		name = "default",
		color = {0, 0, 0},
	},
	
	{
		name = "keyword",
		weight = 60,
		color = {0.3, 0.3, 0.8},
	},
	
	{
		name = "comment",
		weight = 30,
		color = {0, 0.63, 0.14},
	},
	
	{
		name = "number",
		weight = 40,
		color = {0, 0.5, 0.8},
	},
	
	{
		name = "string",
		weight = 40,
		color = {0.8, 0.1, 0.1},
	},
	
	{
		name = "start_long",
		weight = 40,
		color = {0.8, 0.1, 0.1},
	},
}


function C:__call(...)
	
	local m = CodePad(...)
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	-- syntax highlighter
	m.highlighter = Highlighter{
		codepad = m,
	}
	for i, r in ipairs(highlight_rules) do
		m:create_format(r)
	end
	
	-- debugger
	m.debugger = Debugger()
	
	return m
end

local nchunks = 0
function M:eval(code)
	self:highlight_error(nil)

	print("---------------")
	print(code)
	print("---------------")
	local name = "chunk"..nchunks
	local f, err = loadstring(code, name)
	self.debugger:register_chunk(name, code)
	nchunks = nchunks+1
	if(not f) then
		print("error loading code ", err)
	else
		local ok, err = pcall(f)
		if(not ok) then
			print("error running code ", err)

			local s1, s2 = self:selection()

			local error_line = s1[1] + tonumber(err:match("%]:(%d+):")) - 1
			self:highlight_error(error_line)

			self.debugger:debug(self, f)
		end
	end
end