local Def = require "audio.Def"

-- pretty printer:
local util = require "app.utilities"
local printt = util.printt

local m = require "listlpeg"
local L = m.L
local P = m.P
local C = m.C
local Ct = m.Ct
local Cc = m.Cc
local Cg = m.Cg
local Cp = m.Cp
local R = m.R
local S = m.S
local V = m.V

local space = S("\t ")
local _ = space^0
local nl = S"\n\r\f"^1
local ws = (space + nl)^0


local alpha = R"az" + R"AZ"
local digit = P"0" + R"19"
local int = P"-"^-1 * digit^1
local float = (int * P"." * digit^1) 
			+ (P"-"^-1 * P"." * digit^1)
local num = (float + int) / tonumber
local str = P"\"" * C((1-P"\"")^0) * P"\""


local reserved = (P"instr" + P"endin" + P"opcode" + P"endop") 
local idok = alpha + P"_" + "+"
local name = (idok * (idok + digit)^0) - reserved
local iname = (P"i" * (idok + digit)^0) - reserved
local aname = (P"a" * (idok + digit)^0) - reserved
local kname = (P"k" * (idok + digit)^0) - reserved
local xname = (P"x" * (idok + digit)^0) - reserved

local comment = P";" * C(1 - nl)

local eol = space^0 * comment^-1 * nl^1

local
Lexeme = function(patt, name)
	return ws * patt * ws
end

local
Rule = function(patt, name)
	return ws * Ct(
		patt *
		Cg(Cc(name), "rule")
	) * ws
end



--[[

--local orc = header * (udo + instrument)^0

local header
local udo
local instrument

--  label:   result opcode argument1, argument2, ... ;comments
-- Every orchestra statement must be on a single line, \
-- however long lines can be wrapped to a new line using the '\' character.
local statement



-- ; until end of line
-- /* c style multi-line */
local comment


-- rates: 
-- setup-rate (constant)
-- note-rate (i prefix)
-- control-rate (k prefix or x prefix)
-- audio-rate (a prefix or x prefix)


-- HEADER --
-- 0dbfs ctrlinit ftgen kr ksmps massign nchnls pgmassign pset seed sr strset

local alpha = R"az" + R"AZ"
local idok = alpha + P"_" + "+"
local name = alpha + (idok)^0

-- INSTR --
local instr = Token"instr" * (Token(integer) + Token(name))
local endin = Token"endin"

-- expressions -- 
	i1 = 2 ^ 12

-- UDO --
local opcode = Token"opcode" * ?
-- uses xin and xout
local endin = Token"endop"

-- predefined constants
$M_E	2.7182818284590452354	e
$M_LOG2E	1.4426950408889634074	log_2(e)
$M_LOG10E	0.43429448190325182765	log_10(e)
$M_LN2	0.69314718055994530942	log_e(2)
$M_LN10	2.30258509299404568402	log_e(10)
$M_PI	3.14159265358979323846	pi
$M_PI_2	1.57079632679489661923	pi/2
$M_PI_4	0.78539816339744830962	pi/4
$M_1_PI	0.31830988618379067154	1/pi
$M_2_PI	0.63661977236758134308	2/pi
$M_2_SQRTPI	1.12837916709551257390	2/sqrt(pi)
$M_SQRT2	1.41421356237309504880	sqrt(2)
$M_SQRT1_2	0.70710678118654752440	1/sqrt(2)
--]]


local patt = Rule(name, "name")

local expr = V"expr"
--local binop = V"binop"

-- in1, in2...
local arglist = (V"expr" * (_ * P"," * _ * V"expr")^0)


local param = Ct( Cg(Cc"param", "opcode") * P"p" * C(digit^1) )
local ivar = Ct( Cg(Cc"ivar", "opcode") * C(iname) )
local kvar = Ct( Cg(Cc"kvar", "opcode") * C(kname) )
local avar = Ct( Cg(Cc"avar", "opcode") * C(aname) )
local xvar = Ct( Cg(Cc"xvar", "opcode") * C(xname) )
local varexpr = param + ivar + kvar + avar + xvar + Ct( Cg(Cc"var", "opcode") * C(name) )

local funexpr = Ct( Cg(name, "opcode") * P"(" * arglist * P")" )
local parenexpr = P"(" * expr * P")"
local numexpr = Ct( Cg(Cc"constant", "opcode") * num )

