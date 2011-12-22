local setmetatable = setmetatable
local getfenv = getfenv
local assert = assert
local ipairs = ipairs
local print = print

local table = table
local utils = require("utilities")
local substitute = utils.substitute
local Chunk = require("codegen.chunk")

local templates = require("templates")

module(...)


local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M


function C:__call(init)
	assert(init.filename)
	assert(init.title)
	assert(init.content)
	assert(init.content.title)
	assert(init.content.text)
	
	local m = setmetatable(init, M)
	return m
end

function M:generate()
	local page_template = Chunk(templates.html_template)
	local content_template = Chunk(templates.html_content_template)
	
	local content = {
		title = self.content.title,
		summary = self.content.summary or "",
		text = self.content.text,
	}
	
	return substitute(page_template:synthesize(), {
		title = self.title,
		filename = self.filename,
		relpath = utils.relative_path(self.filename, ""),
		header_image = templates.header_image,
		tabs = self.tabcode,
		content = substitute(content_template:synthesize(), content),
	})
end

function M:tabs(tabs, active, inactive)
	local code = {}
	for i, tab in ipairs(tabs) do
		if(self.filename == tab.filename) then
			code[#code+1] = utils.substitute(active, tab)
		else
			code[#code+1] = utils.substitute(inactive, tab)
		end
	end
	self.tabcode = table.concat(code, "\n")
end

function page_by_title(pages, title)
	for i, page in ipairs(pages) do
		if(page.title == title) then
			return page
		end
	end
end