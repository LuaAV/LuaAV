local ipairs = ipairs
local pairs = pairs
local type = type
local format = string.format
local print = print
local unpack = unpack

local table = table
local string = string
local lpeg = require("lpeg")
local lexer = require("doc.lexer")

local R = lpeg.R
local Ct = lpeg.Ct
local Cc = lpeg.Cc
local Cg = lpeg.Cg
local Cf = lpeg.Cf
local Cp = lpeg.Cp
local Cmt = lpeg.Cmt
local C = lpeg.C
local V = lpeg.V
local P = lpeg.P
local R = lpeg.R
local S = lpeg.S
local P = lpeg.P

--- Common language backend functionality
-- Language backends scan source files and produce module 
-- data structures.  doc.lang holds commonly used functionality 
-- across language backends for generating such structures
-- @name doc.lang
module(...)

local SPACE = S"\t "
local SS = SPACE^0
local param = P"@"*C((1 - SPACE)^0) * SS * C((1 - P("\n"))^0)

local html_block_start = {
	["<pre>"] = true,
	["<luacode>"] = true,
}
local html_block_end = {
	["</pre>"] = true,
	["</luacode>"] = true,
}

function highlight_pre(src)
	-- convert quotes
	local src = src:gsub("&quot;", '"')
	-- strip <pre> tag
	src = src:gsub("<luacode>", ''):gsub("</luacode>", '')

	local lexed = lexer(src)
	local str = {}
	for i, v in ipairs(lexed) do
		table.insert(str, string.format("<span class=%s>%s</span>", unpack(v)))
	end
	return "<pre>"..table.concat(str).."</pre>"
end

local
function search(patt)
	return (patt + P(1))^0
end

local highlight_patt = search(P"<luacode>" * Cmt((1 - P"</luacode>")^0, 
	function(s, i, t)
		return i, highlight_pre(t)
	end
) * P"</luacode>")

local highlight_patt = search(Ct(Cp() * P"<luacode>" * (1 - P"</luacode>")^0 * P"</luacode>" * Cp()))

--- Handles comment blocks for generating documentation data structures
-- @ret A function for handling comment blocks one line at a time, starting after the first line.
function document_from_comments()
	local param_set = {}
	
	return
	function(t, text)
		if(html_block_start[text]) then
			t.in_pre_tag = true
		elseif(html_block_end[text]) then
			t.in_pre_tag = nil
		end
		
		local k, v = param:match(text)
		if(k and v) then
			if(param_set[k]) then
				if(type(t[k]) == "string" and t[k]:len() < 1) then
					t[k] = v
				elseif(type(t[k]) == "table") then
					local tt = t[k]
					tt[#tt+1] = v
				else
					t[k] = {t[k], v}
				end
			else
				t[k] = v
			end
			param_set[k] = true
		else
			if(t.description:len() < 1) then
				t.description = "<p>"..text
			else
				if(text:len() < 1) then
					t.description = t.description.."</p><p>"
				else
					if(t.in_pre_tag) then
						text = text.."\n"
					end
					t.description = format("%s %s", t.description, text)
				end
			end
		end
	end
end

--- Finalizes the documentation data structure after processing comments
-- @param t The data structure
function finalize_documentation(t)
	if(t.description:len() > 1) then
		t.description = t.description.."</p>"	
	end
	
	if(t.summary:len() < 1) then
		if(t.description:len() < 1) then
			t.summary = "[NO SUMMARY]"
			t.description = "[NO DESCRIPTION]"
		else
			t.summary = t.description
		end
	end
	
	t.description = t.description:gsub("<p></p>", "")
	
	if(t.description:len() < 1) then
		t.description = "<p>"..t.summary.."</p>"
	end
	
	local res = {highlight_patt:match(t.description)}
	if(#res >= 1) then
		if(type(res[1]) == "table") then
			local description = t.description:sub(1, res[1][1]-1)
			for i=1, #res do
				local pos = res[i]
				local chunk = t.description:sub(pos[1], pos[2]-1)
				description = description..highlight_pre(chunk)
				local next_pos1
				if(i < #res) then
					next_pos1 = res[i+1][1]
				end
				description = description..t.description:sub(pos[2], next_pos1)
			end
			t.description = description
		end
	end
end

--- Puts a listing (array of items) in canonical format
-- @param t The listing
-- @ret The canonicalized listing
function canonicalize_listing(t)
	local tt = {}
	for i, elem in ipairs(t) do
		tt[i] = elem.name
		tt[elem.name] = elem
	end
	return tt
end

--- Canonicalizes a function name (e.g. M:func -> module:func)
-- M is a special token that represents the module's name.  If it's 
-- not present, the current function's name is returned.
-- @param module The module
-- @param f The Function
-- @ret The canonicalized name
function canonicalize_function_name(module, f)
	local modname = module.name:match("([^%.]+)$")
	local prefix, access, fname = f.name:match("^([^%.:]+)([%.:])(.+)")
	if(access and fname) then
		if(	f.function_statement and 
			f.function_statement.params[1] == "self" and 
			access == "."
		) then
			f.param.self = nil
			return format("%s:%s", modname, fname)
		else
			if(prefix == "M") then
				return format("%s%s%s", modname, access, fname)
			else
				return f.name
			end
		end
	else
		return f.name
	end
end

--- Canonicalizes function parameters
-- @param f The fuction
-- @ret The canonicalized parameters
function canonicalize_params(f)
	local params = {}
	if(type(f.param) == "string") then
		local name, description = f.param:match("([^%s]+)%s+(.+)")
		if(not name) then
			name = f.param
			description = "[NO DESCRIPTION]"
		end
		params[name] = description
	elseif(f.param) then
		for i, p in ipairs(f.param) do
			local name, description = p:match("([^%s]+)%s+(.+)")
			params[name] = description
		end
	end
	
	for i, p in ipairs(f.function_statement.params or {}) do
		if(not (i==1 and p == "self")) then
			params[#params+1] = p
			
			if(p == "...") then
				params[p] = params[p] or "varargs"
			elseif(not params[p]) then
				params[p] = "[NO DESCRIPTION]"
			end
		end
	end
	
	local field_params = {}
	for k, v in pairs(params) do
		if(type(k) ~= "number") then
			local tab, field = k:match("([%w_]+)(%.[%w_]+)")
			if(not tab) then
				tab, field = k:match("([%w_]+)(%[[%d]+%])")
				--print(tab, field)
			end
			
			if(tab and field) then
				if(params[tab]) then
					local fields = field_params[tab] or {}
					fields[#fields+1] = field
					field_params[tab] = fields
				end
			end
		end
	end
	
	for tab, fields in pairs(field_params) do
		table.sort(fields)
	end
	
	for tab, fields in pairs(field_params) do
		local tab_idx = 1
		while(params[tab_idx] ~= tab) do
			tab_idx = tab_idx+1
		end
		
		for i, field in ipairs(fields) do
			table.insert(params, tab_idx+1, tab..field)
			tab_idx = tab_idx+1
		end
	end
	
	return params
end