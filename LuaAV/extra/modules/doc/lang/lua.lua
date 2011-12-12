local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local unpack = unpack
local setmetatable = setmetatable
local print = print
local error = error
----------------------------------------------------------
local math = require 'math'
local table = require 'table'
local string = require 'string'
local io = require 'io'
----------------------------------------------------------
local format = string.format
----------------------------------------------------------
local lpeg = require("lpeg")
local lang = require("doc.lang")

local Language = require("language")
local specification = require("language.specification.lua")
local lexer_generator = require("codegen.lexer")

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
----------------------------------------------------------
--- Generate documentation from a Lua source file
-- Extracts documentation from Lua source files using a Lua lexer
-- @name doc.lang.lua
module(...)
----------------------------------------------------------

local SPACE = lexer_generator.SPACE
local SS = lexer_generator.SS
local SSC = lexer_generator.SSC
local KWSC = lexer_generator.KWSC

-- used to generate Lua lexers
local lua = Language{
	specification = specification.get_specification(),
	generate_lexer = true
}

-- grab documented modules
local module_lexer = lexer_generator.create_lexer(lua.lexer_system, {
	[1] = Ct(Cg(V"Comments", "comments") * Cg(V"ModuleFunction", "module_function")),
	
	ModuleFunction = Cmt(
		C(V"functioncall"), function(s, i, fc, name)
			if(name == "module") then
				return i, fc
			end
		end
	),
	
	Comments = Ct((SS*V"comment"*SS)^0),
	comment = C(lua.lexer_system.comment),
	NAME = C(lua.lexer_system.NAME)
})

-- grab documented global functinos
local global_function_lexer = lexer_generator.create_lexer(lua.lexer_system, {
	[1] = Ct(Cg(V"Comments", "comments") * Cg(V"global_function", "function_statement")),
	
	global_function = Cmt(V"function_statement", function(s, i, t)
		if(type(t) == "table") then
			return i, t
		end
	end),
	
	function_statement = Ct(V"FUNCTION" * KWSC * V"funcname" * SSC * V"global_function_body") + 
						V"LOCAL" * KWSC * V"FUNCTION" * KWSC * V"NAME" * SSC * V"funcbody",
	
	global_function_body = V"LEFT_PAREN" * SSC * 
				Cg(
					Ct(V"parlist"), 
					"params"
				)^-1 * 
			SSC * V"RIGHT_PAREN" * 
			(SSC * V"block")^-1 * 
			SSC * V"END",

	funcname = Cg(C(lua.lexer_system.funcname), "name"),
	
	NAME = C(lua.lexer_system.NAME), 
	VARARG = C(lua.lexer_system.VARARG),
	
	Comments = Ct((SS*V"comment"*SS)^1),
	comment = C(lua.lexer_system.comment),
})


--local single_line_comment = SS * C(P"-"^2) * SS * C((1 - P("\n"))^0)
local single_line_comment = SS * C(P"-"^2) * SPACE^-1 * C(SS*(1 - P("\n"))^0)
local param = P"@"*C((1 - SPACE)^0) * SS * C((1 - P("\n"))^0)
local modname = P("module") * SS * P"(" * S[['"]] * C((1 - S[['"]])^0)

local
function has_marker_comment(t)
	for i, c in ipairs(t.comments) do
		local cmt, text = single_line_comment:match(c)
		if(cmt:len() == 3) then
			return true
		end
	end

	return false
end

local
function document_from_comments(t)
	local processed_first = false
	local doc_handler = lang.document_from_comments()
	for i, c in ipairs(t.comments) do
		local cmt, text = single_line_comment:match(c)
		if(not processed_first and cmt:len() == 3) then
			t.summary = text
			processed_first = true
		elseif(processed_first) then
			doc_handler(t, text)
		end
	end
	
	lang.finalize_documentation(t)
end

--- Document modules in Lua code
-- @param code Source code for a Lua file
-- @ret Array of documented modules (only 1 in Lua source code)
function modules(code)
	-- only one module per file in Lua
	return {module(code)}
end

--- Document a module in Lua code
-- @param code Source code for a Lua file
-- @ret Module data structure for generting documentation
function module(code)
	local module = module_lexer:match(code)
	
	module.summary = ""
	module.description = ""
	
	document_from_comments(module)
	
	if(not module.name) then
		module.name = modname:match(module.module_function) or "NO_NAME"
	end
	
	module.functions = lang.canonicalize_listing(
		global_functions(module, code)
	)
	
	return module
end


--- Document global functions in Lua source code
-- @param module The module being documented
-- @param code The Lua source code
-- @ret Array of data structures for documenting global functions
function global_functions(module, code)
	local functions = { global_function_lexer:match(code) }
	
	if(type(functions[1]) == "number") then
		return {}
	end
	
	local documented_functions = {}
	for i, f in pairs(functions) do
		if(has_marker_comment(f)) then
			documented_functions[#documented_functions+1] = f
		end
	end
	functions = documented_functions
	
	for i, f in ipairs(functions) do
		f.function_statement.params = f.function_statement.params or {}
		f.name = f.function_statement.name
		f.summary = ""
		f.description = ""
		f.class = "function"
		f.ret = {}
		document_from_comments(f)
		
		f.param = lang.canonicalize_params(f)
		f.name = lang.canonicalize_function_name(module, f)
		f.private = false
	end
	
	return functions or {}	
end