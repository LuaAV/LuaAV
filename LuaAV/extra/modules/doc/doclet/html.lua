local cosmo = require("cosmo")
local path = require("doc.path")
local util = require("doc.util")
local doclet = require("doc.doclet")
local format = string.format

local table = table
local string = string

local io = io
local loadfile = loadfile
local assert = assert
local ipairs = ipairs
local pairs = pairs
local print = print
local type = type
local unpack = unpack

--- Generate documentation HTML files
-- @name doc.doclet.html
module(...)

local HTML_MOD_FILE_PATH = "/doc/doclet/html"

local
function filepath_for_name(name)
	return util.path_from_name(name)..".html"
end

local
function module_item_tree(treeitem, mod)
	return cosmo.f(treeitem){
		if_submodule = function()
			cosmo.yield{
				_template = 2,
				name = mod.name,
				link = filepath_for_name(mod.name),
				summary = mod.summary or "",
			}
		end
	}
end

local
function module_tree(treeitem, mods)
	return cosmo.f(treeitem){
		if_submodule = function()
			cosmo.yield{
				_template = 1,
				
				do_modules = function()
					for i, cmod in ipairs(mods or {}) do
						cosmo.yield{
							list_item = module_item_tree(treeitem, cmod)
						}
						
						if(#(cmod.mods or {}) > 0) then
							cosmo.yield{
								list_item = module_tree(treeitem, cmod.mods)
							}
						end
					end
				end
			}
		end
	}
end

local
function generate_frames(DOC, path, mods, options)
	local frames = assert(util.loadfile(HTML_MOD_FILE_PATH.."/frames.cosmo"), "unable to find frames.cosmo")
	local tree = assert(util.loadfile(HTML_MOD_FILE_PATH.."/tree.cosmo"), "unable to find tree.cosmo")
	local treeitem = assert(util.loadfile(HTML_MOD_FILE_PATH.."/treeitem.cosmo"), "unable to find treeitem.cosmo")
	
	table.sort(mods, function(a, b)
		return a.name:lower() < b.name:lower()
	end)
	
	local frames_html = cosmo.f(frames){
		title = "Documentation"
	}
	
	local frames_name = path.."/index.html"
	DOC:log(format("Installing file: %s", frames_name))
	util.write(frames_name, frames_html)

	local css = util.verify_file(options.css, "luadoc.css", HTML_MOD_FILE_PATH)
	local tree_html = cosmo.f(tree){
		title = options.title or "Documentation",
		docroot = "",
		css = css,
		
		list_item = module_tree(treeitem, mods),
	}
	local tree_name = path.."/tree.html"
	util.write(tree_name, tree_html)
end

local
function do_breadcrumbs(docroot, mod, options)
	local chunks = util.module_pathchunks(mod.name)
	
	return
	function()
		if(options.index_page) then
			if(options.frames) then
				cosmo.yield{
					link = docroot.."home.html",
					name = "Home",
				}
			else
				cosmo.yield{
					link = docroot.."index.html",
					name = "Home",
				}
			end
		end
	
		if(not mod.is_index) then
			local link = docroot
			for i=#chunks, 1, -1 do
				local name = chunks[i]
				
				link = link..name
				cosmo.yield{
					link = link..".html",
					name = name,
				}
				link = link.."/"
			end
		end
	end
end

local
function handle_see(DOC, docroot, mod, fname, see)
	local prefix, name = see:match("(.-)[%.:]([%w_]+)$")
	
	local anchor = doclet.find_anchor(mod, see)
	
	if(not anchor) then
		if(mod.parent) then
			local ref, fmod = doclet.find_external_reference(mod, see)
			if(fmod) then

				local link = docroot
				for lvl in fmod.name:gmatch("([^%.]+)%.") do
					link = link..lvl.."/"
				end
				link = link .. fmod.name:match("%.?([^%.]+)$")..".html"
				
				if(ref) then
					link = link.."#"..ref
				end

				cosmo.yield{
					link = link,
					name = see,
				}
			else
				DOC:warn(format("Warning: can't find external reference for %s with function %s", see, fname))
			end
		else
			DOC:warn(format("Warning: no reference for @see %s found with function %s", see, fname))
		end
	else
		cosmo.yield{
			link = "#"..anchor,
			name = anchor,
		}
	end
end


--- Generate the documentation for a particular module
-- @param DOC The documentation logger
-- @param mod The module data structure
-- @param docroot The documentation root path
-- @param options The documentation options
-- @ret The HTML documentation code
function genmodule(DOC, mod, docroot, options)
	options = options or {}
	
	local page = assert(util.loadfile(HTML_MOD_FILE_PATH.."/page.cosmo"), "unable to find page.cosmo")
	local header = assert(util.loadfile(HTML_MOD_FILE_PATH.."/header.cosmo"), "unable to find header.cosmo")
	local content = assert(util.loadfile(HTML_MOD_FILE_PATH.."/content.cosmo"), "unable to find content.cosmo")
	local collection = assert(util.loadfile(HTML_MOD_FILE_PATH.."/collection.cosmo"), "unable to find collection.cosmo")
	local module = assert(util.loadfile(HTML_MOD_FILE_PATH.."/module.cosmo"), "unable to find module.cosmo")
	
	mod.mods = mod.mods or {}
	mod.functions = mod.functions or {}
	
	local html_data = {
		title = options.title or "Documentation",
		base = docroot,
		docroot = docroot,
		
		css = util.verify_file(options.css, "luadoc.css", HTML_MOD_FILE_PATH),
		image = util.verify_file(options.image, "lua.png", HTML_MOD_FILE_PATH),
		name = mod.name,
		description = mod.description or "",
		do_breadcrumbs = do_breadcrumbs(docroot, mod, options),
		
		if_has_see = cosmo.cond(mod.see ~= nil, {
			do_see = function()
			if(type(mod.see) ~= "table") then
				handle_see(DOC, docroot, mod, mod.name, mod.see)
			else
				for _, see in ipairs(mod.see) do
					handle_see(DOC, docroot, mod, mod.name, see)
				end
			end
		end,
		}),
		
		if_has_modules = cosmo.cond(#mod.mods > 0, {

			do_modules = function()
				for i, cmod in ipairs(mod.mods or {}) do
					local lpath = path.least_common_path(mod.name, cmod.name)
					lpath = lpath or cmod.name
					
					cosmo.yield{
						name = cmod.name,
						link = filepath_for_name(lpath),
						summary = cmod.summary or "",
					}
				end
			end,
		}),
		
		if_has_functions = cosmo.cond(#mod.functions > 0, {
		
			do_functions = function()
				local functions = mod.functions
				for i, name in ipairs(functions) do
					local f = functions[name]
					f["if"] = function(LuaMethod)
						if(	LuaMethod[1] == "GET" or 
							LuaMethod[1] == "SET" or 
							LuaMethod[1] == "GETSET")
						then
							cosmo.yield{_template=1}
						else
							cosmo.yield{_template=2}
						end
					end
					
					-- show parameter listing if we have any
					f.if_has_params = cosmo.cond(#f.param > 0, {
						do_params = function()
						for i, name in ipairs(f.param) do
							--[[
							if(name:match("[%w_]+(%.)[%w_]+")) then
							
							else
								
							end
							--]]
							cosmo.yield{
								name = name,
								description = f.param[name]
							}
						end
					end,
					})
					
					f.if_has_see = cosmo.cond(f.see ~= nil, {
						do_see = function()
						if(type(f.see) ~= "table") then
							handle_see(DOC, docroot, mod, f.name, f.see)
						else
							for _, see in ipairs(f.see) do
								handle_see(DOC, docroot, mod, f.name, see)
							end
						end
					end,
					})
					
					f.if_has_retval = cosmo.cond(#f.ret > 0, {
						do_retval = function()
						local ret = f.ret
						if(type(ret) == "string") then
							cosmo.yield{
								description = ret
							}
						elseif(type(ret) == "table") then
							for i, r in ipairs(ret) do
								cosmo.yield{
									description = r
								}
							end
						end
					end,
					})
					
					f.do_params = function()
						local param = f.param
						for i, name in ipairs(param) do
							if(name:match("[%w_]+(%.)[%w_]+")) then
							
							elseif(name:match("([%w_]+)(%[[%d]+%])")) then
							
							else
								cosmo.yield{
									name = name,
									description = param[name],
									_template = (i == 1) and 1 or 2
								}
							end
						end
					end
					
					cosmo.yield(f)
				end
			end,
		})
	}
	
	html_data.content = cosmo.f(collection..module)(html_data)
	html_data.body = cosmo.f(header..content)(html_data)
	return cosmo.f(page)(html_data)
