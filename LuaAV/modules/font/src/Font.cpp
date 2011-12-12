#include "Font.h"
#include <vector>


Font :: Font(int font_size, bool anti_aliased)
:	mFontSize(font_size),
	mAntiAliased(anti_aliased),
	mAsciiChars(NULL)
{}

Font :: ~Font() {
	if(mAsciiChars) {
		mAsciiChars->release();
		mAsciiChars = NULL;
	}
}


void Font::render(const char *text, AlloArray *vertex, AlloArray *texcoord) {
	if(
		vertex->header.components == 3 &&
		vertex->header.type == AlloFloat32Ty && 
		
		texcoord->header.components == 2 &&
		texcoord->header.type == AlloFloat32Ty
	) {
		int max_chars = MIN(vertex->header.dim[0], texcoord->header.dim[0])/4;
		int nchars = MIN(strlen(text), max_chars);
		float margin = 2.;
		float csz = (float)mFontSize;
		float cdim = csz+margin;
		float tdim = cdim*GLYPHS_PER_ROW;
		float tcdim = ((float)cdim)/((float)tdim);
		
		float pos[] = {0., 0.};
		
		float *v_data = (float *)(vertex->data.ptr);
		float *tc_data = (float *)(texcoord->data.ptr);
		
		for(int i=0; i < nchars; i++) {
			int idx = text[i];
			const FontCharacter &c = mChars[idx];
			/*int margin = 1;*/
			
			int xidx = idx % GLYPHS_PER_ROW;
			int yidx = idx/GLYPHS_PER_ROW;
			float tc_x = ((float)(xidx))*tcdim;
			float tc_y = ((float)(yidx))*tcdim;
			float yy = c.y_offset;
			
			float tc_right = tc_x+tcdim;
			float tc_bot = tc_y+tcdim;
			
			// texcoords
			*tc_data++ = tc_x;
			*tc_data++ = tc_y;
			
			*tc_data++ = tc_right;
			*tc_data++ = tc_y;
			
			*tc_data++ = tc_right;
			*tc_data++ = tc_bot;
			
			*tc_data++ = tc_x;
			*tc_data++ = tc_bot;
			
			
			
			float pos_right = pos[0]+cdim;
			float pos_top = pos[1]-margin-yy;
			float pos_bot = pos[1]+csz-yy;
			
			// vertices
			*v_data++ = pos[0];
			*v_data++ = pos_top;
			*v_data++ = 0.;
			
			*v_data++ = pos_right;
			*v_data++ = pos_top;
			*v_data++ = 0.;
			
			*v_data++ = pos_right;
			*v_data++ = pos_bot;
			*v_data++ = 0.;
			
			*v_data++ = pos[0];
			*v_data++ = pos_bot;
			*v_data++ = 0.;
			
			pos[0] += (float)c.width;
		}
	}
	else {
		fprintf(stderr, "Font::render: invalid array formats");
	}
}