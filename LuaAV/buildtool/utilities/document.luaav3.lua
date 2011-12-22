LuaAV.addmodulepath(script.path.."/doc.modules")
----------------------------------------------

-- global
docroot = script.path.."/docs"

----------------------------------------------
local apputil = require("app.utilities")
local printt = apputil.printt
local utils = require("utilities")

local navtree = require("navtree")
local search = require("search")
local Chunk = require("codegen.chunk")
local format = string.format

local substitute = utils.substitute
local nbsp = utils.nbsp
local write_file = utils.write_file
local ensurepath = apputil.ensurepath
----------------------------------------------

local
function removepath(path)
	os.execute(format("rm -rf %s", path))
end

local
function copy(src, dst)
	os.execute(format("cp -r %s %s", src, dst))
end

removepath(docroot)
copy(script.path.."/doc.files", docroot)
ensurepath(docroot)	-- redundant

-- load in the data
local templates = require("templates")
local pages = require("pages")
local modules = require("modules")


-- main pages create the tabs
local tabs = {}
for i, mainpage in ipairs(pages.mainpages) do
	tabs[i] = {
		filename = mainpage.filename,
		name = nbsp(mainpage.tabtitle),
	}
end

-- set the navigation tree generating functions
Page.page_by_title(pages.mainpages, "LuaAV Reference").generate_children = function()
	table.sort(modules.mods, function(a, b)
		return a.name < b.name
	end)
	return navtree.modules2tree(modules.mods)
end

Page.page_by_title(pages.mainpages, "LuaAV Manual").generate_children = function()
	return navtree.pagelist2tree(pages.tutorials)
end

local
function generate_manual()
	local text = ""
	for i, page in ipairs(pages.tutorials) do
		text = text..substitute([[
	<li><a href="$link"><h3>$title</h3></li>
]], {
		title = page.content.title,
		link = docroot.."/"..page.filename,
	})
	end
	return format("<ul>\n%s\n</ul>", text)
end

Page.page_by_title(pages.mainpages, "LuaAV Manual").content.text = generate_manual()

function attribute_signature(f)
	return format([[<a href="#%s">%s</a>&nbsp;%s]], 
		f.hash,
		f.name, f.LuaMethod
	)
end

function detail_attribute_signature(f, detail)
	detail.item.header.signature:write(substitute(templates.attribute_signature, f))
end


function function_signature(f)
	return format([[<a href="#%s">%s</a>(%s)]],
		f.hash,
		f.function_statement.name or f.name, 
		table.concat(f.function_statement.params, ", ")
	)
end

