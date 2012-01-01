local string, table, math = string, table, math
local type, tonumber = type, tonumber
local unpack, select = unpack, select
local getfenv = getfenv
local print, tostring = print, tostring
local setmetatable, getmetatable = setmetatable, getmetatable
local pairs, ipairs = pairs, ipairs
local assert, error = assert, error
local io = io

local clang = require "clang"
local app = require"app"
local clang_headers = app.resourcepath .. "/Headers"

local audio = require "audio"
local wait = wait
local collectgarbage = collectgarbage

local lfs = require "lfs"

--- Constructing audio synthesis objects from expression trees
-- The Def module is a table containing a range of expression generators, and is also a callable function as a constructor of Synth definitions from trees of expressions. Better described by example:
-- <luacode>
--  local Def = require "audio.Def"
--  Def.globalize()
--  -- create a concrete synth definition (JIT-compiled), returns a constructor
--  local example = Def{
--  	-- parameters (settable, gettable) of the synth, and default values:
--  	dur = 1, 
--  	amp = 0.5,
--  	-- use lists for multi-channel parameters:
--  	freq = { 400, 400 }, 
--  	-- compound expression describing a synthesis process
--  	Pan2{ 
--  		Lag{ P"amp" * Env{ P"dur" }, 0.9 } 
--  			* SinOsc{ Lag{ P"freq", 0.9 } }, 
--  		pan=SinOsc{ freq=8 } 
--  	},
--  }
--  -- call the generated constructor to create a voice (genarating sound)
--  local synth = example{ amp = 0.2, dur = 2, freq = 800 }
--  -- paramaters become settable/gettable properties
--  synth.freq = { 500, 600 }
-- </luacode>
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @name audio.Def
module(...)

--------------------------------------------------------------------------------
-- utils
--------------------------------------------------------------------------------

local format = string.format
local function prefix(p, v)
	return format("%s_%s", p, tostring(v))
end
local printf = function(...) print(string.format(...)) end
local next_uid = 0
local function uid(p)
	next_uid = next_uid + 1
	return p and prefix(p, next_uid) or next_uid
end

local function table_copy(src, memo)
	local dst = setmetatable({}, getmetatable(src))
	local memo = memo or {}
	for k, v in pairs(src) do
		if memo[v] then
			dst[k] = memo[v]
		elseif type(v) == "table" then
			dst[k] = table_copy(v, memo)
			memo[v] = dst[k]
		else
			dst[k] = v
			memo[v] = dst[k]
		end
	end
	return dst
end


local function arr_concat(a, b)
	local r = {unpack(a)}
	for i, v in ipairs(b) do table.insert(a, v) end
	return r
end

local function splice_duplicates(...)
	local m, r = {}, {}
	for _, t in ipairs{...} do
		for i, v in ipairs(t) do
			if not m[v] then table.insert(r, v) end
			m[v] = true
		end
	end
	return r
end

local function keysorter(a, b) 
	if type(a) == type(b) and (type(a) == "number" or type(a) == "string") then 
		return a<b 
	end
	return type(a) > type(b) 
end


local function table_tostring(tt, indent, done)
	done = done or {}
	indent = indent or 0
	if type(tt) == "table" then
		if done[tt] then
			return format("<memoized: %s>", tostring(tt))
		else
			done[tt] = true
			
			-- sort keys:
			local keys = {}
			for k, v in pairs(tt) do table.insert(keys, k) end
			table.sort(keys, keysorter)
			
			local kt = {}
			for i, key in ipairs(keys) do	
				if type(key) == "string" then
					keystr = format("%s", tostring(key))
				else
					keystr = format("[%s]", tostring(key))
				end
				local value=tt[key]
				table.insert(kt, 
					format("%s%s = %s,\n", 
						string.rep(" ", indent+2), 
						keystr, 
						table_tostring(value, indent+2, done))
				)
			end
			return format("{\n%s%s}", table.concat(kt), string.rep(" ", indent))
		end
	else
		if type(tt) == "string" then
			return format("%q", tt)
		else
			return format("%s", tostring(tt))
		end
 	end
end
local printt = function(t) print(table_tostring(t)) end

-- accumulator, useful in code generation
local accumulator = {}
accumulator.__index = accumulator

function accumulator:write(fmt, ...)
	table.insert(self, format(fmt, ...))
end
function accumulator:concat(delim, close)
	return table.concat(self, delim or "\n") .. (close or "")
end
function accumulator:__tostring()
	return "accumulator: "..self:concat()
end

setmetatable(accumulator, {__call = function(meta, init)
	return setmetatable(init or {}, accumulator)
end})


--------------------------------------------------------------------------------
-- builtins
--------------------------------------------------------------------------------

local intrinsics = {}

intrinsics["constant"] = function(v) return string.format("(%s)", tostring(v[1])) end
intrinsics["and"] = function(v) return string.format("(%s)", table.concat(v, " && ")) end
intrinsics["or"] = function(v) return string.format("(%s)", table.concat(v, " || ")) end

intrinsics.add = function(v) return string.format("(%s)", table.concat(v, " + ")) end
intrinsics.mul = function(v) return string.format("(%s)", table.concat(v, " * ")) end
intrinsics.sub = function(v) return string.format("(%s - %s)", tostring(v[1]), tostring(v[2])) end
intrinsics.div = function(v) return string.format("(%s / (number)%s)", tostring(v[1]), tostring(v[2])) end
intrinsics.mod = function(v) return string.format("fmod(%s,%s)", tostring(v[1]), tostring(v[2])) end
intrinsics.neg = function(v) return format("(-%s)", tostring(v[1])) end
intrinsics.min = function(v) return 
	string.format("(%s<%s?%s:%s)", tostring(v[1]), tostring(v[2]), tostring(v[1]), tostring(v[2])) 
end
intrinsics.max = function(v) return 
	string.format("(%s<%s?%s:%s)", tostring(v[1]), tostring(v[2]), tostring(v[2]), tostring(v[1])) 
end
intrinsics.sign = function(v) return string.format("(%s<0?-1.:(%s>0?1.:0.))", tostring(v[1]), tostring(v[1])) end

intrinsics.gt = function(v) return string.format("(%s > %s)", tostring(v[1]), tostring(v[2])) end
intrinsics.lt = function(v) return string.format("(%s < %s)", tostring(v[1]), tostring(v[2])) end
intrinsics.gte = function(v) return string.format("(%s >= %s)", tostring(v[1]), tostring(v[2])) end
intrinsics.lte = function(v) return string.format("(%s <= %s)", tostring(v[1]), tostring(v[2])) end
intrinsics.eq = function(v) return string.format("(%s == %s)", tostring(v[1]), tostring(v[2])) end
intrinsics.neq = function(v) return string.format("(%s != %s)", tostring(v[1]), tostring(v[2])) end
intrinsics["not"] = function(v) return string.format("(!%s)", tostring(v[1])) end

intrinsics.done = function(v) return string.format("if (%s >= %s) { synth.audio_stop(); return; }", tostring(v[1]), tostring(v[2])) end

intrinsics.notdone = function(v) return string.format("if (%s >= %s) { %s = %s; }", tostring(v[1]), tostring(v[2]), tostring(v[1]), tostring(v[2])) end

