#include "lua_opengl_glue.h"
#include "protocol/al_GraphicsBackendOpenGL.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "lauxlib.h"
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include <string>

#define SURFACE_MAX_RENDER_TARGETS	8

namespace al {
namespace gfx{

static const GLenum s_drawBuffers[] = {
	GL_COLOR_ATTACHMENT0_EXT,
	GL_COLOR_ATTACHMENT1_EXT,
	GL_COLOR_ATTACHMENT2_EXT,
	GL_COLOR_ATTACHMENT3_EXT,
	GL_COLOR_ATTACHMENT4_EXT,
	GL_COLOR_ATTACHMENT5_EXT,
	GL_COLOR_ATTACHMENT6_EXT,
	GL_COLOR_ATTACHMENT7_EXT
};

class RenderingSurface : public GPUObject {
public:
	RenderingSurface() 
	:	mDepthid(0),
		mWidth(0),
		mHeight(0),
		mAutoclear(true)
	{}
	
	virtual ~RenderingSurface() {}
	
	void autoclear(bool v) {
		mAutoclear = v;
	}
	
	bool autoclear() {
		return mAutoclear;
	}
	
	// FBO must be bound, texture must have an ID
	void attach(int idx, Texture *t) {
		GLenum target = GraphicsBackendOpenGL::target_from_texture_target(
			(GraphicsBackendOpenGL *)t->backend(),
			t->target()
		);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+idx, target, t->id(), 0);
	}
	
	bool texturesMatch(int n, Texture **textures) {
		bool matches = true;
		if(n > 1) {
			Texture *first = textures[0];
			int width = first->width();
			int height = first->height();
			Texture::Target target = first->target();
			Texture::Type type = first->type();
			for(int i=1; i < n; i++) {
				Texture *t = textures[i];
				if(
					width != t->width() || height != t->height() ||
					target != t->target() || type != t->type()
				) {
					matches = false;
					break;
				}
			}
		}
		return matches;
	}
	
	void attachTextures(int n, Texture **textures) {
		if(texturesMatch(n, textures)) {
			bool didcreate = false;
			Texture *first = textures[0];
			if(
				mID && 
				(mWidth != first->width() || mHeight != first->height())
			) {
				destroy();
			}
			
			if(!created()) {
				create();
				didcreate = true;
			}
			
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mID);
			for(int i=0; i < n; i++) {
				attach(i, textures[i]);
			}
			
			if(didcreate) {
				mWidth = first->width();
				mHeight = first->height();
				attachDepthRenderBuffer();
			}
			
			checkStatus();
		}
	}
	
	void bind(int n, Texture **textures) {
		if(n <= 0) {
			return;
		}
	
		for(int i=0; i < n; i++) {
			// make sure each texture has an id
			if(!textures[i]->id()) {
				textures[i]->bind();
				textures[i]->unbind();
			}
		}
		
		attachTextures(n, textures);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		
		
		glPushAttrib(GL_VIEWPORT_BIT);
		glDisable(GL_SCISSOR_TEST);
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mID);
		glViewport(0, 0, mWidth, mHeight);
		
		if(mAutoclear) {
			for(int i=0; i < n; i++) {
				Texture *t = textures[i];
				Surface *s = t->surface();
				if(s) {
					if(s->autoClear()) {
						float *cc = s->clearColor().components;
						glClearColor(cc[0], cc[1], cc[2], cc[3]);
						glClearDepth(s->clearDepth());
					}
				}
				else {
					glClearColor(0., 0., 0., 1.);
					glClearDepth(1);
				}
			
				glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT+i);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
		}
		
		glDrawBuffers(n, s_drawBuffers);
	}
	
	void unbind() {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		glPopAttrib();
	}
	
	void attachDepthRenderBuffer() {
		// generate a new renderbuffer 
		glGenRenderbuffersEXT(1, &mDepthid);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mDepthid);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mWidth, mHeight);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mID);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthid);
	}

	
	virtual void onCreate() {
		glGenFramebuffersEXT(1, &mID);
	}
	
	virtual void onDestroy() {
		if(mID) {
			glDeleteFramebuffersEXT(1, &mID);
			mDepthid = 0;
		}
	}
	
	GLenum checkStatus() {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mID);

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status) {
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
				
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				printf("framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");
				/* you gotta choose different formats */
				break;
				
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				printf("framebuffer INCOMPLETE_ATTACHMENT\n");
				break;
			
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				printf("framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");
				break;
			
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				printf("framebuffer FRAMEBUFFER_DIMENSIONS\n");
				break;
			
			case 0x8CD8: //GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
				printf("framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n");
				break;
				
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				printf("framebuffer INCOMPLETE_FORMATS\n");
				break;
				
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				printf("framebuffer INCOMPLETE_DRAW_BUFFER\n");
				break;
				
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				printf("framebuffer INCOMPLETE_READ_BUFFER\n");
				break;
				
			case GL_FRAMEBUFFER_BINDING_EXT:
				printf("framebuffer BINDING_EXT\n");
				break;
				
			case 0x8CDE: // GL_FRAMEBUFFER_STATUS_ERROR_EXT:
				printf("framebuffer STATUS_ERROR\n");
				break;
				
			default:
				/* programming error; will fail on all hardware */
				//exit(0);
				break;
		}
		
		return status;
	}

