LuaAV.addmodulepath(script.path.."/../modules")
------------------------------------------------
local GASpace = require("gaspace")

local space = GASpace{
	name = "c2",
	xml_file = script.path.."/c2ga_cpp.xml",
	header_file = script.path.."/../../src/c2/c2ga.h",
}

local source_filename = script.path.."/../../src/c2/lua_c2ga_ops_gen.cpp"
local ops = {
	"gp",
	--[[
	"add",
	
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