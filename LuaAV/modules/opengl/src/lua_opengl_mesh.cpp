#include "lua_opengl_glue.h"

using al::gfx::GraphicsBackendOpenGL;

/*! Draw buffer of data representing geometry
	Mesh efficiently draws large buffers of geometry data with the constraint that all 
	geometry buffers except for indices must have the same number of cells.
	
	@module opengl.Mesh
*/

/*! Draw the mesh
	@LuaMethod METHOD
	@name M:draw
*/
int lua_mesh_draw(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	if(s) {
		s->draw();
	}
	else {
		luaL_error(L, "Mesh.draw: invalid arguments");
	}
	return 0;
}

/*! Set the vertex data
	Vertex data must have 3 components of type Float32.
	@param vertex Array of vertex data
	@LuaMethod METHOD
	@name M:vertex
*/
int lua_mesh_vertex(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	if(s && lat) {
		s->vertex(lat);
	}
	else {
		luaL_error(L, "Mesh.vertex: invalid arguments");
	}
	return 0;
}

/*! Set the normal data
	Normal data must have 3 components of type Float32.
	@param normal Array of normal data
	@LuaMethod METHOD
	@name M:normal
*/
int lua_mesh_normal(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	if(s && lat) {
		s->normal(lat);
	}
	else {
		luaL_error(L, "Mesh.normal: invalid arguments");
	}
	return 0;
}

/*! Set the color data
	Color data must have 4 components of type Float32.
	@param color Array of color data
	@LuaMethod METHOD
	@name M:color
*/
int lua_mesh_color(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	if(s && lat) {
		s->color(lat);
	}
	else {
		luaL_error(L, "Mesh.color: invalid arguments");
	}
	return 0;
}

/*! Set the texture coordinate data
	Texture coordinate data must have 2 or 3 components of type Float32.
	@param texcoord Array of texture coordinate data
	@LuaMethod METHOD
	@name M:texcoord
*/
int lua_mesh_texcoord(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	if(s && lat) {
		s->texcoord(lat);
	}
	else {
		luaL_error(L, "Mesh.texcoord: invalid arguments");
	}
	return 0;
}

/*! Set the index data
	Indices must have 1 component of type UInt32
	@param index Array of index data
	@LuaMethod METHOD
	@name M:index
*/
int lua_mesh_index(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	if(s && lat) {
		s->index(lat);
	}
	else {
		luaL_error(L, "Mesh.index: invalid arguments");
	}
	return 0;
}

/*! The drawing primitive
	The drawing primitive used by the mesh. Valid values are GL_POINTS, 
	GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_QUADS, and GL_POLYGON.
	
	@LuaMethod GETSET
	@name M.primitive
*/
int lua_mesh_primitive(lua_State *L) {
	Mesh *s = Glue<Mesh>::checkto(L, 1);
	if(s) {
		if(lua::is<GLenum>(L, 2)) {
			al::gfx::Primitive prim = GraphicsBackendOpenGL::primitive(lua::to<GLenum>(L, 2));
			s->primitive(prim);
			return 0;
		}
		else {
			lua::push<GLenum>(L, GraphicsBackendOpenGL::gl_primitive(s->primitive()));
			return 1;
		}
	}
	else {
		luaL_error(L, "Mesh.primitive: invalid arguments");
	}
	return 0;
}



template<> void Glue<Mesh>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "draw", lua_mesh_draw },
		{ "vertex", lua_mesh_vertex },
		{ "normal", lua_mesh_normal },
		{ "color", lua_mesh_color },
		{ "texcoord", lua_mesh_texcoord },
		{ "index", lua_mesh_index },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "primitive", lua_mesh_primitive },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "primitive", lua_mesh_primitive },
		{ NULL, NULL}
	};
	Glue<Mesh>::usr_attr_mt(L, methods, getters, setters);
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_Mesh(lua_State *L) {
	Glue<Mesh>::define(L);
	Glue<Mesh>::register_class(L);
	luaL_getmetatable(L, Glue<Mesh>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif
