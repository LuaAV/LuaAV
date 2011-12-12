local environment = require("codegen.environment")
local function_rule = require("codegen.function")
local declaration = require("codegen.declaration")
local cosmo = require("cosmo")
local compiler = require("codegen.compiler")
local format = string.format
local max = math.max
local min = math.min

module(..., package.seeall)

local template = [====[
#include "lua.h"
#include "matrix_c.h"
#include "math.h"

typedef unsigned char uchar;

void $name{}_perform(int nin, void **m_in, int nout, void **m_out);


int $name{}_calc(lua_State *L) {
	if(lua_istable(L, 1) && lua_istable(L, 2)) {
		void *m_in[$nin];
		void *m_out[$nout];
		
		// verify ins and outs
		int in_size = lua_objlen(L, 1);
		int out_size = lua_objlen(L, 2);
		
		if(in_size < $nin) {
			luaL_error(L, "$name.calc: not enough inputs");
		}
		
		if(out_size < $nout) {
			luaL_error(L, "$name.calc: not enough inputs");
		}
		
		// get in and out matrices
		for(int i=0; i < $nin; i++) {
			lua_rawgeti(L, 1, i+1);
			void *m = muro_matrix_udata_to_base(L, -1);
			if(m) {
				m_in[i] = m;
			}
			else {
				luaL_error(L, "$name.calc: invalid input matrix");
			}
		}
		lua_pop(L, $nin);
		
		for(int i=0; i < $nout; i++) {
			lua_rawgeti(L, 2, i+1);
			void *m = muro_matrix_udata_to_base(L, -1);
			if(m) {
				m_out[i] = m;
			}
			else {
				luaL_error(L, "$name.calc: invalid output matrix");
			}
		}
		lua_pop(L, $nout);
		
		
		// perform calculation
		$name{}_perform($nin, m_in, $nout, m_out);
	}
	else {
		luaL_error(L, "$name.calc: invalid arguments");
	}
	return 0;
}

int $name{}_predicates(int nin, void **m_in, int nout, void **m_out) {
	if(nin < $nin || nout < $nout) {
		// error not enough ins or outs
		printf("not enough ins or outs\n");
		return 1;
	}
	
	if(nin > 0) {
		if(! muro_matrix_equal_planes(m_in[0], m_out[0])) {
			// error not equal planes
			printf("not equal planes\n");
			return 1;
		}
		
		if(! muro_matrix_equal_type(m_in[0], m_out[0])) {
			// error not equal type
			printf("not equal type\n");
			return 1;
		}
		
		if(! muro_matrix_equal_dim(m_in[0], m_out[0])) {
			// error not equal dim
			printf("not equal dim\n");
			return 1;
		}
	}
	
	if(nin > 1) {
		if(! muro_matrices_equal_planes(nin, m_in)) {
			// error not equal planes
			printf("input not equal planes\n");
			return 1;
		}
		
		if(! muro_matrices_equal_type(nin, m_in)) {
			// error not equal type
			printf("input not equal type\n");
			return 1;
		}
		
		if(! muro_matrices_equal_dim(nin, m_in)) {
			// error not equal dim
			printf("input not equal dim\n");
			return 1;
		}
	}
	
	if(nout > 1) {
		if(! muro_matrices_equal_planes(nout, m_out)) {
			// error not equal planes
			printf("output not equal planes\n");
			return 1;
		}
		
		if(! muro_matrices_equal_type(nout, m_out)) {
			// error not equal type
			printf("output not equal type\n");
			return 1;
		}
		
		if(! muro_matrices_equal_dim(nout, m_out)) {
			// error not equal dim
			printf("output not equal dim\n");
			return 1;
		}
	}
	return 0;
}


void $name{}_perform(int nin, void **m_in, int nout, void **m_out) {
	
	// check dims, planecount etc.
	if($name{}_predicates(nin, m_in, nout, m_out)) {
		// error
		printf("predicates error\n");
		return;
	}
	
	Header *h_out0 = (Header *)muro_matrix_header(m_out[0]);
	int width = h_out0->dim[0];
	int height = (h_out0->dimcount > 1) ? h_out0->dim[1] : 1;
	
	$do_inputs[[
	
	int rowstride_in$idx = muro_matrix_rowsize(m_in[$idx]);
	int planes_in$idx = h_out0->planes;
	char *ptr_in$idx = (char *)muro_matrix_data(m_in[$idx]);
	]]
	
	$do_outputs[[
	
	int rowstride_out$idx = muro_matrix_rowsize(m_out[$idx]);
	int planes_out$idx = h_out0->planes;
	char *ptr_out$idx = (char *)muro_matrix_data(m_out[$idx]);
	]]
	

	for(int j=0; j < height; j++) {
		$do_inputs[[$type *in$idx = ($type *)(ptr_in$idx + j*rowstride_in$idx);
		]]
		$do_outputs[[$type *out$idx = ($type *)(ptr_out$idx + j*rowstride_out$idx);
		]]
	
		for(int i=0; i < width; i++) {
			$expr
			$do_inputs[[in$idx += planes_in$idx;
			]]
			$do_outputs[[out$idx += planes_out$idx;
			]]
		}
	}
}
]====]

local math_h = [[
extern float acos(float x);
extern float asin(float x);
extern float atan(float x);
extern float atan2(float x, float y);
extern float ceil(float x);
extern float cos(float x);
extern float cosh(float x);
extern float exp(float x);
extern float fabs(float x);
extern float floor(float x);
extern float fmod(float x, float y);
extern float log(float x);
extern float log10(float x);
extern float pow(float x, float y);
extern float sin(float x);
extern float sinh(float x);
extern float sqrt(float x);
extern float tan(float x);
extern float tanh(float x);
extern float hypot(float y, float x);
]]

local
function add_definitions(env, code)
	local parser = env:get_parser()
	local unit = parser:parse(code)
	for _, u in ipairs(unit) do
		if(u.rule == "declaration") then
			declaration.simplify(env, u)
		elseif(u.rule == "function_definition") then
			function_rule.simplify(env, u)
		else
			error(format("%s not a recognized rule", u.rule))
		end
	end
end

function generate(name, header, code)
	local env = environment()
	local parser = env:get_parser()

	for i=2, 4 do
		env:add_type("vec"..i, header.type, i)
	end
	
	local celltype = "vec"..header.planecount
	for i=0, 3 do
		env:add_symbol("in"..i, celltype)
		env:add_symbol("out"..i, celltype)
	end
	
	-- add external definitions
	add_definitions(env, math_h)

	-- parse the code
	local unit = parser:parse(code)
	
	-- simplify the main function
	local f = function_rule.simplify(env, unit[1])
	local body = f:get_body()
	
	-- get the number of ins and outs
	local nin = 0
	local nout = 1
	for e in body:gmatch("identifier") do
		local name = e[1]
		local n, idx = name:match("([inout]+)(%d+)")
		if(n and idx) then
			if(n == "in") then
				nin = max(tonumber(idx)+1, nin)
			elseif(n == "out") then
				nout = max(tonumber(idx)+1, nout)
			end
		end
	end
	
	-- annotate the code with type information
	body:mark_types()

	-- expand vector ops into scalar ops
	body:scalarize()

	-- generate the code
	local expr = ""
	for i=1, body:number_of_statements() do
		local statement = body:statement_at_index(i)
		expr = format("%s\n%s", expr, statement:code())
	end

	local code = cosmo.f(template){
		name = name,
		nin = nin,
		nout = nout,
		expr = expr,
		type = header.type,
		
		do_inputs = function()
			for i=0, nin-1 do
				cosmo.yield{
					idx = i,
				}
			end
		end,
		
		do_outputs = function()
			for i=0, nout-1 do
				cosmo.yield{
					idx = i,
				}
			end
		end,
	}
	
	print(code)
	
	return compiler.compile(name.."_calc", code)
end