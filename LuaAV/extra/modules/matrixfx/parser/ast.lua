local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local ipairs = ipairs
local pairs = pairs
local print = print
local getfenv = getfenv
local unpack = unpack
local tonumber = tonumber

local math = require 'math'
local table = require 'table'
local string = require 'string'


module("matrixfx.parser.ast")
-----------------------------------------------------
local M = getfenv()

M.type = "float"
M.planes = 3

local built_in_types = {}
local mat_coords = {}
for i=0, 7 do
	built_in_types[ "in"..i ] = "cell"
	built_in_types[ "out"..i ] = "cell"
	built_in_types[ "coord"..i ] = "cell2"
	mat_coords[ "coord"..i ] = true
end

local coord_idx = {
	i = 0,
	j = 1,
	k = 2,

	[0] = "i",
	[1] = "j",
	[2] = "k",
}

local field_idx = {
	x = 0,
	y = 1,
	z = 2,
	w = 3,
	
	[0] = "x",
	[1] = "y",
	[2] = "z",
	[3] = "w",
}


local vec_types = {
	cell = true,
	cell2 = true,
	cell3 = true,
}

local cast_op = {
	float = true,
	int = true,
}

local
function get_type(ty)
	if(ty == "cell" or ty == "cell2" or ty == "cell3") then
		return M.type
	else
		return ty
	end
end

local
function get_planes(ty)
	if(ty == "cell") then
		return M.planes
	elseif(ty == "cell2") then
		return 2
	elseif(ty == "cell3") then
		return 3
	end
end

