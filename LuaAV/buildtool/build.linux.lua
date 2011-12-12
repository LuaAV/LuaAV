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
function addmodulepath(path)
	-- add to package paths (if not already present)
	if not string.find(package.path, path, 0, true) then
		package.path = string.format("%s/?.lua;%s", path, package.path)
		package.path = string.format("%s/?/init.lua;%s", path, package.path)
		package.cpath = string.format("%s/?.so;%s", path, package.cpath)
	end
end

local
function setup_buildtools_modulepath()

	local pwd = exec("pwd")
	local root = arg[0]
	print(pwd)
	if(root and stripfilename(root)) then 
		root = stripfilename(root) .. "/" 
	else 
		root = "" 
	end
	
	local path = string.format("%s/%s%s", pwd, root, "modules")
	--print("module path", path)
	addmodulepath(path)
end

local
function svnroot()
	local shellpath = exec("pwd")
	local repopath = arg[1]

	local one = repopath:sub(1, 1)
	local two = repopath:sub(2, 2)
	if(one == "/") then
		-- nothing for absolute
	elseif(one == "." and two == ".") then
		-- concatenate for relative
		repopath = shellpath .. "/" .. repopath
	elseif(repopath == ".") then
		repopath = shellpath .. "/"
	else
		error("repo root folder path is invalid, must be either a relative or absolute path")
	end
	
	-- remove trailing '/'
	if(repopath:sub(repopath:len()) == "/") then
		repopath = repopath:sub(1, repopath:len()-1)
	end
	return repopath
end

---------------------------------------------------------------
---------------------------------------------------------------

configuration = "Debug"
makedistro = false
action = "build"

local settings = {
	config = function(v)
		if(string.lower(v) == "release") then
			configuration = "Release"
		else
			configuration = "Debug"
		end
	end,

	--[[
	makedistro = function(v)
		if(string.lower(v) == "true") then
			makedistro = true
		else
			makedistro = false
		end
	end,
	--]]

	action = function(v)
		action = string.lower(v)
	end,
}

for k, v in pairs(settings) do
	settings[#settings+1] = k
end
table.sort(settings, function(v1, v2) 
	return v1 < v2
end)

local err_str = table.concat(settings, "\n")
if(not arg[1]) then
	print("-------------------------------")
	print("Invalid Arguments:")	
	print("arguments are: 'repo root' followed by any of the following arguments:\n"
		.. err_str)
	print("-------------------------------")	
	os.exit()
end

---------------------------------------------------------------
setup_buildtools_modulepath()
reporoot = assert(svnroot(), "Invalid repository root folder")
---------------------------------------------------------------
---------------------------------------------------------------
-- Configure system
_OSNAME = nil
_OSBIT = nil


local autoconf = require("autoconf")
local vars = autoconf.test_os(reporoot)
print("*************************")
for k, v in pairs(vars) do
	print("configure: " .. k .. " = " .. tostring(v))
	_G[k] = v
end
print("*************************")
---------------------------------------------------------------
---------------------------------------------------------------
-- Load modules and functions
-- (not local so they are available in _G when loading projects from file)
buildutils = require("buildutils")
platform = require("platform")
Project = require("project")
Config = require("config")
Dependency = require("dependency")
Src = require("src")
Path = require("path")

--buildutils.echo = false

stripfilename = buildutils.stripfilename
strippath = buildutils.strippath
stripextension = buildutils.stripextension
makepath = buildutils.makepath
loadinenv = buildutils.loadinenv
ensurepath = buildutils.ensurepath
stripsvn = buildutils.stripsvn
movefile = buildutils.movefile
copyfile = buildutils.copyfile
copydir = buildutils.copydir
exportdir = buildutils.exportdir
findfileinpath = buildutils.findfileinpath
filter_from_extensions = buildutils.filter_from_extensions
findfilesinpath_filtered = buildutils.findfilesinpath_filtered
cmdline_arg = buildutils.cmdline_arg


table.combine = buildutils.table_combine

---------------------------------------------------------------
---------------------------------------------------------------
-- Handle script arguments
print("-----")
for i=2, #arg do
	local k, v = cmdline_arg(arg[i])
	k = k and string.lower(k) or ""
	if(settings[k]) then
		settings[k](v)
	end
end

-- NEEDED TO AVOID NAME CLASH
config = configuration
print("config", configuration)

local build = "base"

---------------------------------------------------------------
-- Global state
-- (not local so they are available in _G when loading projects from file)
echo = true
src_extensions = {"c", "cpp"}
OS = _OSNAME
OSBIT = _OSBIT
modulesfolder = assert(findfileinpath(reporoot, "modules", false), 
									"Unable to find 'modules' folder")
										

---------------------------------------------------------------

local
function loadproject(name)
	local path = makepath(reporoot, "buildtool/projects")
	local file = findfileinpath(path, name .. ".lua", true)
	
	local t = loadinenv(_G, file)
	return t.project
end


base = {
	-- modules
	[1] = {
		--"cairo",
		"color",
		--"csound",
		"font",
		"image",
		--"kinect",
		--"luaclang",
		--"luasql",
		"mime",
		--"opencl",
		--"opencv",
		"opengl",
		"space",
		"socket",
		"video",
	},

	-- app
	[2] = {
		"libluaav",
		"LuaAV",
	},
}

local actions = {
	clean = "clean",
	build = {
--		"clean",
		"compile",
		"link",
		"install",
	},
	install = "install",
}

local
function build_projects(project_sets)
	for _, projects in ipairs(project_sets) do
		for _, p in ipairs(projects) do
			print("project: ", p)
			local pp = loadproject(p)
			if(action == "all") then

			else
				local a = actions[action]
				if(type(a) == "table") then
					for _, aa in ipairs(a) do
						pp[aa](pp, config)
					end
				else
				--	print(pp, a, config)
					pp[a](pp, config)
				end
			end
		end
	end
end
--print(build, _G[build], action, actions[action])
build_projects(_G[build])


