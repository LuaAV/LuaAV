local clang = require("clang")
compiler = clang.Compiler()

local header_path = app.resourcepath.."/Headers"
compiler:include(header_path)

local code = io.open(script.path.."/test.code.cpp"):read("*a")


if(compiler:compile(code)) then
	compiler:optimize()
--	compiler:dump()
	local jit = assert(compiler:jit())
	local f = assert(jit:pushcfunction("test"))
	
	
	go(1, function()
		print("f(): ")
		f()
	end)
end


--[[
compiler = clang.Compiler()
compiler:include(header_path)

local code = io.open(script.path.."/test.code.c"):read("*a")
compiler.cpp = false
print("compiler.cpp", compiler.cpp)

if(compiler:compile(code)) then
	local jit = assert(compiler:jit())
	local f = assert(jit:pushcfunction("test_c"))
	go(1.2, f)
else
	print("no compile: ", code)
end
--]]