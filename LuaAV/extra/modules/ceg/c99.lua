local m = require("lpeg")
local table = require("table")

local print = print
local ipairs = ipairs
local pairs = pairs
local getfenv = getfenv

local P, V, R, S = m.P, m.V, m.R, m.S

local KEYWORDS = {}

-- Transforms strings into literal patterns of the same name.
-- If "minus" is passed, each pattern is concatenated to it.
local apply = function (dst, src, minus)
	local ret = m.P(false)
	for _, v in ipairs(src) do
		local UP = string.upper(v)
		dst[UP] = m.P(v)
		if minus then
			dst[UP] = dst[UP] * minus
		end
		ret = dst[UP] + ret
	end
	return ret
end

local
function combine(t1, t2, ...)
	if(t2) then
		local t = {}
		for k, v in pairs(t1) do
			t[k] = v
		end
		for k, v in pairs(t2) do
			t[k] = v
		end
		return combine(t, ...)
	else
		return t1
	end
end

module("ceg.c99")
-------------------------------------------
-- Comments
single_line_comment = P"//" * (1 - P"\n")^0
multi_line_comment = P"/*" * (1 - P"*/")^0 * P"*/"
comment = single_line_comment + multi_line_comment

-------------------------------------------
-- Lexical Elements

-- token:
--		keyword 
--		identiﬁer 
--		constant 
--		string-literal 
--		punctuator
token =   V"keyword" 
		+ V"identifier" 
		+ V"constant" 
		+ V"string_literal" 
		+ V"punctuator"
		
-- preprocessing-token:
--		header-name 
--		identiﬁer 
--		pp-number 
--		character-constant 
--		string-literal 
--		punctuator 
preprocessing_token = V"header_name" 
					+ V"identifier" 
					+ V"pp_number" 
					+ V"character_constant" 
					+ V"string_literal"
					+ V"punctuator"


local WS = R("\t\n", "\f\r", "  ")
SS = (WS + V"comment")^0 
SS1 = (WS)^1
space = WS

-------------------------------------------
-- Keywords

-- numeric patterns
local N4 = R('04')
local N8 = R('07')
local N = R('09')

-- alphanumeric pattern
local AZ = m.R('__','az','AZ')

local N_AZ_dot = N+AZ+P"."
local END_N = (1-N_AZ_dot)

--[[
keywords = apply (KEYWORDS, {
	'auto',		'enum',		'restrict',		'unsigned',
	'break',	'extern',	'return',		'void',
	'case',		'float',	'short',		'volatile',
	'char',		'for',		'signed',		'while',
	'const',	'goto',		'sizeof',		'_Bool',
	'continue',	'if',		'static',		'_Complex',
	'default',	'inline',	'struct',		'_Imaginary',
	'do',		'int',		'switch',	
	'double',	'long',		'typedef',
	'else',		'register',	'union',
}, -(N + AZ) )
--]]

_keywords = {
	'auto',		'enum',		'restrict',		'unsigned',
	'break',	'extern',	'return',		'void',
	'case',		'float',	'short',		'volatile',
	'char',		'for',		'signed',		'while',
	'const',	'goto',		'sizeof',		'_Bool',
	'continue',	'if',		'static',		'_Complex',
	'default',	'inline',	'struct',		'_Imaginary',
	'do',		'int',		'switch',	
	'double',	'long',		'typedef',
	'else',		'register',	'union',
}

keyword = P(_keywords[1])
for i=2, #_keywords do
	keyword = keyword + P(_keywords[i])
end

-------------------------------------------
-- Identifiers

-- identifier: 
--		identifier-nondigit 
--		identifier	identifier-nondigit 
--		identifier	digit
-- NOTE: don't match keywords
local
function not_keyword(v)
	print("not_keyword", v)
	return true
end
--TODO: exclude keywords
--identifier = V"identifier_nondigit" * (V"identifier_nondigit" + V"digit")^0
identifier = V"identifier_nondigit" * (V"identifier_nondigit" + V"digit")^0

-- identifier-nondigit: 
--		nondigit 
--		universal-character-name 
--		other implementation-deﬁned characters 
identifier_nondigit = V"nondigit" + V"universal_character_name"

-- nondigit:
nondigit = AZ

-- digit:
digit = N

-------------------------------------------
-- Universal Character Names

-- universal-character-name: 
--	\u hex-quad 
--	\U hex-quad hex-quad
universal_character_name =	P"\\u" * V"hex_quad"
						+ 	P"\\U" * V"hex_quad" * V"hex_quad"
						
-- hex-quad: 
--		hexadecimal-digit hexadecimal-digit hexadecimal-digit hexadecimal-digit
hex_quad = V"hexadecimal_digit" * V"hexadecimal_digit" 
		 * V"hexadecimal_digit" * V"hexadecimal_digit"

-------------------------------------------
-- Constants

-- constant:
--		integer-constant 
--		floating-constant 
--		enumeration-constant 
--		character-constant 
constant = V"floating_constant"
		 + V"integer_constant"
		 + V"character_constant"
		 + V"enumeration_constant"
		 
		 
-- integer-constant: 
--		decimal-constant integer-suffix(opt)
--		octal-constant integer-suffix(opt)
--		hexadecimal-constant integer-suffix(opt)
integer_constant = 
				 V"hexadecimal_constant" * V"integer_suffix"^-1
				 + V"octal_constant" * V"integer_suffix"^-1
				 + V"decimal_constant" * V"integer_suffix"^-1
				 
