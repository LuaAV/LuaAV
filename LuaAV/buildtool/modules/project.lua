--[[
	Maintains all of the information and methods necessary to describe 
	and build a project in a cross-platform manner.  The interface presents
	the basic steps for build a prject at a high level:

	-- operators	
		compile()
		link()
		install()
		clean()
	

	project{
		name = name of the project,
		type = project type (application, module, shared, framework, ...),
		path = path to project root folder,
		dependencies = list of dependencies for linking,
		src = list of source files for compilation,
		
		-- optional members
		include_search_paths = list of search paths for compilation,
		linker_search_paths = list of search paths for linking
		cflags = project-wide compilation flags,
		ldflags = project-wide linking flags,
		
		-- list of configurations
		[Debug] = e.g. Debug configuration,
		[Release] = e.g. Release configuration,
	}
	
	
	project is intended to act as a data description format for listing all of the properties (source files,
	dependencies, search paths, etc.) for implementing the high-level interface.
--]]
local assert, pairs, tostring, type = assert, pairs, tostring, type
local ipairs = ipairs
local next = next
local print = print
local setmetatable = setmetatable

local table = require 'table'
local string = require 'string'
local os = require 'os'
local io = require 'io'
local print = print

local platform = require("platform")
local Config = require("config")
local Src = require("src")
local buildutils = require("buildutils")
local makepath = buildutils.makepath
local compilefile = buildutils.compilefile
local linkobjects = buildutils.linkobjects
local ensurepath = buildutils.ensurepath
local copyfile = buildutils.copyfile
local removefile = buildutils.removefile

local lfs = lfs

local OS = _OSNAME

local C = {}
local M = {}

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end


module('project', setconstructor)
----------------------------------------------------------
function C.__call(M, init)
	assert(init.name, "project.name: no name given")
	assert(init.type, "project.type: no type given")
	assert(init.path, "project.path: no path given")
	assert(init.dependencies, "project.dependencies: no dependencies given")
	assert(init.src, "project.src: no source files given")
	
	if(init.type == "shared") then
		init.product_name = "lib" .. (init.product_name or init.name)
	end

	if(init.type == "application") then
		init.buildname = init.product_name or init.name
	else
		init.buildname = string.format("%s%s.%s", platform.prefix[init.type] or "", init.product_name or init.name, 
									platform.extension[init.type])
	end	

	init.include_search_paths = init.include_search_paths or {}
	init.linker_search_paths = init.linker_search_paths or {}
	
	init.cflags = init.cflags or {}
	init.ldflags = init.ldflags or {}
	
	init.Debug = init.Debug or Config{}
	init.Release = init.Release or Config{}
	
	local m = {}
	
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	for k, v in pairs(init) do
		if(not m[k]) then
			m[k] = v
		end
	end
	
	-- resolve all paths
	for i, p in ipairs(m.include_search_paths) do
		p:resolve(m.path[1])
	end
	
	for i, p in ipairs(m.linker_search_paths) do
		p:resolve(m.path[1])
	end
	
	for i, p in ipairs(m.src) do
		p:resolve(m.path[1])
	end
	
	return m
end

function M:make_cflags(config)
	-- project flags
	local cflags = table.concat(self.cflags, " ")
	-- config flags
	cflags = string.format("%s %s", cflags, table.concat(self[config].cflags, " "))
	-- include paths
	for i, inc in ipairs(self.include_search_paths) do
		cflags = string.format("%s -I%s", cflags, inc[1])
	end
	return cflags
end

function M:build_dir(config)
	return makepath(self.path[1], "build", config)
end

function M:make_src(config)
	local files = {}
	local build_dir = self:build_dir(config)
	for i, s in ipairs(self.src) do
		files[i] = Src{
			src = s[1],
			build_dir = build_dir,
		}
	end
	self[config].src = files
end

function M:compile(config)
	if(self.preprocess) then
		print("self.preprocess", self.preprocess(self))
	end

	local build_dir = self:build_dir(config)
	ensurepath(build_dir)

	self:make_src(config)

	local cflags = self:make_cflags(config)
	for i, f in ipairs(self[config].src) do
		compilefile(f.src, f.obj, cflags)
	end
end

function M:make_ldflags(config)
	-- project flags
	local ldflags = table.concat(self.ldflags, " ")
	-- config flags
	ldflags = string.format("%s %s", ldflags, table.concat(self[config].ldflags, " "))
	-- linker paths
	for i, inc in ipairs(self.linker_search_paths) do
		ldflags = string.format("%s -L%s", ldflags, inc[1])
	end
	-- dependencies
	for i, dep in ipairs(self.dependencies) do
		ldflags = string.format("%s %s", ldflags, dep:linker_flag())
	end
	return ldflags
end

function M:link(config)
	if(not self[config].src) then
		self:make_src(config)
	end

	local objs = {}
	local build_dir = self:build_dir(config)
	for i, s in ipairs(self[config].src) do
		objs[i] = s.obj
	end
	local ldflags = self:make_ldflags(config)
	linkobjects(objs, build_dir, self.buildname, ldflags)
	self[config].product = makepath(build_dir, self.buildname)
end

function M:install(config)
	local build_dir = self:build_dir(config)
	self[config].product = self[config].product or makepath(build_dir, self.buildname)
	if(self[config].install_path[1]:sub(1, 4) == "/usr") then
		copyfile(self[config].product, self[config].install_path[1], true)
	else
		copyfile(self[config].product, self[config].install_path[1])
	end
	if(self.on_install) then
		self:on_install(config)
	end
end

function M:clean(config)
	if(not self[config].src) then
		self:make_src(config)
	end

	local build_dir = self:build_dir(config)
	for i, s in ipairs(self[config].src) do
		removefile(s.obj)
	end
	removefile(makepath(build_dir, self.buildname))
end