local format = string.format
local op_fmt = {
	block = function(t) 
			return format(string.rep("%s;\n", #t-1), unpack(t)) .. 
					tostring(t[#t]) .. ";"
		end,
	decl = function(t) 
		if(vec_types[ t[1] ]) then
			return format("%s %s[%d]", get_type(t[1]), t[2], get_planes(t[1]))
		else
			return format("%s %s", get_type(t[1]), t[2])
		end
	end,
	const = function(t) return tostring(t[1]) end,
	assign = function(t) return format("%s = %s", t[1], t[2]) end,
	mul = function(t) return format("%s*%s", t[1], t[2]) end,
	div = function(t) return format("%s/%s", t[1], t[2]) end,
	add = function(t) return format("%s+%s", t[1], t[2]) end,
	sub = function(t) return format("%s-%s", t[1], t[2]) end,
	paren = function(t) return format("(%s)", t[1]) end,
	array = function(t) return format("%s[%s]", t[1], tostring(t[2])) end,
	preincr = function(t) return format("++%s", t[1]) end,
	predecr = function(t) return format("--%s", t[1]) end,
	negate = function(t) return format("-%s", t[1]) end,
	logicalnot = function(t) return format("!%s", t[1]) end,
	postincr = function(t) return format("%s++", t[1]) end,
	postdecr = function(t) return format("%s--", t[1]) end,
	
	field = function(t) 
		local idx = field_idx[ t[2] ]
		if(mat_coords[ t[1] ]) then
			return coord_idx[idx]
		else
			return format("%s[%d]", t[1], idx)
		end
	end,
	
	call = function(name, t)
			if(name == "sample") then
				local mat_name = t[1]
				local coord_name = t[2]
				local mat_idx = mat_name:sub(mat_name:len())
				local coord_idx = coord_name:sub(coord_name:len())
				--print(t[2], mat_coords[coord_name], mat_idx, coord_idx)
				if(mat_coords[coord_name]) then
					return tostring(t[1])
				else
					return format("(b%s + (( ((int)%s[%d]) %% width) + ( ((int)%s[%d]) %% height)*width)*planes)", 
								mat_name,
								coord_name, 0,
								coord_name, 1)
				end
			elseif(name == "random") then
				return "(((float)(rand()))/((float)RAND_MAX))"
			elseif(cast_op[name]) then
				return format("(%s)(%s)", name, t[1])
			else
				-- general function
				local args = string.rep("%s, ", #t-1)
				if(#t > 0) then
					args = args .. "%s"
				end
				local vals = {}
				for i, v in ipairs(t) do
					vals[i] = tostring(v)
				end
				return format("%s(" .. args .. ")", name, unpack(vals))
			end
		end,
}


function M.print(t, name, lvl)
	name = name or "ast"
	lvl = lvl or 0
	print(string.rep("\t", lvl) .. (name and (name .. " = {") or "{"))
	for k, v in pairs(t) do 
		if(type(v) == "table") then
			M.print(v, k, lvl+1)
		else
			print(string.rep("\t", lvl+1) .. tostring(k) .. " = " .. tostring(v))
		end
	end
	print(string.rep("\t", lvl) ..  "}")
end


function move_declarations(ast)
	local types = {}
	
	local pre_declarations = {}
	for i, statement in ipairs(ast) do
		if(statement.op == "assign") then
			local left_hand = statement[1]
			if(left_hand.op == "decl") then
				pre_declarations[#pre_declarations+1] = left_hand
				
				local new_left_hand = {}
				for k, v in pairs(left_hand[2]) do
					new_left_hand[k] = v
				end
				
				statement[1] = new_left_hand
			end
		elseif(statement.op == "decl") then
			types[ statement[2][1] ] = statement[1][1]
		end
	end
	
	
	for i, decl in ipairs(pre_declarations) do
		table.insert(ast, i, decl)
		types[ decl[2][1] ] = decl[1][1]
	end
	for k, v in pairs(built_in_types) do
		types[k] = v
	end
	
	return ast, types
end

function unroll_loops(ast, n, types)
	function do_unroll(s, idx)
		local new_s = {}
		for i, expr in ipairs(s) do
			-- unroll
			if(expr.op == "const") then
				if(vec_types[ types[ expr[1] ] ]) then
					if(mat_coords[ expr[1] ]) then
						new_s[#new_s+1] = {
							[1] = coord_idx[idx],
							op = "const",
						}
					else
						new_s[#new_s+1] = {
							[1] = expr,
							[2] = field_idx[idx],
							op = "field",
						}
					end
				else
					new_s[#new_s+1] = expr
				end
			-- don't change, already "unrolled"
			elseif(expr.op == "field") then
				new_s[#new_s+1] = {
					expr[1],
					expr[2],
					op = expr.op,
				}
			-- don't change, already "unrolled"
			elseif(expr.op == "decl") then
				new_s[#new_s+1] = {
					expr[1],
					{
						[1] = expr[2],
						[2] = field_idx[idx],
						op = "field",
					},
					op = expr.op,
				}
--			elseif(expr.op == "assign" and expr.op)
			-- unroll return value of function
			elseif(expr.op == "call") then
				new_s[#new_s+1] = {
					expr,
					[2] = field_idx[idx],
					op = "field",
				}
			-- recursively unroll
			else
				local new_expr = {op = expr.op}
				local args = do_unroll(expr, idx)
				for i, a in ipairs(args) do
					new_expr[i] = a
				end
				new_s[#new_s+1] = new_expr
			end
		end
		
		new_s.op = s.op
		return new_s
	end

	--M.print(ast)

	-- unroll block statements
	local new_ast = {op = "block"}
	for i, statement in ipairs(ast) do
		-- if we have an assigment to a vector, unroll
		local lhand_name = statement[1][1]
		local lhand_type = types[lhand_name]
		if(statement.op == "assign" and statement[1].op ~= "field" and 
			vec_types[lhand_type])
		then
			for i=1, get_planes(lhand_type) do
				new_ast[#new_ast+1] = do_unroll(statement, i-1)
			end
		else
			new_ast[#new_ast+1] = statement
		end
	end
	return new_ast
end

M.temps = 0

function call_types(name)
	if(name == "sample") then
		return "cell2"
	else
		return "float"
	end
end

function lift_arguments(AST)
	local new_assignments = {}

	function do_lift_arguments(ast)
		for i, expr in ipairs(ast) do
			if(expr.op == "call" and expr.name ~= "float") then
				for i, arg in ipairs(expr) do
					if(arg.op ~= "const" and arg.op ~= "field") then
						local temp_name = "temp" .. M.temps
						M.temps = M.temps+1

						new_assignments[#new_assignments+1] = {
							{
								{
									call_types(expr.name),
									op = "const",
								},
								{
									temp_name,
									op = "const",
								},
								op = "decl",
							},
							arg,
							op = "assign",
						}
						
						expr[i] = {
							temp_name,
							op = "const",
						}
						--]]
					end
				end
			elseif(type(expr) == "table" and expr.op ~= "const" and expr.op ~= "decl") then
				do_lift_arguments(expr)
			end
		end
	end
	
	do_lift_arguments(AST)
	for i, v in ipairs(new_assignments) do
		table.insert(AST, i, v)
	end
	
	return AST
end

local
function is_constructor(name)
	return name == "cell2" or name == "cell3"
end

function collapse_constructors(ast)
	for i, expr in ipairs(ast) do
		if(expr.op == "field" and is_constructor(expr[1].name)) then
			local idx = field_idx[ expr[2] ]
			expr[1] = expr[1][idx+1][1]
			expr[2] = nil
			expr.op = "const"
		elseif(type(expr) == "table" and expr.op ~= "const" and expr.op ~= "decl") then
			collapse_constructors(expr)
		end
	end
	return ast
end


function inout(ast)
	local nin = 0
	local nout = 1
	for i, s in ipairs(ast) do
		if(s.op == "assign") then
			local ss = s[1]
			if(ss.op == "field") then
				ss = ss[1]
			end
			
			if(ss.op == "const") then
				local lhand = ss
				local idx = lhand[1]:match("out(%d)")
				if(idx) then
					idx = tonumber(idx)
					if(nout <= idx) then
						nout = idx+1
					end
				end
			end
		end
	end
	
	function do_in(ast)
		for i, s in ipairs(ast) do
			local ss = s
			if(ss.op == "field") then
				ss = ss[1]
			end

			if(ss.op == "const") then
				
				local idx = ss[1]:match("in(%d)")
				if(idx) then
					idx = tonumber(idx)
					if(nin <= idx) then
						nin = idx+1
					end
				end
			else
				do_in(s)
			end
		end
	end
	do_in(ast)
	
	return nin, nout
end


function to_c(ast)
	local code = ""
	local args = {}
	for i, v in ipairs(ast) do
		if(type(v) == "table") then
			args[#args+1] = to_c(v)
		else
			args[#args+1] = tostring(v)
		end
	end

	local fmt = op_fmt[ast.op]
	if(fmt) then
		-- special case with extra field
		if(ast.op == "call") then
			code = code .. fmt(ast.name, args)
		else
			code = code .. fmt(args)
		end	
	end
	return code
end