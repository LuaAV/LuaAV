-- language.lua
local transformer_generator = require("codegen.transformer")
local parser_generator = require("codegen.parser")
local lexer_generator = require("codegen.lexer")

local table = require("table")
local math = require("math")
local max = math.max

local format = string.format

local type = type
local print = print
local pairs = pairs
local ipairs = ipairs
local getfenv = getfenv
local setmetatable = setmetatable
local error = error
local assert = assert
local pcall = pcall
local unpack = unpack
local loadfile = loadfile
local loadstring = loadstring

--- An interface to a grammar specification
-- Language manages the low-level interface to a grammar.  It provides access to both 
-- lexers, parsers, and serializers in addition to their subsystems.  Runtime data that can be 
-- added to the grammar is done through the language.
-- @name language
module(...)


local C = {}
local M = setmetatable(getfenv(), C)
M.__index = M

--- Constructor
-- Constructor must be passed a specification in the specification field
-- @param init Initial arguments as a table
-- @name language
function C:__call(init)
	assert(init.specification)

	local m = setmetatable({}, M)
	
	-- Language parser
	m.parser_system = parser_generator.generate_system(
											loadstring(init.specification)
										)
	-- Language transformer
	m.transformer_system = transformer_generator.generate_system(
											loadstring(init.specification)
										)
	
	m.parsers = {
		[1] = parser_generator.create_parser(m.parser_system)
	}
	
	m.serializers = {
		[1] = transformer_generator.create_serializer(m.transformer_system)
	}
	
	if(init.generate_lexer) then
		m.lexer_system = lexer_generator.generate_system(
									loadstring(init.specification)
								)
								
		m.lexers = {
			[1] = lexer_generator.create_lexer(m.lexer_system)
		}
	end
	
	return m
end

--- Set the typedef table with names as keys
-- @param t The typedef table
function M:set_typedefs(t)
	self.parser_system.set_typedefs(t)
	self.transformer_system.set_typedefs(t)
	if(self.lexer_system) then
		self.lexer_system.set_typedefs(t)
	end
end

--- Parser out some code with a parser, returning the AST or an error
-- This is typically used just to test if some code will properly parse
-- @param parser The parser
-- @param code The code
function M:parse(parser, code)
	local ok, res = pcall(parser.match, parser, code)
	if(not ok) then
		error("compilation error: " .. res)
	end

	return res		
end

--- Get a lexer for a given rule
-- If no argument is supplied, the lexer for the root grammar
-- rule will be returned
-- @param rule The root rule for the lexer
function M:lexer(rule)
	assert(self.lexer_system)
	rule = rule or 1
	
	local s = self.lexers[rule]
	if(not s) then
		self.lexer_system:push_root(rule)
		self.lexers[rule] = lexer_generator.create_lexer(
										self.lexer_system
									)
		self.lexer_system:pop_root()
		
		s = self.lexers[rule]
	end
	
	return s
end

--- Get a parser for a given rule
-- If no argument is supplied, the parser for the root grammar
-- rule will be returned
-- @param rule The root rule for the parser
function M:parser(rule)
	rule = rule or 1
	
	local s = self.parsers[rule]
	if(not s) then
		self.parser_system:push_root(rule)
		self.parsers[rule] = parser_generator.create_parser(
										self.parser_system
									)
		self.parser_system:pop_root()
		
		s = self.parsers[rule]
	end
	
	return s
end

--- Get a serializer for a given rule
-- If no argument is supplied, the parser for the root grammar
-- rule will be returned
-- @param rule The root rule for the parser
function M:serializer(rule)
	rule = rule or 1
	
	local s = self.serializers[rule]
	if(not s) then
		self.transformer_system:push_root(rule)
		self.serializers[rule] = transformer_generator.create_serializer(
										self.transformer_system
									)
		self.transformer_system:pop_root()
		
		s = self.serializers[rule]
	end
	
	return s
end
