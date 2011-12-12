local lpeg = require("lpeg")
----------------------------------------
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
----------------------------------------
local table = require("table")
local format = string.format
----------------------------------------
local getfenv = getfenv
local setfenv = setfenv
local pairs = pairs
local ipairs = ipairs
local type = type
local error = error
local print = print
----------------------------------------
--- Generate a parser from a specification
-- codegen.parser provides an interface for creating 
-- parser and patterns over token streams from a 
-- specification.
-- @name codegen.parser
module(...)

local space = R("\t\n", "\f\r", "  ")
local SS = space^0

local
parser_token = function(patt, name)
	return SS * Ct(
		Cg(Cp(), "start_idx") * 
		C(patt) * 
		Cg(Cp(), "end_idx") * 	
		Cg(Cc(name), "token")
	) * SS
end

local
parser_comment = function(patt, name)
	return SS * Ct(
		Cg(Cp(), "start_idx") * 
		C(patt) * 
		Cg(Cp(), "end_idx") * 
		Cg(Cc(name), "comment")
	) * SS
end

local
parser_rule = function(patt, name)
	return Ct(patt * Cg(Cc(name), "rule"))
end


local
function lines(subject)
	local inc = function(l) return l+1 end
	local L = Cf(Cc(1)*(P'\n'*Cc(1) + P(1))^0, inc)
	return L:match(subject)
end

local
function getline(subject, line)
	local n = 0
	local count = function(a, b)
		n = n+1

		if(n >= line) then
			return a
		else
			return b
		end	
	end
	local L = Cf( (C((1 - P"\n")^0) * P"\n")^0, count)
	return L:match(subject)
end


local
function CHECK_SYNTAX(patt, msg, test)
	-- P(function) returns a match-time capture over the empty string
	local error_patt = P(function(s, i)
		local n = lines(s:sub(1, i))
		local line = getline(s, n)
		error(format([[
syntax error at line %d: %s
	%s
]], n, msg, line))
	end)
	
	if(patt) then
		return patt + error_patt
	else
		return error_patt
	end
end


--- Generate a parser system from a specification
-- @param specification The specification
-- @ret The parser system
function generate_system(specification)
	local parser_system = {
		-- PEG operators to build tokens
		V = V,
		P = P,
		R = R,
		S = S,
		C = C,
		Cmt = Cmt,
		
		-- Lua functions to process PEG tokens
		getfenv = getfenv,
		pairs = pairs,
		ipairs = ipairs,
		
		print = print,
		
		-- syntax error checking function
		CHECK_SYNTAX = CHECK_SYNTAX,
		
		-- Capture functions to generate AST
		Keyword = parser_token,
		Token = parser_token,
		Comment = parser_comment,
		Rule = parser_rule,
	}
	
	local f = setfenv(specification, parser_system)
	f()
	
	parser_system.__roots = {}
	
	function parser_system:push_root(name)
		self.__roots[#self.__roots+1] = self[1]
		self[1] = self[name]
	end
	
	function parser_system:pop_root()
		self[1] = self.__roots[#self.__roots]
		self.__roots[#self.__roots] = nil
	end
	
	return parser_system
end

--- Create a parser from a grammar
-- @param parser_system The parser system
-- @ret The parser
function create_parser(parser_system)
	local grammar = {}
	for k, v in pairs(parser_system) do
		if(type(v) == "userdata") then
			grammar[k] = v
		end
	end
	
	local comments
	for k, v in pairs(parser_system.comments) do
		if(comments) then
			comments = comments + v
		else
			comments = v
		end
	end
	
	local cp = (comments + P(1))^0
	local p = (P(grammar) + space)^0
	return {
		match = function(self, s, ...)
			local casts = { cp:match(s, ...) }
			
			local ss
			if(type(casts[1]) == "table") then
				local prev_cast
				
				for i, cast in ipairs(casts) do
					if(prev_cast) then
						ss = ss..s:sub(prev_cast.end_idx+1, cast.start_idx-1)
						prev_cast = cast
					else
						ss = s:sub(1, cast.start_idx-1)
						prev_cast = cast
					end
				end
				if(prev_cast) then
					ss = ss..s:sub(prev_cast.end_idx+1)
				else
					ss = s
				end
			else
				ss = s
			end
		
			local ast = p:match(ss, ...)
			ast.comments = casts
			return ast
		end
	}
end