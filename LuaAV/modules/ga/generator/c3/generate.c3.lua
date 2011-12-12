LuaAV.addmodulepath(script.path.."/../modules")
------------------------------------------------
local GASpace = require("gaspace")

local space = GASpace{
	name = "c3",
	xml_file = script.path.."/c3ga_cpp.xml",
	header_file = script.path.."/../../src/c3/c3ga.h",
}

local source_filename = script.path.."/../../src/c3/lua_c3ga_ops_gen.cpp"
local ops = {
	---[[
	"add",
	"subtract",
	"hip",
	"mhip",
	"op",
	--]]
	"gp",
	"lc",
	"rc",
	"applyUnitVersor",
	"applyVersor",
}

local app_utils = require("app.utilities")
--[[
space:generate{
	"applyUnitVersor",
	"applyVersor",
	"hip",
	
}
--]]
space:generate{
	"sp",
}
--space:write(source_filename, ops)
--space:write_file(source_filename, space:generate_from_op_files(ops))