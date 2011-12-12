#ifndef LUA_OPENGL_GLUE_H
#define LUA_OPENGL_GLUE_H 1

#include "graphics/al_Mesh.hpp"
#include "protocol/al_GraphicsBackendOpenGL.hpp"

#include "lua_glue.h"
#include "lua_utility.h"
#include "lua_array.h"
#include "lua_opengl.h"
#include "lua_opengl_texture.h"


#define LUA_TEXTURE_ARRAY_FIELD	"__LUA_TEXTURE_ARRAY_FIELD"

#include <string>

using al::gfx::Mesh;
using al::gfx::Texture;
using al::gfx::Surface;
using al::gfx::GraphicsBackend;
using std::string;

#pragma mark Mesh

template<> inline const char * Glue<Mesh>::usr_name() { return "Mesh"; }
template<> inline bool Glue<Mesh>::usr_has_index() { return true; }

template<> inline void Glue<Mesh>::usr_push(lua_State * L, Mesh * u) {
	Glue<Mesh>::usr_attr_push(L);
	Glue<Mesh>::usr_attr_prototype(L);
	
	/*
	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_SLAB_TEXTURE_FIELD);
	
	lua_newtable(L);
	for(int i=0; i < SLAB_MAX_RENDER_TARGETS; i++) {
		Glue<LuaTexture>::push(
			L, 
			new LuaTexture(
				u->get_texture(i),
				u->get_surface(i),
				false
			)
		);
		lua_rawseti(L, -2, i+1);
	}
	lua_rawset(L, -3);
	lua_pop(L, 1);
	*/
}

template<> inline Mesh * Glue<Mesh>::usr_new(lua_State * L) {
	string ctx;
	
	if(
		Glue<Mesh>::usr_attr_is_prototype(L) && 
		lua::checkfield<const char *>(L, 1, "ctx")
	) {
		ctx.assign(lua::tofield<const char *>(L, 1, "ctx"));
	}
	else if(lua::is<const char *>(L, 1)) {
		ctx.assign(lua::to<const char *>(L, 1));
	}
	else {
		luaL_error(L, "Mesh(): missing context name");
	}

	GraphicsBackend *backend = lua_opengl_get_backend(ctx.c_str());
	Mesh *mesh = new Mesh(backend);
//	lua_opengl_register(ctx.c_str(), slab);
	return mesh;
}

template<> inline void Glue<Mesh>::usr_index(lua_State * L, Mesh * u) {
	Glue<Mesh>::usr_attr_index(L);
}

template<> inline void Glue<Mesh>::usr_newindex(lua_State * L, Mesh * u) {
	Glue<Mesh>::usr_attr_newindex(L);
}

template<> inline int Glue<Mesh>::usr_tostring(lua_State * L, Mesh * u) {
	lua_pushfstring(L, "%s: %p", Glue<Mesh>::usr_name(), u); 
	return 1;
}

template<> inline void Glue<Mesh>::usr_gc(lua_State * L, Mesh * u) {
//	lua_opengl_unregister(u);
	delete u;
}

#pragma mark LuaTexture


template<> inline const char * Glue<LuaTexture>::usr_name() { return "Texture"; }
template<> inline bool Glue<LuaTexture>::usr_has_index() { return true; }

template<> inline LuaTexture * Glue<LuaTexture>::usr_new(lua_State * L) {
	string ctx;
	
	if(
		Glue<LuaTexture>::usr_attr_is_prototype(L) && 
		lua::checkfield<const char *>(L, 1, "ctx")
	) {
		ctx.assign(lua::tofield<const char *>(L, 1, "ctx"));
	}
	else if(lua::is<const char *>(L, 1)) {
		ctx.assign(lua::to<const char *>(L, 1));
	}
	else {
		luaL_error(L, "Texture(): missing context name");
	}

	GraphicsBackend *backend = lua_opengl_get_backend(ctx.c_str());
	Texture *tex = backend->textureNew();
	Surface *surface = backend->surfaceNew();
	surface->attach(tex);
	LuaTexture *lt = new LuaTexture(tex, surface);
	lua_opengl_register(ctx.c_str(), tex);
	lua_opengl_register(ctx.c_str(), surface);
	return lt;
}

template<> inline void Glue<LuaTexture>::usr_push(lua_State * L, LuaTexture * u) {
	Glue<LuaTexture>::usr_attr_push(L);
	Glue<LuaTexture>::usr_attr_prototype(L);
	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_TEXTURE_ARRAY_FIELD);
	Glue<al::ArrayWrapper>::push(L, u->texture->wrapper());
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

template<> inline void Glue<LuaTexture>::usr_index(lua_State * L, LuaTexture * u) {
	Glue<LuaTexture>::usr_attr_index(L);
}

template<> inline void Glue<LuaTexture>::usr_newindex(lua_State * L, LuaTexture * u) {
	Glue<LuaTexture>::usr_attr_newindex(L);
}

template<> inline int Glue<LuaTexture>::usr_tostring(lua_State * L, LuaTexture * u) {
	lua_pushfstring(L, "%s: %p", Glue<LuaTexture>::usr_name(), u); 
	return 1;
}

template<> inline void Glue<LuaTexture>::usr_gc(lua_State * L, LuaTexture * u) { 
	lua_opengl_unregister(u->texture);
	lua_opengl_unregister(u->surface);
	delete u;
}

#endif // include guard