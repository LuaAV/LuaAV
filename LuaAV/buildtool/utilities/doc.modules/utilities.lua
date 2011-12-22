local assert = assert
local io = io
local print = print
local format = string.format
local table = table
local ipairs = ipairs
local unpack = unpack
local string = string

local lexer = require("doc.lexer")
local apputils = require("app.utilities")
local printt = apputils.printt
local stripfilename = apputils.stripfilename
local ensurepath = apputils.ensurepath
local getextension = apputils.getextension

local m = require("lpeg")
local P, V, R, S, C = m.P, m.V, m.R, m.S, m.C
local Cmt = m.Cmt
local Cs = m.Cs

module(...)

function substitute(str, dict)
	-- letter or _ followed by alphanumeric or _
	local newstr = str:gsub("%$([%a_][%w_]*)[{}]*", dict)
	return newstr
end

function nbsp(str)
	local res = str:gsub(" ", "&#160;")
	return res
end

function write_file(filename, text)
	ensurepath(stripfilename(filename))
	local f = assert(io.open(filename, "w"))
	f:write(text)
	f:close()
end

function read_file(filename)
	local f = assert(io.open(filename))
	local text = f:read("*a")
	f:close()
	return text
end

local filetypes = {
	js = true,
	css = true,
	png = true,
}

local
function isfile(ext)
	if(ext) then
		return filetypes[ext]
	else
		return false
	end
end

function relative_path(filename, dest)
	local path = stripfilename(filename)
	if(path) then
		-- assume filename includes dest, include trailing '/'
		local rel = path:sub(dest:len()+2)
		return rel:gsub("[^/]+/?", "../")
	else
		return ""
	end
end

function relativize_links(filename, dest, text)
	local rel = relative_path(filename, dest)
	text = text:gsub([[src="[^"]+"]], function(s)
		local fname = s:match([["([^"]+)"]])
		if(isfile(getextension(fname))) then
			return format([[src="%s%s"]], rel, fname)
		else
			return s
		end
	end)
	
	text = text:gsub([[href="[^"]+"]], function(s)
		local fname = s:match([["([^"]+)"]])
		if(fname:sub(1, 4) ~= "http") then
			if(isfile(getextension(fname))) then
				return format([[src="%s%s"]], rel, fname)
			end
		end
		return s
	end)
	
	return text
end

local ntabspaces = 4
local
function lua_syntax_highlight(src)
	-- convert quotes
	local src = src:gsub("&quot;", '"')
	src = src:gsub("\t", string.rep(" ", ntabspaces))
	
	local lexed = lexer(src)
	local str = {}
	for i, v in ipairs(lexed) do
		if(v[1] == "whitespace") then
			table.insert(str, v[2])
		else
			table.insert(str, string.format([[<span class="lua-%s">%s</span>]], unpack(v)))
		end
	end
	return table.concat(str)
end

local highlight_start = P"<luacode" * C((1-P">")^0) * P">"
local highlight_end = P"</luacode>"

local highlight_patt = Cmt(highlight_start *Cmt(
		(1-highlight_end)^0, function(s, i, t)
			return i, lua_syntax_highlight(t)
		end
	)* highlight_end, function(s, i, tag, t)
		if(tag and tag:find("class", nil, true)) then
			return i, format([[<pre%s>%s</pre>]], tag, t)
		else
			return i, format([[<pre class="fragment">%s</pre>]], t)
		end
	end)
	highlight_patt = Cs((highlight_patt + 1)^0)


function highlight_luacode(html)
	return highlight_patt:match(html)
end

