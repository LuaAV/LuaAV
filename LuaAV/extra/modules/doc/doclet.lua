local ipairs = ipairs
local pairs = pairs
local print = print

local util = require("doc.util")

--- Common functionality for documentation generation backends
-- @name doc.doclet
module(...)

--- Checks if a path is within a module
-- @param mod The module
-- @param path The path
-- @ret True/False
function within_module(mod, path)
	if(mod.name == path) then
		return true
	else
		local mod_chunks = util.module_pathchunks(mod.name)
		local path_chunks = util.module_pathchunks(path)
		
		local within = true
		for i, lvl in ipairs(path_chunks) do
			if(not mod_chunks[i] or mod_chunks[i] ~= lvl) then
				within = false
				break
			end
		end
		
		return within
	end
end

--- Find an anchor within a module
-- @param mod The module
-- @param name The anchor name
function find_anchor(mod, name)
	for ref in pairs(mod.refs) do
		if(
			name == ref or 
			name == ref:match(".-[%.:]([%w_]+)$")
		) then
			return ref
		end
	end
end

local
function do_find_external_reference(mod, name, processed)
	processed = processed or {}
	processed[mod] = true
	
	local cname = name:gsub("(:)", ".")
	local fcname = cname:match("([%w_]+%.[%w_]+)$") or name

	-- look in this module
	for ref in pairs(mod.refs) do
		local canonical_ref = ref:gsub("(:)", ".")
		local module_item_ref = canonical_ref:match("%.?([%w_]+)$")
		local fully_qualified_module_item = mod.name.."."..module_item_ref
		local qualified_module_item_ref = fully_qualified_module_item:match("([%w_]+%.[%w_]+)$")
	
		if(ref == fcname) then
			return ref, mod
		elseif(canonical_ref == fcname) then
			return ref, mod
		elseif(module_item_ref == fcname) then
			return ref, mod
		elseif(fully_qualified_module_item == fcname) then
			return ref, mod
		elseif(qualified_module_item_ref == fcname) then
			return ref, mod
		end
	end
	
	-- look in submodules
	for _, submod in ipairs(mod.mods or {}) do
		if(not processed[submod]) then
			local ref, fmod = do_find_external_reference(submod, name, processed)
			if(ref) then
				return ref, fmod
			end
		end
	end
	
	-- look in parent
	if(mod.parent and not processed[mod.parent]) then
		local ref, fmod = do_find_external_reference(mod.parent, name, processed)
		if(ref) then
			return ref, fmod
		end
	end
end

local
function do_find_external_module_reference(mod, name, processed)
	processed = processed or {}
	processed[mod] = true
	
	if(mod.name == name) then
		return nil, mod
	end
	
	-- look in submodules
	for _, submod in ipairs(mod.mods or {}) do
		if(not processed[submod]) then
			local _, fmod = do_find_external_module_reference(submod, name, processed)
			if(fmod) then
				return nil, fmod
			end
		end
	end
	
	-- look in parent
	if(mod.parent and not processed[mod.parent]) then
		local _, fmod = do_find_external_module_reference(mod.parent, name, processed)
		if(fmod) then
			return nil, fmod
		end
	end
end

--- Find an external reference from a module
-- Valid external references include: a module name <b>(e.g. gui.slider)</b> or
-- a module function <b>(e.g. gui.slider.draw or gui.slider:draw)</b>
-- @param mod The module
-- @param name The reference name
-- @ret The reference
-- @ret The module the reference is in
function find_external_reference(mod, name)
	local ref, fmod = do_find_external_module_reference(mod, name)
	if(fmod) then
		return ref, fmod
	end
	return do_find_external_reference(mod, name)
end
