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
local setmetatable = setmetatable
----------------------------------------
--- Generate a lexer from a specification
-- codegen.lexer provides an interface for creating 
-- lexers and patterns over token streams from a 
-- specification.
-- @name codegen.lexer
module(...)

SPACE = R("\t\n", "\f\r", "  ")
SS = SPACE^0
SC = SPACE + V"comment"
SSC = SC^0
KWS = R"az" + R"AZ" + P"_"
KWSC = SC^1 + (-KWS)

local
function proxy_arg(arg)
	if(type(arg) == "table") then
		return arg.__proxy and arg.patt or arg
	else
		return arg
	end
end


local
lexer_token = function(patt, name)
	-- handle proxies
	if(type(patt) == "table") then
		return patt.patt and patt.patt or patt
	else
		return patt
	end
end

local
lexer_comment = function(patt, name)
	return patt
end

local
lexer_rule = function(proxy, name)
	-- all rules are proxied
	return proxy.patt
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

local
function get_rules_and_tokens(specification)
	local keywords = {}
	local tokens = {}
	local rules = {}
	
	local lexer_system = {
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
		Keyword = function(patt, name)
			keywords[name] = true
			return patt
		end,
		Token = function(patt, name)
			tokens[name] = true
			return patt
		end,
		Comment = lexer_comment,
		Rule = function(patt, name)
			rules[name] = true
			return patt
		end,
	}
	
	local f = setfenv(specification, lexer_system)
	f()
	
	return keywords, tokens, rules
end

local
function get_lexer_proxy_rule(keywords, tokens, rules)
	
	-- proxy system to insert spaces around tokens and rules without
	-- having any leading or trailing spaces on the rules themselves
	local proxy
	local proxy_meta = {
		__mul = function(a, b)
			
			-- handle corner cases like V * (V)^0 or V * (V)^-1
			if(type(b) == "table") then
				if(b.lazy) then
					return proxy(proxy_arg(a) * b.lazy())
				else
					local argb = b.__proxy and b.patt or b
					if(a.__proxy and a.type == "keyword") then
						return proxy(proxy_arg(a) * KWSC * argb)
					else
						return proxy(proxy_arg(a) * SSC * argb)
					end
				end
			else
				-- function rules don't need spaces
				if(type(b) == "function") then
					return proxy(proxy_arg(a) * b)
				else
					if(a.__proxy and a.type == "keyword") then
						return proxy(proxy_arg(a) * KWSC * b)
					else
						return proxy(proxy_arg(a) * SSC * b)
					end
				end
			end
		end,
		
		__add = function(a, b)
			return proxy(proxy_arg(a) + proxy_arg(b))
		end,
		
		__pow = function(a, b)
			local patt = proxy_arg(a)
			-- add lazy compilation so patt^N can become (SS * patt)^N
			-- when a multiply is detected
			if(b == 0) then
				local p = proxy(patt^-1 * (SSC * patt)^b)
				p.lazy = function()
					return ((SSC * patt)^-1 * (SSC * patt)^b)
				end
				return p
			elseif(b == -1) then
				local p = proxy(patt^b)
				p.lazy = function()
					return (SSC * patt)^b
				end
				return p
			elseif(b < -1) then
				local p = proxy(patt^-1 * (SSC * patt)^(b+1))
				p.lazy = function()
					return (SSC * patt)^-1 * (SSC * patt)^(b+1)
				end
				return p
			else
				return proxy(patt * (SSC * patt)^(b-1))
			end
		end,
	}
	
	local
	function rule_type(rule)
		if(keywords[rule]) then
			return "keyword"
		elseif(tokens[rule]) then
			return "token"
		elseif(rules[rule]) then
			return "rule"
		end
	end
	
	proxy = function(rule)
		local patt
		if(type(rule) == "string") then
			if(keywords[rule] or tokens[rule] or rules[rule]) then
				patt = lpeg.V(rule)
			else
				return V(rule)
			end
		else
			patt = rule
		end
		return setmetatable({
			patt = patt,
			__proxy = true,
			name = rule,
			type = rule_type(rule),
		}, proxy_meta)
	end
	
	return proxy
end

local
function create_token_grammar(keywords, tokens, grammar)
	local TOKEN
	for k, v in pairs(tokens) do
		if(TOKEN) then
			TOKEN = TOKEN + V(k)
		else
			TOKEN = V(k)
		end
	end
	for k, v in pairs(keywords) do
		TOKEN = TOKEN + V(k)
	end
	
	local TOKEN_G = {}
	for k, v in pairs(grammar) do
		if(type(v) == "userdata") then
			if(not TOKEN_G[k]) then
				TOKEN_G[k] = v
			end
		end
	end
	TOKEN_G[1] = TOKEN
	return P(TOKEN_G)
end

--- Generate a lexer system from a specification
-- @param specification The specification
-- @ret The lexer system
function generate_system(specification)
	local keywords, tokens, rules = get_rules_and_tokens(specification)
	
	local lexer_system = {
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
		Keyword = lexer_token,
		Token = lexer_token,		
		Comment = lexer_comment,
		Rule = lexer_rule,
		
	}
	
	lexer_system.V = get_lexer_proxy_rule(keywords, tokens, rules)
	
	
	lexer_system.__index = lexer_system
	local grammar = setmetatable({}, lexer_system)
	local f = setfenv(specification, grammar)
	f()
	
	-- first rule will be proxied
	grammar[1] = grammar[1].patt
	
	
	-- Comments
	local comment
	local COMMENT
	for k, v in pairs(grammar.comments) do
		if(comment) then
			comment = comment + V(k)
			COMMENT = COMMENT + v
		else
			comment = V(k)
			COMMENT = v
		end
		grammar[k] = v
	end
	grammar.comment = comment

	lexer_system.COMMENT = COMMENT
	
	
	-- Tokens
	lexer_system.TOKEN = create_token_grammar(keywords, tokens, grammar)
	
	-- sub-grammar utility functions
	lexer_system.__roots = {}
	function lexer_system:push_root(name)
		self.__roots[#self.__roots+1] = self[1]
		self[1] = self[name]
	end
	function lexer_system:pop_root()
		self[1] = self.__roots[#self.__roots]
		self.__roots[#self.__roots] = nil
	end

	return grammar
end

--- Create a lexer from a grammar
-- @param lexer_system The lexer system
-- @param grammar A table of grammar rules
-- @ret The lexer
function create_lexer(lexer_system, grammar)
	grammar = grammar or {}
	for k, v in pairs(lexer_system) do
		if(type(v) == "userdata") then
			if(not grammar[k]) then
				grammar[k] = v
			end
		end
	end
	
	local p = (P(grammar) + lexer_system.COMMENT + lexer_system.TOKEN + SPACE)^0
	return {
		match = function(self, s, ...)
			return p:match(s, ...)
		end
	}
end