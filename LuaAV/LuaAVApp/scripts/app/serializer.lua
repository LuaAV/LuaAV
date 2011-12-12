local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local print = print
local setmetatable = setmetatable

local string = require 'string'
local table = require 'table'


module(...)
----------------------------------------------------------------------------
-- app.serializer
----------------------------------------------------------------------------

local prototype_start = "%s{"
local table_start = "%s = {"
local entry = "%s = %s,"
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

function val_string.string(v)
	return string.format("\"%s\"", v)
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
	if(v:find("[^%w_]+")) then
		return string.format("['%s']", v)
	else
		return v
	end
end
------------------------------
local
function make_entry(k, v)
	return string.format(entry, k, v)
end

local
function make_table_start(name)
	return string.format(table_start, name)
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

function serialize(name, t, lvl)
	local serialized = ""
	lvl = lvl or 0

	-- get all keys
	local keys = {}
	for k, v in pairs(t) do
		keys[#keys+1] = k
	end
	
	-- sort them
	table.sort(keys, function(v1, v2)
		return tostring(v1) < tostring(v2)
	end)
	
	-- write it
	serialized = add_chunk(serialized, make_indent(lvl) .. make_table_start(name))	
	for i, k in ipairs(keys) do
		local v = t[k]
		local fk = entry_string[type(k)]
		local fv = val_string[type(v)]
		if(fk and fv) then
			serialized = add_chunk(serialized, make_indent(lvl+1) .. make_entry(fk(k), fv(v)))
		else
			if(fk and type(v) == "table") then
				serialized = add_chunk(serialized, serialize(fk(k), v, lvl+1))
			end
		end
	end
	serialized = add_chunk(serialized, make_indent(lvl) .. make_table_end(lvl))
	
	if(lvl == 0) then
		-- filter special tokens like '\'
		serialized = filter_special_tokens(serialized)
	end
	return serialized
end
