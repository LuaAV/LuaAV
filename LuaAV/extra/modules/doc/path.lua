local pairs = pairs
local ipairs = ipairs
local setmetatable = setmetatable
local getmetatable = getmetatable

local doc_util = require("doc.util")

--- Path manipulation functions
-- Paths are sequences of nested table fields such as module.submodule.afunction.
-- doc.path encapsulates the construction and processing of paths.  Individual 
-- locations within a path care called 'nodes'.
-- @name doc.path
module(...)

--- Create a node within a path
-- @param parent The parent node
-- @param name The node name
-- @ret The node
function node(parent, name)
	local meta = {
		__path = (get_path(parent) and get_path(parent).."." or "")..name,
		__parent = parent,
	}
	meta.__index = meta
	
	return setmetatable({}, meta)
end

-- Get the path of a node
-- @param node The node
-- @ret The path
function get_path(node)
	local meta = getmetatable(node)
	if(meta) then
		return meta.__path
	end
end

-- Get the parent of a node
-- @param node The node
-- @ret The parent
function get_parent(node)
	local meta = getmetatable(node)
	if(meta) then
		return meta.__parent
	end
end

--- Checks if a given subpath is a subnode of a path
-- @param subpath The subpath
-- @param path The path
-- @ret True/False
function is_subnode(subpath, path)
	local chunks1 = doc_util.module_pathchunks(subpath)
	local chunks2 = doc_util.module_pathchunks(path)
	if(#chunks1 > #chunks2) then
		for i=1, #chunks2 do
			if(chunks1[#chunks1-i+1] ~= chunks2[#chunks2-i+1]) then
				return false
			end
		end
		return true
	end
	return false
end

--- Add a subnode to a tree of nodes
-- @param nodes The tree of nodes
-- @param name The path name
-- @param v Arbitrary data to stuff in the new subnode
function add_subnode(nodes, name, v)
	local path = doc_util.module_pathchunks(name)
	local spath = nodes
	for i=#path, 1, -1 do
		local sname = path[i]
		if(spath[sname]) then
			spath = spath[sname]
			
			if(i==1) then
				spath[#spath+1] = v
			end
		else
			spath[sname] = node(spath, sname)
			spath = spath[sname]
			
			if(i==1) then
				spath[#spath+1] = v
			end
		end
	end
end

--- Get the parent node of a named node
-- @param nodes The tree of nodes
-- @param name The path name
-- @ret The parent node
function get_parent_node(nodes, name)
	local path = doc_util.module_pathchunks(name)
	for i=#path, 2, -1 do
		local sname = path[i]
		nodes = nodes[sname]
		if(not nodes) then
			break
		end
	end
	return nodes
end

--- Get the least common path between a path and a subpath
-- Given path 'doc.doclet.' and 'doc.doclet.html, the result will 
-- be 'doclet.html' for example.
-- @param path The path name
-- @param subpath The subpath name
-- @ret The least common path
function least_common_path(path, subpath)
	local chunks1 = doc_util.module_pathchunks(path)
	local chunks2 = doc_util.module_pathchunks(subpath)
	
	if(chunks1[#chunks1] ~= chunks2[#chunks2]) then
		return nil
	else
		local common_idx = 1
		for i=2, #chunks2 do
			if(chunks1[#chunks1-i+1] ~= chunks2[#chunks2-i+1]) then
				local common = chunks2[#chunks2-common_idx+1]
				for j=#chunks2-common_idx, 1, -1 do
					common = common.."."..chunks2[j]
				end
				return common
			else
				common_idx = i
			end
		end
	end
end