-- decimal-constant: 
--		nonzero-digit 
--		decimal-constant digit 
decimal_constant = V"nonzero_digit" * V"digit"^0

-- octal-constant: 
--		0 
--		octal-constant octal-digit
octal_constant = P"0" * V"octal_digit"^0

-- hexadecimal-constant: 
--		hexadecimal-prefix hexadecimal-digit 
--		hexadecimal-constant hexadecimal-digit
hexadecimal_constant = V"hexadecimal_prefix" * V"hexadecimal_digit"^1

-- hexadecimal-prefix: one of 
--		0x 0X 
hexadecimal_prefix = P"0x" + P"0X"

-- nonzero-digit: one of 
--		1 2 3 4 5 6 7 8 9
nonzero_digit = R"19"

-- octal-digit: one of 
--		0 1 2 3 4 5 6 7
octal_digit = R"07"

-- hexadecimal-digit: one of 
--		0 1 2 3 4 5 6 7 8 9  
--		a b c d e f 
--		A B C D E F 
hexadecimal_digit = R("09", "af", "AF")

-- integer-suffix: 
--		unsigned-suffix long-suffix(opt)
--		unsigned-suffix long-long-suffix
--		long-long-suffix unsigned-suffix(opt) 
--		long-suffix unsigned-suffix(opt)
integer_suffix = V"unsigned_suffix" * V"long_suffix"^-1
			   + V"unsigned_suffix" * V"long_long_suffix"
			   + V"long_long_suffix" * V"unsigned_suffix"^-1
			   + V"long_suffix" * V"unsigned_suffix"^-1
			   
-- unsigned-suffix: one of 
--		u U 
unsigned_suffix = S"uU"

-- long-suffix: one of 
--		l L 
long_suffix = S"lL"

-- long-long-suffix: one of 
--		ll LL
long_long_suffix = P"ll" + P"LL"

-- floating-constant: 
--		decimal-floating-constant 
--		hexadecimal-floating-constant
floating_constant = V"decimal_floating_constant"
				  + V"hexadecimal_floating_constant"

-- decimal-floating-constant: 
--		fractional-constant exponent-part(opt) floating-suffix(opt)
--		digit-sequence exponent-part floating-suffix(opt)
decimal_floating_constant = V"fractional_constant" * V"exponent_part"^-1 * V"floating_suffix"^-1
						 + V"digit_sequence" * V"exponent_part" * V"floating_suffix"^-1
						 
-- hexadecimal-floating-constant: 
--		hexadecimal-prefix hexadecimal-fractional-constant 
--				binary-exponent-part floating-suffix(opt)
--		hexadecimal-prefix hexadecimal-digit-sequence 
--				binary-exponent-part floating-suffix(opt)
hexadecimal_floating_constant = V"hexadecimal_prefix" * V"hexadecimal_fractional_constant" 
							 			* V"binary_exponent_part" * V"floating_suffix"^-1
							 + V"hexadecimal_prefix" * V"hexadecimal_digit_sequence"
							 			* V"binary_exponent_part" * V"floating_suffix"^-1
							 
-- fractional-constant: 
--		digit-sequence(opt) .digit-sequence 
--		digit-sequence .
fractional_constant = V"digit_sequence"^-1 * P"." * V"digit_sequence"
					+ V"digit_sequence" * P"."
					
-- exponent-part: 
--		e sign(opt) digit-sequence 
--		E sign(opt) digit-sequence 
exponent_part = (P"e" + P"E") * V"sign"^-1 * V"digit_sequence"

-- sign: one of 
--		+ -
sign = S"+-"

-- digit-sequence: 
--		digit 
--		digit-sequence digit
digit_sequence = V"digit"^1

-- hexadecimal-fractional-constant: 
--		hexadecimal-digit-sequence(opt) . hexadecimal-digit-sequence 
--		hexadecimal-digit-sequence .
hexadecimal_fractional_constant = V"hexadecimal_digit_sequence"^-1 * P"." * V"hexadecimal_digit_sequence"
								+ V"hexadecimal_digit_sequence" * P"."
								
-- binary-exponent-part: 
--		p sign(opt) digit-sequence 
--		P sign(opt) digit-sequence
binary_exponent_part = (P"s" + P"S")^-1 * V"digit_sequence"

-- hexadecimal-digit-sequence: 
--		hexadecimal-digit 
--		hexadecimal-digit-sequence hexadecimal-digit
hexadecimal_digit_sequence = V"hexadecimal_digit"^1

-- floating-suffix: one of 
--		f l F L
floating_suffix = S"flFL"

-- enumeration-constant: 
--		identiﬁer
enumeration_constant = V"identifier"

-- character-constant: 
--		' c-char-sequence ' 
--		L' c-char-sequence '
--character_constant = (P"\'" + P"L\'") * V"c_char_sequence" * P"\'"
character_constant = (P"\'" + P"L\'") * V"c_char_sequence" * P"'"

-- c-char-sequence: 
--		c-char 
--		c-char-sequence c-char
c_char_sequence = V"c_char"^1

