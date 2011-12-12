local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print
local unpack = unpack

local math = require 'math'
local table = require 'table'

require 'muro'

local Matrix = muro.Matrix
local codegen = require("matrixfx.opencv.codegen")

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('matrixfx.mask', setconstructor)
-----------------------------------------------------

local calc_code = [[

	int %NAME%_calc(lua_State *L) {
		%NAME% x = %NAME%_to_udata(L, 1);
		void *mask_mat = muro_matrix_udata_to_base(L, 2);
		void *mat = muro_matrix_udata_to_base(L, 3);
		
		if(x && mask_mat && mat) {

			if(! muro_matrix_equal_type(mask_mat, mat)) {
				luaL_error(L, "matrixfx.mask: input matrices don't have same type");
			}
			
			if(! muro_matrix_equal_dim(mask_mat, mat)) {
				luaL_error(L, "matrixfx.mask: input matrices don't have same dimension");
			}
			
			
			Header *h_mask = muro_matrix_header(mask_mat);
			int mask_planes = h_mask->planes;
			int width = h_mask->dim[0];
			int height = h_mask->dim[1];
			
			Header *h = muro_matrix_header(mat);
			int planes = h->planes;
			int mask = x->mask;
			
			if(mask_planes < (planes + mask)) {
				luaL_error(L, "matrixfx.mask: not enough input planes");
			}
			
			char *p_mask = muro_matrix_data(mask_mat);
			char *p_mat = muro_matrix_data(mat);
			int rowstride_mask = muro_matrix_rowsize(mask_mat);
			int rowstride = muro_matrix_rowsize(mat);
			
			for(int j=0; j < height; j++) {
				float *f_mask = ((float *)(p_mask + j*rowstride_mask)) + mask;
				float *f_mat = (float *)(p_mat + j*rowstride);
				
				for(int i=0; i < width; i++) {
					for(int k=0; k < planes; k++) {
						*f_mask = *f_mat++;
						f_mask += mask_planes;
					}
				}
			}
		}
		else {
			luaL_error(L, "matrixfx.mask: invalid arguments");
		}
		return 0;
	}
]]


local pack_meta = codegen.generate(M, {
	name = "mask",
	
	parameters = {
		{name = "mask", type = "int", default = 0},
	},
	calc = calc_code,
})

function C:__call(init)
	init = init or {}
	
	-- setup an instance of the fx
	local m = {}
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.struct = pack_meta.new()
	
	for k, v in pairs(init) do
		if(type(m[k]) == "function") then
			m[k](m, v)
		end
	end

	return m
end

function M:calc(...)
	self.struct:calc(...)
end