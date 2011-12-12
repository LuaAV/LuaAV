local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local loadfile = loadfile
local setfenv = setfenv
local package = package

local print = print
local setmetatable = setmetatable

local string = require 'string'
local table = require 'table'
local io = require 'io'
local os = require 'os'
local lfs = require 'lfs'
local utils = require('app.utilities')
local serializer = require('app.serializer')
local configpath = app.configpath

local writefile = utils.writefile
local findfileinpath = utils.findfileinpath

local max_recent_files = 100

module (...)
----------------------------------------------------------------------------
-- app.configuration
----------------------------------------------------------------------------

-- filename of configuration file
local config_filename = "config.lua"

-- default configuration
local
DefaultConfig = {
	recent_files = {
	},
}

-- setup anenvironment for loading files into the master state
local loadenv = {
	__index = _G,
}
setmetatable(loadenv, loadenv)


--- Generates a file description
--[[ @param t	- a file description table with
					the following format:
					
FileDecription{
	name,			-- name of the file (fully qualified path)
	last_opened,	-- os.time() call from the last time LuaAV opened the file
	path,			-- array of folder names making up the file path with the file name at index 1
}

--]]
function make_filedescription(t)
	local t = t or {}
	t.name = t.name or ""
	t.last_opened = t.last_opened or os.time()
	
	-- chunk it
	t.path = utils.pathchunks(t.name)
	
	return t
end

-- sort from most recent time (buffer number) to most distant
function sort_filedescriptions(a, b)
	return a.last_opened > b.last_opened
end

function load_configfile()
	-- save working dir
	local olddir = lfs.currentdir()

	-- change working dir to config dir
	local config = configpath
	local success, err = lfs.chdir(config)
	
	-- if fail, create it
	if(not success) then
		success, err = lfs.mkdir(config)
		if(not success) then
			error(string.format("%s couldn\'t make config directory: %s", err, config))
		end
		success, err = lfs.chdir(config)
		if(not success) then
			error(string.format("%s couldn\'t change working directory to: %s", err, config))
		end
	end

	-- check if config file already exists
	local config_file = findfileinpath(config, config_filename, false)
	
	-- if so, read it ... if not, make one
	if(config_file) then
		-- read it
		local f, err = loadfile(config_file)
		if(not f) then
			error(string.format("loading config file %s:\n %s", config_file, err))
		else
			-- run f in protected environment
			setfenv(f, loadenv)
			f()
			
			-- save loaded configuration to the global configuration
			Config = loadenv.Config
		end
	end
	
	if(not Config) then
		config_file =  config .. "/" .. config_filename
		writefile(config_file, serializer.serialize("Config", DefaultConfig))
		Config = DefaultConfig
	end
	init_config(Config)

	-- restore working dir
	lfs.chdir(olddir)

	return Config
end

function init_config(Config)
	pathtree = {}
	for i, fd in ipairs(Config.recent_files) do
		fd.path = utils.pathchunks(fd.name)
	end
end

function save_configfile(Config)
	-- get the path to the config file
	local config = configpath
	local config_file = findfileinpath(config, config_filename, false)
	
	-- if it doesn't exist, create one
	if(not config_file) then
		config_file =  config .. "/" .. config_filename
		createfile(config_file)
	end
	
	-- write out the current config
	Config = Config or DefaultConfig
	
	-- clip max recent files to limit
	for i=max_recent_files, #Config.recent_files do
		Config.recent_files[i] = nil
	end
	
	writefile(config_file,  serializer.serialize("Config", Config))
end

function find_recent_by_name(Config, file)
	Config = Config or DefaultConfig
	for i, f in ipairs(Config.recent_files) do
		if(f.name == file) then
			return f
		end
	end
end

function add_recentfile(Config, file)
	Config = Config or DefaultConfig
	local f = find_recent_by_name(Config, file)
	if(f) then
		-- update the last opened time
		f.last_opened = os.time()
	else
		f = make_filedescription{
			name = file,
			last_opened = os.time(),
		}
		
		-- add the file to the list
		Config.recent_files[#Config.recent_files+1] = f
	end
	
	-- sort the files by time
	table.sort(Config.recent_files, sort_filedescriptions)
	
	return f
end
