local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print

local math = require 'math'
local table = require 'table'

require 'muro'
local Matrix = muro.Matrix

local compiler = require("codegen.compiler")

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('meshfx.source.trigrid', setconstructor)
-----------------------------------------------------

local draw_code = [[	
	#include "lua.h"
	#include "matrix_c.h"
	#include "stdlib.h"
	#include "math.h"
	
	int trigrid_gen(lua_State *L) {
		void *mat = muro_matrix_udata_to_base(L, 1);
		if(mat && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
			int ww = (int)lua_tonumber(L, 2);
			int hh = (int)lua_tonumber(L, 3);
		
			int v_w = ww;
			int iw = ww-1;
			int ih = hh-1;
			
			muro_matrix_adapt_2d(mat, 1, UINT, 2*3*iw*ih, 1);
			unsigned int *idx_data = muro_matrix_data(mat);
			
			for(int j=0; j < ih; j++) {
				int voff = j*v_w;
				for(int i=0; i < iw; i++) {
					//Tri 1
					*idx_data++ = voff + i;
					*idx_data++ = voff + i + 1;
					*idx_data++ = voff + i + v_w;
	
					//Tri 2
					*idx_data++ = voff + i + 1;
					*idx_data++ = voff + i + 1 + v_w;
					*idx_data++ = voff + i + v_w;
				}
			}
		}
		else {
			luaL_error(L, "meshfx.source.trigrid: invalid arguments");
		}
		return 0;
	}
]]


local trigrid_gen = compiler.compile("trigrid_gen", draw_code)
assert(trigrid_gen, "error compiling meshfx.source.trigrid")

function C:__call(mm)
	-- setup an instance of the fx
	local m = Matrix()
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	trigrid_gen(m, mm.dim[1], mm.dim[2]);

	return m
end

