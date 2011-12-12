
--- Module implementing the LuaRocks "unpack" command.
-- Unpack the contents of a rock.
module("luarocks.unpack", package.seeall)

local fetch = require("luarocks.fetch")
local fs = require("luarocks.fs")
local util = require("luarocks.util")
local build = require("luarocks.build")
local dir = require("luarocks.dir")

help_summary = "Unpack the contents of a rock."
help_arguments = "{<rock>|<name> [<version>]}"
help = [[
Unpacks the contents of a rock in a newly created directory.
Argument may be a rock file, or the name of a rock in a rocks server.
In the latter case, the app version may be given as a second argument.
]]

--- Load a rockspec file to the given directory, fetches the source
-- files specified in the rockspec, and unpack them inside the directory.
-- @param rockspec_file string: The URL for a rockspec file.
-- @param dir_name string: The directory where to store and unpack files.
-- @return table or (nil, string): the loaded rockspec table or 
-- nil and an error message.
local function unpack_rockspec(rockspec_file, dir_name)
   assert(type(rockspec_file) == "string")
   assert(type(dir_name) == "string")
   
   local rockspec = fetch.load_rockspec(rockspec_file)
   if not rockspec then
      return nil, "Failed loading rockspec "..rockspec_file
   end
   fs.change_dir(dir_name)
   local ok, sources_dir = fetch.fetch_sources(rockspec, true, ".")
   if not ok then
      return nil, sources_dir
   end
   fs.change_dir(dir_name)
   build.apply_patches(rockspec)
   fs.pop_dir()
   return rockspec
end

--- Load a .rock file to the given directory and unpack it inside it.
-- @param rock_file string: The URL for a .rock file.
-- @param dir_name string: The directory where to unpack.
-- @return table or (nil, string): the loaded rockspec table or 
-- nil and an error message.
local function unpack_rock(rock_file, dir_name, kind)
   assert(type(rock_file) == "string")
   assert(type(dir_name) == "string")
   
   local ok, err, errcode = fetch.fetch_and_unpack_rock(rock_file, dir_name)
   if not ok then
      return nil, "Failed unzipping rock "..rock_file, errcode
   end
   fs.change_dir(dir_name)
   local rockspec_file = dir_name..".rockspec"
   local rockspec, err = fetch.load_rockspec(rockspec_file)
   if not rockspec then
      return nil, "Failed loading rockspec "..rockspec_file..": "..err
   end
   if kind == "src" then
      if rockspec.source.file then
         local ok, err = fs.unpack_archive(rockspec.source.file)
         if not ok then
            return nil, err
         end
         fs.change_dir(rockspec.source.dir)
         build.apply_patches(rockspec)
         fs.pop_dir()
      end
   end
   return rockspec
end

--- Create a directory and perform the necessary actions so that
-- the sources for the rock and its rockspec are unpacked inside it,
-- laid out properly so that the 'make' command is able to build the module.
-- @param file string: A rockspec or .rock URL.
-- @return boolean or (nil, string): true if successful or nil followed
-- by an error message.
local function run_unpacker(file)
   assert(type(file) == "string")
   
   local base_name = dir.base_name(file)
   local dir_name, kind, extension = base_name:match("(.*)%.([^.]+)%.(rock)$")
   if not extension then
      dir_name, extension = base_name:match("(.*)%.(rockspec)$")
      kind = "rockspec"
   end
   if not extension then
      return nil, file.." does not seem to be a valid filename."
   end
   
   if (fs.exists(dir_name)) then
      return nil, "Directory "..dir_name.." already exists."
   end
   fs.make_dir(dir_name)
   local rollback = util.schedule_function(fs.delete, fs.absolute_name(dir_name))

   local rockspec, err
   if extension == "rock" then
      rockspec, err = unpack_rock(file, dir_name, kind)
   elseif extension == "rockspec" then
      rockspec, err = unpack_rockspec(file, dir_name)
   end
   if not rockspec then
      return nil, err
   end
   if kind == "src" or kind == "rockspec" then
      if rockspec.source.dir ~= "." then
         local ok = fs.copy(rockspec.local_filename, rockspec.source.dir)
         if not ok then
            return nil, "Failed copying unpacked rockspec into unpacked source directory."
         end
      end
      print()   
      print("Done. You may now enter directory ")
      print(dir.path(dir_name, rockspec.source.dir))
      print("and type 'luarocks make' to build.")
   end
   util.remove_scheduled_function(rollback)
   return true
end

--- Driver function for the "unpack" command.
-- @param name string: may be a rock filename, for unpacking a 
-- rock file or the name of a rock to be fetched and unpacked.
-- @param version string or nil: if the name of a package is given, a
-- version may also be passed.
-- @return boolean or (nil, string): true if successful or nil followed
-- by an error message.
function run(...)
   local flags, name, version = util.parse_flags(...)

   assert(type(version) == "string" or not version)
   if type(name) ~= "string" then
      return nil, "Argument missing, see help."
   end

   if name:match(".*%.rock") or name:match(".*%.rockspec") then
      return run_unpacker(name)
   else
      local search = require("luarocks.search")
      return search.act_on_src_or_rockspec(run_unpacker, name, version)
   end
end
