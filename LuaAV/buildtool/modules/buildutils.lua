--[[
	Collection of utility functions for the build system.  Most of the functions 
	either deal with file name and file path processing or the execution of 
	command line calls.
	
	File name/path processing contains funtions for extracting elements of a 
	file path such as the fully qualified directory of a file, it's file name, 
	extension, and name without extension.  Additionally, there are helper functions 
	for creating files and directories that can verify the existence of a path before 
	a copy of move operation, creating the directory if necessary.
	
	The command line functions provide wrappers around commonly used command line 
	utilities such as gcc, mv, rm, cp, and mkdir.  
--]]
local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local next = next
local print = print
local unpack = unpack
local loadfile = loadfile
local loadstring = loadstring
local setfenv = setfenv
local setmetatable = setmetatable
local getfenv = getfenv
local tonumber = tonumber
local tostring = tostring
local error = error

local table = require 'table'
local string = require 'string'
local os = require 'os'
local io = require 'io'
local print = print

local lfs = require("lfs")

local OS = _OSNAME

--[[
-- for later autoconf type behavior
#include <stdio.h>
#include <iostream>

int main(int argc, const char *argv[]) {
	int x = 1;
	if(*(char *)&x == 1) {
		printf("little endian\n");
	}
	else {
		printf("big endiand\n");
	}
	return 0;
}

--]]


module('buildutils')
----------------------------------------------------------

local M = getfenv()
M.echo = true


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

function makepath(root, ...)
	local v = {...}
	if(#v >= 1) then
		local fmt = string.rep("%s/", #v) .. "%s"
		return string.format(fmt, root, unpack(v))
	else
		return root
	end
end

function cmdline_arg(arg)
	return arg:match("([%w_]+)=([%w_]+)")
end

function table_combine(t1, ...)
	local new_t1 = {}
	for i, v in ipairs(t1) do
		new_t1[i] = v
	end	

	local vv = {...}
	for i, tt in ipairs(vv) do
		for i, v in ipairs(tt) do
			new_t1[#new_t1+1] = v
		end
	end
	return new_t1
end

function loadinenv(_G, filename)
	local loadenv = {}
	loadenv.__index = _G
	setmetatable(loadenv, loadenv)
	
	local file = io.open(filename)
	local code = file:read("*a")
	file:close()
	code = string.format("project = %s", code)
	local f = assert(loadstring(code))
	setfenv(f, loadenv)
	f()
	return loadenv
end

function exec(cmd, echo, sudo)
	echo = echo or M.echo
	sudo = sudo or false
	if(echo) then
		print(cmd)
		print("")
	end
	if(sudo) then
		cmd = "sudo " .. cmd
	end

	local dir = os.getenv("PWD")
	local file = dir .. "/stderr.txt"

	cmd = string.format("%s 2>%s", cmd, file)
	local txt = io.popen(cmd):read("*a")

	local ferr = io.open(file)
	local err_text
	if(ferr) then
		err_text = ferr:read("*a")
		if(err_text:len() > 0 and not err_text:find("warning")) then
			print("ERROR:", err_text)
			os.exit()
		else
			err = nil
		end
		ferr:close()
	end
	if(lfs.attributes(file)) then
		io.popen("rm " .. file)
	end

	return txt, err_text
end

function compilefile(filename, objname, cflags)
	cflags = cflags or ""
	local originalpath = lfs.currentdir()

	local path = stripfilename(filename)	
	lfs.chdir(path)

	-- compile .c files as C code
	if(getextension(filename) == "c") then
		cflags = "-x c " .. cflags
	end

	local build = true	
	if(lfs.attributes(objname)) then
		local filemod = lfs.attributes(filename, "modification")
		local objmod = lfs.attributes(objname, "modification")
		if(filemod < objmod) then
			build = false
		end
	end
	
	if(build) then
		local cmd = string.format("g++ %s -o %s -c %s", cflags, objname, filename)
		local err = exec(cmd)
	end
	
	lfs.chdir(originalpath)
end

function linkobjects(objects, path, name, ldflags)
	ldflags = ldflags or ""
	
	local originalpath = lfs.currentdir()
	lfs.chdir(path)
	local ofiles = table.concat(objects, " ")
	local cmd = string.format("g++ -o %s %s %s", name, ofiles, ldflags)
	local err = exec(cmd)
	lfs.chdir(originalpath)
end

function stripsvn(dir, sudo)
	local cmd = string.format("find %s \\( -name .svn -and -exec rm -rf {} \\; -and -prune \\)", dir)
	exec(cmd, nil, sudo)
end

function stripgit(dir, sudo)
   local cmd = string.format("find %s \\( -name .git -and -exec rm -rf {} \\; -and -prune \\)", dir)
   exec(cmd, nil, sudo)
end

function movefile(src, dst, sudo)
	ensurepath(dst)
	local cmd = string.format("mv %s %s", src, dst)
	exec(cmd, nil, sudo)
end

function copyfile(src, dst, sudo)
	ensurepath(dst)
	local cmd = string.format("cp %s %s", src, dst)
	exec(cmd, nil, sudo)
end

function copydir(src, dst, sudo)
	ensurepath(dst)
	local cmd = string.format("cp -R %s %s", src, dst)
	exec(cmd, nil, sudo)
end

function exportdir(src, dst, sudo)
	ensurepath(dst)
-- 	local cmd = string.format("svn export --force %s %s", src, dst)
   local cmd = string.format("cp -R %s %s", src, dst)
	exec(cmd, nil, sudo)
end

function removefile(filename, sudo)
	if(lfs.attributes(filename) ~= nil) then
		local cmd = string.format("rm %s", filename)
		exec(cmd, nil, sudo)
	end
end

function removedir(dirname, sudo)
	if(lfs.attributes(dirname) ~= nil) then
		local cmd = string.format("rm -r %s", dirname)
		exec(cmd, nil, sudo)
	end
end

function ensurepath(path, sudo)
	local chunks = pathchunks(path)
	local originalpath = lfs.currentdir()
	lfs.chdir("/")
	for i=#chunks, 1, -1 do
		local c = chunks[i]
		local exists = lfs.attributes(c) ~= nil
		if(not exists) then
			if(sudo) then
				exec("mkdir " .. c, nil, sudo)
			else
				lfs.mkdir(c)
			end
		end
		lfs.chdir(c)
	end
	lfs.chdir(originalpath)
end

function findfileinpath(path, filename, recursive)
    local originalpath = lfs.currentdir()
	local result
	if lfs.chdir(path) then
		for f, attr in lfs.dir(path) do
			local subpath = path .. "/" .. f
			if f == filename then
				return subpath
			elseif recursive then
				local attr, err = lfs.attributes(subpath)
				if(attr) then
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

function filter_from_extensions(extensions)
	local patterns = {}
	for i, e in ipairs(extensions) do
		patterns[i] = "%." .. e .. "$"
	end
	
	return
	function(filepath)
		local match = false
		for i, p in ipairs(patterns) do
			match = match or filepath:find(p)
			if(match) then
				return match ~= nil
			end
		end
		return match ~= nil
	end
end

function findfilesinpath_filtered(path, filter, recursive, sourcefiles)
	sourcefiles = sourcefiles or {}

	local originalpath = lfs.currentdir()
	local result
	if lfs.chdir(path) then
		for f, attr in lfs.dir(path) do
			local subpath = path .. "/" .. f
			local attr, err = lfs.attributes(subpath)
			if(attr and attr.mode == "directory") then
				if(f:find('.', 1, true) ~= 1 and recursive) then
					findfilesinpath_filtered(subpath, filter, recursive, sourcefiles)
				end
			elseif(filter(f)) then
				sourcefiles[#sourcefiles+1] = subpath
			end
		end
	end
	lfs.chdir(originalpath)
	return sourcefiles
end


function writefile(filename, str)
	local file, err = io.open(filename, "w")
	if(not file) then
		error(string.format("%s  writing to file %s", err, filename))
		return	
	end
	
	file:write(str)
	file:close()
end


function configure(base_dir, conf_code)
	-- files and paths for compile test
	local temp_dir = base_dir .. "/temp"
	local test_source = temp_dir .. "/test.c"
	local test_build_dir = temp_dir .. "/build"
	local test_object = test_build_dir .. "/test.o"
	local test_bin = test_build_dir .. "/test"
	
	-- setup files and paths
	ensurepath(temp_dir)	
	writefile(test_source, conf_code)
	ensurepath(test_build_dir)
	
	-- compile and run
	compilefile(test_source, test_object)
	linkobjects({test_object}, test_build_dir, "test")
	local result = exec(test_bin)
	
	--print("'".. result.."'")
	local vars = {}
	for k, v in result:gmatch("([%w_]*):([%w_]*)") do
		vars[k] = tonumber(v) or v
	end
	
	-- cleanup
	removedir(temp_dir)
	
	return vars
end
