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

module('matrixfx.pack', setconstructor)
-----------------------------------------------------

local calc_code = [[

	int %NAME%_calc(lua_State *L) {
		%NAME% x = %NAME%_to_udata(L, 1);
		void *pack_mat = muro_matrix_udata_to_base(L, 2);
		
		void *slices[4];
		int nslices = 0;

		int top = lua_gettop(L);
		for(int i=3; i <= top && nslices < 4; i++) {
			void *m = muro_matrix_udata_to_base(L, i);
			if(m) {
				slices[i-3] = m;
				nslices++;
			}
		}
		
		
		if(x && nslices > 0 && pack_mat) {

			if(! muro_matrices_equal_type(nslices, slices)) {
				luaL_error(L, "matrixfx.pack: input matrices don't have same type");
			}
			
			if(! muro_matrices_equal_dim(nslices, slices)) {
				luaL_error(L, "matrixfx.pack: input matrices don't have same dimension");
			}
			
			Header *h = muro_matrix_header(slices[0]);
			int planes = 0;
			int type = h->type;
			int width = h->dim[0];
			int height = h->dim[1];
			
			int bytes_per_plane = muro_bytes_for_type(type);
			char *p_slices[4];
			int rowstride_slices[4];
			int planes_slices[4];
			
			for(int j=0; j < h->planes; j++) {
				p_slices[planes+j] = muro_matrix_data(slices[0])+j*bytes_per_plane;
				rowstride_slices[planes+j] = muro_matrix_rowsize(slices[0]);
				planes_slices[planes+j] = h->planes;
			}

			
			planes = h->planes;
			for(int i=1; i < nslices; i++) {
				h = muro_matrix_header(slices[i]);
				
				for(int j=0; j < h->planes; j++) {
					p_slices[planes+j] = muro_matrix_data(slices[i])+j*bytes_per_plane;
					rowstride_slices[planes+j] = muro_matrix_rowsize(slices[i]);
					planes_slices[planes+j] = h->planes;
				}

				planes += h->planes;
			}
			
			muro_matrix_adapt_2d(pack_mat, planes, type, width, height);
			
			char *p_pack = muro_matrix_data(pack_mat);
			int rowstride = muro_matrix_rowsize(pack_mat);
			for(int j=0; j < height; j++) {
				float *f_pack = (float *)(p_pack + j*rowstride);
				float *f_slices[4];
				for(int k=0; k < planes; k++) {
					f_slices[k] = (float *)(p_slices[k] + j*rowstride_slices[k]);
				}
				
				for(int i=0; i < width; i++) {
					for(int k=0; k < planes; k++) {
						*f_pack++ = *f_slices[k];
						f_slices[k] += planes_slices[k];
					}
				}
			}
		}
		else {
			luaL_error(L, "matrixfx.pack: invalid arguments");
		}
		return 0;
	}
]]


local pack_meta = codegen.generate(M, {
	name = "pack",
	
	parameters = {
	},
	calc = calc_code,
})

function C:__call(...)
	
	-- setup an instance of the fx
	local m = Matrix(...)
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end

	m.struct = pack_meta.new()

	return m
end

function M:calc(...)
	self.struct:calc(self, ...)
end