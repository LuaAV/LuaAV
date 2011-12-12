local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local loadfile = loadfile
local package = package
local setfenv = setfenv

local print = print
local setmetatable = setmetatable

local string = require 'string'
local table = require 'table'
local io = require 'io'
local lfs = require 'lfs'

local format = string.format

-- stderr errors, not halting
local error = error

module(...)
----------------------------------------------------------------------------
-- app.utilities
----------------------------------------------------------------------------

function printTable(t, name)
	print(name and (name .. " {") or "{")
	for k, v in pairs(t) do 
		print("", tostring(k), tostring(v))
	end
	print("}")
end

function array_to_hash(a)
	local h = {}
	for i, v in ipairs(a) do
		h[v] = true
	end
	return h
end

function loadinenv(_G, filename)
	local loadenv = {}
	loadenv.__index = _G
	setmetatable(loadenv, loadenv)
	
	local f, err = loadfile(filename)
	if(err) then
		error(err)
	end
	setfenv(f, loadenv)
	f()
	return loadenv
end


function stripfilename(filename)
	return string.match(filename, "(.+)/[^/]*%.%w+$")
end

function strippath(filename)
	return string.match(filename, ".+/([^/]*%.%w+)$")
end

function stripextension(filename)
	local idx = filename:match(".+()%.%w+$")
	if(idx) then
		return filename:sub(1, idx-1)
	else
		return filename
	end
end

function getextension(filename)
	return filename:match(".+%.(%w+)$")
end

function pathchunks(name)
	local chunks = {}
	for w in string.gmatch(name, "[^/\\]+") do
	   table.insert(chunks, 1, w)
	end
	return chunks
end

function getminimalpath(paths, p)
	local c = pathchunks(p)
	if(#paths <= 0) then
		return c[1]
	else
		local minpath = c[1]
		for _, path in ipairs(paths) do
			if(path ~= p) then
				local pp = minimalpath(p, path)
				if(pp:len() > minpath:len()) then
					minpath = pp
				end
			end
		end
		return minpath
	end
end

function minimalpath(p1, p2)
	local c1 = pathchunks(p1)
	local c2 = pathchunks(p2)
	
	local minpath = ""
	
	for i, c in ipairs(c1) do
	
		if i > 1 then
			minpath = c .. "/" .. minpath
		else
			minpath = c
		end
		
		if #c2 < i then
			break
		else
			if c ~= c2[i] then
				break
			end
		end
	end
	
	return minpath
end

function ensurepath(path)
	local chunks = pathchunks(path)
	local originalpath = lfs.currentdir()
	lfs.chdir("/")
	for i=#chunks, 1, -1 do
		local c = chunks[i]
		local exists = lfs.attributes(c) ~= nil
		if(not exists) then
			lfs.mkdir(c)
		end
		lfs.chdir(c)
	end
	lfs.chdir(originalpath)
	return path
end

function writefile(filename, str)
	local file, err = io.open(filename, "w")
	if(not file) then
		error("%s  writing to file %s", err, filename)
		return	
	end
	
	file:write(str)
	file:close()
end

function addsearchpath(path, Searchpaths, recursive)
	recursive = recursive or false
	table.insert(Searchpaths, { path, recursive } )
	
	addmodulepath(path)
end

function addmodulepath(path)
	-- add to package paths (if not already present)
	if not string.find(package.path, path, 0, true) then
		package.path = string.format("%s/?.lua;%s", path, package.path)
		package.path = string.format("%s/?/init.lua;%s", path, package.path)
		package.cpath = string.format("%s/?.so;%s", path, package.cpath)
	end
end

function findfileinpath(path, filename, recursive)
	local result
	local originalpath = lfs.currentdir()
	if lfs.chdir(path) then
		for f in lfs.dir(path) do
			local subpath = path .. "/" .. f
			if f == filename then
				return subpath
			else
				if recursive then
					local attr = lfs.attributes(subpath)
					if attr.mode == "directory" and f:find('.', 1, true) ~= 1 then
						result = findfileinpath(subpath, filename, true)
						if(result) then
							return result
						end
					end
				end
			end
		end
	end
	lfs.chdir(originalpath)
	return result
end

function findfile(filename, Searchpaths)
	local originalpath = lfs.currentdir()
	local result
	for i, t in ipairs(Searchpaths) do
		result = result or findfileinpath(t[1], filename, t[2])
	end
	lfs.chdir(originalpath)
	return result
end

function makesearching(f, searchfunc)
	-- add search paths to a file-loading function
	return
	function(filename)
		local path = searchfunc(filename)
		if(path) then
			return f(path)
		else
			error(string.format("could not find file %s", filename))
		end
	end
end
--]]


local
function keysorter(a, b) 
	if type(a) == type(b) and (type(a) == "number" or type(a) == "string") then 
		return a<b 
	end
	return type(a) > type(b) 
end

local
function table_tostring(tt, indent, done)
	done = done or {}
	indent = indent or 0
	if type(tt) == "table" then
		if done[tt] then
			return format("<memoized: %s>", tostring(tt))
		else
			--done[tt] = true
			
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

function printt(t)
	print(table_tostring(t))
end