end

--- Generate and install module documentation
-- @param DOC The documentation logger
-- @param path The documentation installation folder
-- @param mod The module to document
-- @param options The documentation options
function install_module(DOC, path, mod, options)
	local relpath = filepath_for_name(mod.name)
	if(mod.is_index) then
		if(options.frames) then
			relpath = "home.html"
		else
			relpath = "index.html"
		end
	end
	
	-- generate paths
	local ndirs = #(util.pathchunks(relpath))-1
	local index_root = string.rep("../", ndirs)
	local install_path = path.."/"..relpath
	local indexroot = string.rep("../", ndirs)
	
	DOC:log(format("Installing HTML file: %s", install_path))
	util.write(
		install_path, 
		genmodule(DOC, mod, indexroot, options)
	)
end

--- Install the supporting documentation files
-- Installs the HTML CSS and logo image
-- @param DOC The documentation logger
-- @param path Installation path
-- @param documentation options
function install(DOC, path, mods, options)
	local css, css_path = util.verify_file(options.css, "luadoc.css", HTML_MOD_FILE_PATH)
	local image, image_path = util.verify_file(options.image, "lua.png", HTML_MOD_FILE_PATH)
	
	DOC:log(format("Installation path: %s", path))
	
	DOC:log(format("Installing CSS file: %s", css))
	io.popen(format("cp %s %s", css_path, path))
	
	util.ensurepath(path.."/images")
	DOC:log(format("Installing image file: %s", image))
	io.popen(format("cp %s %s/images", image_path, path))
	
	if(options.frames) then
		generate_frames(DOC, path, mods, options)
	end
end