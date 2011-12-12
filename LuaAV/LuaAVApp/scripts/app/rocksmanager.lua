local pairs = pairs
local ipairs = ipairs
local print = print

local app = require("app")
local approcks = require("app.rocks")

module(...)


local repositories = {
	LuaAV = approcks.LuaAV_server(),
	LuaRocks = "http://luarocks.org/repositories/rocks/",
	-- Local
}


function search(reponame)
	app.clear_modules()

	local repo = repositories[reponame]
	local res = approcks.search(repo)
	
	--[[
	local res = {
		opengl = {
			["1.0-34234"] = {
				description = "opengl module for xxxxxx",
			}
		}
	}
	--]]
	for name, t in pairs(res) do
		for version, tt in pairs(t) do
			local manifest = approcks.details(repo, name, version)
			local description
			local license
			if(manifest and manifest.description) then
				description = manifest.description.summary or manifest.description.detailed
				license = manifest.description.license
			end
			app.add_module{
				name = name,
				version = version,
				description = description,
				license = license,
				repository = reponame,
			}
			break
		end
	end
end

function install(reponame, name, version)
	approcks.install(repositories[reponame], name, version)
end