-- operators:
local 
function left_op(x, pos, o, y, ...)
	return o and left_op({x, opcode=o, y }, pos, ...) or x
end
function Binop(O, B)
	return (B * Cp() * (_ * O * _ * B)^0) / left_op
end

local left1 = Binop(C"^" + C"%", V"primexpr")
local left2 = Binop(C"/" + C"*", left1)
local left3 = Binop(C"+" + C"-" + C"|", left2)
local left4 = Binop(C"<=" + C">=" + C"==" + C"!=" + C">" + C"<", left3)
local binop = left4
local cond = Ct(Cg(Cc"cond", "opcode") * binop * _ * P"?" * _ * binop * _ * ":" * _ * binop)

local outname = Cg(name, "out")

-- TODO: validate opcode name, or give up
local opcode = Cg(name, "opcode")

-- [out] opcode [expr ...]
local opstat = outname * _ * opcode * _ * arglist
local opstat0 = opcode * _ * arglist
-- [out] = expr
local exprstat = outname * _ * P"=" * Cg(Cc(":="), "opcode") * _ * expr

local statement = Ct(_ * (opstat + opstat0 + exprstat)) * eol

-- instr -- 
local instrbegin = P"instr" * ws * Cg(name, "name") * eol
local endin = P"endin" * eol
local instr = instrbegin * ws * (statement)^0 * ws * endin


local G = P{
	Ct( Rule(instr, "instr")^0 ),
	
	-- complex expr rules:
	primexpr = parenexpr + funexpr + varexpr + numexpr,
	expr = cond + binop,
}

local opcodes = {
	["*"] = Def.Mul,
	["+"] = Def.Add,
	["/"] = Def.Div,
	["-"] = Def.Sub,
	["%"] = Def.Mod,
	["^"] = Def.Pow,
	["<"] = Def.Lt,
	[">"] = Def.Gt,
	["<="] = Def.Lte,
	[">="] = Def.Gte,
	["=="] = Def.Eq,
	["!="] = Def.Neq,
	cosinv = Def.ACos,
	sininv = Def.ASin,
	taninv = Def.ATan,
	taninv2 = Def.ATan2,
	abs = Def.Abs,
	ceil = Def.Ceil,
	floor = Def.Floor,
	log = Def.Log,
	log10 = Def.Log10,
	pow = Def.Pow,
	sqrt = Def.Sqrt,
	max = Def.Max,
	min = Def.Min,
	
	phasor = Def.Phasor,
	noise = Def.noise.White,
	pinkish = Def.noise.Pink,
	lowpass2 = Def.biquad.LP,
	
	-- special:
	oscil = function(args) return Def.Mul{ args[1], Def.SinOsc{ args[2] } } end,
	param = function(args) return Def.P("p"..tostring(args[1])) end,
}

local 
function parseexpr(node, env)
	if type(node) == 'string' then
		return tostring(node)
	else
		local op = node.opcode
		--print("parseexpr", op)
		if op == "constant" then
			return node[1]
		elseif op == "var" or op == "avar" or op == "ivar" or op == "kvar" then
			local name = assert(node[1])	
			local val = assert(env[name], "missing var "..name)
			return val
		end
		local args = {}
		for i, v in ipairs(node) do
			args[i] = parseexpr(v, env)
		end
		if op == "out" then
			return args[1]
		elseif op == ":=" then
			return args[1]
		end
		local ctor = opcodes[op]
		if ctor then
			return ctor(args)
		end
		error("unknown opcode " .. op)
	end
end

local
function parse(str)
	local ast = G:match(str)
	local defs = {}
	for i, instr in ipairs(ast) do
		local def = { p3 = 1, p4 = 440, p5 = 0, p6 = 0 }
		local env = {}
		-- iterate statements:
		for i, stat in ipairs(instr) do
			local e = parseexpr(stat, env)
			local out = stat.out 
			if out then
				env[out] = e
			end
			if stat.opcode == "out" then
				def[1] = e + (Def.EnvDriver{ dur="p3" } * 0)
			end
		end
		--printt(def)
		defs[instr.name] = Def(def)
	end
	return defs
end

return {
	parse = parse,
}