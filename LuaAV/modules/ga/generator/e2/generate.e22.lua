LuaAV.addmodulepath(script.path.."/../modules")
------------------------------------------------
local GASpace = require("gaspace")

local space = GASpace{
	name = "e2",
	xml_file = script.path.."/e2ga_cpp.xml",
	header_file = script.path.."/../../src/e2/e2ga.h",
}

local source_filename = script.path.."/../../src/e2/lua_e2ga_ops_gen.cpp"
--local source_filename = script.path.."/lua_e2ga_ops_gen.cpp"
local ops = {
	"add",
	---[[
	"subtract",
	"hip",
	"mhip",
	"op",
	"gp",
	"lc",
	"rc",
	--]]
}

space:write(source_filename, ops)