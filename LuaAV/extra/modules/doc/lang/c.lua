local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local unpack = unpack
local setmetatable = setmetatable
local print = print
----------------------------------------------------------
local math = require 'math'
local table = require 'table'
local string = require 'string'
local io = require 'io'

local lang = require("doc.lang")

local min = math.min
----------------------------------------------------------
local c99 = require("ceg.c99")
local ceg = require("ceg")
local m = require("lpeg")
local serializer = require("serializer")
local util = require("doc.util")
----------------------------------------------------------
local format = string.format
local P, V, R, S, C = m.P, m.V, m.R, m.S, m.C
local Cmt = m.Cmt
local Cs = m.Cs
----------------------------------------------------------
module(...)
----------------------------------------------------------

c99.typedefs.lua_State = true

----------------------------------------------------------
-- Module Pattern
local module_rules = ceg.combine({
			[1] = V"comment",
		}, 
		{
			comment = c99.comment,
		},
		c99.all_rules
	)

local module_captures = {
	comment = function(v)
		if(v:find("@module")) then
			return v
		end
	end,
}
local module_patt = ceg.scan(ceg.apply(module_rules, module_captures))
----------------------------------------------------------

----------------------------------------------------------
-- Glboal Function Patternm
local function_rules = ceg.combine({
				[1] = V"followed",
				followed = V"comment" * c99.SS * 
							(V"function_definition" + V"declaration" + V"prefunction"),

				-- useful for C++ compound statements that have C++ code in them when matching 
				-- function definitions
				prefunction =	V"declaration_specifiers" * c99.SS * 
								V"declarator" * (c99.SS * V"declaration_list")^-1,
								
				function_definition = C(c99.function_definition),
			}, 
			{
				comment = c99.comment,
				keyword = c99.keyword,
			},
			c99.all_rules
		)

