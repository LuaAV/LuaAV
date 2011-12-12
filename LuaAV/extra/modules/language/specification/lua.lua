--- Lua specification grammar
-- @name language.specification.lua
module(...)

local specification = [=====[
local keywords = {
	"and",
	"break",
	"do",
	"else",
	"elseif",
	"end",
	"false",
	"for",
	"function",
	"if",
	"in",
	"local",
	"nil",
	"not",
	"or",
	"repeat",
	"return",
	"then",
	"true",
	"until",
	"while",
}

local operators = {
	dash = "-",
	add_op = "+",
	mul_op = "*",
	div_op = "/",
	mod_op = "%",
	pow_op = "^",
	length = "#",
	eq_op = "==",
	ne_op = "~=",
	le_op = "<=",
	ge_op = ">=",
	lt_op = "<",
	gt_op = ">",
	assign = "=",
	left_paren = "(",
	right_paren = ")",
	left_brace = "{",
	right_brace = "}",
	left_bracket = "[",
	right_bracket = "]",
	semicolon = ";",
	colon = ":",
	comma = ",",
	dot = ".",
	concat = "..",
	vararg = "...",
}

-- set the Tokens
for i, kw in ipairs(keywords) do
	getfenv()[kw:upper()] = Keyword(P(kw), kw:upper())
end

for name, op in pairs(operators) do
	getfenv()[name:upper()] = Token(P(op), name:upper())
end

-- used in NAME token
local keyword_map = {}
for i, kw in ipairs(keywords) do
	keyword_map[kw] = true
end

-------------------------------------------------------------------------
--- Numbers
nondigit = P"_" + R"az" + R"AZ"
digit = R"09"
digit_sequence = V"digit"^1
exponent_part = S"eE"^-1 * P"-"^-1 * V"digit_sequence"

hexadecimal_digit = R("09", "af", "AF")
hexadecimal_constant = (P"0x") * V"hexadecimal_digit"^1


-- 3   3.0   3.1416   314.16e-2   0.31416E1   0xff   0x56
NUMBER = Token(
		V"hexadecimal_constant" +
		V"digit_sequence" * (P"." * V"digit_sequence")^-1 * (V"exponent_part")^-1 + 
		P"." * V"digit_sequence" * (V"exponent_part")^-1,
		
		"NUMBER"
	)
		
-------------------------------------------------------------------------
--- Strings
simple_escape_sequence = P"\\'" + P"\\\""
					   + P "\\\\" + P"\\a" + P"\\b"
					   + P"\\f" + P"\\n" + P"\\r" 
					   + P"\\t" + P"\\v"
					   
numerical_escape_sequence = P"\\" * V"digit" * V"digit"^-2

escape_sequence = numerical_escape_sequence + simple_escape_sequence

single_line_character = V"escape_sequence" + (1 - S"\'\"\n")
single_line_string = P'"' * (V"single_line_character" + P"'")^0 * P'"' + 
					P"'" * (V"single_line_character" + P'"')^0 * P"'"

open_bracket_level = P"[" * C(P"="^0) * P"["
--close_bracket_level = P"]" * C(P"="^0) * P"]"

multi_line_string = Cmt(V"open_bracket_level",
						function(s, i, level)
							local sidx, eidx = s:find("]"..level.."]", i, true)
							return eidx+1
						end)


STRING = Token(single_line_string + multi_line_string, "STRING")

-------------------------------------------------------------------------
--- Identifiers

NAME = Token(Cmt(C(V"nondigit" * (V"nondigit" + V"digit")^0), function(s, i, a)
	return not keyword_map[a]
end), "NAME")
-------------------------------------------------------------------------

tokens = {NAME = true, NUMBER = true, STRING = true}
for i, kw in ipairs(keywords) do
	tokens[kw:upper()] = true
end

for name, op in pairs(operators) do
	tokens[name:upper()] = true
end



local __PREFIX
local __set_prefix = function(p)
	return
	function(_, i)
		--print("__set_prefix", p)
		__PREFIX = p
		return i
	end
end

local __match_prefix = function(p)
	return
	function(_, i)
		--print("__match_prefix", __PREFIX, p)
		return (__PREFIX == p) and i
	end
end


-------------------------------------------------------------------------
--- Variables
-- var ::= Name
-- var ::= prefixexp[exp]
-- var ::= prefixexp.Name
var = Rule(
		V"prefixexp" * __match_prefix"var",
		
--		V"prefixexp" * V"LEFT_BRACKET" * V"exp" * V"RIGHT_BRACKET" + 
--		V"prefixexp" * V"DOT" * V"NAME" +
--		V"NAME",
		
		"var"
	)


-------------------------------------------------------------------------
--- Chunks
-- chunk ::= {stat [';']} [jump_statement [';']]
chunk = Rule(
		(V"stat" * V"SEMICOLON"^-1)^1 * 
		(V"jump_statement" * V"SEMICOLON"^-1)^-1, 
		
		"chunk"
	)

-- block ::= chunk
block = Rule(V"chunk"^0, "block")

-------------------------------------------------------------------------
--- Statements
-- stat ::= compound_statement
-- stat ::= function_statement
-- stat ::= assignment_statement
-- stat ::= control_statement
-- stat ::= functioncall
-- stat ::= declaration_statement
stat = Rule(
		V"compound_statement" +
		V"function_statement" +
		V"assignment_statement" +
		V"control_statement" + 
		V"functioncall" + 
		V"declaration_statement",
		
		"stat"
	)

-- compound_statement ::= do block end
compound_statement = Rule(V"DO" * V"block" * V"END", "compound_statement")

-- assignment_statement ::= varlist '=' explist
assignment_statement = Rule(V"varlist" * V"ASSIGN" * V"explist", "assignment_statement")

-- varlist ::= var {',', var}
varlist = Rule(V"var" * (V"COMMA" * V"var")^0, "varlist")

-- explist ::= exp {',', exp}
explist = Rule(V"exp" * (V"COMMA" * V"exp")^0, "explist")


-- control_statement ::= iteration_statement
-- control_statement ::= selection_statement
-- control_statement ::= jump_statement
control_statement = Rule(
			V"iteration_statement" + V"selection_statement" + V"jump_statement",
			"control_statement"
		)

-- iteration_statement ::= for_statement
-- iteration_statement ::= while exp do block end
-- iteration_statement ::= repeat block until exp
iteration_statement = Rule(
			V"for_statement" + 
			V"WHILE" * V"exp" * V"DO" * V"block" * V"END" + 
			V"REPEAT" * V"block" * V"UNTIL" * V"exp",
			
			"iteration_statement"
		)
		
-- selection_statement ::= if exp then block {elseif exp then block} [else block] end
selection_statement = Rule(
			V"IF" * V"exp" * V"THEN" * V"block" * 
				(V"ELSEIF" * V"exp" * V"THEN" * V"block")^0 *
				(V"ELSE" * V"block")^-1 * 
				V"END",
			
			"selection_statement"
		)

-- jump_statement ::= return [explist]
-- jump_statement ::= break
jump_statement = Rule(
			V"RETURN" * V"explist" + 
			V"BREAK",
			
			"jump_statement"
		)

-- for_statement ::= for Name '=' exp ',' exp [',' exp] do block end
-- for_statement ::= for namelist in explist do block end
for_statement = Rule(
			V"FOR" * 
				V"NAME" * V"ASSIGN" * V"exp" * 
				V"COMMA" * V"exp" * 
				(V"COMMA" * V"exp")^-1 * 
				V"DO" * V"block" * V"END" + 
				
			V"FOR" * 
				V"namelist" * V"IN" * V"explist" * 
				V"DO" * V"block" * V"END",
				
			"for_statement"
		)

-- namelist ::= Name {',' Name}
namelist = Rule(V"NAME" * (V"COMMA" * V"NAME")^0, "namelist")


-- declaration_statement ::= local namelist ['=' explist]
declaration_statement = Rule(
			V"LOCAL" * V"namelist" * (V"ASSIGN" * V"explist")^-1,
			"declaration_statement"
		)


-------------------------------------------------------------------------
--- Expressions
-- exp ::= prefixexp
-- exp ::= function
-- exp ::= tableconstructor
-- exp ::= '...'
-- exp ::= or_expression
exp = Rule(
		V"or_expression" +
		V"basicexp",
		
		"exp"
	)
	
basicexp = Rule(
		V"function" +
		V"tableconstructor" +
		V"VARARG" +
		V"prefixexp",
	
		"basicexp"
	)

-- constant ::= nil | false | true
-- constant ::= Number
-- constant ::= String
constant = Rule(
		V"NIL" + V"FALSE" + V"TRUE" + 
		V"NUMBER" + V"STRING",
		
		"constant"
	)

-- prefixexp ::= var | functioncall | '(' exp ')'
prefixexp = Rule(
		(
			V"NAME" * __set_prefix"var" +
			V"LEFT_PAREN" * V"exp" * V"RIGHT_PAREN" * __set_prefix(nil)	
		) * (
			V"LEFT_BRACKET" * V"exp" * V"RIGHT_BRACKET" * __set_prefix"var" + 
			V"DOT" * V"NAME" * __set_prefix"var" + 
			
			V"args" * __set_prefix"functioncall" + 
			V"COLON" * V"NAME" * V"args" * __set_prefix"functioncall"
		)^0,

		"prefixexp"
	)
	
-- primary_expression ::= constant | exp
primary_expression = Rule(
		V"constant" + 
		V"basicexp",
		
		"primary_expression"
	)	

-- power_expression ::= postfix_expression ['^' postfix_expression]
power_expression = Rule(
		V"primary_expression" * (
			V"POW_OP" * V"primary_expression"
		)^0,
		
		"power_expression"
	)

-- unary_expression ::= '-' | not | '#' power_expression
unary_expression = Rule(
		(V"DASH" + V"NOT" + V"LENGTH")^-1 * V"power_expression",
		
		"unary_expression"
	)
	
-- multiplicative_expression ::= unary_expression ['*' | '/' unary_expression]
multiplicative_expression = Rule(
		V"unary_expression" * (
			V"MUL_OP" * V"unary_expression" +
			V"DIV_OP" * V"unary_expression"
		)^0,
		
		"multiplicative_expression"
	)
	
-- additive_expression ::= multiplicative_expression ['+' | '-' multiplicative_expression]
additive_expression = Rule(
		V"multiplicative_expression" * (
			V"ADD_OP" * V"multiplicative_expression" +
			V"DASH" * V"multiplicative_expression"
		)^0,
		
		"additive_expression"
	)

-- concat_expression ::= additive_expression ['..' additive_expression]
concat_expression = Rule(
		V"additive_expression" * (
			V"CONCAT" * V"additive_expression"
		)^0,
		
		"concat_expression"
	)

-- relational_expression ::=  concat_expression [ '<' | '>' | '<=' | '>=' | '~=' | '==' concat_expression]
relational_expression = Rule(
		V"concat_expression" * (
			V"LT_OP" * V"concat_expression" +
			V"GT_OP" * V"concat_expression" +
			V"LE_OP" * V"concat_expression" +
			V"GE_OP" * V"concat_expression" +
			V"NE_OP" * V"concat_expression" +
			V"EQ_OP" * V"concat_expression"
		)^0,
		
		"relational_expression"
	)

-- and_expression ::= relational_expression ['and' relational_expression]
and_expression = Rule(
		V"relational_expression" * (
			V"AND" * V"relational_expression"
		)^0,
		
		"and_expression"
	)
	
-- or_expression ::= and_expression ['or' and_expression]
or_expression = Rule(
		V"and_expression" * (
			V"OR" * V"and_expression"
		)^0,
		
		"or_expression"
	)
 
 
-- tableconstructor ::= '{' [fieldlist] '}'
tableconstructor = Rule(
		V"LEFT_BRACE" * V"fieldlist"^-1 * V"RIGHT_BRACE",
		"tableconstructor"
	)

-- fieldlist ::= field {fieldsep field} [fieldsep]
fieldlist = Rule(
		V"field" * (V"fieldsep" * V"field")^0 * V"fieldsep"^-1,
		"fieldlist"
	)

-- field ::= '[' exp ']' '=' exp | Name '=' exp | exp
field = Rule(
		V"LEFT_BRACKET" * V"exp" * V"RIGHT_BRACKET" * V"ASSIGN" * V"exp" +
		V"NAME" * V"ASSIGN" * V"exp" + 
		V"exp" +
		V"NAME",
		
		"field"
	)

-- fieldsep ::= ',' | ';'
fieldsep = Rule(V"COMMA" + V"SEMICOLON", "fieldsep")
 
 

-- functioncall ::= prefixexp args
-- functioncall ::= prefixexp ':' Name args
functioncall = Rule(
		V"prefixexp" * __match_prefix"functioncall",
		
		"functioncall"
	)
 
 -- args ::= '(' [explist] ')'
 -- args ::= tableconstructor
 -- args ::= String
 args = Rule(
 		V"LEFT_PAREN" * V"explist"^-1 * V"RIGHT_PAREN" +
 		V"tableconstructor" + 
 		V"STRING",
 		
 		"args"
 	)
 
 
-- function ::= function funcbody
getfenv()["function"] = Rule(
		V"FUNCTION" * V"funcbody",
		"function"
	)

-- funcbody ::= '(' [parlist] ')' block end
funcbody = Rule(
		V"LEFT_PAREN" * V"parlist"^-1 * V"RIGHT_PAREN" * V"block"^-1 * V"END",
		"funcbody"
	)
 
-- function_statement ::= function funcname funcbody
-- function_statement ::= local function Name funcbody
function_statement = Rule(
			V"FUNCTION" * V"funcname" * V"funcbody" + 
			V"LOCAL" * V"FUNCTION" * V"NAME" * V"funcbody",
			
			"function_statement"
		)

-- funcname ::= Name {'.' Name} [':' Name]
funcname = Rule(
		V"NAME" * (V"DOT" * V"NAME")^0 * (V"COLON" * V"NAME")^-1,
		"funcname"
	)
 
-- parlist ::= namelist [',' ...] | '...'
parlist = Rule(
		V"namelist" * (V"COMMA" * V"VARARG" + V"VARARG")^-1,
		"parlist"
	)

getfenv()[1] = V"chunk"

comments = {
	single_line_comment = Comment(P"--"*(1 - S"\n\r")^0, "single_line_comment"),
--	multi_line_comment = Comment(P"/*"*(1 - P"*/")^0*P"*/", "multi_line_comment")
}

]=====]

--- Get the speficication
-- @ret The specification
function get_specification()
	return specification
end