-- c-char: 
--		anymember of the source character set except the single-quote', backslash\, or new-line character 
--		escape-sequence 
c_char = (1 - S("\'\\\n")) + V"escape_sequence" 

-- escape-sequence: 
--		simple-escape-sequence 
--		octal-escape-sequence 
--		hexadecimal-escape-sequence 
--		universal-character-name
escape_sequence = V"simple_escape_sequence"
				+ V"octal_escape_sequence"
				+ V"hexadecimal_escape_sequence"
				+ V"universal_character_name"
				
-- simple-escape-sequence: one of 
--		\' \" \? \\ 
--		\a \b \f \n \r \t \v
simple_escape_sequence = P"\\'" + P"\\\"" +  P"\\?"
					   + P "\\\\" + P"\\a" + P"\\b"
					   + P"\\f" + P"\\n" + P"\\r" 
					   + P"\\t" + P"\\v"

-- octal-escape-sequence: 
--		\octal-digit 
--		\octal-digit octal-digit 
--		\octal-digit octal-digit octal-digit
octal_escape_sequence = P"\\" * V"octal_digit" * V"octal_digit"^-2

-- hexadecimal-escape-sequence: 
--		\x hexadecimal-digit 
--		hexadecimal-escape-sequence hexadecimal-digit
hexadecimal_escape_sequence = P"\\x" * V"hexadecimal_digit"^1

-------------------------------------------
-- String Literals

-- string-literal: 
--		" s-char-sequence(opt) " 
--		L" s-char-sequence(opt) "
string_literal = (P"\"" + P"L\"") * V"s_char_sequence"^-1 * P"\""

-- s-char-sequence: 
--		s-char 
--		s-char-sequence s-char
s_char_sequence = V"s_char"^1

-- s-char: 
--		any member of the source character set except the double-quote ", backslash \, or new-line character 
--		escape-sequence
s_char = V"escape_sequence" + (1 - S("\"\\\n"))

-------------------------------------------
-- Punctuators

-- punctuator: one of 
punctuators = {
	'[', ']', '(', ')', '{', '}', '.', '->', 
	'++', '--', '&', '*', '+', '-', '~', '!', 
	'/', '%', '<<', '>>', '<', '>', '<=', '>=', '==', '!=', '^', '|', '&&', '||', 
	'?', ':', ';', '...', 
	'=', '*=', '/=', '%=', '+=', '-=', '<<=', '>>=', '&=', '^=', '|=', 
	',', '#', '##', 
	'<:', ':>', '<%', '%>', '%:', '%:%:',
}
punctuator = P(punctuators[1])
for i=2, #punctuators do
	punctuator = punctuator + P(punctuators[i])
end

-------------------------------------------
-- Header Names

-- header-name: 
--		< h-char-sequence > 
--		" q-char-sequence "
header_name = P"<" * V"h_char_sequence" * P">" 
			+ P"\"" * V"q_char_sequence" * P"\"" 
			
-- h-char-sequence: 
--		h-char 
--		h-char-sequence h-char
h_char_sequence = V"h_char"^1

-- h-char: 
--		any member of the source character set except the new-line character and >
h_char = 1 - S"\n>"

-- q-char-sequence: 
--		q-char 
--		q-char-sequence q-char
q_char_sequence = V"q_char"^1

-- q-char: 
--		any member of the source character set except the new-line character and"
q_char = 1 - S"\n\""

-------------------------------------------
-- Preprocessing numbers

-- pp-number: 
--		digit 
--		.digit 
--		pp-number digit 
--		pp-number identifier-nondigit 
--		pp-number esign 
--		pp-number Esign 
--		pp-number psign 
--		pp-number Psign 
--		pp-number .
pp_number = (P"."^-1 * V"digit")
			* (
				V"digit"
			  + V"identifier_nondigit"
			  + (P"e" + P"E" + P"p" + P"P") * V"sign"
			  + P"."
			  )^0
			  
-------------------------------------------
-------------------------------------------
-- Phrase structure grammar

-------------------------------------------
-- Expressions

-- primary-expression: 
--		identiﬁer 
--		constant 
--		string-literal 
--		( expression ) 
primary_expression = V"identifier"
				   + V"constant"
				   + V"string_literal"
				   + V"(" * SS * V"expression" * SS * V")"

-- postfix-expression: 
--		primary-expression 
--		postfix-expression [expression ] 
--		postfix-expression (argument-expression-list(opt) ) 
--		postfix-expression . identifier 
--		postfix-expression -> identifier 
--		postfix-expression ++ 
--		postfix-expression -- 
--		( type-name ) { initializer-list } 
--		( type-name ) { initializer-list , } 
--[[
postfix_expression = V"primary_expression"
				 *  (
				   		V"[" * V"expression" * V"]"
				   	  + V"(" * V"argument_expression_list"^-1 * V")"
				   	  + V"." * V"identifier"
				      + V"->" * V"identifier"
				      + V"++"
				      + V"--"
				    )^-1
				   + V"(" * V"type_name" * V")" * V"{" * V"initializer_list" * V","^-1 * V"}"
--]]
local prefix
local
function set_prefix(p)
	return function (_,i)
		prefix = p
		return i
	end
end

local
function match_prefix(p)
	return function (_, i)
		return (prefix == p) and i
	end
end