local function_captures = {
	comment = function(v)
		if(v:find("/%*!")) then
			return v
		end
	end,
	
	prefunction = function(v)
		return v
	end,

	function_definition = function(v)
		return v
	end,

	declaration = function(v)
		return v
	end,

	followed = function(...)
		local v = {...}

		local c = v[#v-1]
		local f = v[#v]
		if(c and not c:find("@private")) then
			return {
				comment = c,
				["function"] = f,
			}
		end
	end,
}

local function_patt = ceg.scan(ceg.apply(function_rules, function_captures))


local function2_rules = ceg.combine({
			[1] = V"comment",
		}, 
		{
			comment = c99.comment,
		},
		c99.all_rules
	)

local function2_captures = {
	comment = function(v)
		if(v:find("@function")) then
			return v
		end
	end,
}
local function2_patt = ceg.scan(ceg.apply(function2_rules, function2_captures))
----------------------------------------------------------



local NEWLINE = P"\n"
local remove_comment_markers = P"/*" * P"!"^-1 * C((1-P"*/")^0)
local split_line = C((1-NEWLINE)^0) * NEWLINE * C(P(1)^0)

local SPACE = S"\t \n"
local ID = (R"az" + R"AZ" + P"_" + R"09")^1
local FARGS = P"(" * (1-P")")^0 * P")"
local fname = (C(ID) * SPACE^0 * FARGS + ID + SPACE^1)^0

local
function document_from_comments(t, comments)
	local doc_handler = lang.document_from_comments()
	local within_pre = false
	local npre = 0
	local tab = "   "
	for line in comments:gmatch("([^\n]*)\n?") do
		line = line:gsub("\t", tab)
	
		if(line:match(".*(<pre[^>]*>)")) then
			npre = line:match("(.*)<pre[^>]*>"):len()
			line = "\n"..line:match(".*(<pre[^>]*>.*)").."\n"
			within_pre = true
		elseif(line:match(".*(</pre>)")) then
			line = line:match("%s*([^<]*</pre>.*)").."\n"
			within_pre = false
		elseif(within_pre) then
			line = line:sub(npre).."\n"
		else
			line = line:match("%s*(.*)")
			line = line:gsub("@module", "@name")
		end
		
		doc_handler(t, line)
	end
	
	lang.finalize_documentation(t)
end

local highlight_start = P"<luacode>"
local highlight_end = P"</luacode>"

local highlight_patt = Cmt(highlight_start *Cmt(
		(1-highlight_end)^0, function(s, i, t)
			return i, util.highlight(t)
		end
	)* highlight_end, function(s, i, t)
		return i, format([[<pre class="fragment">%s</pre>]], t)
	end)
	highlight_patt = Cs((highlight_patt + 1)^0)

local
function highlight_luacode(comment)
	return highlight_patt:match(comment)
end

local
function preprocess_comment_block(comment)
	local i = 1
	local in_param = false
	local pcomment = ""
	for line in comment:gmatch("([^\n]*)\n?") do
		if(line:match("%s*(@)")) then
			if(in_param) then
				pcomment = pcomment.."\n"..line
			else
				pcomment = pcomment..line
			end
			in_param = true
		elseif(line:match("(%s*)") == line) then
			if(in_param) then
				in_param = false
				pcomment = pcomment.."\n"..line.."\n"
			else
				pcomment = pcomment..line.."\n"
			end
		elseif(in_param) then
			line = line:match("%s*(.*)")
			pcomment = pcomment.." "..line
		else
			pcomment = pcomment..line.."\n"
		end
	end
	
	local patt = Cmt(highlight_start *Cmt(
		(1-highlight_end)^0, function(s, i, t)
			return i, util.highlight(t)
		end
	)* highlight_end, function(s, i, t)
		return i, format([[<pre class="fragment">%s</pre>]], t)
	end)
	patt = Cs((patt + 1)^0)
	pcomment = highlight_luacode(pcomment)
	
	return pcomment
end

local
function param_names(params)
	local pp = {}
	if(type(params) == "string") then
		pp[1] = params:match("%s*([^%s]+)")
	else
		for i, p in ipairs(params) do
			pp[i] = p:match("%s*([^%s]+)")
		end
	end
	return pp
end

function global_functions(module, code)
	local functions = {function_patt:match(code)}
	if(#functions == 1 and type(functions[1]) == "number") then
		return {}
	end
	local functions2 = {function2_patt:match(code)}
	for i, v in ipairs(functions2) do
		if(type(v) ~= "number") then
			functions[#functions+1] = {
				comment = v,
				["function"] = "satic NO_NAME(lua_State *L)"
			}
		end
	end
	
	local documented_functions = {}
	for i, f in ipairs(functions) do
		local df = {}
		df.name = fname:match(f["function"])
		df.description = ""
		
		if(not f.comment:find("@module", nil, true)) then
			
			local comment = remove_comment_markers:match(f.comment)
			local summary, rest = split_line:match(comment)
			df.summary = summary
			
			rest = preprocess_comment_block(rest)
			document_from_comments(df, rest)
			df.name = lang.canonicalize_function_name(module, df)
			df.ret = df.ret or {}
			df.param = df.param or {}
			df.function_statement = {
				params = param_names(df.param)
			}
			df.param = lang.canonicalize_params(df)
			
			documented_functions[#documented_functions+1] = df
		end
	end
	
	return documented_functions
end

function split(code)
	local mod_comments = {module_patt:match(code)}
	if(#mod_comments > 1) then
		local locs = {}
		for i, comment in ipairs(mod_comments) do
			local loc = code:find(comment, nil, true)
			locs[#locs+1] = loc
		end
		
		local subcodes = {}
		for i, loc in ipairs(locs) do
			local subcode = code:sub(loc, (locs[i+1] or code:len()) - 1)
			subcodes[#subcodes+1] = subcode
		end
		return subcodes
	else
		return {code}
	end
end

function modules(code)
	local subcodes = split(code)
	local mods = {}
	for _, subcode in ipairs(subcodes) do
		mods[#mods+1] = module(subcode)
	end
	return mods
end

function module(code)
	local mod_comment = module_patt:match(code)
	mod_comment = remove_comment_markers:match(mod_comment)

	local summary, rest = split_line:match(mod_comment)	
	local mod = {
		summary = summary,
		description = "",
	}

	rest = highlight_luacode(rest)	
	document_from_comments(mod, rest)

	mod.functions = lang.canonicalize_listing(
		global_functions(mod, code)
	)

	return mod
end