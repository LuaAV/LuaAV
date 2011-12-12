LuaAV.addmodulepath(script.path.."/../modules")
------------------------------------------------
local lpeg = require("lpeg")
local format = string.format
local floor = math.floor
local min = math.min

--patterns
local P, R, S, V = lpeg.P, lpeg.R, lpeg.S, lpeg.V
--captures
local C, Ct, Cc, Cmt, Cg = lpeg.C, lpeg.Ct, lpeg.Cc, lpeg.Cmt, lpeg.Cg

local white = S(" \t\r\n")
local whites = white^0
local letter = R("az", "AZ")


local
function search(patt)
	return (patt + P(1))^0
end

local
function ctype(ty)
	return ty.."_t"
end

local file = script.path.."/e2ga_cpp.xml"
local header_file = script.path.."/../../src/e2/e2ga.h"
local data = io.open(file):read("*a")
local header_data = io.open(header_file):read("*a")

function printt(t, name, lvl, memo)
	wait(0.01)

	memo = memo or {}
	local function qv(v) return 
		(type(v)=="number" or type(v)=="boolean") and tostring(v) or
		(v and string.format("%q", tostring(v)) or "") 
	end
	lvl = lvl or 0
	print(string.format("%s[%s] = { -- %s", string.rep("    ", lvl), qv(name), qv(t)))
	if not memo[t] then
		memo[t] = true
		for k, v in pairs(t) do 
			if not (type(k) == "number" and k > 0 and k <= #t) then
				if(type(v) == "table") then
					printt(v, k, lvl+1, memo)
				else
					print(string.format("%s[%s] = %s", string.rep("    ", lvl+1), qv(k), qv(v)))
				end
			end
		end
		for k, v in pairs(t) do 
			if (type(k) == "number" and k > 0 and k <= #t) then
				if(type(v) == "table") then
					printt(v, k, lvl+1, memo)
				else
					print(string.format("%s[%s] = %s", string.rep("    ", lvl+1), qv(k), qv(v)))
				end
			end
		end
	else
		print(string.format("%s<memoed %q>", string.rep("    ", lvl+1), qv(t)))
	end
	print(string.rep("    ", lvl).."}")
end

local xml = require("xml")

local res = xml.match(data)
--printt(res)

local space_name = "e2"
local float_type
local types = {}
local binops = {}

local typedefs = {
}

local
function find_types(t)
	for i, v in ipairs(t) do
		if(v[1] == "smv" or v[1] == "mv") then
			local name = v.attributes.name
			types[#types+1] = name
			types[name] = true
		elseif(v[1] == "floatType") then
			float_type = v.attributes.type
		end
	end
	
	for i=1, #types do
		local ty = types[i]
		local patt = P"class" * whites * C(P(ctype(ty)) + P(ty)) * whites * P";"
		patt = search(patt)

		local cty = patt:match(header_data)
		typedefs[ty] = cty or ty
	end
end

local
function find_functions(t)
	for i, v in ipairs(t) do
		if(v[1] == "function") then
			local name = v.attributes.name
			local arg1 = v.attributes.arg1
			local arg2 = v.attributes.arg2
			
			if(name and arg1 and arg2) then
				if(not binops[name]) then
					binops[name] = {}
				end
				
				if(not binops[name][arg1]) then
					binops[name][arg1] = {}
				end
				
				binops[name][arg1][arg2] = true
			end
		end
	end
end

find_types(res[1])
types[#types+1] = float_type
types[float_type] = true

local type_patt = P(ctype(types[1])) + P(types[1])
for i=2, #types do
	type_patt = type_patt + P(ctype(types[i])) + P(types[i])
end

find_functions(res[1])
--[[
local dgp = binops.gp[float_type]
if(not dgp) then
	binops.gp[float_type] = {}
	dgp = binops.gp[float_type]
end
for i=1, #types do
	local ty = types[i]
	dgp[ty] = ty
end
--]]

local
function find_function_return_type(name, arg1, arg2)
	local apatt1
	if(arg1 == float_type) then
		apatt1 = letter
	else
		apatt1 = P"&" * letter
	end
	
	local apatt2
	if(arg2 == float_type) then
		apatt2 = letter
	else
		apatt2 = P"&" * letter
	end
	
	local patt = C(type_patt) * whites * 
			P(name.."(") * whites * 
			P"const" * whites * (P(ctype(arg1)) + P(arg1)) * whites * apatt1 * P"," * whites *
			P"const" * whites * (P(ctype(arg2)) + P(arg2)) * whites * apatt2 * P");"
			
	patt = search(patt)
	return patt:match(header_data)
end

local
function tag_from_type(ty)
	ty = ty:match("(.+)_t$") or ty
	
	local fc = ty:sub(1, 1)
	if(fc == fc:upper()) then
		return ty
	else
		return ty:upper()
	end
end

local
function arg_from_type(ty, aidx)
	if(ty == float_type) then
		return format("*((%s *)elem%d)", ty, aidx)
	else
		return format("*((%s *)elem%d->elem)", typedefs[ty], aidx)
	end
end

local
function mv_from_type(ty, aidx)
	if(ty == float_type) then
		return format("*((%s *)elem%d)", ty, aidx)
	else
		return format("mv(*((%s *)elem%d->elem))", typedefs[ty], aidx)
	end
end

local
function mv_type_from_type(ty)
	if(ty == float_type) then
		return ty
	else
		return "mv"
	end
end

local
function opcode(name, arg1, arg2)
	local op = [[
	Glue<E2Elem>::push(
		L, 
		new E2Elem(
			%s_TAG,
			new %s(
				%s(%s, %s)
			)
		)
	);]]
		
	if(
		binops[name] and 
		binops[name][arg1] and 
		binops[name][arg1][arg2]
	) then
		local rty = find_function_return_type(name, arg1, arg2)
		if(rty == float_type) then
			local op = [[
				lua::push<%s>(
					L, 
					%s(%s, %s)
				);
			]]
			
			local code = format(op,
				rty,
				name, 
				arg_from_type(arg1, 1), 
				arg_from_type(arg2, 2)
			)
			return code
		else
			local code = format(op, 
				tag_from_type(rty),
				typedefs[rty] or rty,
				name, 
				arg_from_type(arg1, 1), 
				arg_from_type(arg2, 2)
			)
			return code
		end
	elseif(name == "gp" and arg1 == float_type) then
		local rty
		if(arg2 == "vector" or arg2 == "rotor") then
			rty = arg2
		else
			rty = "mv"
		end
		local code = format(op, 
			tag_from_type(rty),
			typedefs[rty] or rty,
			name, 
			arg_from_type(arg1, 1), 
			arg_from_type(arg2, 2)
		)
		return code
	else
		local rty = find_function_return_type(name, mv_type_from_type(arg1), mv_type_from_type(arg2))
		if(type(rty) ~= "string") then
			rty = "mv"
		end
		local code = format(op, 
			tag_from_type(rty),
			rty,
			name, 
			mv_from_type(arg1, 1), 
			mv_from_type(arg2, 2)
		)
		return code
	end
end

local
function tag2(name, arg1)
	local code = "\nswitch(tag2) {"
	for i=1, #types do
		local arg2 = types[i]
		code = format("%s\ncase %s_TAG:", code, tag_from_type(arg2))
		code = format("%s\n%s", code, opcode(name, arg1, arg2))
		code = format("%s\nbreak;\n", code)
	end
	return code.."}"
end

local
function tag1(name)
	local code = "switch(tag1) {"
	for i=1, #types do
		local arg1 = types[i]
		code = format("%s\ncase %s_TAG:", code, tag_from_type(arg1))
		code = code..tag2(name, arg1)
		code = format("%s\nbreak;\n", code)
	end
	return code.."}"
end

local
function op(name)
	local code = format([[
int %sspace_%s(lua_State *L) {
	double d1, d2;
	E2Tag tag1;
	E2Tag tag2;
	E2Elem *elem1 = Glue<E2Elem>::to(L, 1);
	E2Elem *elem2 = Glue<E2Elem>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (E2Elem *)(&d1);
			tag1 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "%s.%s: invalid arguments");
		}
	}
	else {
		tag1 = elem1->tag;
	}
	
	if(!elem2) {
		if(lua::is<double>(L, 2)) {
			d2 = lua::to<double>(L, 2);
			elem2 = (E2Elem *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "%s.%s: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	]], space_name, name, space_name, name, space_name, name)
	return format("%s\n%s\nreturn 1;\n}", code, tag1(name))
end

local ops = {
	"add",
	---[[
	"subtract",
	"hip",
	"mhip",
	"op",
	"gp",
	"lc",
	"rc",
	--]]
}

local cpp_template = [[
// THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT
#include "lua_e2ga.h"

#ifdef __cplusplus
extern "C" {
#endif

%s


#ifdef __cplusplus
}
#endif
]]
local code = ""
for _, v in ipairs(ops) do
	code = format("%s\n\n\n%s", code, op(v))
end

local cpp_code = format(cpp_template, code)

local path = script.path.."/../../src/e2/"
local filename = "lua_e2ga_ops_gen.cpp"

local f = io.open(path..filename, "w")
local code_len = cpp_code:len()
local dt = 1000
for i=1, code_len, dt do
	local chunk = cpp_code:sub(i, min(i+dt, code_len)-1)
	f:write(chunk)
	f:flush()
end