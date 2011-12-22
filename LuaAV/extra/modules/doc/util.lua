local lexer = require("doc.lexer")
local lfs = require "lfs"
local table, string, io, assert = table, string, io, assert
local package = package
local print = print
local format = string.format
local unpack = unpack
local ipairs = ipairs


--- Documentation utility functions
-- The utility functions are a set of essential and generic file path and 
-- file io routines used across the doc modules.
-- @name doc.util
module(...)

function highlight(src)
	-- convert quotes
	local src = src:gsub("&quot;", '"')
	
	local lexed = lexer(src)
	local str = {}
	for i, v in ipairs(lexed) do
		if(v[1] == "whitespace") then
			table.insert(str, v[2])
		else
			table.insert(str, string.format([[<span class="lua-%s">%s</span>]], unpack(v)))
		end
	end
	return table.concat(str)
end

--- Write source text to a file
-- @param DOC The documentation logger
-- @param filename The file to write
-- @param src The file source
function write(filename, src)
	ensurepath(stripfilename(filename))
	
	local file = io.open(filename, "w")
	file:write(src)
	file:close()
end

--- Find a file
-- Searches the module search paths for a file
-- @param subpath The sub-path to search for
function findfile(subpath)
	local path = package.path
	for dir in path:gmatch("([^;]+)/%?[^;];?") do
		local f = io.open(dir..subpath)
		if(f) then
			return dir..subpath
		end
	end
end

--- Load a file
-- Load a file in the search path
-- @param subpath The sub-path to search for
-- @ret File text
function loadfile(subpath)
	local path = findfile(subpath)
	if(path) then
		local f = io.open(path)
		local txt = f:read("*a")
		f:close()
		return txt
	end
end

--- Gives chunks from a module path
-- @param name Module name
-- @ret Chunks (reversed array of module path elements)
function module_pathchunks(name)
	local chunks = {}
	for w in string.gmatch(name, "[^%.]+") do
	   table.insert(chunks, 1, w)
	end
	return chunks
end

--- Verify that a file exists or use a default if it doesn't
-- When verifying whether a file exists, the search will look for 
-- the file and if it can't find it, use the default file instead.  
-- The search is applied in the following order: 1) Look for the file 
-- with no searching, 2) Search for the file in the Lua module paths, 
-- 3) Search for the file in the Lua module paths with the default_subpath 
-- prepended, and 4) Use the default file.
-- @param file The file to check for
-- @param default The default file to use
-- @param default_subpath The sub-path to look for the default in
-- @ret The file found
-- @ret The absoulte path of the file found
function verify_file(file, default, default_subpath)
	file = file or default

	-- adjust default path for concatenation with filenames
	default_subpath = default_subpath or ""
	if(	default_subpath:len() > 0 and 
		default_subpath:sub(default_subpath:len(), default_subpath:len()) ~= "/"
	) then
		default_subpath = default_subpath.."/"
	end

	-- is it a default?
	if(file == default) then
		local abspath = assert(
			findfile(default_subpath..default), 
			"unable to find "..default
		)
		return default, abspath
	end

	-- check for absolute path
	local filename = strippath(file) or file
	if(io.open(file)) then
		return filename, file
	else
		-- try and find it
		local filepath = findfile(filename)
		if(filepath) then
			return filename, filepath
		else
			-- try and find it within default_subopath
			filepath = findfile(default_subpath..filename)
			if(filepath) then
				return filename, filepath
			else
				-- use the default
				print(format(
					"Warning: Unable to find file %s.  Falling back to %s", 
					file, default
				))
				filepath = findfile(default_subpath..default)
				return default, filepath
			end
		end
	end
end

--- Tests if a path is absolute
-- @param path
-- @ret True/False
function is_absolute_path(path)
	return path:sub(1, 1) == "/"
end

--- Gives chunks from a file path
-- @param name File path name
-- @ret Chunks (reversed array of file path elements)
function pathchunks(name)
	local chunks = {}
	for w in string.gmatch(name, "[^/\\]+") do
	   table.insert(chunks, 1, w)
	end
	return chunks
end

--- Strip the file name from a path
-- @ret Path without file name
function stripfilename(filename)
	return string.match(filename, "(.+)/[^/]*%.%w+$")
end

--- Strip the file name from a path
-- @ret File name without path
function strippath(filename)
	return string.match(filename, ".+/([^/]*%.%w+)$")
end

--- Ensure a path exists by creating folders if necessary
-- @param path File path
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

--- File path from a module name
-- @param name Module name
-- @ret The file path name
function path_from_name(name)
	return name:gsub("%.", "/")
end

--- Get the extension of a file
-- @param filename The file name
-- @param The extension
function getextension(filename)
	return filename:match(".+%.(%w+)$")
end