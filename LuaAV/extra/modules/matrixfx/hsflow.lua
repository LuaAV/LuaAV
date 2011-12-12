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

--- Calculates optical flow for two images using the Horn & Schunck algorithm
module('matrixfx.hsflow', setconstructor)
-----------------------------------------------------

local calc_code = [[
	int $name{}_calc(lua_State *L) {
	
		$name x = $name{}_to_udata(L, 1);
		void *in_mat = muro_matrix_udata_to_base(L, 2);
		void *prev_mat = muro_matrix_udata_to_base(L, 3);
		void *x_mat = muro_matrix_udata_to_base(L, 4);
		void *y_mat = muro_matrix_udata_to_base(L, 5);
		void *out_mat = muro_matrix_udata_to_base(L, 6);
		
		if(x && in_mat && prev_mat && x_mat && y_mat && out_mat) {
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
			
			muro_matrix_adapt_2d(x_mat, 1, FLOAT32, in_h->dim[0], in_h->dim[1]);
			muro_matrix_adapt_2d(y_mat, 1, FLOAT32, in_h->dim[0], in_h->dim[1]);
			muro_matrix_adapt_2d(out_mat, 2, FLOAT32, in_h->dim[0], in_h->dim[1]);
		
			CvMat cv_in, cv_prev, cv_x, cv_y;
			muro_mat_to_opencv(&cv_in, in_mat);
			muro_mat_to_opencv(&cv_prev, prev_mat);
			muro_mat_to_opencv(&cv_x, x_mat);
			muro_mat_to_opencv(&cv_y, y_mat);
	
	
			float lambda = 0.001;		
			int max_iter = 3;
			float threshold = 0.;
	
			cvCalcOpticalFlowHS(&cv_prev, &cv_in, 0, &cv_x, &cv_y, x->lambda,
					cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, x->max_iter, x->threshold));
			
			void *xy_mat[] = {x_mat, y_mat};
			muro_matrix_pack(out_mat, 2, xy_mat);
		}
		else {
			luaL_error(L, "matrixfx.$name: invalid matrix arguments");
		}

		return 0;
	}
]]

--- Get/Set the lambda (Lagrangian multiplier)
-- @param v Lambda (default 0.001)
-- @return Returns lambda if v is nil
function M:lambda(v)
end

--- Get/Set the maximum iterations
-- @param v Maximum iterations (default 3)
-- @return Returns maximum iterations if v is nil
function M:max_iter(v)
end

--- Get/Set the threshold
-- @param v Threshold (default 0)
-- @return Returns threshold if v is nil
function M:threshold(v)
end

local opencv_hsflow_meta = codegen.generate(M, {
	name = "hsflow",
	params = {
		{name = "lambda", type = "float", default = 0.001},
		{name = "max_iter", type = "int", default = 3},
		{name = "threshold", type = "float", default = 0.},
	},
	calc = calc_code,
})

--- Constructor
-- @param init Initializers
-- @name hsflow
function C:__call(init)
	init = init or {}
	
	-- setup an instance of the fx
	local m = Matrix()
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.struct = opencv_hsflow_meta.new()
	for k, v in pairs(init) do
		if(type(m[k]) == "function") then
			m[k](m, v)
		end
	end
	
	m.mat_prev = Matrix()
	m.mat_x = Matrix()
	m.mat_y = Matrix()

	return m
end

--- Calculate threshold
-- @param inmat Input Matrix
function M:matrix_calc(inmat)
	self.mat_prev:adapt(inmat)
	self.struct:calc(inmat, self.mat_prev, self.mat_x, self.mat_y, self)
	self.mat_prev:frommatrix(inmat)
end