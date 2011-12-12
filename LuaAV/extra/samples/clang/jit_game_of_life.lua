--[[
	Using JIT to operate over an Array for a Game of Life simulation
--]]

local clang = require("clang")
compiler = clang.Compiler()
compiler:include(app.resourcepath.."/Headers")

local code = [=[

#include "lua.hpp"
#include "lua_array.h"

using al::Array;

// functions exposed to Lua must have this signature:
extern "C" int step(lua_State * L) {
	
	Array * game = (Array *)lua_array_checkto(L, 1);
	Array * game_prev = (Array *)lua_array_checkto(L, 2);
	
	int w = game->header.dim[0];
	int h = game->header.dim[1];

	if(game->data.ptr) {
		for (int x=0; x<w; x++) {
			for (int y=0; y<h; y++) {
				// rule for this cell:
				uint8_t val;
				game_prev->read(&val, x, y);
				 
				uint8_t n[8];
				
				// read neighbor cells:
				game_prev->read_wrap(n+0, x-1, y);
				game_prev->read_wrap(n+1, x+1, y);
				game_prev->read_wrap(n+2, x, y-1);
				game_prev->read_wrap(n+3, x, y+1);
				game_prev->read_wrap(n+4, x-1, y-1);
				game_prev->read_wrap(n+5, x+1, y+1);
				game_prev->read_wrap(n+6, x+1, y-1);
				game_prev->read_wrap(n+7, x-1, y+1);
			
				uint32_t total = 0;
				for (int i=0; i<8; i++) total += n[i]/255;
				
				if (val == 255) {
					val =  total == 2 || total == 3 ? 1 : 0;
				} else {
					val = total == 3 ? 1 : 0;
				}
				
				val = val*255;
				
				game->write(&val, x, y);
			}
		}		
	} else {
		printf("no data\n");
	}	
	return 0;
}

]=]



assert(compiler:compile(code))
compiler:optimize()
local jit = assert(compiler:jit())
local step = assert(jit:pushcfunction("step"))
jit = nil

go(function() while true do collectgarbage() wait(0.1) end end)

local Array = require "Array"

local dim = 600

local game = Array{
	components = 1,
	type = Array.UInt8,
	dim = { dim, dim },
}
local game_prev = Array{
	components = 1,
	type = Array.UInt8,
	dim = { dim, dim },
}

local gl = require "opengl"
local sketch = require "opengl.sketch"
local Texture = require "opengl.Texture"

wait(0.5)

local ctx = "Game of Life"
win = Window(ctx, 0, 0, dim, dim)

local game_tex = Texture {
	ctx = ctx,
	single_channel = gl.LUMINANCE,
	minfilter = gl.GL_NEAREST,
	magfilter = gl.GL_NEAREST,
}

math.randomseed(os.time())

for x=0, dim-1 do
	for y=0, dim-1 do
		if math.random(math.sqrt(dim/4)) == 1 then
			game_prev:setcell(x, y, 255)
		end
	end
end

function win:mouse(e, b, x, y)
	x = math.floor(math.abs(dim * (x / self.dim[1]) % dim))
	y = math.floor(math.abs(dim * (y / self.dim[2]) % dim))
	game_prev:setcell(x, y, { 255 })
	game:setcell(x, y, { 255 })
end

function win:key(e, k)
	if e == "down" and k == 27 then 
		self.fullscreen = not self.fullscreen 
	end
end

function win:draw()

	step(game, game_prev)
	
	-- swap buffers:
	game, game_prev = game_prev, game

	-- copy to GPU and render:
	game_tex:fromarray(game)

	game_tex:bind()
	
	sketch:enter_ortho()
	gl.Begin(gl.QUADS)
		sketch.quad()
	gl.End()
	sketch:leave_ortho()
	game_tex:unbind()
end