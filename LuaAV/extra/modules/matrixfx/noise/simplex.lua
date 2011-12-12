local assert, pairs, tostring, type = assert, pairs, tostring, type
local setmetatable = setmetatable
local assert = assert
local print = print
local unpack = unpack

local math = require 'math'
local table = require 'table'

require 'muro'

local Matrix = muro.Matrix
local Mesh = muro.Mesh


local codegen = require("matrixfx.opencv.codegen")

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('matrixfx.noise.simplex', setconstructor)
-----------------------------------------------------

local calc_code = [[
	
	float simplex_dot2d(int *g, float x, float y){
		return g[0] * x + g[1] * y;
	}
	
	float simplex_dot3d(int *g, float x, float y, float z){
		return g[0] * x + g[1] * y + g[2] * z;
	}
	
	float simplex_dot4d(int *g, float x, float y, float z, float w){
		return g[0] * x + g[1] * y + g[2] * z;
	}
	
	int fastfloor(float x){
		return x > 0 ? (int)x : (int)x-1;
	}

	float simplex_noise_2d(int *perm, float x, float y) {
		float n0 = 0, n1 = 0, n2 = 0; // Noise contributions from the three
		// corners
		// Skew the input space to determine which simplex cell we're in
		float s = (x + y) * SIMPLEX_F2; // Hairy factor for 2D
		int i = fastfloor(x + s);
		int j = fastfloor(y + s);
		float t = (i + j) * SIMPLEX_G2;
		float x0 = x - (i - t); // The x,y distances from the cell origin
		float y0 = y - (j - t);
		// For the 2D case, the simplex shape is an equilateral triangle.
		// Determine which simplex we are in.
		int i1, j1; // Offsets for second (middle) corner of simplex in (i,j)
		if (x0 > y0) {
			i1 = 1;
			j1 = 0;
		} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		else {
			i1 = 0;
			j1 = 1;
		} // upper triangle, YX order: (0,0)->(0,1)->(1,1)
		// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
		// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
		// c = (3-sqrt(3))/6
		float x1 = x0 - i1 + SIMPLEX_G2; // Offsets for middle corner in (x,y) unskewed
		float y1 = y0 - j1 + SIMPLEX_G2;
		float x2 = x0 + SIMPLEX_G22; // Offsets for last corner in (x,y) unskewed
		float y2 = y0 + SIMPLEX_G22;
		// Work out the hashed gradient indices of the three simplex corners
		int ii = i & 0xff;
		int jj = j & 0xff;
		// Calculate the contribution from the three corners
		float t0 = 0.5 - x0 * x0 - y0 * y0;
		if (t0 > 0) {
			t0 *= t0;
			int gi0 = perm[ ii + perm[jj] ] % 12;
			n0 = t0 * t0 * simplex_dot2d(simplex_grad3[gi0], x0, y0); // (x,y) of grad3 used for
			// 2D gradient
		}
		float t1 = 0.5 - x1 * x1 - y1 * y1;
		if (t1 > 0) {
			t1 *= t1;
			int gi1 = perm[ ii + i1 + perm[jj + j1] ] % 12;
			n1 = t1 * t1 * simplex_dot2d(simplex_grad3[gi1], x1, y1);
		}
		float t2 = 0.5 - x2 * x2 - y2 * y2;
		if (t2 > 0) {
			t2 *= t2;
			int gi2 = perm[ ii + 1 + perm[jj + 1] ] % 12;
			n2 = t2 * t2 * simplex_dot2d(simplex_grad3[gi2], x2, y2);
		}
		return 70.0 * (n0 + n1 + n2);
	}
		
	int %NAME%_calc(lua_State *L) {
		%NAME% x = %NAME%_to_udata(L, 1);
		void *mat = muro_matrix_udata_to_base(L, 2);
		if(x && mat) {
			Header *mat_h = muro_matrix_header(mat);
			
			if(mat_h->planes != 1) {
				luaL_error(L, "meshfx.noise.simplex: input planes must be 1");
			}
			
			if(mat_h->type != FLOAT32) {
				luaL_error(L, "meshfx.noise.simplex: input type must be float32");
			}

			int width = mat_h->dim[0];
			int height = mat_h->dim[1];
			int rowstride = muro_matrix_rowsize(mat);
			
			char *p_mat = muro_matrix_data(mat);
			
			float x_off = x->offset[0];
			float y_off = x->offset[1];
			float x_scale = x->scale[0];
			float y_scale = x->scale[1];
			int *perm = x->perm;
			
			for(int j=0; j < height; j++) {
				float *f_mat = (float *)(p_mat + j*rowstride);
				float yy = (((float)j)+y_off)*y_scale;
				
				for(int i=0; i < width; i++) {
					float xx = (((float)i)+x_off)*x_scale;	
					*f_mat = simplex_noise_2d(perm, xx, yy);
					f_mat++;
				}
			}
		}
		else {
			luaL_error(L, "meshfx.noise.simplex: invalid arguments");
		}
		return 0;
	}
]]


