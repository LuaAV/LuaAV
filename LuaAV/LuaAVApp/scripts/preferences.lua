local lfs = require("lfs")
local app = require("app")
Window = require("app.Window")

-- extend LuaAV module
function LuaAV.addmodulepath(path)
	-- add to package paths (if not already present)
	if not string.find(package.path, path, 0, true) then
		package.path = string.format("%s/?.lua;%s", path, package.path)
		package.path = string.format("%s/?/init.lua;%s", path, package.path)
		package.cpath = string.format("%s/?.so;%s", path, package.cpath)
	end
end


function LuaAV.findfileinpath(path, filename, recursive)
	local result
	local originalpath = lfs.currentdir()
	if lfs.chdir(path) then
		for f, attr in lfs.dir(path) do
			local subpath = path .. "/" .. f
			if f == filename then
				return subpath
			else
				if recursive then
					local attr = lfs.attributes(subpath)
					if(attr) then
						if attr.mode == "directory" and f:find('.', 1, true) ~= 1 then
							result = LuaAV.findfileinpath(subpath, filename, true)
							if(result) then
								return result
							end
						end
					end
				end
			end
		end
	end
	lfs.chdir(originalpath)
	return result
end


local searchpaths = {
	{app.apppath.."/extra/resources", true},
	{app.apppath.."/../../extra/resources", true},
}
function LuaAV.findfile(filename)
	for _, p in ipairs(searchpaths) do
		local res = LuaAV.findfileinpath(p[1], filename, p[2])
		if(res) then
			return res
		end
	end
end
function LuaAV.addsearchpath(path, recursive)
	table.insert(searchpaths, { path, recursive or false })
end

-- implement search paths in dofile
local
function makesearching(f)
	-- add functionality to a file-loading function
	return
	function(filename)
		if(filename:sub(1, 1) == "/") then
			return f(filename)
		else
			local path, isDir = LuaAV.findfile(filename)
			--print(path, isDir)
			if(path and not isDir) then
				return f(path)
			else
				local str
				if(isDir) then
					str = "File %s is a directory.  Couldn't open."
				else
					str = "File %s not found"
				end
				str = string.format(str, filename)
				error(str)
			end
		end
	end
end

--dofile = makesearching(dofile)
loadfile = makesearching(loadfile)

-- augment modules search paths
LuaAV.addmodulepath(app.apppath.."/modules")
LuaAV.addmodulepath(app.apppath.."/extra/modules")
LuaAV.addmodulepath(app.apppath.."/../../extra/modules")
LuaAV.addmodulepath(app.resourcepath)