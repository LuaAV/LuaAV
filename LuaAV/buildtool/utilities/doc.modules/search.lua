local apputil = require("app.utilities")
local printt = apputil.printt

local math = math
local string = string
local format = string.format
local table = table
local print = print
local setmetatable = setmetatable
local getfenv = getfenv
local assert = assert
local ipairs = ipairs
local pairs = pairs

local Chunk = require("codegen.chunk")
local templates = require("templates")
local utils = require("utilities")
local substitute = utils.substitute
local write_file = utils.write_file

local docroot = docroot

module(...)

local replace = {
	['_'] = '_5f',
	['.'] = '_2e',
	[':'] = '_3a',
}

local hex = {
	[0] = '0', 
	[1] = '1', 
	[2] = '2', 
	[3] = '3', 
	[4] = '4', 
	[5] = '5', 
	[6] = '6', 
	[7] = '7', 
	[8] = '8', 
	[9] = '9', 
	[10] = 'a', 
	[11] = 'b', 
	[12] = 'c', 
	[13] = 'd', 
	[14] = 'e', 
	[15] = 'f',
}

local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

function C:__call(init)
	local m = setmetatable(init or {}, M)
	m.terms = {}
	return m
end

function M:add_search_term(name, term, link)
	self.terms[#self.terms+1] = {
		name = name,
		term = term,
		link = link
	}
end

function M:add(str, link)
	self:add_search_term(str, str, link)
	for term in str:gmatch("[%w_]+") do
		if(str ~= term) then
			self:add_search_term(str, term, link)
		end
	end
end

local
function hexcode(c)
	local v = string.byte(c)
	local hi = math.floor(v/16)
	local lo = v-16*hi
	return hex[hi]..hex[lo]
end

local
function search_filename(c)
	return format("all_%s.html", hexcode(c))
end

function M:generate()
	local bins = {}
	for i, term in ipairs(self.terms) do
		local c = term.term:sub(1, 1):lower()
		local bin = bins[c]
		if(not bin) then
			bins[c] = {}
			bin = bins[c]
		end
		bin[#bin+1] = term
	end
	for c, bin in pairs(bins) do
		local filename = search_filename(c)
		local search_page = Chunk(templates.search_page)
		
		table.sort(bin, function(a, b)
			return a.name:lower() < b.name:lower()
		end)
		
		for i, term in ipairs(bin) do
			--printt(term)
			
			local item = substitute(templates.search_item, {
				idx = i-1,
				escaped_name = term.term:gsub("(.)", replace),
				name = term.name,
				link = "../"..term.link,
			})
			search_page.items:write(item)
		end
		write_file(docroot.."/search/"..filename, search_page:synthesize())
	end
	
	local binflags = {}
	for i=0, 255 do
		local c = string.char(i)
		if(bins[c]) then
			binflags[i+1] = '1'
		else
			binflags[i+1] = '0'
		end
	end
	local flags = table.concat(binflags)
	local search_js = substitute(templates.search_template, {
		indices = flags,
	})
	write_file(docroot.."/search/search.js", search_js)
end