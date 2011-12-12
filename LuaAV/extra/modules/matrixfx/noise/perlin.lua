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


--local compiler = require("codegen.compiler")
local codegen = require("matrixfx.opencv.codegen")

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('matrixfx.noise.perlin', setconstructor)
-----------------------------------------------------

local calc_code = [[

	float noise_fsc(float *cos_lut, float i){
		return 0.5 * (1.0 - cos_lut[(int)(i * COS_LUT_PI) % COS_LUT_TWOPI]);
	}

	float perlin_noise_2d(%NAME% xx, float x, float y) {
		if(x < 0.) x = -x;
		if(y < 0.) y = -y;
		
		int xi = (int)x;
		int yi = (int)y;
		float xf = x - (float)xi;
		float yf = y - (float)yi;
		
		float r = 0.;
		float ampl = 0.5;
		
		float *noise_lut = xx->noise_lut;
		float *cos_lut = xx->cos_lut;
		for(int i=0; i < xx->octaves; i++) {
			int of = xi + (yi << PERLIN_YWRAPB);
			
			float rxf = noise_fsc(cos_lut, xf);
			float ryf = noise_fsc(cos_lut, yf);
			
			
			// stage 1
			float n1 = noise_lut[of & PERLIN_SIZE];
			n1 += rxf * (noise_lut[(of+1) & PERLIN_SIZE] - n1);
			
			float n2 = noise_lut[(of+PERLIN_YWRAP) & PERLIN_SIZE];
			n2 += rxf * (noise_lut[(of+PERLIN_YWRAP+1) & PERLIN_SIZE] - n2);
			
			n1 += ryf * (n2-n1);
			
			
			// stage 2
			of += PERLIN_ZWRAP;
			
			n2 = noise_lut[of & PERLIN_SIZE];
			n2 += rxf * (noise_lut[(of+1) & PERLIN_SIZE] - n2);
			
			float n3 = noise_lut[(of+PERLIN_YWRAP) & PERLIN_SIZE];
			n3 += rxf * (noise_lut[(of+PERLIN_YWRAP+1) & PERLIN_SIZE] - n3);
			
			n2 += ryf * (n3-n2);
			
			
			// flush
			r += n1 * ampl;
			ampl *= xx->lacunarity;
			
			if(ampl < PERLIN_MIN_AMPLITUDE) break;
			
			xi <<= 1;
			xf *= 2;
			yi <<= 1;
			yf *= 2;
			
			if (xf >= 1.0f) {
				xi++;
				xf--;
			}

			if (yf >= 1.0f) {
				yi++;
				yf--;
			}
		}
		
		return r;
	}
	
	int %NAME%_calc(lua_State *L) {
		%NAME% x = %NAME%_to_udata(L, 1);
		void *mat = muro_matrix_udata_to_base(L, 2);
		if(x && mat) {
			Header *mat_h = muro_matrix_header(mat);
			
			if(mat_h->planes != 1) {
				luaL_error(L, "meshfx.noise.perlin: input planes must be 1");
			}
			
			if(mat_h->type != FLOAT32) {
				luaL_error(L, "meshfx.noise.perlin: input type must be float32");
			}

			int width = mat_h->dim[0];
			int height = mat_h->dim[1];
			int rowstride = muro_matrix_rowsize(mat);
			
			char *p_mat = muro_matrix_data(mat);
			
			float x_off = x->offset[0];
			float y_off = x->offset[1];
			float x_scale = x->scale[0];
			float y_scale = x->scale[1];
			for(int j=0; j < height; j++) {
				float *f_mat = (float *)(p_mat + j*rowstride);
				float yy = (((float)j)+y_off)*y_scale;
				
				for(int i=0; i < width; i++) {
					float xx = (((float)i)+x_off)*x_scale;	
					*f_mat = perlin_noise_2d(x, xx, yy);
					f_mat++;
				}
			}
		}
		else {
			luaL_error(L, "meshfx.noise.perlin: invalid arguments");
		}
		return 0;
	}
]]


local noise_perlin_meta = codegen.generate(M, {
	name = "perlin",
	
	header = [[
		#define PERLIN_SIZE	4095
		#define PERLIN_MIN_AMPLITUDE 0.001
		
		static const int PERLIN_YWRAPB = 4;
		static const int PERLIN_YWRAP = 1 << PERLIN_YWRAPB;
		static const int PERLIN_ZWRAPB = 8;
		static const int PERLIN_ZWRAP = 1 << PERLIN_ZWRAPB;
		
		
		#define COS_LUT_PRECISION 0.25
		static float COS_LUT_INV_PREC = 0.;
		static int COS_LUT_PERIOD = 0;
		static float COS_LUT_DEG2RAD = 0.;
		static float COS_LUT_RAD2DEG = 0.;
		
		
		static int COS_LUT_PI = 0;
		static int COS_LUT_TWOPI = 0;
	]],
	
	parameters = {
		{name = "octaves", type = "int", default = 4},
		{name = "lacunarity", type = "float", default = 0.5},
		{name = "offset", type = "float[2]", default = {0., 0}},
		{name = "scale", type = "float[2]", default = {1., 1.}},
		
		{	
			name = "noise_lut", 
			type = "float *", 
			init = [[
				x->noise_lut = (float *)malloc(sizeof(float)*(PERLIN_SIZE+1));
				for(int i=0; i <= PERLIN_SIZE; i++) {
					x->noise_lut[i] = ((float)(rand()))/(float)RAND_MAX;
				}
			]],
			destroy = [[
				if(x->noise_lut) {
					free(x->noise_lut);
				}
			]],
		},
		
		{	
			name = "cos_lut", 
			type = "float *", 
			init = [[
				COS_LUT_INV_PREC = (float)(1.0/COS_LUT_PRECISION);
				COS_LUT_PERIOD = (int)(360.0f * COS_LUT_INV_PREC);
				COS_LUT_DEG2RAD = (float)(3.14159 / 180.0) * COS_LUT_PRECISION;
				COS_LUT_RAD2DEG = 1./COS_LUT_DEG2RAD;
				
				COS_LUT_TWOPI = COS_LUT_PERIOD;
				COS_LUT_PI = COS_LUT_PERIOD;
				COS_LUT_PI >>= 1;
				
				x->cos_lut = (float *)malloc(sizeof(float)*(COS_LUT_PERIOD));
				for(int i=0; i < COS_LUT_PERIOD; i++) {
					x->cos_lut[i] = cosf(((float)i)*COS_LUT_DEG2RAD);
				}
			]],
			destroy = [[
				if(x->cos_lut) {
					free(x->cos_lut);
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

	m.struct = noise_perlin_meta.new()

	return m
end

function M:calc()
	self.struct:calc(self)
end