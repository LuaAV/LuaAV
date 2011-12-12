local clang = require("clang")
local header_path = app.resourcepath.."/Headers"

go(function() while true do collectgarbage() wait(0.01) end end)
go(function() while true do clang.sweep() wait(0.01) end end)

local t = LuaAV.time.cpu()
local timer = function()
	local t1 = LuaAV.time.cpu()
	local dur = t1-t
	t = t1
	return dur
end

function printf(...) print(string.format(...)) end


local src = [=[
////////////////////////////////////////////////////
#include "luaav_audio_jit.hpp"

struct Foo {
	Synth synth;
	
	Foo(lua_State * L) : synth(2) { 
		synth.init(L);
		synth.portinit(L, "out", 0, 2);
		
		// lua stack: args, ctx, synthuserdata
		printf("hi %p\n", this);
		
	}
	~Foo() { 
		PortReader port_out1(synth, 0);
		printf("bye%d\n", (int)port_out1[0]); 
	}
	
	void audioCB() { 
		PortReader port_out1(synth, 0);
		printf("~%d ", (int)port_out1[0]); 
		while(synth.io()) { 
			sample& out1 = port_out1[synth.mFrame];
		}
	}
};


extern "C" int define(lua_State * L) {
	static luaL_reg lib[] = {
		{ NULL, NULL }
	};
	static luaL_reg getters[] = {
		{ NULL, NULL }
	};
	static luaL_reg setters[] = {
		//{ "amp", LNode<Synth>::port_set<&Synth::amp> },
		//{ "out", LNode<Synth>::port_set<&Synth::out> },
		{ "out", portset<0, 2> },
		{ NULL, NULL }
	};
	// create metatable:
	luaav_synth_newmetatable(L, lib, getters, setters);
	
	// return a constructor function with upvalues ctx, meta, jit:
	lua_pushvalue(L, lua_upvalueindex(1));	
	lua_pushcclosure(L, SynthBinding<Foo>::create, 3);	
	return 1;
}

]=]

compiler = clang.Compiler()
compiler:include(header_path)

local f = io.open(header_path .. "/luaav_audio_jit.hpp")
local header = f:read("*a")
f:close()
compiler:header("luaav_audio_jit.hpp", header)

local c, j = 0, 0
local lim = 10

for i = 1, lim do
	timer()
	compiler:compile(src)
	c = c + timer()

	local jit = assert(compiler:jit())
	local define = assert(jit:pushcfunction("define"))
	j = j + timer()
	
	local ctor = define(audio.root)
	local s = ctor{}
	--print(i, s)
	s.out = i
	wait(0.1)
	s:stop()
end
	
--print(header)
--print(src)
printf("compile %2.4f", c/lim)
printf("jit %2.4f", j/lim)
printf("total %2.4f", (c+j)/lim)

