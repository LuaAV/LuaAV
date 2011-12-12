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

--- Implements Canny algorithm for edge detection
module('matrixfx.canny', setconstructor)
-----------------------------------------------------

local calc_code = [[
	
	int $name{}_calc(lua_State *L) {
		$name x = $name{}_to_udata(L, 1);
		void *in_mat = muro_matrix_udata_to_base(L, 2);
		void *out_mat = muro_matrix_udata_to_base(L, 3);
		if(x && in_mat && out_mat) {
			Header *in_h = muro_matrix_header(in_mat);
			
			if(in_h->planes != 1) {
				luaL_error(L, "matrixfx.$name: input planes must be 1");
			}
			
			if(in_h->type != UCHAR) {
				luaL_error(L, "matrixfx.$name: input type must be char");
			}
			
			if(in_h->dimcount != 2) {
				luaL_error(L, "matrixfx.$name: dimcount must be 2");
			}
			
			muro_matrix_adapt_2d(out_mat, 1, UCHAR, in_h->dim[0], in_h->dim[1]);
		
			CvMat cv_in, cv_out;
			muro_mat_to_opencv(&cv_in, in_mat);
			muro_mat_to_opencv(&cv_out, out_mat);
	
			double thresh1 = x->threshold - x->range;
			double thresh2 = x->threshold + x->range;
			cvCanny(&cv_in, &cv_out, thresh1, thresh2, x->aperture);
		}
		else {
			luaL_error(L, "matrixfx.$name: invalid matrix arguments");
		}

		return 0;
	}
]]

--- Get/Set the threshold
-- @param v Threshold (default 150)
-- @return Returns threshold if v is nil
function M:threshold(v)
end

--- Get/Set the range for the threshold
-- @param v Range (default 10)
-- @return Returns range if v is nil
function M:range(v)
end

--- Get/Set the aperture
-- @param v Aperture (default 3)
-- @return Returns aperture if v is nil
function M:aperture(v)
end

local opencv_canny_meta = codegen.generate(M, {
	name = "canny",
	params = {
		{name = "threshold", type = "double", default = 150},
		{name = "range", type = "double", default = 10},
		
		-- filter aperture values
		{	
			name = "aperture", 
			type = "int", 
			default = 3,
			setter = function(val)
				if((val%2)==0) then
					val = val+1
				end
				if(val < 3) then
					val = 3
				end
				if(val > 7) then
					val = 7
				end
				return val
			end,
		},
	},
	calc = calc_code,
})

--- Constructor
-- @param init Initializers
-- @name canny
function C:__call(init)
	init = init or {}
	
	-- setup an instance of the fx
	local m = Matrix()
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.struct = opencv_canny_meta.new()
	for k, v in pairs(init) do
		if(type(m[k]) == "function") then
			m[k](m, v)
		end
	end

	return m
end

--- Calculate threshold
-- @param inmat Input Matrix
function M:matrix_calc(inmat)
	self.struct:calc(inmat, self)
end