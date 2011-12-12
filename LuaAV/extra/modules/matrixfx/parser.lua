local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local ipairs = ipairs
local pairs = pairs
local print = print

local math = require 'math'
local table = require 'table'
local string = require 'string'

local lpeg = require("lpeg")
local P, R, S, C, Ca, Carg, Cc, Cg, Ct, Cp, Cmt, V = lpeg.P, lpeg.R, lpeg.S, lpeg.C, lpeg.Ca, lpeg.Carg, lpeg.Cc, lpeg.Cg, lpeg.Ct, lpeg.Cp, lpeg.Cmt, lpeg.V


module("matrixfx.parser")
-----------------------------------------------------
-- ignored:
local any = P(1)
local newline = S('\r\n\f')
local space = S('\t\v ')
local white = (space + newline)^0
local terminator = (space^-1 * newline) + (white^-1 * P';')	-- semicolon or newline

local digit = R('09')

-- strings - anything surrounded by a pair of quotes (can run over newlines)
local quoted = P("'") * ((1 - P("'"))^0) * P("'")
local doublequoted = P('"') * ((1 - P('"'))^0) * P('"')
local anystring = quoted + doublequoted

-- comments (C/C++ style):
local singlecomment = P('//') * space^-1 * C((1 - newline)^0)
local multicomment = P('/*') * space^-1 * C((1 - P('*/'))^0) * P('*/')
local comment = singlecomment + multicomment

-- numbers
local number_sign = S'+-'^-1
local number_decimal = digit^1
local number_hexadecimal = P'0' * S'xX' * R('09', 'AF', 'af')^1
local number_float = (P'.' * digit^1 +
                     number_sign*digit^1 * (P'.' * digit^0)^-1 ) *
                    (S'eE' * number_sign * digit^1)^-1
local number = number_hexadecimal + number_float + number_decimal

local keywords = 
	P"return"
	
local var_type = 
	P"cell"

-- valid characters for identifiers (except first character)
local letter = R('az', 'AZ')
local idsafe = letter + digit + P'_'
local identifier = (letter * idsafe^0) - keywords * -idsafe
-----------------------------------------------------


--local function ptag(x) return Cp() * Cc(x) end
local function ptag(x) return Cc(x) end
local function op(x) return space^0 * x * space^0 end
local function preop(x) return space^0 * x end
local function postop(x) return x * space^0 end

local
function ast(v1, op, v2, ...)
	if(op) then
		return ast({
				v1, v2, 
				op = op
			},
			...)
	else
		return v1
	end
end


