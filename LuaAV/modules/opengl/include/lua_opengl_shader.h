#ifndef LUA_OPENGL_SHADER_H
#define LUA_OPENGL_SHADER_H 1

#include "lua_opengl_config.h"
#include "lua_utility_opengl.h"
#include "graphics/al_Shader.hpp"
#include "graphics/al_ShaderAttribute.hpp"
#include "protocol/al_GraphicsBackend.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_glue.h"
#include "lua_utility.h"


class PushParam{
public:
	static int push(lua_State *L, al::gfx::Shader::ShaderParamIter &it){
		lua_newtable(L);
		lua::pushfield<const char *>(L, -1, "name", it->second->name());
		
		al::gfx::ShaderParam::Type type = it->second->get_type();
		lua::pushfield<const char *>(L, -1, "type", al::gfx::ShaderParam::name_for_type(type));
		
		lua_pushstring(L, "default");
		int n = al::gfx::ShaderParam::size_for_type(type);
		switch(al::gfx::ShaderParam::data_type_for_type(type)) {
			case al::gfx::ShaderParam::FLOAT: {
				float *fdata = it->second->fdata();
				lua::push_vec_t<float>(L, n, fdata);
			}
			break;
			
			case al::gfx::ShaderParam::INT: {
				int *idata = it->second->idata();
				lua::push_vec_t<int>(L, n, idata);
			}
			break;
		}
		lua_settable(L, -3);
		
		return 1;
	}
};

extern void set_shader_param(lua_State *L, al::gfx::Shader *s, const char *param_name);
extern void push_shader_params(lua_State *L, al::gfx::Shader *s);
extern void setup_prototype_params(lua_State *L);
extern void set_prototype_params(lua_State *L);

#endif // LUA_OPENGL_SHADER_H