postfix_suffix =  SS * V"[" * SS * V"expression" * SS * V"]" 
				+ SS * V"(" * (SS * V"argument_expression_list")^-1 * SS * V")"
				+ SS * V"." * SS * V"identifier"
				+ SS * V"->" * SS * V"identifier"
				+ SS * V"++"
				+ SS * V"--"

_postfix_expression = V"primary_expression" * 
								set_prefix("postfix_expression") * ( postfix_suffix )^0 +
				
					  V"(" * SS * V"type_name" * SS * V")" 
								* V"{" * SS * V"initializer_list" * (SS * V",")^-1 * SS * V"}"
					  			* set_prefix("postfix_expression") * ( postfix_suffix )^0

postfix_expression = V"_postfix_expression"*match_prefix("postfix_expression")


-- argument-expression-list: 
--		assignment-expression 
--		argument-expression-list , assignment-expression
argument_expression_list = V"assignment_expression" * (SS * V"," * SS * V"assignment_expression")^0

-- unary-expression: 
--		postfix-expression 
--		++ unary-expression 
--		-- unary-expression 
--		unary-operator cast-expression 
--		sizeof unary-expression 
--		sizeof ( type-name ) 
unary_expression = V"postfix_expression"
				 + (V"++" + V"--") * V"unary_expression"
				 + V"unary_operator" * SS * V"cast_expression"
				 + P"sizeof" * SS * V"unary_expression"
				 + P"sizeof" * SS * V"(" * SS * V"type_name" * SS * V")"
				 
-- unary-operator: one of 
--		& * + - ~ !
unary_operator = V"&" + V"*" + V"+" + V"-" + V"~" + V"!"

-- cast-expression: 
--		unary-expression 
--		( type-name ) cast-expression
-- cast_expression = V"unary_expression" * SS * 
--					( V"(" * SS * V"type_name" * SS * V")" )^0
-- cast_expression = (V"(" * SS * V"type_name" * SS * V")" * SS
--				 + V"unary_expression" ) * V"cast_expression"^0
cast_expression = (V"(" * SS * V"type_name" * SS * V")" * SS * V"cast_expression"^0
				 + V"unary_expression" ) 
				
-- multiplicative-expression: 
--		cast-expression 
--		multiplicative-expression * cast-expression 
--		multiplicative-expression / cast-expression 
--		multiplicative-expression % cast-expression
multiplicative_expression = V"cast_expression" * SS * (
						  		V"*" * SS * V"cast_expression" * SS
						  	  + V"/" * SS * V"cast_expression" * SS
						  	  + V"%" * SS * V"cast_expression" * SS
						  )^0
						  
-- additive-expression: 
--		multiplicative-expression 
--		additive-expression + multiplicative-expression 
--		additive-expression - multiplicative-expression
additive_expression = V"multiplicative_expression" * SS * (
						  		V"+" * SS * V"multiplicative_expression"
						  	  + V"-" * SS * V"multiplicative_expression"
						  )^0
						  
-- shift-expression: 
--		additive-expression 
--		shift-expression << additive-expression 
--		shift-expression >> additive-expression
shift_expression = V"additive_expression" * SS * (
							V"<<" * SS * V"additive_expression"
						  + V">>" * SS * V"additive_expression"
						 )^0

-- relational-expression: 
--		shift-expression 
--		relational-expression < shift-expression 
--		relational-expression > shift-expression 
--		relational-expression <= shift-expression 
--		relational-expression >= shift-expression 
relational_expression = V"shift_expression" * SS * (
							V"<" * SS * V"shift_expression"
						  + V">" * SS * V"shift_expression"
						  + V"<=" * SS * V"shift_expression"
						  + V">=" * SS * V"shift_expression"
						)^0
						
-- equality-expression: 
--		relational-expression 
--		equality-expression == relational-expression 
--		equality-expression != relational-expression
equality_expression = V"relational_expression" * SS * (
							V"==" * SS * V"relational_expression"
						  + V"!=" * SS * V"relational_expression"
						)^0

-- AND-expression: 
--		equality-expression 
--		AND-expression & equality-expression
-- NOTE: have to worry about case where there's AND_expression followed by unary operator
AND_expression = V"equality_expression" * SS * (-V"&&"*V"&" * SS * V"equality_expression")^0

-- exclusive-OR-expression: 
--		AND-expression 
--		exclusive-OR-expression ^ AND-expression
exclusive_OR_expression = V"AND_expression" * SS * (V"^" * SS * V"AND_expression")^0

-- inclusive-OR-expression: 
--		exclusive-OR-expression 
--		inclusive-OR-expression | exclusive-OR-expression
inclusive_OR_expression = V"exclusive_OR_expression" * SS * 
									(V"|" * SS * V"exclusive_OR_expression")^0

-- logical-AND-expression: 
--		inclusive-OR-expression 
--		logical-AND-expression && inclusive-OR-expression
logical_AND_expression = V"inclusive_OR_expression" * SS * 
									(V"&&" *SS * V"inclusive_OR_expression")^0

-- logical-OR-expression: 
--		logical-AND-expression 
--		logical-OR-expression || logical-AND-expression
logical_OR_expression = V"logical_AND_expression" * (V"||" * V"logical_AND_expression")^0

-- conditional-expression: 
--		logical-OR-expression 
--		logical-OR-expression ? expression : conditional-expression
conditional_expression = V"logical_OR_expression" * SS * 
								(V"?" * SS * V"expression" * SS * 
								 V":" * SS * V"conditional_expression")^-1

