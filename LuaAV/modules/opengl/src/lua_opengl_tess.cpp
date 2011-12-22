#include "lua_opengl.h"
#include <vector>
#include "lua_glue.h"
#include "lua_utility.h"
#include "lua_array.h"

using std::vector;


typedef void (*GLUTessCallback)();

struct vec2 {
	vec2(double x=0., double y=0.)
	:	x(x),
		y(y),
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

struct Tesselator{
	Tesselator();
	~Tesselator();
	
	void BeginPolygon();
	void EndPolygon();
	void BeginContour();
	void EndContour();
	void Vertex(vec2 &v);
	void geometry(AlloArray *vertex, AlloArray *index);
	
	GLUtesselatorObj *tesselator;
	vector<vec2 *> vertices;
	vector<vec2> positions;
	vector<unsigned int> indices;
	int begin_idx;
	int count;
	int idx_count;
	GLenum prim;
};


static Tesselator *g_tesselator = 0;

void tess_vertex(vec2* vertex) {
	g_tesselator->positions.push_back(vec2(*vertex));
	g_tesselator->count++;
}


void tess_combine_data(
	GLdouble coords[3], vec2 *vertex_data[4],
	GLfloat weight[4], vec2 **out_data,
	Tesselator *data
) {
	vec2 *n = new vec2(coords);
	*out_data = n;

	g_tesselator->vertices.push_back(n);
}

void tess_begin(GLenum prim) {
	g_tesselator->prim = prim;
	g_tesselator->begin_idx = g_tesselator->count;
}

void tess_end(void) {
	int begin_idx = g_tesselator->begin_idx;
	int count = g_tesselator->count;

	switch(g_tesselator->prim) {
		case GL_TRIANGLES:
			for(unsigned int i=begin_idx; i < count; i++) {
				g_tesselator->indices.push_back(i);
			}
			break;
			
		case GL_TRIANGLE_FAN:
			for(unsigned int i=begin_idx+1; i < count-1; i++) {
				g_tesselator->indices.push_back(begin_idx);
				g_tesselator->indices.push_back(i);
				if(i+1 >= count) {
					g_tesselator->indices.push_back(begin_idx+1);
				}
				else {
					g_tesselator->indices.push_back(i+1);
				}
			}
			break;
			
		case GL_TRIANGLE_STRIP:
			for(unsigned int i=0; i < count-begin_idx-2; i++) {
				g_tesselator->indices.push_back(i+begin_idx);
				g_tesselator->indices.push_back(i+begin_idx+1);
				g_tesselator->indices.push_back(i+begin_idx+2);
			}
			break;
			
		case GL_QUADS:
			for(unsigned int i=begin_idx; i < count; i++) {
				g_tesselator->indices.push_back(i);
			}
			break;
			
		case GL_QUAD_STRIP:
			
			break;
	}
}

void tess_error(GLenum error_code) {
	printf("GluTess Error: %s\n", gluErrorString(error_code));
}


Tesselator :: Tesselator()
:	tesselator(0),
	begin_idx(0),
	count(0),
	idx_count(0),
	prim(0)
{
	tesselator = gluNewTess();
    gluTessCallback(tesselator, GLU_TESS_VERTEX, (GLUTessCallback)tess_vertex);
    gluTessCallback(tesselator, GLU_TESS_BEGIN, (GLUTessCallback)tess_begin);
    gluTessCallback(tesselator, GLU_TESS_END, (GLUTessCallback)tess_end);
    gluTessCallback(tesselator, GLU_TESS_COMBINE_DATA, (GLUTessCallback)tess_combine_data);
    gluTessCallback(tesselator, GLU_TESS_ERROR, (GLUTessCallback)tess_error);
}

Tesselator :: ~Tesselator() {
	gluDeleteTess(tesselator);
	
	for(int i=0; i < vertices.size(); i++) {
		delete vertices[i];
	}
	vertices.clear();
}

void Tesselator :: BeginPolygon() {
	g_tesselator  = this;
	positions.clear();
	indices.clear();
	
	for(int i=0; i < vertices.size(); i++) {
		delete vertices[i];
	}
	vertices.clear();
	gluTessBeginPolygon(tesselator, this);
}

void Tesselator :: EndPolygon() {
	g_tesselator  = this;
	gluTessEndPolygon(tesselator);
}

void Tesselator :: BeginContour() {
	g_tesselator  = this;
	gluTessBeginContour(tesselator);
}

void Tesselator :: EndContour() {
	g_tesselator  = this;
	gluTessEndContour(tesselator);
}

void Tesselator :: Vertex(vec2 &v) {
	g_tesselator  = this;
	vec2 *vv = new vec2(v);
	gluTessVertex(
		tesselator,
		&(vv->x),
		vv
	);
	vertices.push_back(vv);
}

void Tesselator :: geometry(AlloArray *vertex, AlloArray *index) {
	AlloArrayHeader h;
	h.components = 3;
	h.type = AlloFloat32Ty;
	h.dimcount = 2;
	h.dim[0] = positions.size();
	h.dim[1] = 1;
	allo_array_setstride(&h, 1);
	allo_array_adapt(vertex, &h);
	
	float *vdata = (float *)vertex->data.ptr;
	for(int i=0; i < positions.size(); i++) {
		vec2 &v = positions[i];
		
		*vdata++ = (float)v.x;
		*vdata++ = (float)v.y;
		*vdata++ = 0.;
	}
	
	h.components = 1;
	h.type = AlloUInt32Ty;
	h.dimcount = 2;
	h.dim[0] = indices.size();
	h.dim[1] = 1;
	allo_array_setstride(&h, 1);
	allo_array_adapt(index, &h);
	
	unsigned int *idata = (unsigned int *)index->data.ptr;
	for(int i=0; i < indices.size(); i++) {
		*idata++ = indices[i];
	}
}


/*! Polygon tesselator for filling in complex 2D shapes
	The Tesselator module translates complex 2D shape outlines into 
	triangles, filling in the shapes.  It handles both concave and 
	convex shapes.
	
	<luacode>
	local tess = Tesselator()
	tess:BeginPolygon()

	-- first contour
	tess:BeginContour()
		tess:Vertex(0, 0)
		tess:Vertex(1, 0)
		tess:Vertex(1, 1)
		tess:Vertex(0.5, 1.5)
		tess:Vertex(0, 1)
	tess:EndContour()

	-- second contour
	tess:BeginContour()
		tess:Vertex(0.25, 0.25)
		tess:Vertex(0.75, 0.25)
		tess:Vertex(0.75, 0.75)
		tess:Vertex(0.25, 0.75)
	tess:EndContour()

	tess:EndPolygon()
	</luacode>
	
	@module opengl.Tesselator
*/


/*! Begin a polygon
	
	@name Tesselator:BeginPolygon
*/
int lua_tess_BeginPolygon(lua_State *L) {
	Tesselator *s = Glue<Tesselator>::checkto(L, 1);
	s->BeginPolygon();
	return 0;
}

/*! End a polygon
	
	@name Tesselator:EndPolygon
*/
int lua_tess_EndPolygon(lua_State *L) {
	Tesselator *s = Glue<Tesselator>::checkto(L, 1);
	s->EndPolygon();
	return 0;
}

/*! Begin a contour within a polygon
	
	@name Tesselator:BeginContour
*/
int lua_tess_BeginContour(lua_State *L) {
	Tesselator *s = Glue<Tesselator>::checkto(L, 1);
	s->BeginContour();
	return 0;
}

/*! End a contour within a polygon
	
	@name Tesselator:EndContour
*/
int lua_tess_EndContour(lua_State *L) {
	Tesselator *s = Glue<Tesselator>::checkto(L, 1);
	s->EndContour();
	return 0;
}

/*! Set a contour vertex
	@param pos The position
	@name Tesselator:Vertex
*/
int lua_tess_Vertex(lua_State *L) {
	Tesselator *s = Glue<Tesselator>::checkto(L, 1);
	vec2 v;
	if(lua::to_vec<double>(L, 2, 2, &(v.x))) {
		s->Vertex(v);
	}
	else {
		luaL_error(L, "Tesselator.vertex: invalid arguments\n");
	}
	return 0;
}

/*! Get the tesselated shape as arrays of vertices and indices
	The geometry function extracts the triangle information from 
	the tesselated contours.  It fills an array of vertices and an 
	array of indices with the tesselation data, which can be used 
	with a Mesh to draw it to screen.
	
	<luacode>
	local vertex = Array()
	local index = Array()

	tess:geometry(vertex, index)

	local mesh = Mesh(ctx)
	mesh:vertex(vertex)
	mesh:index(index)
	mesh.primitive = GL.TRIANGLES
	</luacode>
	
	@param vertices The vertex array
	@param indices The indices array
	
	@name Tesselator:geometry
	
	@see Array
	@see opengl.Mesh
*/
int lua_tess_geometry(lua_State *L) {
	Tesselator *s = Glue<Tesselator>::checkto(L, 1);
	AlloArray *vertex = lua_array_checkto(L, 2);
	AlloArray *index = lua_array_checkto(L, 3);
	s->geometry(vertex, index);
	return 0;
}

template<> const char * Glue<Tesselator>::usr_name() { return "Tesselator"; }
template<> bool Glue<Tesselator>::usr_has_index() { return true; }

template<> void Glue<Tesselator>::usr_push(lua_State * L, Tesselator * u) {
	Glue<Tesselator>::usr_attr_push(L);
	Glue<Tesselator>::usr_attr_prototype(L);
}

template<> Tesselator * Glue<Tesselator>::usr_new(lua_State * L) {
	Tesselator *tess = new Tesselator();
	return tess;
}

template<> void Glue<Tesselator>::usr_index(lua_State * L, Tesselator * u) {
	Glue<Tesselator>::usr_attr_index(L);
}

template<> void Glue<Tesselator>::usr_newindex(lua_State * L, Tesselator * u) {
	Glue<Tesselator>::usr_attr_newindex(L);
}

template<> int Glue<Tesselator>::usr_tostring(lua_State * L, Tesselator * u) {
	lua_pushfstring(L, "%s: %p", Glue<Tesselator>::usr_name(), u); 
	return 1;
}

template<> void Glue<Tesselator>::usr_gc(lua_State * L, Tesselator * u) { 
	delete u;
}

template<> void Glue<Tesselator>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "BeginPolygon", lua_tess_BeginPolygon },
		{ "EndPolygon", lua_tess_EndPolygon },
		{ "BeginContour", lua_tess_BeginContour },
		{ "EndContour", lua_tess_EndContour },
		{ "Vertex", lua_tess_Vertex },
		{ "geometry", lua_tess_geometry },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<Tesselator>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_Tesselator(lua_State *L) {
	Glue<Tesselator>::define(L);
	Glue<Tesselator>::register_class(L);
	luaL_getmetatable(L, Glue<Tesselator>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif
