local setmetatable = setmetatable
local getfenv = getfenv
local assert = assert
local ipairs = ipairs
local pairs = pairs
local type = type
local print = print
local wait = wait

local io = io
local format = string.format
local floor = math.floor
local min = math.min

local save_path = script.path

local xml = require("xml")
local lpeg = require("lpeg")
local P, R, S, V = lpeg.P, lpeg.R, lpeg.S, lpeg.V
local C, Ct, Cc, Cmt, Cg = lpeg.C, lpeg.Ct, lpeg.Cc, lpeg.Cmt, lpeg.Cg

local app_utils = require("app.utilities")

module(...)

local Class = {}
local M = setmetatable(getfenv(), Class)
M.__index = M

local white = S(" \t\r\n")
local whites = white^0
local letter = R("az", "AZ")
local char = R("az", "__", "AZ", "09")

local
function search(patt)
	return (patt + P(1))^0
end

local
function ctype(ty)
	return ty.."_t"
end

local
function ID(p)
	return p*-char
end


function Class:__call(init)
	assert(init.name)
	assert(init.xml_file)
	assert(init.header_file)
	local m = setmetatable(init, M)
	
	m.elem_name = m.name:upper().."Elem"
	m.elem_tag = m.name:upper().."Tag"
	m.types = {}
	m.typedefs = {}
	m.binops = {}
	m:setup()
	
	return m
end

function M:setup()
	local types = self.types

	-- load in the XML and header file
	self.xml_data = assert(io.open(self.xml_file)):read("*a")
	self.header_data = assert(io.open(self.header_file)):read("*a")
	
	-- parse out the XML
	self.xml_structure = xml.match(self.xml_data)
	
	-- extract space information
	self:find_types(self.xml_structure[1])
	self:find_functions(self.xml_structure[1])
	
	-- generate some LPEG patterns
	local type_patt = ID(P(ctype(types[1]))) + ID(P(types[1]))
	for i=2, #types do
		type_patt = type_patt + ID(P(ctype(types[i]))) + ID(P(types[i]))
	end
	self.type_patt = type_patt
end


