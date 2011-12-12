LuaAV.addmodulepath(script.path.."/../modules")
------------------------------------------------
local GASpace = require("gaspace")

local space = GASpace{
	name = "e3",
	xml_file = script.path.."/e3ga_cpp.xml",
	header_file = script.path.."/../../src/e3/e3ga.h",
}

local source_filename = script.path.."/../../src/e3/lua_e3ga_ops_gen.cpp"
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

local app_utils = require("app.utilities")
space:write(source_filename, ops)