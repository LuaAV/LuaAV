local string = require("string")
local format = string.format
----------------------------------------
local cosmo = require("cosmo")
----------------------------------------
module(..., package.seeall)

local _print = print

-- OP patterns
local op_fmt = {
	assign = "%s = %s",
	decl = "%s %s",
	deref = "%s.%s",
	index = "%s[%s]",
	add = "%s + %s",
	sub = "%s - %s",
	mul = "%s * %s",
	div = "%s / %s",
	lt = "%s < %s",
	gt = "%s > %s",
	paren = "(%s)",
	block = [[{
		%s	
	}]]
}

-- IF pattern
local if_template = [===[
$do_blocks{}[[
	if($cond) $expr
]],
[[
	else if($cond) $expr
]],
[[
	else $expr

]]
]===]


--- Stringify an IF operation
function if_to_string(e)
		
	return cosmo.f(if_template){
		do_blocks = function()
			for i, b in ipairs(e) do
				if(i == 1) then
					cosmo.yield{
						cond = expr_to_string(b.predicate),
						expr = expr_to_string(b.consequent),
						_template = 1,
					}
				elseif(b.predicate) then
					cosmo.yield{
						cond = expr_to_string(b.predicate),
						expr = expr_to_string(b.consequent),
						_template = 2,
					}
				else
					cosmo.yield{
						expr = expr_to_string(b.consequent),
						_template = 3,
					}
				end
			end
		end
	}
end

--- Stringify a single expressions
function expr_to_string(e)
	if(e.op == "if") then
		return if_to_string(e)
	end

	local args = {}
	for i, a in ipairs(e) do
		if(a.op == "const") then
			args[i] = a[1]
		else
			args[i] = expr_to_string(a)
		end
	end
	
	local fmt = op_fmt[e.op]
	if(fmt) then
		return format(fmt, unpack(args))
	elseif(e.op == "call") then
		_print(e.name, unpack(args))
		fmt = "%s("..string.rep("%s, ", #args-1).."%s)"
		return format(fmt, e.name, unpack(args))
	else
		fmt = "%s("..string.rep("%s, ", #args-1).."%s)"
		return format(fmt, e.op, unpack(args))
	end
end

function copy(t)
	local tt = {}
	for k, v in pairs(t) do
		if(type(v) == "table") then
			tt[k] = copy(v)
		else
			tt[k] = v
		end
	end
	return tt
end

function vector_index(e, idx)
	local ee = {
		op = e.op,
	}
	
	if(e.type) then
		ee.type = copy(e.type)
	end
	
	for i, s in ipairs(e) do
		if(s.op == "const") then
			if(s.type.size > 0) then
				ee[i] = {
					[1] = copy(s),
					[2] = {
						[1] = idx,
						op = "const",
					},
					op = "index",
				}
			else
				ee[i] = copy(s)
			end
		else
			ee[i] = vector_index(s, idx)
		end
	end
	return ee
end

function print(t, name, lvl)
	lvl = lvl or 0
	_print(string.rep("    ", lvl)..(name and (name .. " = {") or "{"))
	for k, v in pairs(t) do 
		if not (type(k) == "number" and k > 0 and k <= #t) then
			if(type(v) == "table") then
				print(v, k, lvl+1)
			elseif type(k) == "number" then
				_print(string.rep("    ", lvl+1)..""..tostring(k).." = "..tostring(v))
			else
				_print(string.rep("    ", lvl+1).."['"..tostring(k).."'] = "..tostring(v))
			end
		end
	end
	for k, v in pairs(t) do 
		if (type(k) == "number" and k > 0 and k <= #t) then
			if(type(v) == "table") then
				print(v, k, lvl+1)
			elseif type(k) == "number" then
				_print(string.rep("    ", lvl+1)..""..tostring(k).." = "..tostring(v))
			else
				_print(string.rep("    ", lvl+1).."["..tostring(k).."] = "..tostring(v))
			end
		end
	end
	_print(string.rep("    ", lvl).."}")
end

function print_ignoring(t, ignore, name, lvl)
	lvl = lvl or 0
	_print(string.rep("    ", lvl)..(name and (name .. " = {") or "{"))
	for k, v in pairs(t) do
		if(not ignore[k]) then
			if(type(v) == "table") then
				print_ignoring(v, ignore, k, lvl+1)
			else
				_print(string.rep("    ", lvl+1).."["..tostring(k).."] = "..tostring(v))
			end
		end
	end
	_print(string.rep("    ", lvl).."}")
end

function print_code(code)
	local i = 1
	for line in code:gmatch("([^\n]+)\n?") do
		_print(i, line)
		i = i+1
	end
end