function M:find_types(t)
	local types = self.types
	local typedefs = self.typedefs

	for i, v in ipairs(t) do
		if(v[1] == "smv" or v[1] == "mv") then
			local name = v.attributes.name
			types[#types+1] = name
			types[name] = true
		elseif(v[1] == "floatType") then
			-- set the floating point type
			self.float_type = v.attributes.type
		end
	end
	
	for i=1, #types do
		local ty = types[i]
		local patt = P"class" * whites * C(P(ctype(ty)) + P(ty)) * whites * P";"
		patt = search(patt)

		local cty = patt:match(self.header_data)
		typedefs[ty] = cty or ty
	end
	
	types[#types+1] = self.float_type
	types[self.float_type] = true
end

function M:find_functions(t)
	local binops = self.binops

	for i, v in ipairs(t) do
		if(v[1] == "function") then
			local name = v.attributes.outputName or v.attributes.name
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


function M:tag_from_type(ty)
	ty = ty:match("(.+)_t$") or ty
	
	local fc = ty:sub(1, 1)
	if(fc == fc:upper()) then
		return ty
	else
		return ty:upper()
	end
end

function M:arg_from_type(ty, aidx)
	if(ty == self.float_type) then
		return format("*((%s *)elem%d)", ty, aidx)
	else
		return format("*((%s *)elem%d->elem)", self.typedefs[ty], aidx)
	end
end

function M:mv_from_type(ty, aidx)
	if(ty == self.float_type) then
		return format("*((%s *)elem%d)", ty, aidx)
	else
		return format("mv(*((%s *)elem%d->elem))", self.typedefs[ty], aidx)
	end
end

function M:mv_type_from_type(ty)
	if(ty == self.float_type) then
		return ty
	else
		return "mv"
	end
end

function M:find_function_return_type(name, arg1, arg2)
	local apatt1
	if(arg1 == self.float_type) then
		apatt1 = letter
	else
		apatt1 = P"&" * letter
	end
	
	local apatt2
	if(arg2 == self.float_type) then
		apatt2 = letter
	else
		apatt2 = P"&" * letter
	end
	
	local patt = C(self.type_patt) * whites * 
			P(name.."(") * whites * 
			P"const" * whites * (P(ctype(arg1)) + P(arg1)) * whites * apatt1 * P"," * whites *
			P"const" * whites * (P(ctype(arg2)) + P(arg2)) * whites * apatt2 * P");"
			
	patt = search(patt)
	return patt:match(self.header_data)
end


function M:opcode(name, arg1, arg2)
	local types = self.types
	local typedefs = self.typedefs
	local binops = self.binops
	
	local op = [[
	Glue<_ELEM_>::push(
		L, 
		new _ELEM_(
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
		local rty = self:find_function_return_type(name, arg1, arg2)
		if(type(rty) == "number") then
			print(name, arg1, arg2)
		end
		if(rty == self.float_type) then
			local op = [[
				lua::push<%s>(
					L, 
					%s(%s, %s)
				);
			]]
			
			local code = format(op,
				rty,
				name, 
				self:arg_from_type(arg1, 1), 
				self:arg_from_type(arg2, 2)
			)
			return code
		else
			local code = format(op, 
				self:tag_from_type(rty),
				typedefs[rty] or rty,
				name, 
				self:arg_from_type(arg1, 1), 
				self:arg_from_type(arg2, 2)
			)
			return code
		end
	elseif(name == "gp" and arg1 == self.float_type) then
		local rty
		if(arg2 == "vector" or arg2 == "rotor") then
			rty = arg2
		else
			rty = "mv"
		end
		local code = format(op, 
			self:tag_from_type(rty),
			typedefs[rty] or rty,
			name, 
			self:arg_from_type(arg1, 1), 
			self:arg_from_type(arg2, 2)
		)
		return code
	else
		local rty = self:find_function_return_type(
			name, 
			self:mv_type_from_type(arg1), 
			self:mv_type_from_type(arg2)
		)
		if(type(rty) ~= "string") then
			rty = "mv"
		end
		local code = format(op, 
			self:tag_from_type(rty),
			rty,
			name, 
			self:mv_from_type(arg1, 1), 
			self:mv_from_type(arg2, 2)
		)
		return code
	end
end

function M:tag2(name, arg1)
	local types = self.types

	local code = "\nswitch(tag2) {"
	for i=1, #types do
		local arg2 = types[i]
		code = format("%s\ncase %s_TAG:", code, self:tag_from_type(arg2))
		code = format("%s\n%s", code, self:opcode(name, arg1, arg2))
		code = format("%s\nbreak;\n", code)
	end
	return code.."}"
end

function M:tag1(name)
	local types = self.types
	
	local code = "switch(tag1) {"
	for i=1, #types do
		local arg1 = types[i]
		print("processing argument: "..arg1)
		wait(0.02)
		code = format("%s\ncase %s_TAG:", code, self:tag_from_type(arg1))
		code = code..self:tag2(name, arg1)
		code = format("%s\nbreak;\n", code)
	end
	return code.."}"
end

function M:op(name)
	local code = format([[
int %sspace_%s(lua_State *L) {
	double d1, d2;
	_TAG_ tag1;
	_TAG_ tag2;
	_ELEM_ *elem1 = Glue<_ELEM_>::to(L, 1);
	_ELEM_ *elem2 = Glue<_ELEM_>::to(L, 2);
	
	
	if(!elem1) {
		if(lua::is<double>(L, 1)) {
			d1 = lua::to<double>(L, 1);
			elem1 = (_ELEM_ *)(&d1);
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
			elem2 = (_ELEM_ *)(&d2);
			tag2 = DOUBLE_TAG;
		}
		else {
			luaL_error(L, "%s.%s: invalid arguments");
		}
	}
	else {
		tag2 = elem2->tag;
	}
	]], self.name, name, self.name, name, self.name, name)
	return format("%s\n%s\nreturn 1;\n}", code, self:tag1(name))
end

function M:generate(ops)
	local lua_space_include = "lua_"..app_utils.strippath(self.header_file)
	local cpp_template = format([[
// THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT
#include "%s"

#ifdef __cplusplus
extern "C" {
#endif

%s


#ifdef __cplusplus
}
#endif
]], lua_space_include, "%s")

	local code = ""
	for _, v in ipairs(ops) do
		print("Generating op: "..v)
		wait(0.1)
		local newcode = self:op(v)
		self:write_file(format("%s/%s_%s.cpp", save_path, self.name, v), newcode)
		code = format("%s\n\n\n%s", code, newcode)
	end
	
	local code = format(cpp_template, code)
	code = code:gsub("_ELEM_", self.elem_name)
	code = code:gsub("_TAG_", self.elem_tag)
	
	return code
end

function M:generate_from_op_files(ops)
	local lua_space_include = "lua_"..app_utils.strippath(self.header_file)
	local cpp_template = format([[
// THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT
#include "%s"

#ifdef __cplusplus
extern "C" {
#endif

%s


#ifdef __cplusplus
}
#endif
]], lua_space_include, "%s")

	local code = ""
	for _, v in ipairs(ops) do
		print("Generating op from file: "..v)
		wait(0.1)
		local filename = format("%s/%s_%s.cpp", save_path, self.name, v)
		local newcode = io.open(filename):read("*a").."\n}\n"
		code = format("%s\n\n\n%s", code, newcode)
	end
	
	local code = format(cpp_template, code)
	code = code:gsub("_ELEM_", self.elem_name)
	code = code:gsub("_TAG_", self.elem_tag)
	
	return code
end

function M:write(filename, ops)
	local code = self:generate(ops)

	local f = assert(io.open(filename, "w"))
	local code_len = code:len()
	local dt = 1000
	for i=1, code_len, dt do
		local chunk = code:sub(i, min(i+dt, code_len)-1)
		f:write(chunk)
		f:flush()
	end
end

function M:write_file(filename, code)
	local f = assert(io.open(filename, "w"))
	local code_len = code:len()
	local dt = 1000
	for i=1, code_len, dt do
		local chunk = code:sub(i, min(i+dt, code_len)-1)
		f:write(chunk)
		f:flush()
	end
end