-- assignment-expression: 
--		conditional-expression 
--		unary-expression assignment-operator assignment-expression
assignment_expression = V"unary_expression" * SS * V"assignment_operator" * SS * 
									 V"assignment_expression"
					  +	V"conditional_expression"
					  
-- assignment-operator: one of 
--		= *= /= %= += -= <<= >>= &= ^= |=
assignment_operator = V"=" + V"*=" + V"/=" + V"%=" + V"+=" + V"-=" 
					+ V"<<=" + V">>=" + V"&=" + V"^=" + V"|="

-- expression: 
--		assignment-expression 
--		expression , assignment-expression
expression = V"assignment_expression" * SS * (V"," * SS * V"assignment_expression")^0

-- constant-expression:
--		conditional-expression
constant_expression = V"conditional_expression"

-------------------------------------------
-- Declarations

-- declaration: 
--		declaration-specifiers init-declarator-list(opt) ;
declaration = V"declaration_specifiers" * (SS * V"init_declaration_list")^-1 * SS * V";"
--declaration = (SS * V"init_declaration_list")^-1 * SS * V";"
--declaration = V"declaration_specifiers" * ( V"init_declaration_list")^-1 * SS * V";"

-- declaration-specifiers: 
--		storage-class-speciﬁer declaration-specifiers(opt )
--		type-specifier declaration-specifiers(opt)
--		type-qualifier declaration-specifiers(opt)
--		function-specifier declaration-specifiers(opt)
declaration_specifiers = (V"storage_class_specifier"
					   + V"type_specifier"
					   + V"type_qualifier"
					   + V"function_specifier") * (SS * V"declaration_specifiers")^-1
					  
-- init-declarator-list: 
--		init-declarator 
--		init-declarator-list , init-declarator
init_declaration_list = V"init_declarator" * (SS * V"," * SS * V"init_declarator")^0

-- init-declarator: 
--		declarator 
--		declarator = initializer 
init_declarator = V"declarator" * (SS * V"=" * SS * V"initializer")^-1

-- storage-class-specifier: 
--		typedef 
--		extern 
--		static 
--		auto 
--		register
storage_class_specifier = V"typedef" + V"extern" + V"static" + V"auto" + V"register"

-- type-speciﬁer: 
--		void 
--		char 
--		short 
--		int 
--		long 
--		float 
--		double 
--		signed 
--		unsigned 
--		_Bool 
--		_Complex 
--		struct-or-union-speciﬁer
--		enum-speciﬁer 
--		typedef-name
type_specifier = V"void" + V"char" + V"short" + V"int" + V"long" 
			   + V"float" + V"double" + V"signed" + V"unsigned" 
			   + V"_Bool" + V"_Complex"
			   + V"struct_or_union_specifier"
			   + V"enum_specifier"
			   + V"typedef_name" --(interferes by usurping from declarator), need to track in table
			   
-- struct-or-union-specifier: 
--		struct-or-union identifier(opt) {struct-declaration-list } 
--		struct-or-union identifier
struct_or_union_specifier = V"struct_or_union" * (SS * V"identifier")^-1 * SS * V"{" 
									* SS * V"struct_declaration_list" * SS * V"}"
						  + V"struct_or_union" * SS * V"identifier"

-- struct-or-union: 
--		struct 
--		union
struct_or_union = V"struct" + V"union"

-- struct-declaration-list: 
--		struct-declaration 
--		struct-declaration-list struct-declaration
struct_declaration_list = (V"struct_declaration" * SS)^1

-- struct-declaration: 
--		specifier-qualifier-list struct-declarator-list ;
struct_declaration = V"specifier_qualifier_list" * SS * V"struct_declarator_list" * SS * V";"
--struct_declaration = V"specifier_qualifier_list" * SS1 * V"struct_declarator_list" * SS * V";"

-- specifier-qualiﬁer-list: 
--		type-specifier specifier-qualifier-list(opt)
--		type-qualifier specifier-qualifier-list(opt)
specifier_qualifier_list = (V"type_specifier" + V"type_qualifier") * 
								(SS * V"specifier_qualifier_list")^-1

-- struct-declarator-list: 
--		struct-declarator 
--		struct-declarator-list , struct-declarator
struct_declarator_list = V"struct_declarator" * (SS * V"," * SS * V"struct_declarator")^0
					   
-- struct-declarator: 
--		declarator 
--		declarator(opt) : constant-expression
struct_declarator = V"declarator"
				  + V"declarator"^-1 * SS * V":" * SS * V"constant_expression"
				  
-- enum-specifier: 
--		enum identifier(opt) { enumerator-list } 
--		enum identifier(opt) { enumerator-list , } 
--		enum identifier
enum_specifier = V"enum" * (SS * V"identifier")^-1 
						* SS * V"{" 
						* SS * V"enumerator_list" * (SS * V",")^-1 
						* SS * V"}"
			   + V"enum" * SS * V"identifier"
			   
-- enumerator-list: 
--		enumerator 
--		enumerator-list , enumerator
enumerator_list = V"enumerator" * (SS * V"," * SS * V"enumerator")^0

