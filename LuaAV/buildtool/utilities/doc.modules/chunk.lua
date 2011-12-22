local format = string.format

local util = require("app.utilities")
local printt = util.printt

local lpeg = require("lpeg")
local C, Cc, Cg, Ct, P, R, S, V = lpeg.C, lpeg.Cc, lpeg.Cg, lpeg.Ct, lpeg.P, lpeg.R, lpeg.S, lpeg.V



local Chunk = {}
Chunk.__index = Chunk

function Chunk.substitute(str, dict)
	-- letter or _ followed by alphanumeric or _
	local newstr = str:gsub("%$([%a_][%w_]*)[{}]*", dict)
	return newstr
end

function Chunk:insert(fmt, ...)
	local args = {...}
	if(#args >= 1) then
		table.insert(self, 1, format(self._indent .. fmt, ...))
	else
		table.insert(self, 1, self._indent .. fmt)
	end
end

function Chunk:inserts(fmt, dict)
	table.insert(self, 1, self.substitute(self._indent .. fmt, dict))
end

function Chunk:write(fmt, ...)
	local args = {...}
	if(#args >= 1) then
		table.insert(self, format("\n"..self._indent .. fmt, ...))
	else
		table.insert(self, "\n"..self._indent .. fmt)
	end
end

function Chunk:writes(fmt, dict)
	table.insert(self, "\n"..self._indent..self.substitute(fmt, dict))
end

function Chunk:append(v)
	table.insert(self, v)
end

function Chunk:synthesize(delim)
	local delim = delim or ""
	for i, v in ipairs(self) do
		if type(v) == "table" then
			self[i] = v:synthesize()	
		end
	end

	self[1] = self._firstindent..table.concat(self, delim)
	for i = 2, #self do self[i] = nil end
	return self[1]
end

function Chunk:replace(name, blocks)
	local chunk = assert(self[name])
	for i=1, #self do
		if(self[i] == chunk) then
			self[i] = blocks
			self[name] = blocks
			break
		end
	end
end

local
function split_indent(indent)
	local first, remainder = "", ""
	for x1, x2 in indent:gmatch("([\n\r]*)([^\n\r]*)") do
		if(x1:len() > 0 or x2:len() > 0) then
			if(remainder:len() > 0) then
				first = first..remainder
			end
			
			first = first..x1
			remainder = x2
		end
	end
	return first, remainder
end

-- convert table into Chunk:
local
function chunkify(t)
	-- a chunk is a list of string fragments and table sub-chunks
	for i, v in ipairs(t) do
		-- add reference to sub-chunk to dictionary component of parent chunk:
		if type(v) == "table" then t[v._name] = v end
	end
	t._name = t._name or "main"
	t._indent = t._indent or ""
	t._firstindent, t._indent = split_indent(t._indent)
	
	setmetatable(t, Chunk)
	return t
end

local az = R("az", "AZ")
local az_ = az+P"_"
local number = R"09"
local idname = az_ * (az_ + number)^0
local nl = S("\n\r")
local indent = S(" \t\n\r")
local sp = nl + indent
-- marks end of a chunk
local cend = P"@end"
-- marks start of a chunk, and captures name
local cstart = Cg(indent^0, "_indent") * (P"@" * Cg((idname - P"end"), "_name"))

-- any control characters
local control = (cend + cstart)
-- anything else
local usercode = C((1 - control)^1)

-- the grammar
local chunktemplate = P{ 
	Ct((V"chunk" + usercode)^0) / chunkify,
	chunk = Ct(cstart * (usercode + V"chunk")^0 * cend) / chunkify,
}

function constructor(_, str)
	return chunktemplate:match(str)
end

setmetatable(Chunk, { __call = constructor })

return Chunk