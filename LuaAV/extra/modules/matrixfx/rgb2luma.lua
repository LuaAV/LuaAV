local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print

local math = require 'math'
local table = require 'table'

require 'muro'

local codegen = require("matrixfx.opencv.codegen")
local Matrix = muro.Matrix

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

--- Converts a color (RGB or RBGA) image into a luminance image
module('matrixfx.rgb2luma', setconstructor)
-----------------------------------------------------

local calc_code = [[
	
	int $name{}_calc(lua_State *L) {
		$name x = $name{}_to_udata(L, 1);
		void *in_mat = muro_matrix_udata_to_base(L, 2);
		void *out_mat = muro_matrix_udata_to_base(L, 3);
		if(x && in_mat && out_mat) {
			Header *in_h = muro_matrix_header(in_mat);
			
			if(in_h->type != UCHAR) {
				luaL_error(L, "matrixfx.$name: input type must be char");
			}
			
			if(in_h->dimcount != 2) {
				luaL_error(L, "matrixfx.$name: input dimcount must be 2");
			}
			
			muro_matrix_adapt_2d(out_mat, 1, in_h->type, in_h->dim[0], in_h->dim[1]);
		
			CvMat cv_in, cv_out;
			muro_mat_to_opencv(&cv_in, in_mat);
			muro_mat_to_opencv(&cv_out, out_mat);
	
			switch(in_h->planes) {
				case 4:
					cvCvtColor(&cv_in, &cv_out, CV_RGBA2GRAY);
					break;
	
				case 3:
					cvCvtColor(&cv_in, &cv_out, CV_RGB2GRAY);
					break;
			}
			
		}
		else {
			luaL_error(L, "matrixfx.$name: invalid matrix arguments");
		}

		return 0;
	}
]]


local opencv_rgb2luma_meta = codegen.generate(M, {
	name = "rgb2luma",
	params = {},
	calc = calc_code,
})

--- Constructor
-- @param init Initializers
-- @name rgb2luma
function C:__call(init)
	init = init or {}
	
	-- setup an instance of the fx
	local m = Matrix()
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	---[[
	m.struct = opencv_rgb2luma_meta.new()
	for k, v in pairs(init) do
		if(type(m[k]) == "function") then
			m[k](m, v)
		end
	end
	--]]

	return m
end

--- Calculate threshold
-- @param inmat Input Matrix
function M:matrix_calc(inmat)
	self.struct:calc(inmat, self)
end
