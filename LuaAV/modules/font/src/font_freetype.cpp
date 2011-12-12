#include "font_freetype.h"

#include "Font.h"
#include <vector>

#if defined(WIN32) || defined(__WINDOWS_MM__)
    // Windows
	#include <windows.h>
	#include <gl/glu.h>
#elif defined( __APPLE__ ) && defined( __MACH__ ) || defined (OSX)
    // OSX
	#include <OpenGL/glu.h>
#else
    // Linux
	#include <GL/glu.h>
#endif

typedef void (*GLUTessCallback)();

struct vec2 {
	vec2(double x=0., double y=0.)
	:	x(x),
		y(y),
		z(0)
	{}
	
	vec2(FT_Vector *v)
	:	x(v->x),
		y(v->y),
		z(0)
	{}
	
	vec2(const vec2 &v)
	:	x(v.x),
		y(v.y),
		z(0)
	{}
	
	vec2(double *v)
	:	x(v[0]),
		y(v[1]),
		z(0)
	{}

	double x;
	double y;
	double z;
};

using std::vector;

FontFreetype :: FontFreetype(FT_Face face, int font_size, bool anti_aliased)
:	Font(font_size, anti_aliased),
	mFace(face)
{
	if(FT_Set_Pixel_Sizes(mFace, 0, font_size)) {
		fprintf(stderr, "font: error setting font size");
	}
}

FontFreetype :: ~FontFreetype() {
	FT_Done_Face(mFace);
}

FT_Face FontFreetype :: open_face(const char *filename) {
	FT_Face face = 0;
	FT_Error err = FT_New_Face(front_freetype_library, filename, 0, &face);
	/*
	if(err) {
		fprintf(stderr, "font: error loading font face for %s\n", filename);
		return 0;
	}
	else {
		return face;
	}
	*/
	if(err) {
		return 0;
	}
	else {
		return face;
	}
}

al::ArrayWrapper * FontFreetype :: get_ascii_chars() {
	if(! mAsciiChars) {
		mAsciiChars = new al::ArrayWrapper(1);
		
		AlloArrayHeader h;
		h.components = 1;
		h.type = AlloUInt8Ty;
		h.dim[0] = (mFontSize+2)*GLYPHS_PER_ROW;
		h.dim[1] = (mFontSize+2)*(ASCII_SIZE/GLYPHS_PER_ROW);
		h.dimcount = 2;
		allo_array_setstride(&h, 1);
		
		allo_array_create(mAsciiChars, &h);
		memset(mAsciiChars->data.ptr, '\0', allo_array_size(mAsciiChars));
		
		int rowstride = mAsciiChars->header.stride[1];
		
		for(int i=0; i < ASCII_SIZE; i++) {
			load_glyph(i);
			set_char_with_glyph(i);
			
			int xidx = i % GLYPHS_PER_ROW;
			int yidx = (int)((float)i/(float)GLYPHS_PER_ROW);
			
			int padding_x = 1;
			int padding_y = 1;
			int glyph_width = mFontSize+2*padding_x;
			int glyph_height = mFontSize+2*padding_y;
			
			
			unsigned char *pix = (unsigned char *)(
									mAsciiChars->data.ptr + 
									(padding_y + yidx*glyph_height)*rowstride + 
									padding_x + xidx*glyph_width
								);
								
			fill_image(pix, rowstride);
			
		}
	}
	
	return mAsciiChars;
}

