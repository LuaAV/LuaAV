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

module('matrixfx.contours', setconstructor)
-----------------------------------------------------

local calc_code = [[
	#define MAX_NUM_CONTOURS_TO_FIND 128
	
	int $name{}_calc(lua_State *L) {
		$name x = $name{}_to_udata(L, 1);
		void *in_mat = muro_matrix_udata_to_base(L, 2);
		if(x && in_mat) {
			Header *in_h = muro_matrix_header(in_mat);
			
			if(in_h->planes != 1) {
				luaL_error(L, "matrixfx.$name: input planes must be 1");
			}
			
			if(! in_h->type == UCHAR) {
				luaL_error(L, "matrixfx.$name: input type must be char");
			}
			
			if(in_h->dimcount != 2) {
				luaL_error(L, "matrixfx.$name: dimcount must be 2");
			}
			
			CvMat cv_in;
			muro_mat_to_opencv(&cv_in, in_mat);
			
			cvClearMemStorage(x->storage);
			CvSeq *first_contour = 0;
			int mode = x->find_holes ? CV_RETR_LIST : CV_RETR_EXTERNAL;
			
			
			cvFindContours(
				&cv_in, 
				x->storage, 
				&first_contour, 
				sizeof(CvContour),
				mode,
				CV_CHAIN_APPROX_SIMPLE,
				cvPoint(0, 0)
			);
			
			
			int num_contours = 0;		
			if(first_contour) {
				CvSeq *contour = first_contour;
				t_blob *blobs = x->blobs;
				float min_area = x->min_area;
				float max_area = x->max_area;
				
				while(contour) {
					float area = cvContourArea(contour, CV_WHOLE_SEQ);
					float abs_area = fabs(area);
					
					if((abs_area > min_area) && (abs_area < max_area)) {
						if(num_contours < MAX_NUM_CONTOURS_TO_FIND){
							blobs[num_contours].contour = contour;
							blobs[num_contours].area = area;
							num_contours++;
						}
					}
					
					contour = contour->h_next;
				}
			}
			x->num_contours = num_contours;

		}
		else {
			luaL_error(L, "matrixfx.$name: invalid matrix arguments");
		}

		return 0;
	}

/*	
#define CV_READ_SEQ_ELEMX( elem, reader )                       \
{                                                              \
    memcpy( &(elem), (reader).ptr, sizeof((elem)));            \
    CV_NEXT_SEQ_ELEM( sizeof(elem), reader )                   \
}
*/
	
	int $name{}_geometry(lua_State *L) {
		$name x = $name{}_to_udata(L, 1);
//		void *vertex_mat = muro_matrix_udata_to_base(L, 2);
//		void *index_mat = muro_matrix_udata_to_base(L, 2);
		if(x /*&& vertex_mat && index_mat*/) {
			/*
			int total_elems = 0;
			t_blob *blobs = x->blobs;
			for(int i=0; i < x->num_contours; i++) {
				total_elems += blobs[i].contour->total;
			}
			
			muro_matrix_adapt_2d(vertex_mat, 3, FLOAT32, total_elems, 1);
			*/
			
			t_blob *blobs = x->blobs;
			for(int i=0; i < x->num_contours; i++) {
				glBegin(GL_LINE_LOOP);
				
				CvPoint pt;	
				CvSeqReader reader;
				cvStartReadSeq(blobs[i].contour, &reader, 0);
				
				int npts = blobs[i].contour->total;
				for(int j=0; j < npts; j++) {
					//CV_READ_SEQ_ELEMX(pt, reader);
					muro_cv_read_seq_pt(&pt, &reader);
					glVertex2f((float)pt.x, (float)pt.y);
				}
				glEnd();
			}
		}
		else {
			luaL_error(L, "matrixfx.$name: invalid matrix arguments");
		}
		return 0;
	}
]]


local opencv_contours_meta = codegen.generate(M, {
	name = "contours",
	
	header = [[
		#include "glo_c.h"
	
		typedef struct _blob {
			CvSeq *contour;
			float area;
		} t_blob;
	]],
	
	params = {
		{name = "find_holes", type = "char", default = 0},
		{name = "min_area", type = "float", default = 1},
		{name = "max_area", type = "float", default = 25600},
		{name = "num_contours", type = "int", default = 0},
		{	
			name = "storage", 
			type = "CvMemStorage *", 
			init = [[
				x->storage = cvCreateMemStorage(0);
			]],
			destroy = [[
				if(x->storage) {
					cvReleaseMemStorage(&(x->storage));
				}
			]],
		},
		{	
			name = "blobs[128]", 
			type = "t_blob", 
			init = "",
			destroy = "",
		},
	},
	
	methods = {
		"geometry"
	},
		
	calc = calc_code,
})


function C:__call(t)
	t = t or {}
	
	-- setup an instance of the fx
	local m = Matrix()
	for k, v in pairs(M) do
		if(type(v) == "function") then
			m[k] = v
		end
	end
	
	m.copy = Matrix()
	m.struct = opencv_contours_meta.new()
	for k, v in pairs(t) do
		if(type(m[k]) == "function") then
			m[k](m, v)
		end
	end

	return m
end

function M:matrix_calc(inmat)
	self.copy:frommatrix(inmat)
	self.struct:calc(self.copy, self)
end

function M:draw()
	self.struct:geometry()
end