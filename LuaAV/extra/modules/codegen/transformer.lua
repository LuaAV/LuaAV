local lpeg = require("lpeg")
local listlpeg = require("listlpeg")
----------------------------------------
local Ct = listlpeg.Ct
local Cc = listlpeg.Cc
local Cg = listlpeg.Cg
local Cmt = listlpeg.Cmt
local C = listlpeg.C
local V = listlpeg.V
local P = listlpeg.P
local S = listlpeg.S
local R = listlpeg.R
local P = listlpeg.P
local L = listlpeg.L
----------------------------------------
local table = require("table")
local format = string.format
----------------------------------------
local getfenv = getfenv
local setfenv = setfenv
local pairs = pairs
local ipairs = ipairs
local type = type
local assert = assert
local loadstring = loadstring
local print = print
local unpack = unpack
local getmetatable = getmetatable
local setmetatable = setmetatable

local max = math.max
----------------------------------------
--- Create AST transformations and serializers
-- This module contains ways of generating and operating on syntax trees.  It defines 
-- functions for generating an AST from Token and Rule definitions, rules for searching 
-- over an AST for a pattern, rules for transforming AST nodes with pattern definitions, 
-- and a function for generating a serializer of the AST.
-- 
-- All functionality is based around creating an AST transformation system from a grammar 
-- specification.  The transformation system contains a list of all of the rules, the tokens, 
-- and the grammar itself.  It can be used to generate sub-grammars that are capable of operating 
-- on ASTs that don't derive from the root grammar rule.
-- @name codegen.transformer
module(...)

--------------------------------------------------------------------------------
-- Token and Rule definitions
local
ast_token = function(patt, name)
	return Cmt(L(P(1)), function(s, i)
				if(s[i].token == name) then
					return i, nil
				end
			end
		)
end

local
ast_comment = function(patt, name)
	return Cmt(L(P(1)), function(s, i)
				if(s[i].comment == name) then
					return i, nil
				end
			end
		)
end