void FontFreetype :: load_glyph(int idx) {
	int glyph_index = FT_Get_Char_Index(mFace, idx);
//	FT_Set_Char_Size(mFace, 0, mFontSize*PIX_TO_EM, 0, 0);

	if(mAntiAliased) {
		FT_Load_Glyph(mFace, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(mFace->glyph, FT_RENDER_MODE_NORMAL);
	}
	else {
		FT_Load_Glyph(mFace, glyph_index, FT_LOAD_MONOCHROME);
		FT_Render_Glyph(mFace->glyph, FT_RENDER_MODE_MONO);
	}
}

void FontFreetype :: set_char_with_glyph(int idx) {
	mChars[idx].width = mFace->glyph->advance.x/PIX_TO_EM;
	mChars[idx].y_offset = mFace->glyph->bitmap_top;
}
	
void FontFreetype :: fill_image(unsigned char *image, int rowstride) {
	FT_Bitmap *bitmap = &mFace->glyph->bitmap;
	
	if(mAntiAliased) {
		for(int j=0; j < bitmap->rows; j++) {
			unsigned char *pix = image + j*rowstride;
			unsigned char *font_pix = bitmap->buffer + j*bitmap->width;
			for(int k=0; k < bitmap->width; k++) {
				*pix++ = *font_pix++;
			}
		}
	}
}


typedef vec2 VertexInfo;

struct OutlineData{
	OutlineData(int nsteps=4)
	:	tesselator(0),
		scale(1.),
		contour_open(false),
		nsteps(nsteps),
		begin_idx(0),
		count(0),
		idx_count(0),
		prim(0)
	{
		delta = 1. / (double)nsteps;
		delta2 = delta * delta;
		delta3 = delta2 * delta;
	}
	
	GLUtesselatorObj *tesselator;
	vec2 last_vertex;
	double scale;
	bool contour_open;
	vector<VertexInfo *> vertices;
	vector<VertexInfo *> extra_vertices;
	vector<vec2> positions;
	vector<unsigned int> indices;
	int nsteps;
	double delta;
	double delta2;
	double delta3;
	int begin_idx;
	int count;
	int idx_count;
	GLenum prim;
};


static OutlineData *g_outline_data = 0;

void tess_vertex(VertexInfo* vertex) {
	g_outline_data->positions.push_back(vec2(*vertex));
	g_outline_data->count++;
}

//VertexInfoList extra_vertices_;
//VertexInfoList& extraVertices ( void ) { return extra_vertices_; }

void tess_combine_data(
	GLdouble coords[3], VertexInfo *vertex_data[4],
	GLfloat weight[4], VertexInfo **out_data,
	OutlineData *data
) {
	VertexInfo *n = new VertexInfo();
	n->x = coords[0];	
	n->y = coords[1];
	*out_data = n;
	
	/*
	printf("tess_combine_data\n");
	printf("coords: %f %f %f\n", coords[0], coords[1], coords[2]);
	
	if(vertex_data[0]) {
		printf("v1: %f %f\n", vertex_data[0]->x, vertex_data[0]->y);
	}
	if(vertex_data[1]) {
		printf("v2: %f %f\n", vertex_data[1]->x, vertex_data[1]->y);
	}
	if(vertex_data[2]) {
		printf("v3: %f %f\n", vertex_data[2]->x, vertex_data[2]->y);
	}
	if(vertex_data[3]) {
		printf("v4: %f %f\n", vertex_data[3]->x, vertex_data[3]->y);
	}
	printf("weight: %f %f %f %f\n", weight[0], weight[1], weight[2], weight[3]);
	*/
	g_outline_data->extra_vertices.push_back(n);

	/*
	(void)vertex_data;
	(void)weight;
	//    std::cerr << "called combine" << std::endl;
	VertexInfo* vertex = new VertexInfo( coords );
	*out_data = vertex;
	filled->extraVertices().push_back( vertex );
	*/
}

void tess_begin(GLenum prim) {
	g_outline_data->prim = prim;
	g_outline_data->begin_idx = g_outline_data->count;
}

void tess_end(void) {
	int begin_idx = g_outline_data->begin_idx;
	int count = g_outline_data->count;
	
//	if(begin_idx == 31) {
	
	switch(g_outline_data->prim) {
		case GL_TRIANGLES:
			for(unsigned int i=begin_idx; i < count; i++) {
				g_outline_data->indices.push_back(i);
			}
			break;
			
		case GL_TRIANGLE_FAN:
			for(unsigned int i=begin_idx+1; i < count-1; i++) {
				g_outline_data->indices.push_back(begin_idx);
				g_outline_data->indices.push_back(i);
				if(i+1 >= count) {
					g_outline_data->indices.push_back(begin_idx+1);
				}
				else {
					g_outline_data->indices.push_back(i+1);
				}
			}
			break;
			
		case GL_TRIANGLE_STRIP:
			for(unsigned int i=0; i < count-begin_idx-2; i++) {
				g_outline_data->indices.push_back(i+begin_idx);
				g_outline_data->indices.push_back(i+begin_idx+1);
				g_outline_data->indices.push_back(i+begin_idx+2);
			}
			break;
			
		case GL_QUADS:
			for(unsigned int i=begin_idx; i < count; i++) {
				g_outline_data->indices.push_back(i);
			}
			break;
			
		case GL_QUAD_STRIP:
			
			break;
	}
//	}
}

void tess_error(GLenum error_code) {
	printf("GluTess Error: %s\n", gluErrorString(error_code));
}


int moveto_callback(FT_Vector* to, OutlineData* data) {
	if(data->contour_open) {
		gluTessEndContour(data->tesselator);
	}

	data->last_vertex.x = to->x;
	data->last_vertex.y = to->y;
	gluTessBeginContour(data->tesselator);
	data->contour_open = true;

	return 0;
}

int lineto_callback(FT_Vector* to, OutlineData* data) {
	data->last_vertex.x = to->x;
	data->last_vertex.y = to->y;

	VertexInfo* vertex = new VertexInfo(to);
	g_outline_data->vertices.push_back(vertex);

	vertex->x *= data->scale;
	vertex->y *= data->scale;

	gluTessVertex(
		data->tesselator,
		&(vertex->x),
		vertex
	);

	return 0;
}

int conicto_callback(
	FT_Vector* control, 
	FT_Vector* to, 
	OutlineData* data
) {
	// This is crude: Step off conics with a fixed number of increments
	VertexInfo to_vertex(to);
	VertexInfo control_vertex(control);

	vec2 b(
		data->last_vertex.x - 2 * control_vertex.x + to_vertex.x,
		data->last_vertex.y - 2 * control_vertex.y + to_vertex.y
	);

	vec2 c(
		-2 * data->last_vertex.x + 2 * control_vertex.x,
		-2 * data->last_vertex.y + 2 * control_vertex.y
	);

	vec2 d(data->last_vertex.x, data->last_vertex.y);
	vec2 f(d.x, d.y);

	vec2 df(
		c.x * data->delta + b.x * data->delta2,
		c.y * data->delta + b.y * data->delta2
	);
	
	vec2 d2f(
		2 * b.x * data->delta2,
		2 * b.y * data->delta2
	);

	for(int i=0; i < data->nsteps-1; i++) {
		f.x += df.x;
		f.y += df.y;

		VertexInfo *vertex = new VertexInfo(f);
		g_outline_data->vertices.push_back(vertex);

		vertex->x *= data->scale;
		vertex->y *= data->scale;

		gluTessVertex(
			data->tesselator,
			&(vertex->x),
			vertex
		);

		df.x += d2f.x;
		df.y += d2f.y;
	}

	VertexInfo *vertex = new VertexInfo(to);
	g_outline_data->vertices.push_back(vertex);

	vertex->x *= data->scale;
	vertex->y *= data->scale;

	gluTessVertex(
		data->tesselator,
		&(vertex->x),
		vertex
	);

	data->last_vertex.x = to_vertex.x;
	data->last_vertex.y = to_vertex.y;

	return 0;
}

int cubicto_callback(
	FT_Vector* control1, 
	FT_Vector* control2,
	FT_Vector* to, 
	OutlineData* data
) {
	// This is crude: Step off cubics with a fixed number of increments
	VertexInfo to_vertex(to);
	VertexInfo control1_vertex(control1);
	VertexInfo control2_vertex(control2);

	//double b[2], c[2], d[2], f[2], df[2], d2f[2], d3f[2];

	vec2 a(
		-data->last_vertex.x + 3 * control1_vertex.x
			-3 * control2_vertex.x + to_vertex.x,
		-data->last_vertex.y + 3 * control1_vertex.y
			-3 * control2_vertex.y + to_vertex.y
	);

	vec2 b(
		3 * data->last_vertex.x - 6 * control1_vertex.x +
			3 * control2_vertex.x,
		3 * data->last_vertex.y - 6 * control1_vertex.y +
			3 * control2_vertex.y
	);

	vec2 c(
		-3 * data->last_vertex.x + 3 * control1_vertex.x,
		-3 * data->last_vertex.y + 3 * control1_vertex.y
	);
	
	
	vec2 d(data->last_vertex);
	vec2 f(d);

	vec2 df(
		c.x * data->delta + b.x * data->delta2
			+ a.x * data->delta3,
		c.y * data->delta + b.y * data->delta2
			+ a.y * data->delta3
	);
	
	vec2 d2f(
		2 * b.x * data->delta2 + 6 * a.x * data->delta3,
		2 * b.y * data->delta2 + 6 * a.y * data->delta3
	);

	vec2 d3f(
		6 * a.x * data->delta3,
		6 * a.y * data->delta3
	);

	for(int i = 0; i < data->nsteps-1; i++ ) {
		f.x += df.x;
		f.y += df.y;

		VertexInfo *vertex = new VertexInfo(f);
		g_outline_data->vertices.push_back(vertex);

		vertex->x *= data->scale;
		vertex->y *= data->scale;

		gluTessVertex(
			data->tesselator,
			&(vertex->x),
			vertex
		);

		df.x += d2f.x;
		df.y += d2f.y;
		d2f.x += d3f.x;
		d2f.y += d3f.y;
	}

	VertexInfo *vertex = new VertexInfo(to);
	g_outline_data->vertices.push_back(vertex);

	vertex->x *= data->scale;
	vertex->y *= data->scale;

	gluTessVertex(
		data->tesselator,
		&(vertex->x),
		vertex
	);

	data->last_vertex.x = to_vertex.x;
	data->last_vertex.y = to_vertex.y;

	return 0;
}

int FontFreetype  :: outline(int idx, AlloArray *vertex, AlloArray *index) {
	int glyph_index = FT_Get_Char_Index(mFace, idx);
	FT_Load_Glyph(mFace, glyph_index, FT_LOAD_DEFAULT);

	if(! mFace->glyph->format & FT_GLYPH_FORMAT_OUTLINE) {
		return 0;
	}

	FT_OutlineGlyph g;
	FT_Get_Glyph(mFace->glyph, (FT_Glyph *)&g );
	
	FT_Outline_Funcs outline_funcs;
	outline_funcs.move_to = (FT_Outline_MoveTo_Func)moveto_callback;
    outline_funcs.line_to = (FT_Outline_LineTo_Func)lineto_callback;
    outline_funcs.conic_to = (FT_Outline_ConicTo_Func)conicto_callback;
    outline_funcs.cubic_to = (FT_Outline_CubicTo_Func)cubicto_callback;
    outline_funcs.shift = 0;
    outline_funcs.delta = 0;

	OutlineData outline_data;
	outline_data.scale = 0.002;
	outline_data.tesselator = gluNewTess();
    gluTessCallback(outline_data.tesselator, GLU_TESS_VERTEX, (GLUTessCallback)tess_vertex);
    gluTessCallback(outline_data.tesselator, GLU_TESS_BEGIN, (GLUTessCallback)tess_begin);
    gluTessCallback(outline_data.tesselator, GLU_TESS_END, (GLUTessCallback)tess_end);
    gluTessCallback(outline_data.tesselator, GLU_TESS_COMBINE_DATA, (GLUTessCallback)tess_combine_data);
    gluTessCallback(outline_data.tesselator, GLU_TESS_ERROR, (GLUTessCallback)tess_error);

	/*
	if ( resolution_ != 0 )
      vector_scale_ = (GLdouble)( point_size_ * resolution_ ) /
	(GLdouble)( faces_.front().face_->units_per_EM * 72 );
    else // According to the FreeType documentation, resolution == 0 -> 72 DPI
      vector_scale_ = (GLdouble)( point_size_ ) /
	(GLdouble)( faces_.front().face_->units_per_EM );
	*/
	
	g_outline_data = &outline_data;
	
	gluTessBeginPolygon(outline_data.tesselator, &outline_data);
	FT_Outline_Decompose(&(g->outline), &outline_funcs, &outline_data);
	if(outline_data.contour_open) {
		gluTessEndContour(outline_data.tesselator);
	}
	gluTessEndPolygon(outline_data.tesselator);
	
	g_outline_data = 0;;
	gluDeleteTess(outline_data.tesselator);
	outline_data.tesselator = 0;
	
	FT_Done_Glyph((FT_Glyph)g);
	
	AlloArrayHeader h;
	h.components = 3;
	h.type = AlloFloat32Ty;
	h.dimcount = 2;
	h.dim[0] = outline_data.positions.size();
	h.dim[1] = 1;
	allo_array_setstride(&h, 1);
	allo_array_adapt(vertex, &h);
	
	float *vdata = (float *)vertex->data.ptr;
	for(int i=0; i < outline_data.positions.size(); i++) {
		vec2 &v = outline_data.positions[i];
		
		*vdata++ = (float)v.x;
		*vdata++ = (float)v.y;
		*vdata++ = 0.;
	}
	
	h.components = 1;
	h.type = AlloUInt32Ty;
	h.dimcount = 2;
	h.dim[0] = outline_data.indices.size();
	h.dim[1] = 1;
	allo_array_setstride(&h, 1);
	allo_array_adapt(index, &h);
	
	unsigned int *idata = (unsigned int *)index->data.ptr;
	for(int i=0; i < outline_data.indices.size(); i++) {
		*idata++ = outline_data.indices[i];
	}
	
	for(int i=0; i < outline_data.vertices.size(); i++) {
		delete outline_data.vertices[i];
	}
	
	for(int i=0; i < outline_data.extra_vertices.size(); i++) {
		delete outline_data.extra_vertices[i];
	}

	return 0;
}

float FontFreetype :: width(const char *text) {
	float total = 0.f;

	for (int i=0; i < (int)strlen(text); i++) {
		total += mChars[text[i]].width;
	}

	return total;
}
