#include "lua_opengl_shader.h"
#include "graphics/al_Slab.hpp"

#include "lua_opengl_glue.h"

//using al::gfx::Shader;
using al::gfx::Slab;
using al::gfx::Texture;
using al::gfx::GraphicsBackend;

#define LUA_SLAB_TEXTURE_FIELD	"__LUA_SLAB_TEXTURE_FIELD"

/*! Load and manage OpenGL shader objects
	Shaders are specified as .shl files, which are a Lua-based file format for describing shader 
	code and parameters.  Shader uniforms can be set via the Shader:param() method.  
	Shader attributes are accessed through internal ShaderAttribute objects that can be 
	retrieved by name from the Shader:attr() method.
	
	@see Shader
	@module opengl.Slab
*/

/*! The shader file
	@LuaMethod GETSET
	@name M.file
*/
int lua_slab_file(lua_State *L) {
	Slab *s = Glue<Slab>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		s->get_shader()->set_filename(lua::to<const char *>(L, 2));
	}
	else {
		lua_pushstring(L, s->get_shader()->get_filename().c_str());
		return 1;
	}
	return 0;
}

Texture * to_texture(lua_State *L, int idx) {
	LuaTexture *lt = Glue<LuaTexture>::to(L, idx);
	if(lt) {
		return lt->texture;
	}
	else {
		Slab *s = Glue<Slab>::to(L, idx);
		if(s) {
			return s->get_texture(0);
		}
		else {
			return NULL;
		}
	}
}

/*! Draw the slab to invoke processing
	@param ... Either a table or unpacked list of Textures and Slab
	@LuaMethod METHOD
	@name M:draw
*/
int lua_slab_draw(lua_State *L) {
	Slab *s = Glue<Slab>::checkto(L, 1);
	
	int ntexs = 0;
	Texture *texs[SLAB_MAX_RENDER_TARGETS];
	if(lua_istable(L, 1)) {
		int len = MIN(SLAB_MAX_RENDER_TARGETS, lua_objlen(L, 2));
		for(int i=0; i < len; i++) {
			lua_rawgeti(L, 2, i+1);
			Texture *tex = to_texture(L, -1);
			if(tex) {
				texs[ntexs] = tex;
				ntexs++;
			}
			lua_pop(L, 1);
		}
	}
	else {
		int len = MIN(SLAB_MAX_RENDER_TARGETS, lua_gettop(L)-1);
		for(int i=2; i <= len+1; i++) {
			Texture *tex = to_texture(L, i);
			if(tex) {
				texs[ntexs] = tex;
				ntexs++;
			}
		}
	}
	
	s->draw(ntexs, texs);
	
	return 0;
}

/*! Set a parameter
	@param name The parameter name
	@param ... The parameter values as a table or unpacked
	@LuaMethod METHOD
	@name M:param
*/
int lua_slab_param(lua_State *L) {
	Slab *s = Glue<Slab>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		const char *param_name = lua::to<const char *>(L, 2);
		set_shader_param(L, s->get_shader(), param_name);
	}
	else {
		luaL_error(L, "Slab.param: invalid arguments");
	}
	return 0;
}

/*! Get an iterator over the parameters
	@ret The iterator function
	@LuaMethod METHOD
	@name M:params
*/
int lua_slab_params(lua_State *L) {
	Slab *s = Glue<Slab>::checkto(L, 1);
	push_shader_params(L, s->get_shader());
	return 1;
}

/*! Get an output texture
	@param [idx] The Texture idx (default 0)
	@ret The Texture
	@LuaMethod METHOD
	@name M:texture
*/
int lua_slab_texture(lua_State *L) {
	Slab *s = Glue<Slab>::checkto(L, 1);
	if(s) {
		int idx = MAX(0, lua::opt<int>(L, 2, 0));
		lua_getfenv(L, 1);
		lua_pushstring(L, LUA_SLAB_TEXTURE_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
		
		lua_rawgeti(L, -1, idx+1);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Slab.texture: invalid arguments");
	}

	return 1;
}


template<> const char * Glue<Slab>::usr_name() { return "Slab"; }
template<> bool Glue<Slab>::usr_has_index() { return true; }

template<> void Glue<Slab>::usr_push(lua_State * L, Slab * u) {
	Glue<Slab>::usr_attr_push(L);
	Glue<Slab>::usr_attr_prototype(L);
	set_prototype_params(L);
	
	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_SLAB_TEXTURE_FIELD);
	
	lua_newtable(L);
	lua_getfield(L, 1, "texture");
	for(int i=0; i < SLAB_MAX_RENDER_TARGETS; i++) {
		Glue<LuaTexture>::push(
			L, 
			new LuaTexture(
				u->get_texture(i),
				u->get_surface(i),
				false
			)
		);
		lua_rawseti(L, -3, i+1);
	}
	lua_pop(L, 1);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

template<> Slab * Glue<Slab>::usr_new(lua_State * L) {
	string ctx;
	
	if(
		Glue<Slab>::usr_attr_is_prototype(L) && 
		lua::checkfield<const char *>(L, 1, "ctx")
	) {
		ctx.assign(lua::tofield<const char *>(L, 1, "ctx"));
		setup_prototype_params(L);
	}
	else if(lua::is<const char *>(L, 1)) {
		ctx.assign(lua::to<const char *>(L, 1));
	}
	else {
		luaL_error(L, "Slab(): missing context name");
	}

	GraphicsBackend *backend = lua_opengl_get_backend(ctx.c_str());
	Slab *slab = new Slab(backend);
	lua_opengl_register(ctx.c_str(), slab);
	return slab;
}

template<> void Glue<Slab>::usr_index(lua_State * L, Slab * u) {
	Glue<Slab>::usr_attr_index(L);
}

template<> void Glue<Slab>::usr_newindex(lua_State * L, Slab * u) {
	Glue<Slab>::usr_attr_newindex(L);
}

template<> int Glue<Slab>::usr_tostring(lua_State * L, Slab * u) {
	lua_pushfstring(L, "%s: %p", Glue<Slab>::usr_name(), u); 
	return 1;
}

template<> void Glue<Slab>::usr_gc(lua_State * L, Slab * u) { 
	lua_opengl_unregister(u);
	delete u;
}

template<> void Glue<Slab>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "draw", lua_slab_draw },
		{ "param", lua_slab_param },
		{ "params", lua_slab_params },
		{ "texture", lua_slab_texture },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "file", lua_slab_file },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "file", lua_slab_file },
		{ NULL, NULL}
	};
	Glue<Slab>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_opengl_Texture(lua_State *L);

int luaopen_opengl_Slab(lua_State *L) {
	Glue<Slab>::define(L);
	Glue<Slab>::register_class(L);
	luaL_getmetatable(L, Glue<Slab>::mt_name(L));
	
	lua::call<const char *>(L, luaopen_opengl_Texture, "opengl.Texture");
	lua_pop(L, 1);

	return 1;
}

#ifdef __cplusplus
}
#endif
