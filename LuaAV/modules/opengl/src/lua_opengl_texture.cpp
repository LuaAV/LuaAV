#include "lua_opengl_glue.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lauxlib.h"
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include <string>

using std::string;
using al::gfx::Texture;
using al::gfx::Surface;
using al::gfx::GraphicsBackend;
using al::gfx::GraphicsBackendOpenGL;

/*! An object for manipulating buffers of data on the GPU
	Texture can be used to upload buffers of data to the GPU as well as allocate new 
	buffers that reside solely on the GPU for offscreen rendering.  Additionally, data 
	can be read from the GPU using a fast-path readback based on Pixel Buffer Objects 
	by calling Texture:toarray().
	
	@module opengl.Texture
*/


/*! Clear the texture
	@LuaMethod METHOD
	@name M:clear
*/
int lua_texture_clear(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	s->texture->clear(0, true, true);
	return 0;
}


/*! Bind a texture to a texture unit
	@param [unit] The index of the texture unit (default is 0)
	@LuaMethod METHOD
	@name M:bind
*/
int lua_texture_bind(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	int unit = lua::opt<int>(L, 2, 0);
	s->texture->bind(unit);
	return 0;
}

/*! Unbind a texture from a texture unit
	@param [unit] The index of the texture unit (default is 0)
	@LuaMethod METHOD
	@name M:unbind
*/
int lua_texture_unbind(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	int unit = lua::opt<int>(L, 2, 0);
	s->texture->unbind(unit);
	return 0;
}

/*! Start rendering to the Texture
	@LuaMethod METHOD
	@name M:startcapture
*/
int lua_texture_startcapture(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	s->texture->mode(Texture::SURFACE);
	s->surface->enter();
	return 0;
}

/*! End rendering to the Texture
	@LuaMethod METHOD
	@name M:endcapture
*/
int lua_texture_endcapture(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	s->surface->leave();
	return 0;
}

/*! Flag for clearing Texture on startcapture()
	@LuaMethod GETSET
	@name M.autoclear
*/
int lua_texture_autoclear(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->surface->autoClear(lua::to<bool>(L, 2));
	}
	else {
		lua::push(L, s->surface->autoClear());
		return 1;
	}
	return 0;
}

/*! Copy a Texture
	@param tex The Texture to copy
	@LuaMethod METHOD
	@name M:copy
*/
int lua_texture_copy(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	LuaTexture *t = Glue<LuaTexture>::checkto(L, 2);
	s->surface->copy(t->texture);
	return 0;
}

/*! OpenGL Clear color
	The RGBA values as a table
	@LuaMethod GETSET
	@name M.clearcolor
*/
int lua_texture_clearcolor(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	al::Color c;
	if(lua::to_vec_t<float>(L, 2, 4, c.components)) {
		s->surface->clearColor(c);
	}
	else {
		lua::push_vec_t(L, 4, s->surface->clearColor().components);
		
		return 1;
	}
	return 0;
}

/*! OpenGL Depth clear value
	The depth value on clear
	@LuaMethod GETSET
	@name M.cleardepth
*/
int lua_texture_cleardepth(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->surface->clearDepth(lua::to<float>(L, 2));
	}
	else {
		lua::push(L, s->surface->clearDepth());
		return 1;
	}
	return 0;
}

/*! Send a array to the GPU
	@param [offset] The offset into the internal texture buffer
	@param array The array
	@LuaMethod METHOD
	@name M:fromarray
*/
int lua_texture_fromarray(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	al::Vec3i offset;
	if(lat) {
		s->texture->fromArray(lat);
	}
	else if(
		lua::to_vec_t<int>(L, 2, 3, offset.ptr()) &&
		(lat = lua_array_checkto(L, 3))
	) {
		s->texture->fromArray(offset, lat);
	}
	else {
		luaL_error(L, "Texture.fromarray: invalid arguments");
	}
	return 0;
}

/*! Grab the Texture data from the GPU and store it in the Texture's Array
	@LuaMethod METHOD
	@name M:toarray
*/
int lua_texture_toarray(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	s->texture->toArray();
	return 0;
}

