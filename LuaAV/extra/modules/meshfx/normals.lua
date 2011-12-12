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

local compiler = require("codegen.compiler")

local C = {}
local M

local
function setconstructor(m)
	M = m
	setmetatable(M, C)
end

module('meshfx.normals', setconstructor)
-----------------------------------------------------

local calc_open_code = [[	
	#include "lua.h"
	#include "matrix_c.h"
	#include "glo_c.h"
	#include "vec3_c.h"
	#include "stdlib.h"
	
	int normals_calc_open(lua_State *L) {
		void *norms = muro_matrix_udata_to_base(L, 1);
		void *pos = muro_matrix_udata_to_base(L, 2);
		if(pos && norms) {
			Header *pos_h = muro_matrix_header(pos);
			
			if(pos_h->planes != 3) {
				luaL_error(L, "meshfx.normals: input planes must be 3");
			}
			
			if(pos_h->type != FLOAT32) {
				luaL_error(L, "meshfx.normals: input type must be float32");
			}

			muro_matrix_adapt(norms, pos_h);
			
			int width = pos_h->dim[0];
			int height = pos_h->dim[1];
			int rowstride = muro_matrix_rowsize(pos);
			
			char *p_pos = muro_matrix_data(pos);
			char *p_norms = muro_matrix_data(norms);
			
			for(int j=0; j < height-1; j++) {
				vec3 *p1 = (vec3 *)(p_pos + j*rowstride);
				vec3 *p3 = (vec3 *)(p_pos + (j+1)*rowstride);
				vec3 *n = (vec3 *)(p_norms + j*rowstride);
				
				for(int i=0; i < width-1; i++) {
					vec3 *p2 = p1+1;
					
					muro_vec3_normal(n, p1, p3, p2);
					muro_vec3_normalize(n);
					
					p1++;
					p3++;
					n++;
				}
			}
			
			{
				vec3 *n1 = (vec3 *)(p_norms + (height-3)*rowstride);
				vec3 *n2 = (vec3 *)(p_norms + (height-2)*rowstride);
				vec3 *n = (vec3 *)(p_norms + (height-1)*rowstride);
				
				for(int i=0; i < width-1; i++) {
					muro_vec3_sub(n, n2, n1);					
					muro_vec3_add(n, n, n2);
					muro_vec3_normalize(n);
					
					n->x = 1.;
					n->y = 1.;
					n->z = 0.;
					
					n1++;
					n2++;
					n++;
				}
			}
			
			for(int j=0; j < height-1; j++) {
				vec3 *n1 = ((vec3 *)(p_norms + j*rowstride))+width-3;
				vec3 *n2 = ((vec3 *)(p_norms + j*rowstride))+width-2;
				vec3 *n = ((vec3 *)(p_norms + j*rowstride))+width-1;
			
				muro_vec3_sub(n, n2, n1);					
				muro_vec3_add(n, n, n2);
				muro_vec3_normalize(n);
				
				n1++;
				n2++;
				n++;
			}
			
			{
				vec3 *n11 = ((vec3 *)(p_norms + (height-3)*rowstride))+width-1;
				vec3 *n12 = ((vec3 *)(p_norms + (height-2)*rowstride))+width-1;
				
				vec3 *n21 = ((vec3 *)(p_norms + (height-1)*rowstride))+width-3;
				vec3 *n22 = ((vec3 *)(p_norms + (height-1)*rowstride))+width-2;
				
				vec3 *n = ((vec3 *)(p_norms + (height-1)*rowstride))+width-1;
				
				vec3 nn1, nn2;
				muro_vec3_sub(&nn1, n12, n11);
				muro_vec3_add(&nn1, &nn1, n12);
				
				muro_vec3_sub(&nn2, n22, n21);
				muro_vec3_add(&nn2, &nn2, n22);
				
				muro_vec3_lerp(n, &nn1, &nn2, 0.5);
				muro_vec3_normalize(n);
			}
		}
		else {
			luaL_error(L, "meshfx.normals: invalid arguments");
		}
		return 0;
	}
]]