-- enumerator: 
--		enumeration-constant 
--		enumeration-constant = constant-expression
enumerator = V"enumeration_constant" * (SS * V"=" * SS * V"constant_expression")^-1

-- type-qualifier: 
--		const 
--		restrict 
--		volatile
type_qualifier = (V"const" + V"restrict" + V"volatile") --* S" "

-- function-speciﬁer: 
--		inline
function_specifier = V"inline"

-- declarator: 
--		pointer(opt) direct-declarator
declarator = (SS * V"pointer" * SS)^-1 * V"direct_declarator"

-- direct-declarator: 
--		identifier 
--		( declarator ) 
--		direct-declarator [ type-qualifier-list(opt) assignment-expression(opt) ] 
--		direct-declarator [ static type-qualifier-list(opt) assignment-expression ] 
--		direct-declarator [ type-qualifier-list static assignment-expression ] 
--		direct-declarator [ type-qualifier-list(opt) * ] 
--		direct-declarator ( parameter-type-list ) 
--		direct-declarator ( identifier-list(opt) )
direct_declarator = 
				 -- + --V"(" + V"declarator" + ")"
				  --+ V"direct_declarator" * 
				  V"identifier" * SS *
				  (
				  		 V"[" 
				  				* (SS * V"type_qualifier_list")^-1 
				  				* (SS*V"assignment_expression")^-1 
				  				* SS * V"]"
				  	  
				  	  + V"[" * SS * V"static" 
				  	  			* (SS * V"type_qualifier_list")^-1 
				  	  			* SS * V"assignment_expression" 
				  	  			* SS * V"]"
				  	  
				  	  + V"[" * SS * V"type_qualifier_list"
				  	  		 * SS * V"static" 
				  	  		 * SS * V"assignment_expression" 
				  	  		 * SS * V"]"
				  	  		 
				  	  + V"[" * (SS * V"type_qualifier_list")^-1 
				  	  		 * SS * V"*" 
				  	  		 * SS * V"]"
				  	  		 
				  	  + V"(" * SS * V"parameter_type_list" * SS * V")"
				  	  + V"(" * (SS * V"identifier_list")^-1 * SS * V")"
				  )
				  + V"identifier"
				  
-- pointer: 
--		* type-qualifier-list(opt) 
--		* type-qualifier-list(opt) pointer
pointer = V"*" * (SS * V"type_qualifier_list")^-1 * (SS * V"pointer")^-1

-- type-qualiﬁer-list: 
--	type-qualifier 
--	type-qualifier-list type-qualiﬁer
type_qualifier_list = V"type_qualifier"^1

-- parameter-type-list: 
--		parameter-list 
--		parameter-list , ...
parameter_type_list = V"parameter_list" * (SS * V"," * SS * V"...")^-1

-- parameter-list: 
--		parameter-declaration 
--		parameter-list , parameter-declaration 
parameter_list = V"parameter_declaration" * (SS * V"," * SS * V"parameter_declaration")^0

-- parameter-declaration: 
--		declaration-specifiers declarator 
--		declaration-specifiers abstract-declarator(opt)
parameter_declaration = V"declaration_specifiers" * SS * V"declarator" 
								* (SS * V"abstract_declarator")^-1
					  + V"declaration_specifiers" * SS * V"declarator"
					  + V"declaration_specifiers" * (SS * V"abstract_declarator")^-1
					  
					  
					  
-- identifier-list: 
--		identifier 
--		identifier-list , identiﬁer
identifier_list = V"identifier" * (SS * V"," * SS * V"identifier")^0

-- type-name: 
--		specifier-qualiﬁer-list abstract-declarator(opt)
type_name = V"specifier_qualifier_list" * (SS * V"abstract_declarator")^-1

-- abstract-declarator: 
--		pointer 
--		pointer(opt) direct-abstract-declarator
abstract_declarator = V"pointer"
					+ V"pointer"^-1 * SS * V"direct_abstract_declarator"
					
-- direct-abstract-declarator: 
--		( abstract-declarator ) 
--		direct-abstract-declarator(opt) [ assignment-expression(opt) ] 
--		direct-abstract-declarator(opt) [ * ] 
--		direct-abstract-declarator(opt) ( parameter-type-list(opt) )
direct_abstract_declarator_suffix = V"[" * (SS * V"assignment_expression")^-1 * SS * V"]"
								  + V"[" * SS * V"*" * SS * V"]"
								  + V"(" * (SS * V"parameter_type_list")^-1 * SS * V")"
direct_abstract_declarator = 
					V"(" * SS * V"abstract_declarator" * SS * V")"
				  + (
						V"pointer"^-1 * SS * V"[" * (SS * V"assignment_expression")^-1 * SS * V"]"
					  + V"[" * SS * V"*" * SS * V"]"
					  + V"(" * (SS * V"parameter_type_list")^-1 * SS * V")"
					) * (SS * V"direct_abstract_declarator")^0

-- typedef-name: 
--		identifier
typedefs = {}
local 
function test_typedef(match, pos)
	local patt = AZ*(AZ+N)^0 / function(v) return v end
	local str = match:sub(pos)
	local typename = patt:match(str)

	if(typedefs[typename]) then
		return pos
	else
		if(typename) then
			--print("typename", typename)
		end
		return nil
	end
end
typedef_name = test_typedef*V"identifier"

