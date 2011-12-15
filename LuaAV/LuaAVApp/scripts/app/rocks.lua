local require = require
local unpack = unpack
local pairs = pairs
local ipairs = ipairs
local type = type
local print = print
local tostring = tostring
local error = error

local format = string.format

local apputils = require("app.utilities")
local command_line = require("luarocks.command_line")

local local_tree = app.apppath.."/testrepo"
local LuaAV_rocks_server = "http://lua-av.mat.ucsb.edu/luarocks"

module(...)

-- ensure a local repository exists
--apputils.ensurepath(local_tree)


-- setup luarocks
command_line.program_name = "luarocks"
command_line.program_description = "LuaRocks main command-line interface"

local commands = {}
command_line.commands = commands
commands.help = require("luarocks.help")
commands.pack = require("luarocks.pack")
commands.unpack = require("luarocks.unpack")
commands.build = require("luarocks.build")
commands.install = require("luarocks.install")
commands.search = require("luarocks.search")
commands.list = require("luarocks.list")
commands.remove = require("luarocks.remove")
commands.make = require("luarocks.make")
commands.download = require("luarocks.download")
commands.path = require("luarocks.path")
commands.show = require("luarocks.show")
commands.details = require("luarocks.details")

command_line.add_tree(local_tree)


local
function do_command(args)
	return command_line.run_command(unpack(args))
end

function LuaAV_server()
	return LuaAV_rocks_server
end

local
function optbool(v, def)
	if(type(v) == "nil") then
		return def
	else
		return v
	end
end

function search(src_repo)
	return do_command{
		format("--only-from=%s", src_repo),
		"search",
		"--all"
	}
end

function install(src_repo, name, version)
	print(src_repo, name, version)
	return do_command{
		format("--only-from=%s", src_repo),
		format("--to=%s", local_tree),
		"install",
		name,
		version,
	}
end

function details(src_repo, name, version)
	return do_command{
		format("--only-from=%s", src_repo),
		"details",
		name,
		version,
	}
end