local calc_closed_code = [[	
	#include "lua.h"
	#include "matrix_c.h"
	#include "vec3_c.h"
	#include "stdlib.h"
	
	int normals_calc_closed(lua_State *L) {
		void *norms = muro_matrix_udata_to_base(L, 1);
		void *pos = muro_matrix_udata_to_base(L, 2);
		if(pos && norms) {
			Header *pos_h = muro_matrix_header(pos);
			
			if(pos_h->planes != 3) {
				luaL_error(L, "meshfx.normals: input planes must be 3");
			}
			
			if(pos_h->type != FLOAT32) {
				luaL_error(L, "meshfx.normals: input type must be float32");
			}

			muro_matrix_adapt(norms, pos_h);
			
			int width = pos_h->dim[0];
			int height = pos_h->dim[1];
			int rowstride = muro_matrix_rowsize(pos);
			
			char *p_pos = muro_matrix_data(pos);
			char *p_norms = muro_matrix_data(norms);
			
			for(int j=0; j < height-1; j++) {
				vec3 *p1 = (vec3 *)(p_pos + j*rowstride);
				vec3 *p3 = (vec3 *)(p_pos + (j+1)*rowstride);
				vec3 *n = (vec3 *)(p_norms + j*rowstride);
				
				for(int i=0; i < width-1; i++) {
					vec3 *p2 = p1+1;
					
					muro_vec3_normal(n, p1, p3, p2);
					muro_vec3_normalize(n);
					
					p1++;
					p3++;
					n++;
				}
			}
			
			{
				vec3 *p1 = (vec3 *)(p_pos + (height-1)*rowstride);
				vec3 *p3 = (vec3 *)(p_pos);
				vec3 *n = (vec3 *)(p_norms + (height-1)*rowstride);
				
				for(int i=0; i < width-1; i++) {
					vec3 *p2 = p1+1;
					
					muro_vec3_normal(n, p1, p3, p2);
					muro_vec3_normalize(n);
					
					p1++;
					p3++;
					n++;
				}
			}
			
			for(int j=0; j < height-1; j++) {
				vec3 *p1 = ((vec3 *)(p_pos + j*rowstride))+width-1;
				vec3 *p2 = ((vec3 *)(p_pos + j*rowstride))+1;	// for 0 length
				vec3 *p3 = ((vec3 *)(p_pos + (j+1)*rowstride))+width-1;
				vec3 *n = ((vec3 *)(p_norms + j*rowstride))+width-1;
				
				muro_vec3_normal(n, p1, p3, p2);
				muro_vec3_normalize(n);
			}
			
			{
				vec3 *p1 = ((vec3 *)(p_pos + (height-1)*rowstride))+width-1;
				vec3 *p2 = (vec3 *)(p_pos);
				vec3 *p3 = (vec3 *)(p_pos + rowstride);
				vec3 *n = ((vec3 *)(p_norms + (height-1)*rowstride))+width-1;
				
				muro_vec3_normal(n, p1, p3, p2);
				muro_vec3_normalize(n);
			}
		}
		else {
			luaL_error(L, "meshfx.normals: invalid arguments");
		}
		return 0;
	}
]]


local normals_calc_open = compiler.compile("normals_calc_open", calc_open_code)
assert(normals_calc_open, "error compiling meshfx.normals normals_calc_open")

local normals_calc_closed = compiler.compile("normals_calc_closed", calc_closed_code)
assert(normals_calc_closed, "error compiling meshfx.normals normals_calc_closed")

function C:__call(...)
	local v = {...}
	local info
	if(type(v[1]) == "userdata") then
		info = { v[1]:info() }
	else
		info = v
	end
	
	-- setup an instance of the fx
	local m = Matrix(unpack(info))
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end

	if(type(v[1]) == "userdata") then
		m:calc(unpack(v))
	end

	return m
end

function M:calc(pos, closed)
	if(closed) then
		normals_calc_closed(self, pos)
	else
		normals_calc_open(self, pos)
	end
end