#ifndef LUA_OPENGL_TEXTURE_H
#define LUA_OPENGL_TEXTURE_H 1

#include "lua_opengl_config.h"
#include "graphics/al_Texture.hpp"
#include "graphics/al_Surface.hpp"

struct LuaTexture {
	LuaTexture(al::gfx::Texture *t=0, al::gfx::Surface *s=0, bool owns_udata=true)
	:	texture(t),
		surface(s),
		owns_udata(owns_udata)
	{}
	
	~LuaTexture() {
		if(owns_udata) {
			delete texture;
			delete surface;
		}
	}

	al::gfx::Texture *texture;
	al::gfx::Surface *surface;
	bool owns_udata;
};


#endif // LUA_OPENGL_TEXTURE_H