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

module('meshfx.source.sphere', setconstructor)
-----------------------------------------------------

local draw_code = [[	
	#include "lua.h"
	#include "matrix_c.h"
	#include "stdlib.h"
	#include "math.h"
	
	int sphere_gen(lua_State *L) {
		void *mat = muro_matrix_udata_to_base(L, 1);
		if(mat) {
			float radius = lua_isnumber(L, 2) ? lua_tonumber(L, 2) : 1.;
			Header *h = muro_matrix_header(mat);
			
			if(h->planes != 3) {
				luaL_error(L, "meshfx.source.sphere: position planes must be 3");
			}
			
			if(h->type != FLOAT32) {
				luaL_error(L, "meshfx.source.sphere: position type must be float32");
			}

			int width = h->dim[0];
			int height = (h->dimcount < 2) ? 1 : h->dim[1];
			int planes = h->planes;
			int pos_rowsize = muro_matrix_rowsize(mat);

			unsigned char *ptr_pos = muro_matrix_data(mat);
			
			float d_theta = 2.*3.14159/((float)(width-1));
			float d_phi = 3.14159/((float)(height-1));

			for(int j=0; j < height; j++) {
				float phi = ((float)j) * d_phi;
				float *f_pos = (float *)(ptr_pos + j*pos_rowsize);
				
				for(int i=0; i < width; i++) {
					float theta = ((float)i)*d_theta;
					f_pos[0] = radius*cos(theta)*sin(phi);
					f_pos[1] = radius*sin(theta)*sin(phi);
					f_pos[2] = radius*cos(phi);
				
					f_pos += planes;
				}
			}
		}
		else {
			luaL_error(L, "meshfx.source.sphere: invalid arguments");
		}
		return 0;
	}
]]


local sphere_gen = compiler.compile("sphere_gen", draw_code)
assert(sphere_gen, "error compiling meshfx.source.sphere")

function C:__call(...)
	-- setup an instance of the fx
	local m = Matrix(...)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	local planes = 3
	local type = "float32"
	local dim = m.dim
	m:adapt(planes, type, dim)
	
	sphere_gen(m, 0.3)

	return m
end