local
function compress(t)
	local tt = {}
	local maxidx = 0
	for k, v in pairs(t) do
		maxidx = max(maxidx, k)
	end
	
	for i=1, maxidx do
		tt[#tt+1] = t[i]
	end
	
	return tt
end

local
function ret(i, t)
	if(#t == 0) then
		return i, nil
	else
		return i, unpack(t)
	end
end

local
ast_rule = function(patt, name)
	return Cmt(L(patt), function(s, i, ...)
				if(s[i].rule == name) then
					return ret(i, compress{...})
				end
			end
		)
end

--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
-- Search over the AST
local
function apply_depth_first(grammar, p)
	local rules = {
		_patt = p,
		-- (the pattern) + (the pattern down one level in the list) + (look in the next element)
		_depth_first = (V"_patt" + L(V"_depth_first") + P(1))^0
	}
	for k, v in pairs(grammar) do
		rules[k] = v
	end
	
	rules[1] = V"_depth_first"
	
	return P(rules)
end
--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
-- Serializer generation

-- serialize a Token
local
function serialize_token(patt, fmt)
	fmt = fmt or "%s"
	return Cmt(C(patt), 
			function(s, i, t)
				return i, {format(fmt, t[1]), idx=i}
			end)
end

local
function tables_to_code(...)
	local parts = {...}
	table.sort(parts, function(a, b)
		return a.idx < b.idx
	end)
	
	local code = ""
	for i, p in ipairs(parts) do
		code = code..p[1]
	end
	
	return code
end

-- serialize a Rule
local
function serialize_rule(patt, fmt, name)
	fmt = fmt or "%s"
	return Cmt(C(patt),
			function(s, i, t, ...)
				local code = tables_to_code(...)
				return i, {format(fmt, code), idx=i}
			end)
end

-- grammar to serialize an AST
local
function serializing_grammar(transformer_system, grammar, tokens, formats)
	formats = formats or {}
	
	local sgrammar = {}
	for k, v in pairs(grammar) do
		if(tokens[k]) then
			sgrammar[k] = serialize_token(v, formats[k])
		elseif(k == 1) then
			sgrammar[k] = Cmt(v, function(s, i, ...)
									return i, tables_to_code(...)
								end)
		else
			sgrammar[k] = serialize_rule(v, formats[k], k)
		end
	end
	
	return sgrammar
end

--- Create a serializer
-- @param transformer_system The transformer system
-- @ret The serializer
function create_serializer(transformer_system)
	local grammar = grammar_from_system(transformer_system)
	
	local p = P(serializing_grammar(
		transformer_system,
		grammar, 
		transformer_system.tokens, 
		transformer_system.serialization_formats
	))
	
	return {
		match = function(self, s, ...)
			return p:match(s, ...)
		end,
	}
end
--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
-- Utility functions
local
function create_env(t)
	t = t or {}
	t.V = V
	t.P = P
	t.R = R
	t.S = S
	t.C = C
	t.Cmt = Cmt
	t.print = print
	t.table = table
	t.ipairs = ipairs
	t.pairs = pairs
	t.type = type
	
	return t
end

local
function get_sub_grammar_pattern(transformer_system, patt_string, env)
	env = env or {}
	env.create_pattern_search = create_pattern_search
	env.transformer_system = transformer_system
	env.grammar_from_system = grammar_from_system
		
	local grammar = grammar_from_system(transformer_system)	
	local load_env = create_env(env)
	local patt = setfenv(assert(loadstring("return "..patt_string)), load_env)()
	
	return grammar, patt
end
--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
--- Generate a transformer system from a specification
-- @param specification The specification
-- @ret The transformer system
function generate_system(specification)
	local transformer_system = {
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
		
		-- syntax error checking (no-op)
		CHECK_SYNTAX = function(patt)
							return patt
						end,
		
		-- AST matching patterns
		Keyword = ast_token,
		Token = ast_token,
		Comment = ast_comment,
		Rule = ast_rule,
	}
	

	local f = setfenv(specification, transformer_system)
	f()
	
	transformer_system.__roots = {}
	
	function transformer_system:push_root(name)
		self.__roots[#self.__roots+1] = self[1]
		self[1] = self[name]
	end
	
	function transformer_system:pop_root()
		self[1] = self.__roots[#self.__roots]
		self.__roots[#self.__roots] = nil
	end
	
	return transformer_system
end


--- Create a grammar from the transformer system
-- @param transformer_system The transformer system
-- @ret The grammar
function grammar_from_system(transformer_system)
	local grammar = {}
	for k, v in pairs(transformer_system) do
		if(type(v) == "userdata") then
			grammar[k] = v
		end
	end
	
	return grammar
end

--- Create a sub-grammar of a transformer system
-- A sub-grammar is created by setting a pattern to the head 
-- of the transformer_system grammar. The pattern can be an arbitrary 
-- PEG pattern, including Match-time captures with functions.  In such cases, 
-- it may be necessary to provide definitions for variables used.  These 
-- can be defined in the env parameter.  When the sub-grammar is created, 
-- patt_string is evaluated using loadstring.
-- @param transformer_system The transformer system
-- @param patt_string The root grammar pattern
-- @param env The environment of patt_string
-- @ret The sub-grammar
function create_subgrammar(transformer_system, patt_string, env)
	local grammar, patt = get_sub_grammar_pattern(transformer_system, patt_string, env)
	grammar[1] = patt
	return P(grammar)
end

-- search for a field in the AST
function create_search(transformer_system, field)
	local grammar = grammar_from_system(transformer_system)
	
	-- search pattern
	local patt = Cmt(C(grammar[field]),
					-- keeps sub-captures from propagating
					function(s, i, t)
						return i, t
					end
					
				)
	return P(apply_depth_first(grammar, patt))
end


--- Create a search for a pattern over an AST
-- @param transformer_system The transformer system
-- @param patt_string The root grammar pattern
-- @param env The environment of patt_string
-- @ret The AST search pattern
function create_pattern_search(transformer_system, patt_string, env)
	local grammar, patt = get_sub_grammar_pattern(transformer_system, patt_string, env)
	return P(apply_depth_first(grammar, patt))
end

--- Create an in-place AST transformation pattern
-- In-place transformations take in an AST node and return 
-- a modified AST node of the same rule.  The function is passed 
-- the AST itself as a first argument and the specific AST node 
-- matching the pattern as the second argument.  The only return value 
-- should be the transformed node.
-- @param transformer_system The transformer system
-- @param patt_string The root grammar pattern
-- @param transformation The transformation function
-- @param env The environment of patt_string
-- @ret The AST search pattern
function create_transformation(transformer_system, patt_string, transformation, env)
	local grammar, patt = get_sub_grammar_pattern(transformer_system, patt_string, env)
	
	
	local T = {
		transformation = transformation
	}
	
	-- generate the pattern
	local AST
	patt = Cmt(patt, function(s, i, ...)
						s[i] = T.transformation(AST, ...)
						return i
					end)

	patt = P(apply_depth_first(grammar, patt))
	
	-- match function for transformation
	T.match = function(self, _AST)
		AST = _AST
		patt:match(AST)
	end
	
	return T
end

--- Search through an AST up to a particular AST node
-- @param transformer_system The transformer system
-- @param patt_string The root grammar pattern
-- @param f The callback function on pattern match
-- @ret The AST search pattern
function search_up_to(transformer_system, patt_string, f)
	local AST, stop
			
	local patt = create_subgrammar(transformer_system, patt_string)
	patt = Cmt(patt, function(s, i)
		f(AST, s[i])
	end)
	
	local
	function depth_first(t)
		if(t == stop) then
			return false
		else
			patt:match(t)
			
			for i, tt in ipairs(t) do
				if(type(tt) == "table") then
					if(not depth_first(tt)) then
						return false
					end
				end
			end
			return true
		end
	end
	

	return {
		match = function(self, _AST, _stop)
			AST = _AST
			stop = _stop
			depth_first(AST)
		end,
	}
end