function typedef_name_rule(typedefs)
	typedefs = typedefs or {}

	local 
	function test_typedef(match, pos)
		local patt = AZ*(AZ+N)^0 / function(v) return v end
		local str = match:sub(pos)
		local typename = patt:match(str)
	
		if(typedefs[typename]) then
			return pos
		else
			if(typename) then
				--print("typename", typename)
			end
			return nil
		end
	end
	
	return test_typedef*V"identifier"
end


-- initializer: 
--		assignment-expression 
--		{ initializer-list } 
--		{ initializer-list , }
initializer = V"assignment_expression"
			+ V"{" * SS * V"initializer_list" * (SS * V",")^-1 * SS * V"}"

-- initializer-list: 
--		designation(opt) initializer 
--		initializer-list , designation(opt) initializer
local init = (SS * V"designation")^-1 * SS * V"initializer"
initializer_list = init * (SS * V"," * init)^0

-- designation: 
--		designator-list =
designation = V"designator_list" * SS * V"="

-- designator-list: 
--		designator 
--		designator-list designator
designator_list = (SS * V"designator")^1

-- designator: 
--		[ constant-expression ] 
--		. identiﬁer
designator = V"[" * SS * V"constant_expression" * SS * V"]"
		   + V"." * SS * V"identifier"
		   
-------------------------------------------
-- Statements

-- statement: 
--		labeled-statement 
--		compound-statement 
--		expression-statement 
--		selection-statement 
--		iteration-statement 
--		jump-statement
statement = V"labeled_statement"
		  + V"compound_statement"
		  + V"expression_statement"
		  + V"selection_statement"
		  + V"iteration_statement"
		  + V"jump_statement"
		  
-- labeled-statement: 
--		identifier : statement 
--		case constant-expression : statement 
--		default : statement
labeled_statement = V"identifier" * SS * V":" * SS * V"statement"
				  + V"case" * SS * V"constant_expression" * SS * V":" * SS * V"statement"
				  + V"default" * SS * V":" * SS * V"statement"
				  
-- compound-statement: 
--		{block-item-list(opt) }
compound_statement = V"{" * SS * V"block_item_list"^-1 * SS * V"}"

-- block-item-list: 
--		block-item 
--		block-item-list block-item
block_item_list = (SS * V"block_item")^1

-- block-item: 
--		declaration 
--		statement
block_item = V"declaration"
		   + V"statement"
		   
-- expression-statement: 
--		expression(opt) ;
expression_statement = V"expression"^-1 * SS * V";"

-- selection-statement: 
--		if ( expression ) statement 
--		if ( expression ) statement else statement 
--		switch ( expression ) statement
selection_statement = V"if" * SS * V"(" * SS * V"expression" * SS * V")" * SS * V"statement" 
								* SS * V"else" * SS * V"statement"
					+ V"if" * SS * V"(" * SS * V"expression" * SS * V")" * SS * V"statement"
					+ V"switch" * SS * V"(" * SS * V"expression" * SS * V")" * SS * V"statement"
					
-- iteration-statement: 
--		while ( expression ) statement 
--		do statement while ( expression ) ; 
--		for ( expression(opt) ; expression(opt) ; expression(opt) ) statement 
--		for ( declaration expression(opt) ; expression(opt) ) statement
iteration_statement = V"while" * SS * V"(" * SS * V"expression" * SS * V")" * SS * V"statement"
					+ V"do" * SS * V"statement" * SS * V"while" 
							* SS * V"(" * SS * V"expression" * SS * V")" * SS * V";"
					+ V"for" * SS * V"(" 
							 * (SS * V"expression")^-1 * SS * V";" 
							 * (SS * V"expression")^-1 * SS * V";" 
							 * (SS * V"expression")^-1 * SS * V")" 
							 * SS * V"statement"
					+ V"for" * SS * V"(" * SS * V"declaration" 
							 * (SS * V"expression")^-1 * SS * V";" 
							 * (SS * V"expression")^-1
							 * SS * V")" * SS * V"statement"
													
-- jump-statement: 
--		goto identiﬁer ; 
--		continue ; 
--		break ; 
--		return expression(opt) ;
jump_statement = V"goto" * SS * V"identifier" * SS * V";"
			   + V"continue" * SS * V";"
			   + V"break" * SS * V";"
			   + V"return" * (SS * V"expression")^-1 * SS * V";"
			   
-------------------------------------------
-- External Definitions

-- translation-unit: 
--		external-declaration 
--		translation-unit external-declaration
translation_unit = (SS * V"external_declaration")^1

-- external-declaration: 
--		function-definition 
--		declaration
external_declaration = V"function_definition" + V"declaration"

-- function-definition: 
--		declaration-specifiers declarator declaration-list(opt) compound-statement
function_definition = V"declaration_specifiers" * SS * V"declarator"
								* (SS * V"declaration_list")^-1 
								* SS * V"compound_statement"
								
function_declaration = V"declaration_specifiers" * SS * V"declarator"
								* (SS * V"declaration_list")^-1 
								
-- declaration-list: 
--		declaration 
--		declaration-list declaration
declaration_list = (SS * V"declaration")^1

			   
identifier_rules = {
	keyword = keyword,
	identifier = identifier,
	identifier_nondigit = identifier_nondigit,
	nondigit = nondigit,
	digit = digit,
	
	universal_character_name = universal_character_name,
	hex_quad = hex_quad,
	
	hexadecimal_digit = hexadecimal_digit,
}