intrinsics["switch"] = function(v)
	local result = v[1]
	local option = v[#v]
	local code = { format("switch(int(%s)) {", option) }
	for i = 2, #v-1 do
		code[#code+1] = format("\tcase %d: %s = %s; break;", i-2, tostring(result), tostring(v[i]))
	end
	code[#code+1] = "\tdefault: break;"
	code[#code+1] = "}"
	return table.concat(code, "\n\t\t\t")
end

intrinsics.pass = function(v) return tostring(v[1]) end
intrinsics.assign = function(v) return format("%s = %s", tostring(v[1]), tostring(v[2])) end
intrinsics.mix = function(v) return format("%s += %s", tostring(v[1]), tostring(v[2])) end
intrinsics.call = function(v) 
	local args = {}
	for i=2,#v do table.insert(args, tostring(v[i])) end
	return format("%s(%s)", tostring(v[1]), table.concat(args, ",")) 
end
intrinsics.methodcall = function(v) 
	local args = {}
	for i=3,#v do table.insert(args, tostring(v[i])) end
	return format("%s.%s(%s)", tostring(v[1]), tostring(v[2]), table.concat(args, ",")) 
end

--------------------------------------------------------------------------------
-- Expr
--------------------------------------------------------------------------------

Expr = {}
function Expr:__tostring() return string.format("Expr(%s)", self.op) end

local function ugen(op)
	local f = function(...)
		local t = {...}
		if #t == 1 and type(t[1]) == "table" and getmetatable(t[1]) ~= Expr then 
			t = t[1] 
		end
		t.op = op
		return setmetatable(t, Expr)
	end
	return f
end

Expr.__add = ugen"Add"
Expr.__mul = ugen"Mul"
Expr.__sub = ugen"Sub"
Expr.__div = ugen"Div"
Expr.__mod = ugen"Mod"
Expr.__pow = ugen"Pow" 
Expr.__unm = ugen"Neg"
Expr.__index = Expr
-- __index? (for channel selection? can't index table directly {}[i] is bad syntax, shoudl be ({})[i]
-- __concat? (for sequence/parallel? will be confusing for "in" .. "freq" is not V"in" .. V"freq")

function Expr:dump() 
	printt(self)
end

--------------------------------------------------------------------------------
-- generate
--------------------------------------------------------------------------------

local function gen_expr(expr)
	local args = { name=expr.name }
	for i, v in ipairs(expr) do
		if type(v) == "table" then
			args[i] = gen_expr(v)
		else
			args[i] = tostring(v)
		end
	end
	local call
	local op = intrinsics[expr.op]
	if op then 
		call = op(args)
	else
		call = string.format("%s(%s)", expr.op, table.concat(args, ","))
	end
	return call
end

local function generate(unit)

	-- annotate all ports with offsets:
	local channelcount = 0
	for k, v in pairs(unit.state) do
		if v.ty == "Port" then
			local channels = #v
			v.offset = channelcount
			channelcount = channelcount + channels
		end
	end
	
	local flags = {}
	for k in pairs(unit.flags) do
		table.insert(flags, k)
	end
	if #flags == 0 then
		flags = "Synth::SYNTH_NONE"
	else 
		flags = table.concat(flags, "|")
	end
		
	local unit_block = accumulator{
		'#include "luaav_audio_jit.hpp"'
	}
	local class_block = accumulator{
		"struct JSynth {",
			"Synth synth;"
	}
	local init_block = accumulator{ string.format("synth(%d, %s)", channelcount, flags) }
	local ctor_block = accumulator{ "{", "synth.init(L);" }
	--local lua_block = accumulator{ "int init(lua_State * L) {" } 
	local audio_block = accumulator{ "void audioCB() {", 
		--'printf(".");'
	}
	local calc_block = accumulator{ 
		--"while (synth.io()) {" 
		"for (int i=synth.mStart; i<synth.mEnd; i++) {"
	}
	local define_block = accumulator{
		'extern "C" int define(lua_State * L) {'
	}
	local lib_block = accumulator{}
	local getters_block = accumulator{}
	local setters_block = accumulator{}
	
	
	for k in pairs(unit.depends) do
		unit_block:write(k)
	end
	--unit_block:write("using namespace gam;")
	unit_block:write("typedef sample number;")
	
	for k, v in pairs(unit.state) do
		if v.ty == "Port" then
			-- Port has special semantics
			local channels = #v
			-- TODO: init Port value?
			--init_block:write("%s(%s)", k, channels)
			--for i, default in ipairs(v) do
				-- set port:
				--ctor_block:write("%s[%d].setvalue(%s);", k, i-1, default)
			--end
			for i = 1, channels do
				--audio_block:write("Port::Reader %s_p%d = %s.read(*this, %d);", k, i, k, i-1)
				audio_block:write("PortReader %s_p%d(synth, %d);", k, i, v.offset+i-1)
				--calc_block:write("sample& %s%d = %s_p%d();", k, i, k, i)
				calc_block:write("sample& %s%d = %s_p%d[i];",  k, i, k, i)
			end
			if v.setter then
				--setters_block:write('{ "%s", LNode<Synth>::port_set<&Synth::%s> }', v.setter, k)
				setters_block:write('{ "%s", portset<%d, %d> }', v.setter, v.offset, channels)
			end
			
			-- set defaults from Context:
			--lua_block:write('port_init(L, "%s", 3, 2);', k)
			--ctor_block:write('synth.portinit(L, "%s", %d, %d);', k, v.offset, channels)
		else
			class_block:write("%s %s;", v.ty, k)
			local init = table.concat(v, ",")
			init_block:write("%s(%s)", k, init)
			if v.setter then
				setters_block:write('{ "%s", LNode<Synth>::set<%s, &Synth::%s> }', v.setter, v.ty, k)
			end
		end
	end
	
	for rate, b in pairs(unit.blocks) do
		local block = ctor_block
		if rate == "a" then block = calc_block
		elseif rate == "c" then block = audio_block
		end
		for i, v in ipairs(b) do
			local call = gen_expr(v)
			if v.name then
				block:write("%s %s = %s;", "sample", v.name, call)
			else
				block:write("%s;", call)
			end
		end
	end
	
	class_block:write("JSynth(lua_State * L) \n\t:\t%s %s", init_block:concat(",\n\t\t"), ctor_block:concat("\n\t\t"))
	class_block:write("}")
	class_block:write('~JSynth() { /*printf("delete synth\\n");*/ }')
	
	audio_block:write("%s", calc_block:concat("\n\t\t\t"))
	audio_block:write("}")
	
	class_block:write("%s", audio_block:concat("\n\t\t"))
	class_block:write("}")
	
	--class_block:write("%s", lua_block:concat("\n\t\t"))
	--class_block:write("}")

	define_block:write("static luaL_reg lib[] = {")
	lib_block:write("{ NULL, NULL }")
	define_block:write("\t%s", lib_block:concat(",\n\t\t"))
	define_block:write("};")
	
	define_block:write("static luaL_reg getters[] = {")
	getters_block:write("{ NULL, NULL }")
	define_block:write("\t%s", getters_block:concat(",\n\t\t"))
	define_block:write("};")
	
	define_block:write("static luaL_reg setters[] = {")
	setters_block:write("{ NULL, NULL }")
	define_block:write("\t%s", setters_block:concat(",\n\t\t"))
	define_block:write("};")
	
	--define_block:write("LNode<Synth>::defineJIT(L, lib, getters, setters);")
	
	define_block:write("luaav_synth_newmetatable(L, lib, getters, setters);")
	define_block:write("lua_pushvalue(L, lua_upvalueindex(1));")
	define_block:write("lua_pushcclosure(L, SynthBinding<JSynth>::create, 4);")
	define_block:write("return 1;")

	unit_block:write("%s", class_block:concat("\n\t"))
	unit_block:write("};")
	
	unit_block:write("%s", define_block:concat("\n\t"))
	unit_block:write("}")

	return unit_block:concat("\n")
end

-- re-use the same compiler:
local cc = clang.Compiler()
cc:include(clang_headers)
-- preload the standard header:
---[[
local name = "luaav_audio_jit.hpp"
local path = clang_headers .. "/" .. name
local file = io.open(string.format("%s/%s", clang_headers, name))
local code = file:read("*a")
file:close()
cc:header(name, code)
--]]

--local 
function compile(code, proto)
	--print(code)
	--wait(0.1)
	
	cc:compile(code)
	cc:optimize()
	--cc:dump()	
	
	local jit = assert(cc:jit(), "unable to JIT-compile")
	local define = assert(jit:pushcfunction("define"), "JITted define-function not found")
	local ctor = define(audio.root, proto)
	collectgarbage()
	assert(ctor and type(ctor) == "function", "couldn't JIT the define function")
	return ctor
end

--------------------------------------------------------------------------------
-- Parse
--------------------------------------------------------------------------------



local function rate_max(a, b) return a < b and a or b end
local function max_rate(inputs, init) 
	local rate = init or "c"
	for i, v in ipairs(inputs) do
		rate = rate < v.rate and rate or v.rate
	end
	return rate
end

local function max_channels(inputs)
	local channels = 1
	for i, v in ipairs(inputs) do
		if type(v) == "table" then
			channels = math.max(channels, #v)
		end
	end
	return channels
end

local function input_channel(input, c)
	return input[1+((c-1) % #input)]
end

local function inputsforchannel(inputs, c)
	local res = {}
	for i, v in ipairs(inputs) do
		res[i] = v[1+((c-1) % #v)]
	end
	return unpack(res)
end

local function statement(unit, rate, expr)
	assert(expr.op)
	table.insert(unit.blocks[rate], expr)
end

local function member(unit, name, ty, setter, ...)
	assert(type(name) == "string")
	assert(ty and type(ty) == "string")
	assert(setter==nil or type(setter) == "string")
	assert(unit.state[name] == nil, string.format("member %s already exists", name))
	local v = { ty=ty, setter=setter, ... }
	unit.state[name] = v
	return v
end

local parser = {}

-- main function used to traverse the def tree (recursive)
-- the return value is a data-flow value (an array of names), annotated with rate
-- table nodes are handled recursively, delegating to the parser[op] handler
-- string nodes lookup names in the unit.state list
local function parse(unit, node, memo)
	memo = memo or {}
	local visited = memo[node]
	if visited then 
		if visited == "feedback" then
			-- TODO: some way to handle feedback nodes
			error("feedback loop at "..tostring(node))
		else
			return memo[node] 
		end
	end
	if type(node) == "table" then
		memo[node] = "feedback"	-- flag to detect loops during traversal
		--print("begin parsing", node)
		local inputs = {}
		-- handle inputs:
		for i, v in ipairs(node) do
			inputs[i] = parse(unit, v, memo)
		end
		
		-- if node has no op, interpret as multi-channel node.
		if not node.op then
			node.op = "multichannel"
		end
		
		-- parse node op:
		assert(parser[node.op], "no parser for op "..(node.op or "?"))
		--print(node.op)
		local outputs = parser[node.op](unit, node, inputs)
		--print(unpack(outputs), outputs.rate)
		--print("end parsing", node)
		memo[node] = outputs
		return outputs
	elseif type(node) == "string" then
		-- lookup string in unit:
		local outputs = parser.id(unit, {node}, nil)
		memo[node] = outputs
		return outputs
	elseif type(node) == "number" then
		return { rate="k", tostring(node) }
	else
		error("unexpected node type "..type(node))
	end	
end

--------------------------------------------------------------------------------
-- ugen parsers
--------------------------------------------------------------------------------

function parser.Port(unit, node, inputs)
	local name = node.name
	-- lookup string in unit:
	local v = unit.state[name]
	if not v then
		-- lookup name in context?
		-- create a default Port member?
		-- guess number of channels?
		v = member(unit, name, "Port", name, unpack(node))
	end
	assert(v.ty == "Port")
	
	local outputs = {}
	-- return no. channels according to port
	outputs.rate="a"
	for i=1,#v do
		outputs[i] = string.format("%s%d", name, i)
	end
	return outputs
end

function parser.id(unit, node, inputs)
	local name = node[1]
	-- lookup string in unit:
	local v = unit.state[name]
	if not v then
		-- lookup name in context?
		-- create a default Port member?
		-- guess number of channels?
		v = member(unit, name, "Port", name, 0, 0)
	end
	
	local outputs = {}
	-- Port is a special case because it supports multi-channel semantics:
	if v.ty == "Port" then
		-- return no. channels according to port
		outputs.rate="a"
		for i=1,#v do
			outputs[i] = string.format("%s%d", name, i)
		end
	else
		-- as long as we assume this is a state variable, rate can be 'i'
		outputs.rate = "i"
		outputs[1] = name
	end
	return outputs
end

-- mixes multiple input streams into a multi-channel stream
-- sort of a no-op; just passes names through
-- except: what to do if inputs have > 1 channel?
function parser.multichannel(unit, node, inputs)
	
	local rate = max_rate(inputs, "i")
	local outputs = { rate=rate }
	for i, input in ipairs(inputs) do
		outputs[i] = input[1]
	end
	return outputs
end

-- first input is the stream to select, 
-- remaining inputs are channel selections
function parser.Channels(unit, node, inputs)
	assert(#inputs >= 2)
	local src = inputs[1]
	local rate = src.rate
	local outputs = { rate=rate }
	for i = 2, #inputs do
		local input = inputs[i]
		assert(input.rate == "k", "Channels can only be selected with constants")
		local ch = tonumber(input[1])
		outputs[i-1] = input_channel(src, ch)
		
		print(ch, i-1, outputs[i-1])
	end
	return outputs
end

-- merge all input channels to a mono mix
function parser.Mono(unit, node, inputs)
	assert(#inputs >= 1)
	local src = inputs[1]
	
	if #src <= 1 then return src end
	
	local rate = src.rate
	local out = uid("mono")
	local outputs = { rate=rate, out }
	local mono = src[1]
	for i = 2, #src do
		mono = { op="add", mono, src[i] }
	end
	mono.name = out
	statement(unit, rate, mono)
	
	return outputs
end

-- merge all input channels to a stereo mix
function parser.Stereo(unit, node, inputs)
	assert(#inputs >= 1)
	local src = inputs[1]
	local rate = src.rate
	
	if #src <= 1 then return { src[1], src[1], rate=rate } end
	if #src == 2 then return src end
	
	local lname = uid("left")
	local rname = uid("right")
	
	local lsum = { name=lname, op="add", src[1] }
	local rsum = { name=rname, op="add", src[2] }
	
	local i = 3
	while i <= #src do
		table.insert(lsum, src[i])
		i = i + 1
		if i <= #src then
			table.insert(rsum, src[i])
			i = i + 1
		end
	end
	
	statement(unit, rate, lsum)
	statement(unit, rate, rsum)
	
	local outputs = { rate=rate, lname, rname }
	
	return outputs
end

local function binop(op)
	return function(unit, node, inputs)
		assert(#inputs >= 2)
		-- derive num channels:
		local channels = max_channels(inputs)
		local rate = max_rate(inputs)
		local outputs = { rate=rate }
		for c=1,channels do
			local out = uid(op)
			local a, b = inputsforchannel(inputs, c)
			statement(unit, rate, { name=out, op=op, a, b })
			outputs[c] = out
		end
		return outputs 
	end
end

local function unop(op)
	return function(unit, node, inputs)
		assert(#inputs >= 1)
		-- derive num channels:
		local channels = max_channels(inputs)
		local rate = max_rate(inputs)
		local outputs = { rate=rate }
		for c=1,channels do
			local out = uid(op)
			local a = input_channel(inputs[1], c)
			statement(unit, rate, { name=out, op=op, a })
			outputs[c] = out
		end
		return outputs 
	end
end

parser.Add = binop"add"
parser.Mul = binop"mul"
parser.Sub = binop"sub"
parser.Div = binop"div"
parser.Neg = unop"neg"

parser.Sign = unop("sign")
parser.Gt = binop("gt")
parser.Lt = binop("lt")
parser.Gte = binop("gte")
parser.Lte = binop("lte")
parser.Eq = binop("eq")
parser.Neq = binop("neq")
parser.Not = unop("not")

parser.Min = binop("min")
parser.Max = binop("max")

-- math.h bindings:
local function mathbinop(op, depend)
	return function(unit, node, inputs)
		assert(#inputs >= 2)
		unit.depends[depend] = true
		-- derive num channels:
		local channels = max_channels(inputs)
		local rate = max_rate(inputs)
		local outputs = { rate=rate }
		for c=1,channels do
			local out = uid(op)
			local a, b = inputsforchannel(inputs, c)
			statement(unit, rate, { name=out, op=op, a, b })
			outputs[c] = out
		end
		return outputs 
	end
end

local function mathunop(op, depend)
	return function(unit, node, inputs)
		assert(#inputs >= 1)
		unit.depends[depend] = true
		-- derive num channels:
		local channels = max_channels(inputs)
		local rate = max_rate(inputs)
		local outputs = { rate=rate }
		for c=1,channels do
			local out = uid(op)
			local a = inputsforchannel(inputs, c)
			statement(unit, rate, { name=out, op=op, a })
			outputs[c] = out
		end
		return outputs 
	end
end

parser.Pow = mathbinop("pow", 'extern "C" double pow(double a, double b);')
parser.Mod = mathbinop("fmod", 'extern "C" double fmod(double a, double b);')

parser.Abs = mathunop("fabs", 'extern "C" double fabs(double v);')
parser.Ceil = mathunop("ceil", 'extern "C" double ceil(double v);')
parser.Floor = mathunop("floor", 'extern "C" double floor(double v);')
parser.Sqrt = mathunop("sqrt", 'extern "C" double sqrt(double v);')
parser.Exp = mathunop("exp", 'extern "C" double exp(double v);')
parser.Log = mathunop("log", 'extern "C" double log(double v);')
parser.Log10 = mathunop("log10", 'extern "C" double log10(double v);')

parser.Sin = mathunop("sin", 'extern "C" double sin(double v);')
parser.Cos = mathunop("cos", 'extern "C" double cos(double v);')
parser.Tan = mathunop("tan", 'extern "C" double tan(double v);')
parser.ASin = mathunop("asin", 'extern "C" double asin(double v);')
parser.ACos = mathunop("acos", 'extern "C" double acos(double v);')
parser.ATan = mathunop("atan", 'extern "C" double atan(double v);')
parser.Sinh = mathunop("sinh", 'extern "C" double sinh(double v);')
parser.Cosh = mathunop("cosh", 'extern "C" double cosh(double v);')
parser.Tanh = mathunop("tanh", 'extern "C" double tanh(double v);')
parser.ATan2 = mathbinop("atan2", 'extern "C" double atan2(double a, double b);')


-- Accum?

-- library of constructors:
function parser.Mix(unit, node, inputs)
	assert(#inputs >= 2)
	
	local channels = #inputs[1] --max_channels(inputs) --#inputs[2]
	local rate = inputs[2].rate
	local outputs = { rate=rate }
	for c=1,channels do
		local instr = { 
			op="mix", 
			input_channel(inputs[1], c), 
			input_channel(inputs[2], c) 
		}
		table.insert(unit.blocks[rate], instr)
		outputs[c] = input_channel(inputs[1], c)
	end
	return outputs
end

function parser.Pan2(unit, node, inputs)
	assert(#inputs >= 1)
	
	--unit.depends['#include "Gamma/Effects.h"'] = true
	unit.depends['#include "luaav_audio_spatial.hpp"'] = true
	
	-- derive num channels:
	local channels = #inputs[1]
	local inrate = rate_max(inputs[1].rate, "c")
	local panrate = rate_max(inputs[2].rate, "i")
	local outputs = { rate="a" }
	
	local panner = uid("panner")
	
	member(unit, panner, "Pan2")
	statement(unit, panrate, { op="methodcall", panner, "pos", inputs[2][1] })
	
	local left = uid("left")
	local right = uid("right")
	if channels==1 then
		statement(unit, inrate, { name=left, op="methodcall", panner, "left", inputs[1][1] })
		statement(unit, inrate, { name=right, op="methodcall", panner, "right", inputs[1][1] })
	elseif channels==2 then
		statement(unit, inrate, { name=left, op="methodcall", panner, "left", inputs[1][1], inputs[1][2] })
		statement(unit, inrate, { name=right, op="methodcall", panner, "right", inputs[1][1], inputs[1][2] })
	else
		error("Pan2 accepts mono or stereo inputs")
	end
	outputs[1] = left
	outputs[2] = right
	return outputs
end



function parser.SinOsc(unit, node, inputs)
	assert(#inputs >= 1)
	
	unit.depends['#include "luaav_audio_oscillator.hpp"'] = true
	
	-- derive num channels:
	local channels = max_channels(inputs)
	local freqrate = inputs[1].rate or "i"
	local phaserate = inputs[2].rate or "i"
	
	local rate = "a"	-- SinOsc is audio rate.
	local outputs = { rate=rate }
	for c=1,channels do
		-- add a sine object to state:
		local objname = uid(node.op:lower())
		local out = uid(objname)
		
		member(unit, objname, "SinOsc", nil, audio.samplerate() )
		
		-- add frequency instruction:
		statement(unit, freqrate, { 
			op="methodcall", 
			objname, 
			"freq",
			input_channel(inputs[1], c) 
		})
		
		-- add phase instruction:
		statement(unit, phaserate, { 
			op="methodcall", 
			objname, 
			"phase",
			input_channel(inputs[2], c) 
		})
		
		-- add next_sample instruction:
		statement(unit, rate, { 
			name=out, 
			op="call", 
			objname 
		})
		
		outputs[c] = out
	end
	return outputs 
end

function parser.Phasor(unit, node, inputs)
	assert(#inputs >= 1)
	
	unit.depends['#include "luaav_audio_oscillator.hpp"'] = true
	
	-- derive num channels:
	local channels = max_channels(inputs)
	local freqrate = inputs[1].rate or "i"
	local phaserate = inputs[2].rate or "i"
	local rate = "a"	-- SinOsc is audio rate.
	local outputs = { rate=rate }
	for c=1,channels do
		-- add a sine object to state:
		local objname = uid(node.op:lower())
		local out = uid(objname)
		
		member(unit, objname, "Phasor", nil, audio.samplerate() )
		
		-- add frequency instruction:
		statement(unit, freqrate, { 
			op="methodcall", 
			objname, 
			"freq",
			input_channel(inputs[1], c) 
		})
		
		-- add phase instruction:
		statement(unit, phaserate, { 
			op="methodcall", 
			objname, 
			"phase",
			input_channel(inputs[2], c) 
		})
		
		-- add next_sample instruction:
		statement(unit, rate, { 
			name=out, 
			op="call", 
			objname 
		})
		
		outputs[c] = out
	end
	return outputs 
end

function parser.Buzz(unit, node, inputs)
	assert(#inputs >= 1)
	
	unit.depends['#include "luaav_audio_oscillator.hpp"'] = true
	
	-- derive num channels:
	local channels = max_channels(inputs)
	local freqrate = inputs[1].rate or "i"
	local phaserate = inputs[2].rate or "i"
	local rate = "a"	-- SinOsc is audio rate.
	local outputs = { rate=rate }
	for c=1,channels do
		-- add a sine object to state:
		local objname = uid(node.op:lower())
		local out = uid(objname)
		
		member(unit, objname, "Buzz", nil, audio.samplerate() )
		
		statement(unit, freqrate, { 
			op="methodcall", 
			objname, 
			"freq", 
			input_channel(inputs[1], c) 
		})
		
		-- add phase instruction:
		statement(unit, phaserate, { 
			op="methodcall", 
			objname, 
			"phase",
			input_channel(inputs[2], c) 
		})
		
		-- add next_sample instruction:
		statement(unit, rate, { name=out, op="call", objname })
		
		outputs[c] = out
	end
	return outputs 
end

function parser.Saw(unit, node, inputs)
	assert(#inputs >= 1)
	
	unit.depends['#include "luaav_audio_oscillator.hpp"'] = true
	
	-- derive num channels:
	local channels = max_channels(inputs)
	local freqrate = inputs[1].rate or "i"
	local phaserate = inputs[2].rate or "i"
	local rate = "a"	-- SinOsc is audio rate.
	local outputs = { rate=rate }
	for c=1,channels do
		-- add a sine object to state:
		local objname = uid(node.op:lower())
		local obj = { ty="Buzz", audio.samplerate() }
		unit.state[objname] = obj
		
		-- add frequency instruction:
		local instr = { 
			op="methodcall", 
			objname, 
			"freq",
			input_channel(inputs[1], c) 
		}
		table.insert(unit.blocks[freqrate], instr)
		
		-- add phase instruction:
		statement(unit, phaserate, { 
			op="methodcall", 
			objname, 
			"phase",
			input_channel(inputs[2], c) 
		})
		
		-- add next_sample instruction:
		local instr = { 
			name=uid(objname), 
			op="methodcall", 
			objname,
			"saw"
		}
		table.insert(unit.blocks[rate], instr)
		outputs[c] = instr.name
	end
	return outputs 
end

function parser.Square(unit, node, inputs)
	assert(#inputs >= 1)
	
	unit.depends['#include "luaav_audio_oscillator.hpp"'] = true
	
	-- derive num channels:
	local channels = max_channels(inputs)
	local freqrate = inputs[1].rate or "i"
	local phaserate = inputs[2].rate or "i"
	local rate = "a"	-- SinOsc is audio rate.
	local outputs = { rate=rate }
	for c=1,channels do
		-- add a sine object to state:
		local objname = uid(node.op:lower())
		unit.state[objname] = { ty="Buzz", audio.samplerate() }
		
		-- add frequency instruction:
		local instr = { 
			op="methodcall", 
			objname, 
			"freq",
			input_channel(inputs[1], c) 
		}
		table.insert(unit.blocks[freqrate], instr)
		
		-- add phase instruction:
		statement(unit, phaserate, { 
			op="methodcall", 
			objname, 
			"phase",
			input_channel(inputs[2], c) 
		})
		
		-- add next_sample instruction:
		local instr = { 
			name=uid(objname), 
			op="methodcall", 
			objname,
			"square"
		}
		table.insert(unit.blocks[rate], instr)
		outputs[c] = instr.name
	end
	return outputs 
end

function parser.Biquad(unit, node, inputs)
	assert(#inputs >= 3)
	unit.depends['#include "luaav_audio_filter.hpp"'] = true
	
	local channels = max_channels(inputs)
	local freqrate = inputs[2].rate
	local resrate = inputs[3].rate
	local outputs = { rate="a" }
	for c=1,channels do
		local input, freq, res = inputsforchannel(inputs, c)
		local objname = uid("biquad")
		local filtered = uid("filtered")
		
		unit.state[objname] = { ty="Biquad", audio.samplerate(), "Biquad::"..node.mode or "LP" }
		statement(unit, freqrate, { op="methodcall", objname, "freq", freq })
		statement(unit, resrate, { op="methodcall", objname, "res", res })
		statement(unit, "a", { name=filtered, op="call", objname, input })
		
		outputs[c] = filtered
	end
	return outputs
end

function parser.Reverb(unit, node, inputs)
	assert(#inputs >= 8)
	unit.depends['#include "luaav_audio_reverb.hpp"'] = true

	local objname = uid("reverb")
	unit.state[objname] = { ty="Reverb" }		
	local left = uid("left")
	local right = uid("right")

	-- reverb forces inputs to mono, 
	-- provides stereo outputs.
	local input = inputs[1][1]
	local decay = inputs[2][1]
	local bandwidth = inputs[3][1]
	local damping = inputs[4][1]
	local diffusionIn1 = inputs[5][1]
	local diffusionIn2 = inputs[6][1]
	local diffusionDecay1 = inputs[7][1]
	local diffusionDecay2 = inputs[8][1]

	statement(unit, inputs[2].rate, { op="methodcall", objname, "decay", inputs[2][1] })
	statement(unit, inputs[3].rate, { op="methodcall", objname, "bandwidth", inputs[3][1] })
	statement(unit, inputs[4].rate, { op="methodcall", objname, "damping", inputs[4][1] })
	statement(unit, inputs[5].rate, { op="methodcall", objname, "diffusionIn1", inputs[5][1] })
	statement(unit, inputs[6].rate, { op="methodcall", objname, "diffusionIn2", inputs[6][1] })
	statement(unit, inputs[7].rate, { op="methodcall", objname, "diffusionDecay1", inputs[7][1] })
	statement(unit, inputs[8].rate, { op="methodcall", objname, "diffusionDecay2", inputs[8][1] })

	statement(unit, "a", { name=left, op="constant", 0 })
	statement(unit, "a", { name=right, op="constant", 0 })
	statement(unit, "a", { op="call", objname, input, left, right })
		
	return { left, right, rate="a" }
end

-- computed trig based oscillator (no Gamma):
local function oscradop(op)
	return function(unit, node, inputs)
		assert(#inputs >= 1)
		
		unit.depends['#include <math.h>'] = true
		
		-- derive num channels:
		local channels = max_channels(inputs)
		local freqrate = inputs[1].rate or "i"
		local rate = "a"	-- SinOsc is audio rate.
		local outputs = { rate=rate }
		for c=1,channels do
			
			local freq = input_channel(inputs[1], c)
			local phase = uid("phase")
			local sin = uid("sin")
			
			unit.state[phase] = { ty="number", 0 }
			
			statement(unit, "a", { op="mix", phase, { op="div", freq, audio.samplerate() } } )
			
			statement(unit, "a", { name=sin, op="sin", { op="mul", (math.pi * 2), phase } })
			
			outputs[c] = sin
		end
		return outputs 
	end
end

local function oscop(op)
	return function(unit, node, inputs)
		assert(#inputs >= 1)
		
		unit.depends['#include <math.h>'] = true
		
		-- derive num channels:
		local channels = max_channels(inputs)
		local freqrate = inputs[1].rate or "i"
		local rate = "a"	-- SinOsc is audio rate.
		local outputs = { rate=rate }
		for c=1,channels do
			
			local freq = input_channel(inputs[1], c)
			local phase = uid("phase")
			local sin = uid("sin")
			
			unit.state[phase] = { ty="number", 0 }
			
			statement(unit, "a", { op="mix", phase, { op="div", freq, audio.samplerate() } } )
			
			statement(unit, "a", { name=sin, op="sin", { op="mul", phase } })
			
			outputs[c] = sin
		end
		return outputs 
	end
end

parser.CosOsc = oscradop"cos"
parser.TanOsc = oscradop"tan"

parser.SinhOsc = oscop"sinh"
parser.CoshOsc = oscop"cosh"
parser.TanhOsc = oscop"tanh"

parser.ASinOsc = oscop"sin"
parser.ACosOsc = oscop"cos"
parser.ATanOsc = oscop"tan"

parser.ASinhOsc = oscop"sinh"
parser.ACoshOsc = oscop"cosh"
parser.ATanhOsc = oscop"tanh"

function parser.EnvDriver(unit, node, inputs)
	assert(#inputs >= 2)	-- dur, gate, done
	local channels = 1
	local rate = max_rate(inputs, "c")
	local outputs = { rate="a" }
	local done = node.done
	
	unit.flags['Synth::SYNTH_HAS_ENV'] = true
	
	for c=1,channels do
		local dur, hold = inputsforchannel(inputs, c)
		
		local env_t = uid("env_t")
		local env_dur = uid("env_dur")
		local ramp = uid("ramp")
		
		member(unit, env_t, "number", nil, 0)
		
		-- calculate duration in samples:
		statement(unit, rate, { name=env_dur, op="mul", dur, audio.samplerate() })
		-- increment sample counter (if not gated)
		--statement(unit, "a", { op="mix", env_t, { op="eq", hold, 0 } })
		statement(unit, "a", { op="mix", env_t, { op="or", "synth.envIsReleased()", { op="eq", hold, 0 } } })
		-- stop synth if counter >= dur
		if done then 
			statement(unit, "a", { op="done", env_t, env_dur })
		else
			statement(unit, "a", { op="notdone", env_t, env_dur })
		end
		-- output result as t/dur
		statement(unit, "a", { name=ramp, op="div", env_t, env_dur })
		
		outputs[c] = ramp
	end
	return outputs
end

function parser.Switch(unit, node, inputs)
	local option = inputs[#inputs]
	local rate = max_rate(inputs, "c")
	local channels = max_channels(inputs)
	local outputs = { rate=rate }
	for c=1, channels do
		output = uid(format("switch%d", c))
		statement(unit, rate, { name=output, op="pass", 0 })
		statement(unit, rate, { op="switch", output, inputsforchannel(inputs, c) })
		
		outputs[c] = output
	end
	return outputs
end

local lagcode = [=[
class Lag {
public:
	double history;
	Lag(double init) : history(init) {}
	inline double operator()(const double& in, const double& lag) {
		return history = in + lag*(history-in);
	}
};
]=]

function parser.Lag(unit, node, inputs)
	assert(#inputs >= 2)
	
	unit.depends[lagcode] = true
	
	local channels = max_channels(inputs)
	local outputs={ rate="a" }
	for c = 1,channels do
		local x, lagtime = inputsforchannel(inputs, c)
		local obj = uid("lag")
		local y = uid("lag_out")
		
		member(unit, obj, "Lag", nil, node.init or 0);
		
		--unit.state[y] = { ty="number", node.init or 0 }
		--statement(unit, "a", { name=y, op="add", x, { op="mul", lagtime, { op="sub", y, x } } })
		
		statement(unit, "a", { name=y, op="call", obj, x, lagtime })
		
		outputs[c] = y
	end
	return outputs
end

local function NoiseOp(op)
	--local ty = string.format("gam::%s<>", op)
	return function(unit, node, inputs)
		--unit.depends['#include "Gamma/Noise.h"'] = true
		unit.depends['#include "luaav_audio_noise.hpp"'] = true
		
		-- derive num channels:
		local channels = max_channels(inputs)
		local rate = "a"	-- SinOsc is audio rate.
		local outputs = { rate="a" }
		for c=1,channels do
			local noisegen = uid("noisegen")
			local noise = uid("noise")
			
			-- TODO: add seed as init message?
			
			unit.state[noisegen] = { ty=op, input_channel(inputs[1], c) }
			statement(unit, "a", { name=noise, op="call", noisegen })
			
			outputs[c] = noise
		end
		return outputs 
	end
end

parser.NoiseWhite = NoiseOp"NoiseWhite"
parser.NoisePink = NoiseOp"NoisePink"
parser.NoiseBrown = NoiseOp"NoiseBrown"

--------------------------------------------------------------------------------
-- ugens
--------------------------------------------------------------------------------

function P(arg)
	if type(arg) == "string" then 
		return setmetatable({ op="id", arg }, Expr)
	elseif type(arg) == "number" then
		return setmetatable({ op="constant", arg }, Expr)
	elseif type(arg) == "table" and getmetatable(arg) == Expr then
		return arg
	else
		error("don't know what to do")
	end
end

--- define an input port of the Def:
-- Also generated by P"name"
-- @param args Constructor properties
-- @param args[1] name (string)
-- @param args[2+] default values
function Port(args)
	assert(args[1] and type(args[1]) == "string", "Port must be named")
	assert(#args > 1, "Port must have at least 1 default value")
	return setmetatable({ op="Port", name=args[1], select(2, unpack(args)) }, Expr)
end

--- select input channels from an input stream
-- @param args Constructor properties
-- @param args[1] input stream to select from
-- @param args[2+] channels to select (must be integer, compile-time)
function Channels(args) 
	return setmetatable({ op="Channels", unpack(args) }, Expr)
end

--- mix input stream to mono
-- @param args Constructor properties
-- @param args[1] input stream to mix
function Mono(args) 
	return setmetatable({ op="Mono", args[1] }, Expr)
end

--- mix input stream to stereo
-- @param args Constructor properties
-- @param args[1] input stream to mix
function Stereo(args) 
	return setmetatable({ op="Stereo", args[1] }, Expr)
end

local function unop(name)
	return function(args) 
		assert(type(args) == "table", "Expression requires a table argument")
		return setmetatable({ op=name, args[1] or 0 }, Expr)
	end
end

local function binop(name)
	return function(args) 
		assert(type(args) == "table", "Expression requires a table argument")
		return setmetatable({ op=name, args[1] or 0, args[2] or 0 }, Expr)
	end
end
local function binop1(name)
	return function(args) 
		assert(type(args) == "table", "Expression requires a table argument")
		return setmetatable({ op=name, args[1] or 1, args[2] or 1 }, Expr)
	end
end

---  basic math
-- Also generated by Expr + Expr
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Add(args) end
Add = binop("Add")

---  basic math
-- Also generated by Expr - Expr. If second operand is omitted, is equivalent to (0-args[1]). 
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Sub(args) 
	if args[2] then
		return setmetatable({ op="Sub", args[1] or 0, args[2] }, Expr)
	else
		return setmetatable({ op="Neg", args[1] or 0 }, Expr)
	end
end

---  basic math
-- Also generated by Expr * Expr
-- @param args Constructor properties
-- @param args[1] first operand (default 1)
-- @param args[2] second operand (default 1)
function Mul(args) end
Mul = binop1("Mul")

---  basic math
-- Also generated by Expr / Expr
-- @param args Constructor properties
-- @param args[1] first operand (default 1)
-- @param args[2] second operand (default 1)
function Div(args) end
Div = binop1("Div")

---  basic math
-- Also generated by Expr ^ Expr
-- @param args Constructor properties
-- @param args[1] first operand (default 1)
-- @param args[2] second operand (default 1)
function Pow(args) end
Pow = binop1("Pow")

---  basic math
-- Also generated by Expr % Expr
-- @param args Constructor properties
-- @param args[1] first operand (default 1)
-- @param args[2] second operand (default 1)
function Mod(args) end
Mod = binop1("Mod")

---  absolute value
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Abs(args) end
Abs = unop("Abs")

---  round to next highest integer
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Ceil(args) end
Ceil = unop("Ceil")

---  round to next lowest integer
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Floor(args) end
Floor = unop("Floor")

---  get sign of input
-- Returns 1 if input > 0, 0 if input == 0, and -1 otherwise.
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Sign(args) end
Sign = unop("Sign")

---  compute square root
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Sqrt(args) end
Sqrt = unop("Sqrt")

---  compute e^input
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Exp(args) end
Exp = unop("Exp")

---  compute natural logarithm
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Log(args) end
Log = unop("Log")

---  compute base 10 logarithm
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Log10(args) end
Log10 = unop("Log10")

---  greater than operator
-- boolean operators return 1 for true, 0 for false
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Gt(args) end
Gt = binop("Gt")

---  less than operator
-- boolean operators return 1 for true, 0 for false
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Lt(args) end
Lt = binop("Lt")

---  greater than or equal operator
-- boolean operators return 1 for true, 0 for false
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Gte(args) end
Gte = binop("Gte")

---  less than or equal operator
-- boolean operators return 1 for true, 0 for false
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Lte(args) end
Lte = binop("Lte")

---  equal operator
-- boolean operators return 1 for true, 0 for false
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Eq(args) end
Eq = binop("Eq")

---  not equal operator
-- boolean operators return 1 for true, 0 for false
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Neq(args) end
Neq = binop("Neq")

--- Convert input to 0 or 1
-- returns 1 if the input is 0, and 0 for any other value
-- @param args Constructor properties
-- @param args[1] operand (default 0)
function Not(args) end
Not = unop("Not")

--- Convert input to 0 or 1
-- returns 0 if the input is 0, and 1 for any other value
-- @param args Constructor properties
-- @param args[1] operand (default 0)
function Bool(args) 
	return Neq{ args[1], 0 }
end

---  return lesser of inputs
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Min(args) end
Min = binop("Min")

---  return greater of inputs
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Max(args) end
Max = binop("Max")

---  return mean of inputs
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function Mean(args) end
Mean = function(args)
	return Mul{ 0.5, Add(args) }
end

---  clip signal between range of lo to hi
-- @param args Constructor properties
-- @param args.input or args[1] value to clip (default 0)
-- @param args.lo or args[2] lo limit (default -1)
-- @param args.hi or args[3] hi limit (default 1)
function Clip(args) end
Clip = function(args)
	local input = (args.input or args[1] or 0)
	local lo = (args.lo or args[2] or -1)
	local hi = (args.hi or args[3] or 1)
	return Min{ hi, Max{ lo, input }}
end

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] angle in radians (default 0)
function Sin(args) end
Sin = unop("Sin")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] angle in radians (default 0)
function Cos(args) end
Cos = unop("Cos")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] angle in radians (default 0)
function Tan(args) end
Tan = unop("Tan")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Sinh(args) end
Sinh = unop("Sinh")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Cosh(args) end
Cosh = unop("Cosh")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function Tanh(args) end
Tanh = unop("Tanh")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function ASin(args) end
ASin = unop("ASin")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function ACos(args) end
ACos = unop("ACos")

---  basic trigonometry
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
function ATan(args) end
ATan = unop("ATan")

---  basic trigonometry
-- ATan2 takes two arguments, with the same semantics as the standard cmath atan2 operator.
-- @param args Constructor properties
-- @param args[1] first operand (default 0)
-- @param args[2] second operand (default 0)
function ATan2(args) end
ATan2 = binop("ATan2")


---  smoothing filter (running average lowpass)
-- Smoothes input, initialized at init. 
-- @param args Constructor properties
-- @param args.input or args[1]: input to smooth
-- @param args.lag or args[2]: lag factor (0 for no lag, 1 for infinite lag)
-- @param args.init or args[3]: initial value of filter (default 0)
function Lag(args)
	local input = args.input or args[1] or 0
	local lag = args.lag or args[2] or 0.1
	local init = args.init or args[3] or 0
	return setmetatable({ op="Lag", input, lag, init=init }, Expr)
end

--- Aggregates input into a signal port (mixing overdub) 
-- Adds src to dst, and returns src
-- @param args Constructor properties
-- @param args.dst or args[1]: mix destination (default V"out")
-- @param args.src or args[2]: source to mix into destination (default 0)
function Mix(args)
	local dst = (args.dst or args[1] or "out")
	assert(type(dst)=="string" or dst.op == "id", "Mix dst must be a variable name")
	local src = (args.src or args[2] or 0)
	return setmetatable({ op="Mix", dst, src }, Expr)
end

---  Stereo panner
-- Pan range is -1 to 1.
-- Does a quadratic approximation of equal power pan.
-- For stereo input signals, behaves like a balance control.
-- @param args Constructor properties
-- @param args[1] or args.input: signal to be panned
-- @param args[2] or args.pan: signal to drive panner
function Pan2(args)
	local input = (args.input or args[1] or 0)
	local pan = (args.pan or args[2] or 0)
	return setmetatable({ op="Pan2", input, pan }, Expr)
end



---  envelope driver (used to build envelopes)
-- Returns a signal ramp from 0 to 1 over the duration specified. If done == true (the default case), then the envelope will cause the removal of the containing synth at the end of the ramp.
-- @param args Constructor properties
-- @param args.dur or args[1]: envelope duration; a constant or variable expression (default auto-generated)
-- @param args.hold or args[2]: envelope hold; while gate parameter is nonzero, the envelope will not progress.
-- @param args.done boolean flag for removing the synth at envelope end (default true)
function EnvDriver(args)
	local dur = args.dur or args[1] or 1
	local hold = args.hold or args[2] or 0
	local done = args.done
	if done == nil then done = true end
	return setmetatable({ op="EnvDriver", dur, hold, done=done }, Expr)
end

---  ramp envelope
-- The simplest envelope: ramps from 1 to 0 over the duration.
-- @param args Constructor properties (same as EnvDriver)
-- @see EnvDriver
function Env(args)
	return 1 - EnvDriver(args)
end

--- switch between inputs
-- Switches between one out of sevseral inputs
-- @param args Constructor properties
-- @param args[1]: default, first input
-- @param args[2..(N-1)]: additional inputs
-- @param args[N]: choose input (0..N-1)
function Switch(args)
	assert(#args > 1, "Switch{} requires at least two arguments")
	return setmetatable({ op="Switch", unpack(args) }, Expr)
end

local
function oscfreq(args)
	local freq = args.freq or args[1] or 440
	if(args.op) then
		freq = args
	end
	return freq
end

--- Sine-wave oscillator (polynomial approximation)
-- An audio oscillator. 
-- @param args Constructor properties
-- @param args.freq or args[1]: frequency in Hertz (default 440)
-- @param args.phase or args[2]: phase (default 0)
function SinOsc(args) 
	--local freq = oscfreq(args)
	local freq = args.freq or args[1] or 440 
	local phase = args.phase or args[2] or 0
	return setmetatable({ op="SinOsc", freq, phase }, Expr)
end

--- Accumulating phasor wrapped in range [0, 1)
-- An audio oscillator. 
-- @param args Constructor properties
-- @param args.freq or args[1]: frequency in Hertz (default 440)
-- @param args.phase or args[2]: phase (default 0)
function Phasor(args) 
	--local freq = oscfreq(args)
	local freq = args.freq or args[1] or 440 
	local phase = args.phase or args[2] or 0
	return setmetatable({ op="Phasor", freq, phase }, Expr)
end

--- Impulse anti-aliased oscillator
-- An audio oscillator. 
-- @param args Constructor properties
-- @param args.freq or args[1]: frequency in Hertz (default 440)
-- @param args.phase or args[2]: phase (default 0)
function Buzz(args) 
	--local freq = oscfreq(args)
	local freq = args.freq or args[1] or 440 
	local phase = args.phase or args[2] or 0
	return setmetatable({ op="Buzz", freq, phase }, Expr)
end

--- Saw-wave anti-aliased oscillator
-- An audio oscillator. 
-- @param args Constructor properties
-- @param args.freq or args[1]: frequency in Hertz (default 440)
-- @param args.phase or args[2]: phase (default 0)
function Saw(args) 
	--local freq = oscfreq(args)
	local freq = args.freq or args[1] or 440 
	local phase = args.phase or args[2] or 0
	return setmetatable({ op="Saw", freq, phase }, Expr)
end

--- Square-wave anti-aliased oscillator
-- An audio oscillator. 
-- @param args Constructor properties
-- @param args.freq or args[1]: frequency in Hertz (default 440)
-- @param args.phase or args[2]: phase (default 0)
function Square(args) 
	--local freq = oscfreq(args)
	local freq = args.freq or args[1] or 440 
	local phase = args.phase or args[2] or 0
	return setmetatable({ op="Square", freq, phase }, Expr)
end


noise = {}

--- white noise generator
-- White noise has a uniform power spectrum.
-- @param args Constructor properties
-- @param args.seed or args[1]: seed for random number generator
function noise.White(args) 
	local seed = args.seed or args[1] or 1234567
	return setmetatable({ op="NoiseWhite", seed }, Expr) 
end

---  pink noise generator
-- Pink noise has a power spectrum of 1/f.
-- It is produced by summing together 12 octaves of white noise.
-- @param args Constructor properties
-- @param args.seed or args[1]: seed for random number generator
function noise.Pink(args) 
	local seed = args.seed or args[1] or 1234567
	return setmetatable({ op="NoisePink", seed }, Expr) 
end

---  brown noise generator
-- Brownian noise has a power spectrum of 1/f^2.
-- It is produced by integrating white (uniform) noise.
-- @param args Constructor properties
-- @param args.seed or args[1]: seed for random number generator
function noise.Brown(args) 
	local seed = args.seed or args[1] or 1234567
	return setmetatable({ op="NoiseBrown", seed }, Expr) 
end


biquad = {}

--- 2-pole/2-zero IIR low-pass filter.
-- @param args Constructor properties
-- @param args.input or args[1]: input signal
-- @param args.freq or args[2]: cutoff frequency (default 1000)
-- @param args.res or args[3]: resonance (default 1)
function biquad.LP(args)
	local input = args.input or args[1] or 0
	local freq = args.freq or args[2] or 1000
	local res = args.res or args[3] or 1
	return setmetatable({ op="Biquad", input, freq, res, mode="LP" }, Expr) 
end

--- 2-pole/2-zero IIR high-pass filter.
-- @param args Constructor properties
-- @param args.input or args[1]: input signal
-- @param args.freq or args[2]: cutoff frequency (default 1000)
-- @param args.res or args[3]: resonance (default 1)
function biquad.HP(args)
	local input = args.input or args[1] or 0
	local freq = args.freq or args[2] or 1000
	local res = args.res or args[3] or 1
	return setmetatable({ op="Biquad", input, freq, res, mode="HP" }, Expr) 
end

--- 2-pole/2-zero IIR band-pass filter.
-- @param args Constructor properties
-- @param args.input or args[1]: input signal
-- @param args.freq or args[2]: cutoff frequency (default 1000)
-- @param args.res or args[3]: resonance (default 1)
function biquad.BP(args)
	local input = args.input or args[1] or 0
	local freq = args.freq or args[2] or 1000
	local res = args.res or args[3] or 1
	return setmetatable({ op="Biquad", input, freq, res, mode="BP" }, Expr) 
end

--- 2-pole/2-zero IIR band-pass filter.
-- @param args Constructor properties
-- @param args.input or args[1]: input signal
-- @param args.freq or args[2]: cutoff frequency (default 1000)
-- @param args.res or args[3]: resonance (default 1)
function biquad.BPC(args)
	local input = args.input or args[1] or 0
	local freq = args.freq or args[2] or 1000
	local res = args.res or args[3] or 1
	return setmetatable({ op="Biquad", input, freq, res, mode="BPC" }, Expr) 
end

--- 2-pole/2-zero IIR band-reject filter.
-- @param args Constructor properties
-- @param args.input or args[1]: input signal
-- @param args.freq or args[2]: cutoff frequency (default 1000)
-- @param args.res or args[3]: resonance (default 1)
function biquad.BR(args)
	local input = args.input or args[1] or 0
	local freq = args.freq or args[2] or 1000
	local res = args.res or args[3] or 1
	return setmetatable({ op="Biquad", input, freq, res, mode="BR" }, Expr) 
end

--- 2-pole/2-zero IIR all-pass filter.
-- @param args Constructor properties
-- @param args.input or args[1]: input signal
-- @param args.freq or args[2]: cutoff frequency (default 1000)
-- @param args.res or args[3]: resonance (default 1)
function biquad.AP(args)
	local input = args.input or args[1] or 0
	local freq = args.freq or args[2] or 1000
	local res = args.res or args[3] or 1
	return setmetatable({ op="Biquad", input, freq, res, mode="AP" }, Expr) 
end

--- Plate reverberation simulator
-- Based on: Dattorro, J. (1997). Effect design: Part 1: Reverberator and other filters. Journal of the Audio Engineering Society, 45(9):660–684
-- @param args Constructor properties
-- @param args.input or args[1]: input
-- @param args.decay or args[2]: decay 0-1 (default 0.85)
-- @param args.bandwidth or args[3]: bandwidth 0-1 (default 0.9995)
-- @param args.damping or args[4]: damping 0-1 (default 0.4)
-- @param args.diffusionIn1 or args[5]: diffusion input 1 0-1 (default 0.76)
-- @param args.diffusionIn2 or args[6]: diffusion input 2 0-1 (default 0.666)
-- @param args.diffusionDecay1 or args[7]: diffusion decay 1 0-1 (default 0.707)
-- @param args.diffusionDecay2 or args[8]: diffusion decay 2 0-1 (default 0.571)
function Reverb(args) 
	local input = args.input or args[1] or 0
	local decay = args.decay or args[2] or 0.85
	local bandwidth = args.bandwidth or args[3] or 0.9995
	local damping = args.damping or args[4] or 0.4
	local diffusionIn1 = args.diffusionIn1 or args[5] or 0.76
	local diffusionIn2 = args.diffusionIn2 or args[6] or 0.666
	local diffusionDecay1 = args.diffusionDecay1 or args[7] or 0.707
	local diffusionDecay2 = args.diffusionDecay2 or args[8] or 0.571
	return setmetatable({ op="Reverb", input, decay, bandwidth, damping, diffusionIn1, diffusionIn2, diffusionDecay1, diffusionDecay2 }, Expr)
end

--------------------------------------------------------------------------------
-- public API
--------------------------------------------------------------------------------

function Def(def)
	local unit = {
		depends = {},
		state = {},
		blocks = {
			a = {},
			c = {},
			i = {},
			k = {},
		},
		flags = {},
	}
	
	-- copy in named fields
	for k, v in pairs(def) do
		if type(k) == "string" then
			-- add member variable to unit:
			if type(v) == "number" then
				member(unit, k, "Port", k, v)
			elseif type(v) == "table" then
				if not v.ty then
					member(unit, k, "Port", k, unpack(v))
				else
					member(unit, k, v.ty, k, unpack(v))
				end
			else
				error("unrecognized type for member")
			end
		end
	end
	
	-- handle numeric exprs
	-- outputs will store the return value of the last expression in the sequence
	local outputs
	for i, expr in ipairs(def) do
		outputs = parse(unit, expr)
	end
	
	-- if there is a return value, write to output:
	if outputs and #outputs > 0 then
		local dst = parser.id(unit, {"out"}, nil)
		--print(unpack(dst))
		parser.Mix(unit, nil, { dst, outputs } )
	end
	
	-- build default prototype:
	local proto = {}
	for k, v in pairs(unit.state) do
		if v.setter then
			if #v == 1 then
				proto[k] = v[1]
			else
				proto[k] = v
			end
		end
	end
	
	--printt(unit)
	--print"Proto:"	printt(proto)
	local code = generate(unit)
	print(code)
	local ctor = compile(code, proto)
	return ctor, code, unit
end

--- Push all audio.def functions into the globals scope table
-- A utility for quick-coding, to create global references for all audio.def functions
function globalize()
	local _G = getfenv(2)
	for k, v in pairs(_M) do 
		if type(k) == "string" and k:sub(1, 1) ~= "_" then
			_G[k] = v
		end
	end
end

-- make the module callable:
setmetatable(_M, { __call = function(M, ...)
	return Def(...)
end })

