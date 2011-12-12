local lpeg = require('lpeg')
--patterns
local P, R, S, V = lpeg.P, lpeg.R, lpeg.S, lpeg.V
--captures
local C, Ct, Cc, Cmt, Cg = lpeg.C, lpeg.Ct, lpeg.Cc, lpeg.Cmt, lpeg.Cg

local table = table

local print = print
local ipairs = ipairs
local pairs = pairs
local type = type

module(...)


local white = S(" \t\r\n")
local whites = white^0
local letter = R("az", "AZ")
local digit = R("09")

local markup_tag_char = letter + digit + S(".-_:")
local attribute_key_char = P(1) - S('=\"<>/') - white
local attribute_value_char = P(1) - S('"')
local data_char = P(1) - S("<>")

local attribute_key = V"attribute_key_char"^1
local attribute_value = P'"' * C(V"attribute_value_char"^1) * P'"'
local attribute = Ct(V"attribute_key" * P"=" * V"attribute_value")

local header = P"<?xml" * whites * (V"attribute" * whites)^0 * P"?>"
local markup_tag = (letter + S("_:")) * V"markup_tag_char"^0
local start_element = P"<" * C(V"markup_tag") * whites * (V"attribute" * whites)^0 * P">"
local empty_element = P"<" * C(V"markup_tag") * whites * (V"attribute" * whites)^0 * P"/>"
local end_element = P"</" * V"markup_tag" * P">"
local data = data_char^0
local element = 
	V"start_element" * whites * (
		(V"element" * whites)^1  + 
		V"data" * whites * (V"element" * whites)^0
	) * V"end_element" + 
	V"empty_element"

local document = 
	V"header" * whites * (V"element" * whites)^0

local
function process_header(s, i, t)
	local tt = {}
	for i, attr in pairs(t) do
		tt[i] = attr[1]
		tt[ attr[1] ] = attr[2]
	end
	return i, tt
end

local
function process_elem(s, i, t)
	local tt = {}
	local tag

	for i, attr in pairs(t) do
		if(i==1) then
			tag = attr
		else
			tt[i-1] = attr[1]
			tt[ attr[1] ] = attr[2]
		end
	end
	
	return i, tag, tt
end

local
function process_element(s, i, t)
	local tt = {}
	local tag
	for i, attr in pairs(t) do
		if(type(i) ~= "number") then
			tt[i] = attr
		elseif(i==2) then
			tt.attributes = attr
			tt[2] = nil
		elseif(i>2) then
			tt[i-1] = attr
		else
			tt[i] = attr
		end
	end

	return i, tt
end

local
function process_document(s, i, t)
	local tt = {}
	for i, attr in pairs(t) do
		if(i==1) then
			tt.attributes = attr
		else
			tt[i-1] = attr
		end
	end

	return i, tt
end

local G = {
	[1] = "document",
	
	markup_tag_char = markup_tag_char,
	attribute_key_char = attribute_key_char,
	attribute_value_char = attribute_value_char,
	
	attribute_key = C(attribute_key),
	attribute_value = (attribute_value),
	attribute = (attribute),
	
	header = Cmt(Ct(header), process_header),
	markup_tag = markup_tag,
	start_element = Cmt(Ct(start_element), process_elem),
	empty_element = Cmt(Ct(empty_element), process_elem),
	end_element = end_element,
	
	data = Cg(C(data), "data"),
	
	element = Cmt(Ct(element), process_element),
	
	document = Cmt(Ct(document), process_document),
}

local
function search(patt)
	return (patt + P(1))^0
end

local patt = search(P(G))

function match(data)
	return patt:match(data)
end