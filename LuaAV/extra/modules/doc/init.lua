local assert = assert
local print = print
local ipairs = ipairs
local pairs = pairs
local tostring = tostring
local require = require
local type = type
local setmetatable = setmetatable
local unpack = unpack

local floor = math.floor
local format = string.format

local string = string
local io = io
local table = table
local doc_util = require("doc.util")
local doc_html = require("doc.doclet.html")
local path = require("doc.path")

--- Documentation generator for Lua
-- Documentation is generated from source files.  These can 
-- be Lua files, C/C++ source files, or whatever is supported 
-- by the doc.lang modules
-- @name doc
module(...)

local language_extensions = {
	h = "doc.lang.c",
	hh = "doc.lang.c",
	c = "doc.lang.c",
	cpp = "doc.lang.c",
	cxx = "doc.lang.c",
	lua = "doc.lang.lua",
}

local
function process_module_file(filename, lang)
	local code = assert(io.open(filename), filename .. " not found"):read("*a")
	return lang.modules(code)
end

local
function language_for_file(file)
	local ext = doc_util.getextension(file)
	return require(language_extensions[ext] or "doc.lang"..ext)
end

local
function generate_references(mod)
	local refs = {[mod.name] = true}
	for _, fname in ipairs(mod.functions or {}) do
		refs[fname] = true
	end
	for _, submod in ipairs(mod.mods or {}) do
		refs[submod.name] = true
	end
	mod.refs = refs
end

local
function generate_missing_sub_mods(mod, paths, subpaths)
	subpaths = subpaths or paths
	
	for k, v in pairs(paths) do
		if(type(k) ~= "number") then
			if(
				#v == 0 and 
				not path.is_subnode(mod.name, path.get_path(v))
			) then
	
				local nmod = {
					name = path.get_path(v),
					mods = {},
					parent = mod
				}
				path.add_subnode(subpaths, nmod.name, nmod)
				
				mod.mods[#mod.mods+1] = nmod
			end
			generate_missing_sub_mods(mod, v, subpaths)
		end
	end
end

local
function add_submodule(t, subpaths, mod)
	path.add_subnode(subpaths, mod.name, mod)
	generate_references(mod)
	mod.parent = t
	mod.mods = mod.mods or {}
	return mod
end

--- Document a module
-- Generates a data structure from documentation source files 
-- for a module. The data structure can then be used to produce 
-- documentation output such as HTML
-- @param t Module data
-- @param t.name Module name (required)
-- @param t.path Module path on file system (required if file or files is given)
-- @param t.file Source file for module itself (optional)
-- @param t.files Source file array for sub-modules (optional)
-- @ret Module data structure
function module(t)
	assert(t.name)
	if(t.file or t.files) then
		assert(t.path)
	end
	
	t.mods = t.mods or {}
	t.mods.name = t.name
	
	local subpaths = {}
	for i, mod in ipairs(t.mods) do
		add_submodule(t, subpaths, mod)
	end
	
	if(t.files) then
		for i, f in ipairs(t.files) do
			local lang = language_for_file(f)
			local mods = process_module_file(t.path.."/"..f, lang)
			for i, mod in ipairs(mods) do
				t.mods[#t.mods+1] = add_submodule(t, subpaths, mod)
			end
		end
	end
	
	if(t.file) then
		local lang = language_for_file(t.file)
		local mods = process_module_file(t.path.."/"..t.file, lang)
		for i, mod in ipairs(mods) do
			if(mod.name == t.name) then
				for k, v in pairs(mod) do
					t[k] = v
				end
			else
				t.mods[#t.mods+1] = add_submodule(t, subpaths, mod)
			end
		end
	end
	
	path.add_subnode(subpaths, t.name, t)
	generate_missing_sub_mods(t, subpaths)
	
	-- reorder nodes to belong to their immediate parent node
	for i=#t.mods, 1, -1 do
		local mod = t.mods[i]
		local pnode = assert(path.get_parent_node(subpaths, mod.name))
		local pmod = assert(pnode[1])

		if(pmod ~= mod.parent) then
			pmod.mods[#pmod.mods+1] = mod
			mod.parent = pmod
			
			table.remove(t.mods, i)
		end
	end
	
	generate_references(t)
	return t
end

local default_options = {
	alphabetize = true,
	css = "luadoc.css",
	image = "lua.png",
	index_page = true,
	verbose = false,
	frames = false,
}

local DOC = {
	log = function(self, msg)
		if(self.stdout) then
			print(msg)
		end
	end,
	
	warn = function(self, msg)
		print(msg)
	end
}

local
function do_install_module_files(DOC, docroot, mod, options)
	DOC:log(format("Generating module %s", mod.name))

	-- sort items
	if(options.alphabetize ~= false) then
		table.sort(mod.files or {})
		table.sort(mod.mods or {}, function(a, b)
			return a.name:lower() < b.name:lower()
		end)
		
		local mod_chunks = doc_util.module_pathchunks(mod.name)
		local fpatt = mod_chunks[1]..".([%w_]+)"
		table.sort(mod.functions or {}, function(a, b)
			local fname_a = a:match(fpatt)
			local fname_b = b:match(fpatt)
			if(not fname_a and not fname_b) then
				return a:lower() < b:lower()
			elseif(not fname_a) then
				return true
			elseif(not fname_b) then
				return false
			else
				return fname_a:lower() < fname_b:lower()
			end
		end)
	end

	-- install module
	doc_html.install_module(DOC, docroot, mod, options)
	
	-- install sub-modules
	for i, mod in ipairs(mod.mods) do
		do_install_module_files(DOC, docroot, mod, options)
	end
end

--- Install module documentation
-- @param docroot The root folder of the documentation
-- @param mods The array of modules to document
-- @param options A table of options
-- @param options.index_page Flag for generating an index file
-- @param options.css Location of CSS file to use
-- @param options.image The logo image file location
-- @param options.alphabetize Whether to alphabetize the documentation
-- @param options.verbose Verbose flag
function install_modules(docroot, mods, options)
	options = options or {}
	for k, v in pairs(default_options) do
		if(options[k] == nil) then
			options[k] = v
		end
	end
	
	-- with frames, an index page will be generated
	if(options.frames) then
		options.index_page = true
	end
	
	
	-- setup lua doc data structure
	DOC.stdout = options.verbose
	
	DOC:log("LuaDoc Options: ")
	local list = {}
	for k, v in pairs(options) do
		list[#list+1] = k
	end
	table.sort(list)
	for i, k in ipairs(list) do
		DOC:log(format("... %s = %s", tostring(k), tostring(options[k])))
	end
	DOC:log("")
	
	-- build the index module if requested
	if(options.index_page) then
		local index_mod = module{
			name = "Home",
			path = "",
			is_index = true,
		}
		for i, mod in ipairs(mods) do
			index_mod.mods[i] = mod
			mod.parent = index_mod
		end
		do_install_module_files(DOC, docroot, index_mod, options)
	else
		for i, mod in ipairs(mods) do
			do_install_module_files(DOC, docroot, mod, options)
		end
	end	
	DOC:log("")
	doc_html.install(DOC, docroot, mods, options)
end