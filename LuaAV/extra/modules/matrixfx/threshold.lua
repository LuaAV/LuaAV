local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print
local ipairs = ipairs
local getmetatable = getmetatable

local string = require 'string'
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

--- Applies a fixed-level threshold
module('matrixfx.threshold', setconstructor)
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
			
			if(! (in_h->type == UCHAR || in_h->type == FLOAT32)) {
				luaL_error(L, "matrixfx.$name: input type must be char or float32");
			}
			
			if(in_h->dimcount != 2) {
				luaL_error(L, "matrixfx.$name: dimcount must be 2");
			}
			
			muro_matrix_adapt_2d(out_mat, 1, in_h->type, in_h->dim[0], in_h->dim[1]);
		
			CvMat cv_in, cv_out;
			muro_mat_to_opencv(&cv_in, in_mat);
			muro_mat_to_opencv(&cv_out, out_mat);
	
			int threshold_type = CV_THRESH_BINARY;
			cvThreshold(&cv_in, &cv_out, x->threshold, x->max_value, threshold_type);
		}
		else {
			luaL_error(L, "matrixfx.$name: invalid matrix arguments");
		}

		return 0;
	}
]]

--- Get/Set the threshold
-- @param v Threshold (default 100)
-- @return Returns threshold if v is nil
function M:threshold(v)
end

--- Get/Set the max value
-- @param v Max value (default 255)
-- @return Returns max value if v is nil
function M:max_value(v)
end

local opencv_threshold_meta = codegen.generate(M, {
	name = "threshold",
	params = {
		{name = "threshold", type = "double", default = 100},
		{name = "max_value", type = "double", default = 255},
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
	
	m.struct = opencv_threshold_meta.new()
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