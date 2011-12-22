local pairs = pairs
local ipairs = ipairs
local table = table
local string = string
local format = string.format


module(...)

function tree2json(tree, depth)
	depth = depth or 0
	local code = {}
	if(depth == 0) then
		code[1] = "["
	end
	
	for i, node in ipairs(tree) do
		if(node.children) then
			local offset = string.rep("  ", depth)
			code[#code+1] = format([[%s[ "%s", "%s", [
%s
%s] ] ]], offset, node.title, node.filename, tree2json(node.children, depth+1), offset)
		else
			code[#code+1] = format([[%s[ "%s", "%s", null ] ]], string.rep("  ", depth), node.title, node.filename)
		end
		if(i < #tree) then
			code[#code] = code[#code]..","
		end
	end
	
	if(depth == 0) then
		code[#code+1] = "];"
	end
	
	return table.concat(code, "\n")
end

function pagelist2tree(pages)
	local pagenodes = {}
	for i, page in ipairs(pages) do
		local pagenode = {
			title = page.title,
			filename = page.filename,
		}
		if(page.generate_children) then
			pagenode.children = page.generate_children()
		end
		pagenodes[#pagenodes+1] = pagenode
	end
	return pagenodes
end

function pages2tree(pages)
	local tree = {{
		title = "LuaAV", 
		filename = "index.html", 
		children = {},
	}}
	-- tree[1] is the rootnode
	local pageroot = tree[1].children
	for i, page in ipairs(pages) do
		local pagenode = {
			title = page.title,
			filename = page.filename,
		}
		if(page.generate_children) then
			pagenode.children = page.generate_children()
		end
		pageroot[#pageroot+1] = pagenode
	end
	return tree
end

function module2tree(mod)
	local node = {
		title = mod.name,
		filename = mod.name..".html",
	}
	if(mod.mods and #mod.mods >= 1) then
		node.children = {}
		for i, submod in ipairs(mod.mods) do
			node.children[#node.children+1] = module2tree(submod)
		end
	end
	return node
end

function modules2tree(mods)
	local treemods = {}
	table.sort(mods, function(a, b)
		return a.name < b.name
	end)
	for i, mod in ipairs(mods) do
		treemods[#treemods+1] = module2tree(mod)
	end
	return treemods
end