local noise_simplex_meta = codegen.generate(M, {
	name = "simplex",
	
	header = [[
		#define SIMPLEX_SQRT3 1.73205080756888
		#define SIMPLEX_SQRT5 2.23606797749979
		
		#define SIMPLEX_F2 0.36602540378444
		#define SIMPLEX_G2 0.211325
		#define SIMPLEX_G22 -0.57735
		
		#define SIMPLEX_F3 0.33333333333333
		#define SIMPLEX_G3 0.16666666666667
		
		#define SIMPLEX_F4 0.309017
		#define SIMPLEX_G4 0.138197
		#define SIMPLEX_G42 0.276393
		#define SIMPLEX_G43 0.41459
		#define SIMPLEX_G44 -0.447214
		
		static int simplex_grad3[][3] = { { 1, 1, 0 }, { -1, 1, 0 },
		{ 1, -1, 0 }, { -1, -1, 0 }, { 1, 0, 1 }, { -1, 0, 1 },
		{ 1, 0, -1 }, { -1, 0, -1 }, { 0, 1, 1 }, { 0, -1, 1 },
		{ 0, 1, -1 }, { 0, -1, -1 } };
		
		static int simplex_grad4[][4] = { { 0, 1, 1, 1 }, { 0, 1, 1, -1 },
		{ 0, 1, -1, 1 }, { 0, 1, -1, -1 }, { 0, -1, 1, 1 },
		{ 0, -1, 1, -1 }, { 0, -1, -1, 1 }, { 0, -1, -1, -1 },
		{ 1, 0, 1, 1 }, { 1, 0, 1, -1 }, { 1, 0, -1, 1 }, { 1, 0, -1, -1 },
		{ -1, 0, 1, 1 }, { -1, 0, 1, -1 }, { -1, 0, -1, 1 },
		{ -1, 0, -1, -1 }, { 1, 1, 0, 1 }, { 1, 1, 0, -1 },
		{ 1, -1, 0, 1 }, { 1, -1, 0, -1 }, { -1, 1, 0, 1 },
		{ -1, 1, 0, -1 }, { -1, -1, 0, 1 }, { -1, -1, 0, -1 },
		{ 1, 1, 1, 0 }, { 1, 1, -1, 0 }, { 1, -1, 1, 0 }, { 1, -1, -1, 0 },
		{ -1, 1, 1, 0 }, { -1, 1, -1, 0 }, { -1, -1, 1, 0 },
		{ -1, -1, -1, 0 } };
		
		static int simplex_p[] = { 151, 160, 137, 91, 90, 15, 131, 13, 201,
		95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37,
		240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62,
		94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
		87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139,
		48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
		230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25,
		63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200,
		196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3,
		64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255,
		82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
		223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153,
		101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79,
		113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242,
		193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249,
		14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204,
		176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222,
		114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156,180 };
		
		
		static int simplex_simplex[][4] = { { 0, 1, 2, 3 }, { 0, 1, 3, 2 },
		{ 0, 0, 0, 0 }, { 0, 2, 3, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 1, 2, 3, 0 }, { 0, 2, 1, 3 }, { 0, 0, 0, 0 },
		{ 0, 3, 1, 2 }, { 0, 3, 2, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 1, 3, 2, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 1, 2, 0, 3 }, { 0, 0, 0, 0 },
		{ 1, 3, 0, 2 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 2, 3, 0, 1 }, { 2, 3, 1, 0 }, { 1, 0, 2, 3 }, { 1, 0, 3, 2 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 0, 3, 1 },
		{ 0, 0, 0, 0 }, { 2, 1, 3, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 0, 1, 3 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 0, 1, 2 }, { 3, 0, 2, 1 },
		{ 0, 0, 0, 0 }, { 3, 1, 2, 0 }, { 2, 1, 0, 3 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 1, 0, 2 }, { 0, 0, 0, 0 },
		{ 3, 2, 0, 1 }, { 3, 2, 1, 0 } };
	]],
	
	parameters = {
		{name = "offset", type = "float[2]", default = {0., 0}},
		{name = "scale", type = "float[2]", default = {1., 1.}},
		
		{	
			name = "perm", 
			type = "int *", 
			init = [[
				x->perm = (int *)malloc(sizeof(int)*(0x200));
				for(int i=0; i < 0x200; i++) {
					x->perm[i] = simplex_p[i & 0xff];
				}
			]],
			destroy = [[
				if(x->perm) {
					free(x->perm);
				}
			]],
		},
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

	m.struct = noise_simplex_meta.new()

	return m
end

function M:calc()
	self.struct:calc(self)
end