function detail_function_signature(f, detail)
	if(#f.param <= 1) then
		local signature = Chunk(substitute(templates.single_parameter_signature, f))
		if(#f.param == 1) then
			signature.firstparameter:writes(templates.parameter_name_no_comma, {name = f.param[1]})
		end
		detail.item.header.signature:write(signature:synthesize())
	else
		local signature = Chunk(substitute(templates.multi_parameter_signature, f))
		signature.firstparameter:writes(templates.parameter_name_comma, {name = f.param[1]})
		for i=2, #f.param do
			local name_template
			if(i < #f.param) then
				name_template = templates.parameter_name_comma
			else
				name_template = templates.parameter_name_no_comma
			end
			local param = substitute(templates.multi_parameter_signature_parameter_nth, {
				param = substitute(name_template, {name = f.param[i]})
			})
			signature.parameters:write(param)
		end
		detail.item.header.signature:write(signature:synthesize())
	end
end

function detail_function_parameters(f, detail)
	if(#f.param >= 1) then
		local body_parameters = Chunk(templates.parameters)
		for i, pname in ipairs(f.param) do
			body_parameters.parameters:writes(templates.parameter, {
				name  = pname,
				description = f.param[pname],
			})
		end
		detail.item.body.parameters:write(body_parameters:synthesize())
	end
end

function detail_function_return(f, detail)
	if(f["return"]) then
		local body_returns = Chunk(templates.return_values)
		body_returns.return_values:writes(templates.return_value, {
			description = f["return"]
		})
		detail.item.body.return_values:write(body_returns:synthesize())
	end
end

function mod_reference(mod, sections)
	local reference = Chunk(substitute(templates.reference_pane, {
		description = mod.description or mod.summary
	}))
	for i, sname in ipairs(sections) do
		local sitems = sections[sname]
		if(#sitems >= 1) then
			-- generate the section overview and detail
			local soverview = Chunk(substitute(templates.section_overview, {
				link = sname,
				title = sname,
			}))
			local sdetail = Chunk(substitute(templates.section_detail, {
				title = sname,
			}))
			for j, item in ipairs(sitems) do
				soverview.items:write(item.overview)
				sdetail.items:write(item.detail)
			end
			
			-- write the section overview and detail to the reference page
			reference.overview.sections:write(soverview:synthesize())
			reference.details.sections:write(sdetail:synthesize())
		end
	end
	return reference:synthesize()
end

function document_module(mod)
	mod.filename = mod.name..".html"
	local init = {
		title = mod.name,
		filename = mod.filename,
	}
	local content = {}
	init.content = content
	
	local sections = {
		[1] = "Attributes",
		[2] = "Functions",
		Attributes = {},
		Functions = {},
	}
	
	if(mod.functions and #mod.functions >= 1) then
		table.sort(mod.functions)

		for i, fname in ipairs(mod.functions) do
			local f = mod.functions[fname]
			f.hash = f.name
			if(f.LuaMethod and f.LuaMethod ~= "METHOD") then
				local overview = substitute(templates.section_overview_item, {
					signature = attribute_signature(f),
					description = f.summary
				})
				
				local detail = Chunk(substitute(templates.item_detail, {
					hash = f.hash,
					text = f.description,
				}))
				
				detail_attribute_signature(f, detail)
				
				sections.Attributes[#sections.Attributes+1] = {
					overview = overview,
					detail = detail:synthesize(),
				}
				
			else
				local overview = substitute(templates.section_overview_item, {
					signature = function_signature(f),
					description = f.summary
				})
				local detail = Chunk(substitute(templates.item_detail, {
					hash = f.hash,
					text = f.description,
				}))
				-- write the function signature to the header
				detail_function_signature(f, detail)
				
				-- write the parameter descriptions to the body
				detail_function_parameters(f, detail)
				
				-- write the return values to the body
				detail_function_return(f, detail)
				
				-- remarks
				-- see also
				
				sections.Functions[#sections.Functions+1] = {
					overview = overview,
					detail = detail:synthesize(),
				}
			end
		end	
	end
	
	content.text = mod_reference(mod, sections)
	content.title = format([[<a href="#details">%s</a>]], mod.name)
	--content.title = mod.name
	content.summary = mod.summary:gsub(" ", "&nbsp;")
	
	local page = Page(init)
	page:tabs(tabs, templates.active_tab_template, templates.tab_template)
	write_file(docroot.."/"..page.filename, page:generate())
end

function document_modules(mods)
	for i, mod in ipairs(mods) do
		document_module(mod)
		if(mod.mods and #mod.mods >= 1) then
			document_modules(mod.mods)
		end
	end
end
document_modules(modules.mods)


-- generate the main pages
for i, page in ipairs(pages.mainpages) do
	local filename = docroot.."/"..page.filename
	page:tabs(tabs, templates.active_tab_template, templates.tab_template)
	write_file(filename, utils.highlight_luacode(page:generate()))
end

-- generate tutorial files
for i, page in ipairs(pages.tutorials) do
	local filename = docroot.."/"..page.filename
	page:tabs(tabs, templates.active_tab_template, templates.tab_template)
	write_file(filename, utils.highlight_luacode(page:generate()))
end

-- generate the navigation tree JavaScript
write_file(docroot.."/navtree.js", substitute(templates.navtree_template, {
	navtree = navtree.tree2json(navtree.pages2tree(pages.mainpages))
}))

local docsearch = search()

function generate_search_module(mod)
	docsearch:add(mod.name, mod.filename)
	if(mod.functions and #mod.functions >= 1) then
		for i, fname in ipairs(mod.functions) do
			local f = mod.functions[fname]
			-- should link to function hash
			docsearch:add(f.name, mod.filename.."#"..f.hash)
		end
	end
end

function generate_search_modules(mods)
	for i, mod in ipairs(mods) do
		generate_search_module(mod)
		if(mod.mods and #mod.mods >= 1) then
			generate_search_modules(mod.mods)
		end
	end
end
generate_search_modules(modules.mods)
--printt(docsearch)
docsearch:generate()
