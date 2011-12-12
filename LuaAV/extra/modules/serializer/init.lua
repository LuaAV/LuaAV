local assert, pairs, ipairs, tostring, type = assert, pairs, ipairs, tostring, type
local print = print

local string = require 'string'
local table = require 'table'

--- Serialize a Lua table to a string
-- @name serializer
module(...)

local prototype_start = "%s{"
local table_start = "%s = {"
local constructor_start = "%s{"
local entry = "%s = %s%s"
local table_end = "}"
local indent = "\t"



------------------------------
local val_string = {}
function val_string.boolean(v)
	if(v) then
		return "true"
	else
		return "false"
	end
end

function val_string.number(v)
	if((v % 1.) == 0) then
		return string.format("%d", v)
	else
		return string.format("%f", v)
	end
end

local
function escape_quotes(v)
	return v:gsub("]]", function(x)
		print(x)
		return "] ]"
	end)
end

function val_string.string(v)
	v = escape_quotes(v)
	return string.format("[=[%s]=]", v)
end
------------------------------
local entry_string = {}
function entry_string.boolean(v)
	return string.format("[%s]", val_string.boolean())
end

function entry_string.number(v)
	return string.format("[%s]", val_string.number(v))
end

function entry_string.string(v)
	if(v == "..." or v:find("[%.:%[%]%s<>/]")) then
		return string.format("['%s']", v)
	else
		return v
	end
end
------------------------------
local
function make_entry(k, v, sep)
	sep = sep or ","
	return string.format(entry, k, v, sep)
end

local
function make_table_start(name)
	return string.format(table_start, name)
end

local
function make_constructor_start(name)
	return string.format(constructor_start, name)
end

local
function make_table_end(lvl)
	if(lvl == 0) then
		return table_end
	else
		return table_end .. ","
	end
end

local
function make_indent(lvl)
	local str = ""
	for i=1, lvl do
		str = str .. indent
	end
	return str
end

local
function add_chunk(s, c)
	return s .. c .. "\n"
end

local
function filter_special_tokens(str)
	return string.gsub(str, "\\", "\\\\")
end

local
function serialize_table(name, t, lvl)
	local serialized = ""
	lvl = lvl or 0

	-- get all keys
	local keys = {}
	for k, v in pairs(t) do
		keys[#keys+1] = k
	end
	
	-- sort them
	table.sort(keys, function(k1, k2)
		return tostring(k1) < tostring(k2)
	end)
	
	-- write it
	if(t.__constructor) then
		local entry_string = make_entry(name, make_constructor_start(t.__constructor), "")
		serialized = add_chunk(serialized, make_indent(lvl) .. entry_string)	
		t.__constructor = nil
	else
		serialized = add_chunk(serialized, make_indent(lvl) .. make_table_start(name))	
	end
	for i, k in ipairs(keys) do
		local v = t[k]
		local fk = entry_string[type(k)]
		local fv = val_string[type(v)]
		if(fk and fv) then
			serialized = add_chunk(serialized, make_indent(lvl+1) .. make_entry(fk(k), fv(v)))
		elseif(fk and type(v) == "table") then
			serialized = add_chunk(serialized, serialize(fk(k), v, lvl+1)..",")
		end
	end
	serialized = add_chunk(serialized, make_indent(lvl) .. make_table_end(lvl))
	
	if(lvl == 0) then
		-- filter special tokens like '\'
		serialized = filter_special_tokens(serialized)
	end
	return serialized
end

--- Serialize a table
-- @param name The name of the table
-- @param t The table
-- @ret The table as a string
function serialize(name, t)
	assert(name, "first argument is the name of the table")
	assert(type(t) == "table", "second argument must be a table")
	return serialize_table(name, t)
end