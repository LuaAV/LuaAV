local apputil = require("app.utilities")

--- Module implementing the LuaRocks "details" command.
-- Installs binary rocks.
module("luarocks.details", package.seeall)

local path = require("luarocks.path")
local rep = require("luarocks.rep")
local fetch = require("luarocks.fetch")
local util = require("luarocks.util")
local fs = require("luarocks.fs")
local deps = require("luarocks.deps")
local manif = require("luarocks.manif")
local cfg = require("luarocks.cfg")
local persist = require("luarocks.persist")

help_summary = "Get the details a rock."

help_arguments = "{<rock>|<name> [<version>]}"

help = [[
Argument may be the name of a rock to be fetched from a repository
or a filename of a locally available rock.
]]


--- Driver function for the "details" command.
-- @param name string: name of a binary rock. If an URL or pathname
-- to a binary rock is given, fetches and installs it. If a rockspec or a
-- source rock is given, forwards the request to the "build" command.
-- If a package name is given, forwards the request to "search" and,
-- if returned a result, installs the matching rock.
-- @param version string: When passing a package name, a version number
-- may also be given.
-- @return boolean or (nil, string): True if installation was
-- successful, nil and an error message otherwise.
function run(...)
	
   local flags, name, version = util.parse_flags(...)
   if type(name) ~= "string" then
      return nil, "Argument missing, see help."
   end

   local ok, err = fs.check_command_permissions(flags)
   if not ok then return nil, err end

	local specpath

   if name:match("%.rockspec$") or name:match("%.src%.rock$") then
      --local build = require("luarocks.build")
      --return build.run(name, flags["local"] and "--local")
      specpath = name
   elseif name:match("%.rock$") then
      --return install_binary_rock(name)
      specpath = name
   else
   	---[[
      local search = require("luarocks.search")
      local results, err = search.find_suitable_rockspec(search.make_query(name:lower(), version))
      
      if err then
         return nil, err
      elseif type(results) == "string" then
         local url = results
         --print("Installing "..url.."...")
         --return run(url)
         specpath = url
      else
         print()
         print("Could not determine which rock to install.")
         print()
         print("Search results:")
         print("---------------")
         search.print_results(results)
         return nil, (next(results) and "Please narrow your query." or "No results found.")
      end
      --]]
   end
   ---[=[
   local name = specpath:gsub("[/:]","_")
   local file, err, errcode = fetch.fetch_url_at_temp_dir(specpath, name)
	if not file then
		return nil, "Failed fetching manifest for "..specpath, errcode
	end
	
	local t = persist.load_into_table(file, nil, true)
	if(not t) then
		return nil, "couldn't load file "..file
	end
	return t
	--]=]
end