constant_rules = {
	constant = constant,
	integer_constant = integer_constant,
	floating_constant = floating_constant,
	enumeration_constant = enumeration_constant,
	character_constant = character_constant,
	
	decimal_constant = decimal_constant,
	nonzero_digit = nonzero_digit,
	octal_constant = octal_constant,
	decimal_floating_constant = decimal_floating_constant,
	fractional_constant = fractional_constant,
	digit_sequence = digit_sequence,
	hexadecimal_constant = hexadecimal_constant,
	hexadecimal_prefix = hexadecimal_prefix,
	hexadecimal_floating_constant = hexadecimal_floating_constant,
	binary_exponent_part = binary_exponent_part,
	octal_digit = octal_digit,
	c_char_sequence = c_char_sequence,
	c_char = c_char,
	escape_sequence = escape_sequence,
	simple_escape_sequence = simple_escape_sequence,
	octal_escape_sequence = octal_escape_sequence,
	hexadecimal_escape_sequence = hexadecimal_escape_sequence,
	hexadecimal_fractional_constant = hexadecimal_fractional_constant,
	hexadecimal_digit_sequence = hexadecimal_digit_sequence,
	floating_suffix = floating_suffix,
	integer_suffix = integer_suffix,
	unsigned_suffix = unsigned_suffix,
	long_suffix = long_suffix,
	long_long_suffix = long_long_suffix,
	exponent_part = exponent_part,
	sign = sign,
	--universal_escape_sequence = universal_escape_sequence,
}

string_literal_rules = {
	string_literal = string_literal,
	s_char_sequence = s_char_sequence,
	s_char = s_char,
}

expression_rules = {
	primary_expression = primary_expression,
	argument_expression_list = argument_expression_list,
	postfix_expression = postfix_expression,
		_postfix_expression = _postfix_expression,
		postfix_suffix = postfix_suffix,
	unary_expression = unary_expression,
	unary_operator = unary_operator,
	cast_expression = cast_expression,
	multiplicative_expression = multiplicative_expression,
	additive_expression = additive_expression,
	shift_expression = shift_expression,
	relational_expression = relational_expression,
	equality_expression = equality_expression,
	AND_expression = AND_expression,
	exclusive_OR_expression = exclusive_OR_expression,
	inclusive_OR_expression = inclusive_OR_expression,
	logical_AND_expression = logical_AND_expression,
	logical_OR_expression = logical_OR_expression,
	conditional_expression = conditional_expression,
	assignment_expression = assignment_expression,
	assignment_operator = assignment_operator,
	expression = expression,
	constant_expression = constant_expression,
}
for i, v in ipairs(punctuators) do
	expression_rules[v] = P(v)
end

declaration_rules = {
	declaration = declaration,
	declaration_specifiers = declaration_specifiers,
	init_declaration_list = init_declaration_list,
	
	init_declarator = init_declarator,
	storage_class_specifier = storage_class_specifier,
	type_specifier = type_specifier,
	struct_or_union_specifier = struct_or_union_specifier,
	struct_or_union = struct_or_union,
	struct_declaration_list = struct_declaration_list,
	
	struct_declaration = struct_declaration,
	specifier_qualifier_list = specifier_qualifier_list,
	struct_declarator_list = struct_declarator_list,
	struct_declarator = struct_declarator,
	enum_specifier = enum_specifier,
	enumerator_list = enumerator_list,
	
	enumerator = enumerator,
	type_qualifier = type_qualifier,
	function_specifier = function_specifier,
	declarator = declarator,
	
	direct_declarator = direct_declarator,
	pointer = pointer,
	type_qualifier_list = type_qualifier_list,
	parameter_type_list = parameter_type_list,
	parameter_list = parameter_list,
	parameter_declaration = parameter_declaration,
	
	identifier_list = identifier_list,
	type_name = type_name,
	abstract_declarator = abstract_declarator,
	direct_abstract_declarator_suffix = direct_abstract_declarator_suffix,
	direct_abstract_declarator = direct_abstract_declarator,
	typedef_name = typedef_name,
	initializer = initializer,
	
	initializer_list = initializer_list,
	designation = designation,
	designator_list = designator_list,
	designator = designator,
}
for i, v in pairs(_keywords) do
	declaration_rules[v] = P(v)*(-N_AZ_dot)
end

statement_rules = {
	statement = statement,
	labeled_statement = labeled_statement,
	compound_statement = compound_statement,
	block_item_list = block_item_list,
	block_item = block_item,
	expression_statement = expression_statement,
	selection_statement = selection_statement,
	iteration_statement = iteration_statement,
	jump_statement = jump_statement,
}

external_definition_rules = {
	translation_unit = translation_unit,
	external_declaration = external_declaration,
	function_definition = function_definition,	
	declaration_list = declaration_list,
}

all_rules = combine(
	identifier_rules, 
	constant_rules, 
	string_literal_rules, 
	expression_rules,
	declaration_rules,
	statement_rules,
	external_definition_rules
)

token_patt = P(combine({
		[1] = "token",
		token = token,
		keyword = keyword,
		punctuator = punctuator,
	},
	identifier_rules,
	constant_rules,
	string_literal_rules
))

any = comment + token_patt + space