local g = {	
	"Block",
	
	Block = (ptag("block") * (V"Statement"^0)) / function(tag, ...)
			local statements = {...}
			statements.op = tag
			return statements
		end,
	Statement = ((white * V"LeftHand" * ptag("assign")*P"=" * space^0 * V"Expr" * terminator) + 
						white * V"LeftHand") / ast,
	
	-- matches 1 or more indentifiers
	LeftHand = (space^0*V"ExprRef"*space^0)^1 / function(...)
	
		local args = {...}
		if(#args == 2) then
			return ast(
				args[1],
				"decl",
				args[2])
		else
			return args[1]
		end
	end,
	
	Expr = V"ExprIncOr",
	
	-- in order of increasin precedence
	ExprIncOr = V"ExprExcOr" * (V"OpIncOr" * V"ExprExcOr")^0 / ast,
	ExprExcOr = V"ExprAnd" * (V"OpExcOr" * V"ExprAnd")^0 / ast,
	ExprAnd = V"ExprBitIncOr" * (V"OpAnd" * V"ExprBitIncOr")^0 / ast,
	ExprBitIncOr = V"ExprBitExcOr" * (V"OpBitIncOr" * V"ExprBitExcOr")^0 / ast,
	ExprBitExcOr = V"ExprBitAnd" * (V"OpBitExcOr" * V"ExprBitAnd")^0 / ast,
	ExprBitAnd = V"ExprEquality" * (V"OpBitAnd" * V"ExprAnd")^0 / ast,
	ExprEquality = V"ExprRelational" * (V"OpEquality" * V"ExprRelational")^0 / ast,
	ExprRelational = V"ExprAdditive" * (V"OpRelational" * V"ExprAdditive")^0 / ast,
	ExprAdditive = V"ExprMultiplicative" * (V"OpAdditive" * V"ExprMultiplicative")^0 / ast,
	
	ExprMultiplicative = V"ExprPrefix" * (V"OpMultiplicative" * V"ExprPostfix")^0 / ast,
	
	
	ExprPrefix = (V"OpPrefix" / function(tag, v1) 
					return ast(v1, tag)
				end) + V"ExprPostfix",
	ExprPostfix = (V"OpPostfix" + V"ExprRef") / ast,
	ExprRef = V"Constant" * (V"OpRef" * V"Constant")^0 / ast,
	Constant = (V"OpArray" + V"OpParen" + V"OpFunc" + V"Const") / ast,
	Const = C(ptag("const")*(number + identifier)) / ast,
	Num = C(ptag("const")*(number)) / ast,

	-- operators and precedence groupings
	OpIncOr = ptag("inc_or")*op(P"||"),			-- 14
	OpExcOr = ptag("exc_or")*op(P"^^"),			-- 13
	OpAnd = ptag("and")*op(P"&&"),				-- 12
	OpBitIncOr = ptag("bit_inc_or")*op(P"|"),	-- 11
	OpBitExcOr = ptag("bit_exc_or")*op(P"^"),	-- 10
	OpBitAnd = ptag("bit_and")*op(P"^"),		-- 09
	
	OpEquality = V"OpEq" + V"OpNotEq",			-- 08
		OpEq = ptag("eq")*op(P"=="),
		OpNotEq = ptag("not_eq")*op(P"!="),
	
	OpRelational = V"OpLess" + V"OpGreater" +	-- 07
				V"OpLessEq" + V"OpGreaterEq",
		OpLess = ptag("less")*op(P"<"),
		OpGreater = ptag("greater")*op(P">"),
		OpLessEq = ptag("less_eq")*op(P"<="),
		OpGreaterEq = ptag("greater_eq")*op(P">="),
	
	-- bit shift								-- 06
	
	OpAdditive = V"OpAdd" + V"OpSub",			-- 05
		OpAdd = ptag("add")*op(P"+"),
		OpSub = ptag("sub")*op(P"-"),
		
	OpMultiplicative = V"OpMul" + V"OpDiv",		-- 04
		OpMul = ptag("mul")*op(P"*"),
		OpDiv = ptag("div")*op(P"/"),
		-- OpModulo
		
	-- prefix									-- 03
	OpPrefix = (V"OpPreIncr" + V"OpPreDecr" + 
			V"OpNegate" + V"OpNot") * V"ExprRef",
		OpPreIncr = ptag("preincr")*preop(P"++"),
		OpPreDecr = ptag("predecr")*preop(P"--"),
		OpNegate = V"Num" + ptag("negate")*preop(P"-"),
		OpNot = ptag("logicalnot")*preop(P"!"),
		
	-- postfix
	OpPostfix = V"Constant" * (V"OpPostIncr" + V"OpPostDecr"),
		OpPostIncr = ptag("postincr")*postop(P"++"),
		OpPostDecr = ptag("postdecr")*postop(P"--"),
		
	OpRef = V"OpField",							-- 02
		OpField = ptag("field")*P".",

	OpArray = (V"Const"*ptag("array")*(op(P"[") * V"Expr" * op(P"]"))) / ast,

	OpParen = ptag("paren")*(op(P"(") * V"Expr" * op(P")")) / function(tag, expr) return ast(expr, tag) end,

	OpFunc = (ptag("call")*(C(identifier) * white * op(P"(") * V"Sequence" * op(P")"))) /
		 function(tag, name, ...)
				local args = {...}
				args.op = tag
				args.name = name
				return args
			end,
	Sequence = V"Expr" * (white * P"," * white * V"Expr")^0,
}

local parser = P(g)

function parse(code)
	return lpeg.match(parser, code)
end
