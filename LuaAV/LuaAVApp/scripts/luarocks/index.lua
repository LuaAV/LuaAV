
module("luarocks.index", package.seeall)

local util = require("luarocks.util")
local fs = require("luarocks.fs")
local deps = require("luarocks.deps")
local persist = require("luarocks.persist")
local dir = require("luarocks.dir")
local manif = require("luarocks.manif")

local index_header = [[
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Available rocks</title>
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
<style>
body {
   background-color: white;
   font-family: "bitstream vera sans", "verdana", "sans";
   font-size: 14px;
}
a {
   color: #0000c0;
   text-decoration: none;
}
a:hover {
   text-decoration: underline;
}
td.main {
   border-style: none;
}
blockquote {
   font-size: 12px;
}
td.package {
   background-color: #f0f0f0;
   vertical-align: top;
}
td.spacer {
   height: 5px;
}
td.version {
   background-color: #d0d0d0;
   vertical-align: top;
   text-align: left;
   padding: 5px;
   width: 100px;
}
p.manifest {
   font-size: 8px;
}
</style>
</head>
<body>
<h1>Available rocks</h1>
<p>
Lua modules available from this location for use with <a href="http://www.luarocks.org">LuaRocks</a>:
</p>
<table class="main">
]]

local index_package_start = [[
<td class="package">
<p><a name="$anchor"></a><b>$package</b> - $summary<br/>
</p><blockquote><p>$detailed<br/>
$externaldependencies
<font size="-1"><a href="$original">latest sources</a> $homepage | License: $license</font></p>
</blockquote></a></td>
<td class="version">
]]

local index_package_end = [[
</td></tr>
<tr><td colspan="2" class="spacer"></td></tr>
]]

local index_footer = [[
</table>
<p class="manifest">
<a href="manifest">manifest file</a>
</p>
</body>
</html>
]]

function format_external_dependencies(rockspec)
   if rockspec.external_dependencies then
      local deplist = {}
      local listed_set = {}
      local plats = nil
      for name, desc in util.sortedpairs(rockspec.external_dependencies) do
         if name ~= "platforms" then
            table.insert(deplist, name:lower())
            listed_set[name] = true
         else
            plats = desc
         end
      end
      if plats then
         for plat, entries in util.sortedpairs(plats) do
            for name, desc in util.sortedpairs(entries) do
               if not listed_set[name] then
                  table.insert(deplist, name:lower() .. " (on "..plat..")")
               end
            end
         end
      end
      return '<p><b>External dependencies:</b> ' .. table.concat(deplist, ',&nbsp;').. '</p>'
   else
      return ""
   end
end

function make_index(repo)
   if not fs.is_dir(repo) then
      return nil, "Cannot access repository at "..repo
   end
   local manifest = manif.load_manifest(repo)
   local out = io.open(dir.path(repo, "index.html"), "w")
   
   out:write(index_header)
   for package, version_list in util.sortedpairs(manifest.repository) do
      local latest_rockspec = nil
      local output = index_package_start
      for version, data in util.sortedpairs(version_list, deps.compare_versions) do
         local versions = {}
         output = output..version..':&nbsp;'
         table.sort(data, function(a,b) return a.arch < b.arch end)
         for _, item in ipairs(data) do
            local link = '<a href="$url">'..item.arch..'</a>'
            if item.arch == 'rockspec' then
               local rs = ("%s-%s.rockspec"):format(package, version)
               if not latest_rockspec then latest_rockspec = rs end
               link = link:gsub("$url", rs)
            else
               link = link:gsub("$url", ("%s-%s.%s.rock"):format(package, version, item.arch))
            end
            table.insert(versions, link)
         end
         output = output .. table.concat(versions, ',&nbsp;') .. '<br/>'
      end
      output = output .. index_package_end
      if latest_rockspec then
         local rockspec = persist.load_into_table(dir.path(repo, latest_rockspec))
         local vars = {
            anchor = package,
            package = rockspec.package,
            original = rockspec.source.url,
            summary = rockspec.description.summary or "",
            detailed = rockspec.description.detailed or "",
            license = rockspec.description.license or "N/A",
            homepage = rockspec.description.homepage and ("| <a href="..rockspec.description.homepage..">project homepage</a>") or "",
            externaldependencies = format_external_dependencies(rockspec)
         }
         vars.detailed = vars.detailed:gsub("\n\n", "</p><p>"):gsub("%s+", " ")
         output = output:gsub("$(%w+)", vars)
      else
         output = output:gsub("$anchor", package)
         output = output:gsub("$package", package)
         output = output:gsub("$(%w+)", "")
      end
      out:write(output)
   end
   out:write(index_footer)
   out:close()
end