/*! Allocate internal texture data
	@param components The number of components 
	@param type The data type
	@param ... The x, y, z dimensions as a table or unpacked
	@LuaMethod METHOD
	@name M:allocate
*/
int lua_texture_allocate(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	int dimcount = 3;
	uint32_t dim[] = {0, 0, 0};
	if(
		lua::is<uint8_t>(L, 2) && 
		lua::is<AlloTy>(L, 3) && 
		lua::to_vec_n<uint32_t>(L, 4, dimcount, dim)
	) {
		AlloArrayHeader header;
		header.components = lua::to<uint8_t>(L, 2);
		header.type = lua::to<AlloTy>(L, 3);
		header.dimcount = (uint8_t)dimcount;
		for(int i=0; i < dimcount; i++) {
			header.dim[i] = dim[i];
		}
		s->texture->allocate(header);
	}
	else {
		luaL_error(L, "Texture.allocate: invalid arguments");
	}
	return 0;
}

/*! Dimensions
	@param dim The dimensions
	@LuaMethod GETSET
	@name M.dim
*/
int lua_texture_dim(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	int dim[] = {0, 0};
	if(lua::to_vec_t(L, 2, 2, dim)) {
		s->texture->dimensions(dim[0], dim[1]);
	}
	else {
		s->texture->getDimensions(dim[0], dim[1]);
		lua::push_vec_t(L, 2, dim);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture ID
	@ret The Texture ID
	@LuaMethod GET
	@name M.texid
*/
int lua_texture_texid(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	lua::push<uint32_t>(L, s->texture->id());
	return 1;
}

/*! OpenGL Texture target
	Possible targets GL_TEXTURE_1D, GL_TEXTURE_2D, 
	GL_TEXTURE_RECTANGLE_ARB, or GL_TEXTURE_3D
	@LuaMethod GETSET
	@name M.target
*/
int lua_texture_target(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		s->texture->target(
			GraphicsBackendOpenGL::texture_target_from_target(
				(GraphicsBackendOpenGL *)t->backend(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::target_from_texture_target(
				(GraphicsBackendOpenGL *)t->backend(),
				t->target()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture format
	Possible formats GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE,
	GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, 
	GL_LUMINANCE_ALPHA, or GL_DEPTH_COMPONENT
	@LuaMethod GETSET
	@name M.format
*/
int lua_texture_format(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		t->format(
			GraphicsBackendOpenGL::texture_format_from_format(
				(GraphicsBackendOpenGL *)t->backend(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::format_from_texture_format(
				(GraphicsBackendOpenGL *)t->backend(),
				t->format()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture format for single channel images
	Possible formats GL_ALPHA, GL_LUMINANCE
	@LuaMethod GETSET
	@name M.single_channel
*/
int lua_texture_single_channel(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		t->singleChannel(
			GraphicsBackendOpenGL::texture_format_from_format(
				(GraphicsBackendOpenGL *)t->backend(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::format_from_texture_format(
				(GraphicsBackendOpenGL *)t->backend(),
				t->singleChannel()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture type
	Possible types type GL_UNSIGNED_BYTE, GL_BYTE, GL_BITMAP,
	GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, 
	GL_INT, and GL_FLOAT
	@LuaMethod GETSET
	@name M.type
*/
int lua_texture_type(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		t->type(
			GraphicsBackendOpenGL::texture_type_from_type(
				(GraphicsBackendOpenGL *)t->backend(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::type_from_texture_type(
				(GraphicsBackendOpenGL *)t->backend(),
				t->type()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture wrap mode
	Possible wrap modesGL_CLAMP_TO_EDGE, GL_CLAMP, GL_CLAMP_TO_BORDER, 
	GL_MIRRORED_REPEAT (2D only), or GL_REPEAT (2D only)
	@LuaMethod GETSET
	@name M.wrap
*/
int lua_texture_wrap(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		t->wrap(
			GraphicsBackendOpenGL::texture_wrap_from_wrap(
				(GraphicsBackendOpenGL *)t->backend(),
				t->target(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::wrap_from_texture_wrap(
				(GraphicsBackendOpenGL *)t->backend(),
				t->target(),
				t->wrap()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture minification filter
	Possible minfilters GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, 
	or GL_LINEAR_MIPMAP_LINEAR
	@LuaMethod GETSET
	@name M.minfilter
*/
int lua_texture_minfilter(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		t->minFilter(
			GraphicsBackendOpenGL::texture_filter_from_filter(
				(GraphicsBackendOpenGL *)t->backend(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::filter_from_texture_filter(
				(GraphicsBackendOpenGL *)t->backend(),
				t->minFilter()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture magnification filter
	Possible magfilters GL_NEAREST or GL_LINEAR
	@LuaMethod GETSET
	@name M.magfilter
*/
int lua_texture_magfilter(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	if(lua::is<GLenum>(L, 2)) {
		t->magFilter(
			GraphicsBackendOpenGL::texture_filter_from_filter(
				(GraphicsBackendOpenGL *)t->backend(),
				lua::to<GLenum>(L, 2)
			)
		);
	}
	else {
		lua::push<GLenum>(L, 
			GraphicsBackendOpenGL::filter_from_texture_filter(
				(GraphicsBackendOpenGL *)t->backend(),
				t->magFilter()
			)
		);
		return 1;
	}
	return 0;
}

/*! OpenGL Texture border color
	The RGBA values as a table
	@LuaMethod GETSET
	@name M.bordercolor
*/
int lua_texture_bordercolor(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	Texture *t = s->texture;
	al::Color c;
	if(lua::to_vec_t<float>(L, 2, 4, c.components)) {
		t->borderColor(c);
	}
	else {
		lua::push_vec_t(L, 4, t->borderColor().components);
		
		return 1;
	}
	return 0;
}

/*! Get the Texture's internal Array
	@ret The array
	@LuaMethod METHOD
	@name M.array
*/
int lua_texture_array(lua_State *L) {
	LuaTexture *s = Glue<LuaTexture>::checkto(L, 1);
	if(s) {
		lua_getfenv(L, 1);
		lua_pushstring(L, LUA_TEXTURE_ARRAY_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Texture.array: invalid arguments");
	}
	return 1;
}


template<> void Glue<LuaTexture>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{"clear", lua_texture_clear},
		{"bind", lua_texture_bind},
		{"unbind", lua_texture_unbind},
		{"startcapture", lua_texture_startcapture},
		{"endcapture", lua_texture_endcapture},
		{"copy", lua_texture_copy},
		{"fromarray", lua_texture_fromarray},
		{"toarray", lua_texture_toarray},
		{"allocate", lua_texture_allocate},
		{"array", lua_texture_array},
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{"autoclear", lua_texture_autoclear},
		{"clearcolor", lua_texture_clearcolor},
		{"cleardepth", lua_texture_cleardepth},
		{"dim", lua_texture_dim},
		{"texid", lua_texture_texid},
		{"target", lua_texture_target},
		{"format", lua_texture_format},
		{"single_channel", lua_texture_single_channel},
		{"type", lua_texture_type},
		{"wrap", lua_texture_wrap},
		{"minfilter", lua_texture_minfilter},
		{"magfilter", lua_texture_magfilter},
		{"bordercolor", lua_texture_bordercolor},
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{"autoclear", lua_texture_autoclear},
		{"clearcolor", lua_texture_clearcolor},
		{"cleardepth", lua_texture_cleardepth},
		{"dim", lua_texture_dim},
		{"target", lua_texture_target},
		{"format", lua_texture_format},
		{"single_channel", lua_texture_single_channel},
		{"type", lua_texture_type},
		{"wrap", lua_texture_wrap},
		{"minfilter", lua_texture_minfilter},
		{"magfilter", lua_texture_magfilter},
		{"bordercolor", lua_texture_bordercolor},
		{ NULL, NULL}
	};
	Glue<LuaTexture>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_Texture(lua_State *L) {
	Glue<LuaTexture>::define(L);
	Glue<LuaTexture>::register_class(L);
	luaL_getmetatable(L, Glue<LuaTexture>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif
