local format = string.format

---------------------------------------------------------------
-- Bootstrapping functions required to coalesce paths
local
function exec(cmd, echo)
	echo = echo or true
	if(echo) then
		print(cmd)
		print("")
	end
	local res = io.popen(cmd):read("*a")
	return res:sub(1, res:len()-1)
end

local
function stripfilename(filename)
	return string.match(filename, "(.+)/[^/]*%.%w+$")
end

local
function strippath(filename)
	return string.match(filename, ".+/([^/]*%.%w+)$")
end

local
function stripextension(filename)
	local idx = filename:match(".+()%.%w+$")
	if(idx) then
		return filename:sub(1, idx-1)
	else
		return filename
	end
end

local
function addmodulepath(path)
	-- add to package paths (if not already present)
	if not string.find(package.path, path, 0, true) then
		package.path = string.format("%s/?.lua;%s", path, package.path)
		package.path = string.format("%s/?/init.lua;%s", path, package.path)
		package.cpath = string.format("%s/?.so;%s", path, package.cpath)
	end
end

local
function setup_modulepath()

	local pwd = exec("pwd")
	local root = arg[0]
	if(root and stripfilename(root)) then 
		root = stripfilename(root) .. "/"
	else 
		root = "" 
	end
	
	local script_path
	local path

	if(root:sub(1, 1) == "/") then
		script_path = root
		path = string.format("%s%s", root, "modules")
	else
		script_path = string.format("%s/%s", pwd, root)
		path = string.format("%s/%s%s", pwd, root, "modules")
	end
	addmodulepath(path)
	
	return script_path:sub(1, script_path:len()-1)
end
---------------------------------------------------------------
-- Script Initialization
local script = {}
script.path = setup_modulepath()

local login = assert(arg[1], "must be username@server")
local tmp_dir = script.path.."/tmp"

local copy_doc_cmd = format("svn export %s %s", script.path.."/../../extra/doc", tmp_dir)
os.execute(copy_doc_cmd)

local update_server_cmd = format("scp -r %s/* %s:/mat/sites/lua-av/doc", tmp_dir, login)
print(update_server_cmd)
os.execute(update_server_cmd)

os.execute("rm -rf "..tmp_dir)