protected:
	GLuint mDepthid;
	int mWidth;
	int mHeight;
	bool mAutoclear;
};


} // ::al::gfx
} // ::al


using al::gfx::RenderingSurface;
using al::gfx::Texture;


int lua_surface_bind(lua_State *L) {
	RenderingSurface *s = Glue<RenderingSurface>::checkto(L, 1);
	int n = 0;
	Texture *textures[SURFACE_MAX_RENDER_TARGETS];
	
	if(lua_istable(L, 2)) {
		n = lua_objlen(L, 2);
		for(int i=0; i < n; i++) {
			lua_rawgeti(L, 2, i+1);
			LuaTexture *ltex = Glue<LuaTexture>::checkto(L, -1);
			if(!ltex || !ltex->texture) {
				luaL_error(L, "Surface.bind: invalid texture object");
			}
			textures[i] = ltex->texture;
			lua_pop(L, 1);
		}
	}
	else {
		int top = lua_gettop(L);
		n = top-1;
		for(int i=2; i <= top; i++) {
			LuaTexture *ltex = Glue<LuaTexture>::checkto(L, i);
			if(!ltex || !ltex->texture) {
				luaL_error(L, "Surface.bind: invalid texture object");
			}
			textures[i-2] = ltex->texture;
		}
	}
	
	if(n > 0) {
		s->bind(n, textures);
	}
	
	return 0;
}

int lua_surface_unbind(lua_State *L) {
	RenderingSurface *s = Glue<RenderingSurface>::checkto(L, 1);
	s->unbind();
	return 0;
}

int lua_surface_autoclear(lua_State *L) {
	RenderingSurface *s = Glue<RenderingSurface>::checkto(L, 1);
	if(lua::is<bool>(L, 2)) {
		s->autoclear(lua::to<bool>(L, 2));
	}
	else  {
		lua::push<bool>(L, s->autoclear());
		return 1;
	}
	return 0;
}

int lua_surface_id(lua_State *L) {
	RenderingSurface *s = Glue<RenderingSurface>::checkto(L, 1);
	lua::push<uint32_t>(L, s->id());
	return 1;
}

template<> const char * Glue<RenderingSurface>::usr_name() { return "Surface"; }
template<> bool Glue<RenderingSurface>::usr_has_index() { return true; }

template<> void Glue<RenderingSurface>::usr_push(lua_State * L, RenderingSurface * u) {
	Glue<RenderingSurface>::usr_attr_push(L);
	Glue<RenderingSurface>::usr_attr_prototype(L);
}

template<> RenderingSurface * Glue<RenderingSurface>::usr_new(lua_State * L) {
	string ctx;
	
	if(
		Glue<RenderingSurface>::usr_attr_is_prototype(L) && 
		lua::checkfield<const char *>(L, 1, "ctx")
	) {
		ctx.assign(lua::tofield<const char *>(L, 1, "ctx"));
	}
	else if(lua::is<const char *>(L, 1)) {
		ctx.assign(lua::to<const char *>(L, 1));
	}
	else {
		luaL_error(L, "Surface(): missing context name");
	}

	RenderingSurface *surface = new RenderingSurface();
	lua_opengl_register(ctx.c_str(), surface);
	return surface;
}

template<> void Glue<RenderingSurface>::usr_index(lua_State * L, RenderingSurface * u) {
	Glue<RenderingSurface>::usr_attr_index(L);
}

template<> void Glue<RenderingSurface>::usr_newindex(lua_State * L, RenderingSurface * u) {
	Glue<RenderingSurface>::usr_attr_newindex(L);
}

template<> int Glue<RenderingSurface>::usr_tostring(lua_State * L, RenderingSurface * u) {
	lua_pushfstring(L, "%s: %p", Glue<RenderingSurface>::usr_name(), u); 
	return 1;
}

template<> void Glue<RenderingSurface>::usr_gc(lua_State * L, RenderingSurface * u) { 
	lua_opengl_unregister(u);
	delete u;
}

template<> void Glue<RenderingSurface>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "bind", lua_surface_bind },
		{ "unbind", lua_surface_unbind },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "autoclear", lua_surface_autoclear },
		{ "id", lua_surface_id },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "autoclear", lua_surface_autoclear },
		{ NULL, NULL}
	};
	Glue<LuaTexture>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_opengl_Texture(lua_State *L);

int luaopen_opengl_Surface(lua_State *L) {
	Glue<RenderingSurface>::define(L);
	Glue<RenderingSurface>::register_class(L);
	luaL_getmetatable(L, Glue<RenderingSurface>::mt_name(L));

	lua::call<const char *>(L, luaopen_opengl_Texture, "opengl.Texture");
	lua_pop(L, 1);

	return 1;
}

#ifdef __cplusplus
}
#endif