local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print

local math = require 'math'
local table = require 'table'

require 'muro'

local Matrix = muro.Matrix
local Mesh = muro.Mesh

local compiler = require("codegen.compiler")

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('meshfx.multiple', setconstructor)
-----------------------------------------------------

local draw_code = [[	
	#include "lua.h"
	#include "matrix_c.h"
	#include "glo_c.h"
	#include "stdlib.h"
	
	int multiple_draw(lua_State *L) {
		void *mesh = muro_mesh_udata_to_base(L, 1);
		void *pos_mat = muro_matrix_udata_to_base(L, 2);
		if(mesh && pos_mat) {
			Header *pos_h = muro_matrix_header(pos_mat);
			
			if(pos_h->planes != 3) {
				luaL_error(L, "meshfx.multiple: position planes must be 3");
			}
			
			if(pos_h->type != FLOAT32) {
				luaL_error(L, "meshfx.multiple: position type must be float32");
			}

			int width = pos_h->dim[0];
			int height = (pos_h->dimcount < 2) ? 1 : pos_h->dim[1];
			int planes = pos_h->planes;
			int pos_rowsize = muro_matrix_rowsize(pos_mat);

			unsigned char *ptr_pos = muro_matrix_data(pos_mat);

			muro_mesh_bind(mesh);
			for(int j=0; j < height; j++) {
				float *f_pos = (float *)(ptr_pos + j*pos_rowsize);
				
				for(int i=0; i < width; i++) {
					glPushMatrix();
						glTranslatef(f_pos[0], f_pos[1], f_pos[2]);
						muro_mesh_drawraw(mesh);
					glPopMatrix();
					
					f_pos += planes;
				}
			}
			muro_mesh_unbind(mesh);
		}
		else {
			luaL_error(L, "meshfx.multiple: invalid arguments");
		}
		return 0;
	}
]]


local multiple_draw = compiler.compile("multiple_draw", draw_code)
assert(multiple_draw, "error compiling meshfx.multiple")

function C:__call(t)
	t = t or {}
	
	-- setup an instance of the fx
	local m = {}
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end

	return m
end

function M:draw(mesh, pos_mat)
	multiple_draw(mesh, pos_mat)
end