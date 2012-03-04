#include "lua_opengl_shader.h"
#include "lua_array.h"

#define ATTR_UDATA_FIELD	"ATTR_UDATA_FIELD"

using al::gfx::Shader;
using al::gfx::ShaderAttribute;
using al::gfx::ShaderParam;
using al::gfx::GraphicsBackend;


struct ShaderAttributeWrapper{
	ShaderAttributeWrapper(ShaderAttribute *attr)
	:	attr(attr)
	{}
	
	~ShaderAttributeWrapper() {}
	
	ShaderAttribute *attr;
};



void set_shader_param(lua_State *L, al::gfx::Shader *s, const char *param_name) {
	ShaderParam *param = s->param(param_name);
	if(! param) {
		//luaL_error(L, "Shader.param: %s is not a parameter", param_name);
	}

	ShaderParam::Type data_type = ShaderParam::data_type_for_type(param->get_type());
	switch(data_type) {
		case ShaderParam::FLOAT: {
			float values[16];
			int nvals = 16;
			lua::to_vec_n<float>(L, 3, nvals, values);
			param->set(nvals, values);
		} break;
		
		case ShaderParam::INT: {
			int values[16];
			int nvals = 16;
			lua::to_vec_n<int>(L, 3, nvals, values);
			param->set(nvals, values);
		} break;
		
		default:
			break;
			//luaL_error(L, "Shader.param: invalid parameter type");
		
	}
}

void push_shader_params(lua_State *L, al::gfx::Shader *s) {
	if(s->get_shader_backend()) {
		lua::Iterator<Shader::ShaderParamIter, PushParam>::push(
			L, 
			s->param_begin(), 
			s->param_end()
		);
	}
}

#define SHADER_TEMP_PARAM_FIELD	"__SHADER_TEMP_PARAM_FIELD"
void setup_prototype_params(lua_State *L) {
	lua_getfield(L, 1, "param");
	lua_setfield(L, 1, SHADER_TEMP_PARAM_FIELD);
	lua_pushnil(L);
	lua_setfield(L, 1, "param");
}

void set_prototype_params(lua_State *L) {
	int udata_idx = lua_gettop(L);
	
	lua_getfield(L, 1, SHADER_TEMP_PARAM_FIELD);
	if(lua_istable(L, -1)) {
		int t = lua_gettop(L);
	
		lua_getfield(L, udata_idx, "param");
		int param_func_idx = lua_gettop(L);

		lua_pushnil(L);  // first key
		while (lua_next(L, t) != 0) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			lua_pushvalue(L, param_func_idx);	// param function
			lua_pushvalue(L, udata_idx);		// self
			lua_pushvalue(L, -4);				// param name
			lua_pushvalue(L, -4);				// param value
			lua_call(L, 3, 0);

			// removes 'value'; keeps 'key' for next iteration
			lua_pop(L, 1);
		}
		
		lua_pop(L, 1);	// pop param func
	}
	lua_setfield(L, 1, "param");
	
	lua_pushnil(L);
	lua_setfield(L, 1, SHADER_TEMP_PARAM_FIELD);
}

/*! Load and manage OpenGL shader objects
	Shaders are specified as .shl files, which are a Lua-based file format for describing shader 
	code and parameters.  Shader uniforms can be set via the Shader:param() method.  
	Shader attributes are accessed through internal ShaderAttribute objects that can be 
	retrieved by name from the Shader:attr() method.
	
	@see ShaderAttribute
	@module opengl.Shader
*/

/*! Set shader source code
	@LuaMethod SET
	@name M.source
*/
int lua_shader_source(lua_State * L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(s && lua::is<const char *>(L, 2)) {
		s->source(lua::to<const char *>(L, 2));
		
		// put shader attr userdata in shader env table
		lua_getfenv(L, 1);
		lua_pushstring(L, ATTR_UDATA_FIELD);
		lua_newtable(L);
		
		map<string, ShaderAttribute *> *attrs = s->attributes();
		if(attrs) {
			map<string, ShaderAttribute *>::iterator it = attrs->begin();
			map<string, ShaderAttribute *>::iterator ite = attrs->end();
			for(; it != ite; ++it) {
				ShaderAttribute *a = it->second;
				ShaderAttributeWrapper *aw = new ShaderAttributeWrapper(a);
				lua_pushstring(L, a->name());
				Glue<ShaderAttributeWrapper>::push(L, aw);
				lua_rawset(L, -3);
			}
		}
		
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Shader.source: invalid arguments");
	}
	return 0;
}

/*! The shader file
	@LuaMethod GETSET
	@name M.file
*/
int lua_shader_file(lua_State *L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(s) {
		if(lua::is<const char *>(L, 2)) {
			s->set_filename(lua::to<const char *>(L, 2));
			
			// put shader attr userdata in shader env table
			lua_getfenv(L, 1);
			lua_pushstring(L, ATTR_UDATA_FIELD);
			lua_newtable(L);
			
			map<string, ShaderAttribute *> *attrs = s->attributes();
			if(attrs) {
				map<string, ShaderAttribute *>::iterator it = attrs->begin();
				map<string, ShaderAttribute *>::iterator ite = attrs->end();
				for(; it != ite; ++it) {
					ShaderAttribute *a = it->second;
					ShaderAttributeWrapper *aw = new ShaderAttributeWrapper(a);
					lua_pushstring(L, a->name());
					Glue<ShaderAttributeWrapper>::push(L, aw);
					lua_rawset(L, -3);
				}
			}
			
			lua_rawset(L, -3);
			lua_pop(L, 1);
			
			return 0;
		}
		else {
			lua_pushstring(L, s->get_filename().c_str());
			return 1;
		}
	}
	else {
		luaL_error(L, "Shader.file: invalid arguments");
	}
	return 0;
}

/*! Bind a shader
	@LuaMethod METHOD
	@name M:bind
*/
int lua_shader_bind(lua_State *L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(s) {
		s->bind();
	}
	else {
		luaL_error(L, "Shader.bind: invalid arguments");
	}
	return 0;
}

/*! Unbind a shader
	@LuaMethod METHOD
	@name M:unbind
*/
int lua_shader_unbind(lua_State *L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(s) {
		s->unbind();
	}
	else {
		luaL_error(L, "Shader.unbind: invalid arguments");
	}
	return 0;
}

/*! Set a parameter
	@param name The parameter name
	@param ... The parameter values as a table or unpacked
	@LuaMethod METHOD
	@name M:param
*/
int lua_shader_param(lua_State *L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(
		s && 
		lua::is<const char *>(L, 2)
	) {
		const char *param_name = lua::to<const char *>(L, 2);
		set_shader_param(L, s, param_name);
	}
	else {
		luaL_error(L, "Shader.param: invalid arguments");
	}
	return 0;
}

/*! Get an iterator over the parameters
	@ret The iterator function
	@LuaMethod METHOD
	@name M:params
*/
int lua_shader_params(lua_State *L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(s) {
		push_shader_params(L, s);
	}
	else {
		luaL_error(L, "Shader.params: invalid arguments");
	}
	return 1;
}

/*! Get a shader attribute
	@ret The shader attribute
	@LuaMethod METHOD
	@name M:attr
*/
int lua_shader_attr(lua_State *L) {
	Shader *s = Glue<Shader>::checkto(L, 1);
	if(s) {
		lua_getfenv(L, 1);

		lua_getfield(L, -1, ATTR_UDATA_FIELD);
		
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);

		// set as return value (nil or userdata)
		lua_insert(L, 3);

		lua_pop(L, 2);
	}
	else {
		luaL_error(L, "Shader.attr: invalid arguments");
	}
	return 1;
}

template<> const char * Glue<Shader>::usr_name() { return "Shader"; }
template<> bool Glue<Shader>::usr_has_index() { return true; }

template<> void Glue<Shader>::usr_push(lua_State * L, Shader * u) {
	Glue<Shader>::usr_attr_push(L);
	Glue<Shader>::usr_attr_prototype(L);
	set_prototype_params(L);
	
	lua_getfenv(L, -1);
	lua_pushstring(L, ATTR_UDATA_FIELD);
	lua_rawget(L, -2);
	if(lua_istable(L, -1)) {
		lua_pop(L, 2);
	}
	else {
		lua_pop(L, 1);
		lua_pushstring(L, ATTR_UDATA_FIELD);
		lua_newtable(L);
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}
}

template<> Shader * Glue<Shader>::usr_new(lua_State * L) {
	string ctx;
	
	if(
		Glue<Shader>::usr_attr_is_prototype(L) && 
		lua::checkfield<const char *>(L, 1, "ctx")
	) {
		ctx.assign(lua::tofield<const char *>(L, 1, "ctx"));
		setup_prototype_params(L);
	}
	else if(lua::is<const char *>(L, 1)) {
		ctx.assign(lua::to<const char *>(L, 1));
	}
	else {
		luaL_error(L, "Shader(): missing context name");
	}

	GraphicsBackend *backend = lua_opengl_get_backend(ctx.c_str());
	Shader *shader = new Shader(backend);
	lua_opengl_register(ctx.c_str(), shader);
	return shader;
}

template<> void Glue<Shader>::usr_index(lua_State * L, Shader * u) {
	Glue<Shader>::usr_attr_index(L);
}

template<> void Glue<Shader>::usr_newindex(lua_State * L, Shader * u) {
	Glue<Shader>::usr_attr_newindex(L);
}

template<> int Glue<Shader>::usr_tostring(lua_State * L, Shader * u) {
	lua_pushfstring(L, "%s: %p", Glue<Shader>::usr_name(), u); 
	return 1;
}

template<> void Glue<Shader>::usr_gc(lua_State * L, Shader * u) { 
	// TODO: finalize Attrs
	lua_opengl_unregister(u);
	delete u;
}

template<> void Glue<Shader>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "bind", lua_shader_bind },
		{ "unbind", lua_shader_unbind },
		{ "param", lua_shader_param },
		{ "params", lua_shader_params },
		{ "attr", lua_shader_attr },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "file", lua_shader_file },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "file", lua_shader_file },
		{ "source", lua_shader_source },
		{ NULL, NULL}
	};
	Glue<Shader>::usr_attr_mt(L, methods, getters, setters);
}

/*! An object for submitting buffers of data to a shader
	Shaders can have both parameters and attributes.  Attributes are 
	per-vertex data that can be sent at once as an entire buffer or 
	as a tuple of values along with a glVertex call.
	
	@see Shader
	@module opengl.ShaderAttribute
*/

/*! Submit a buffer or tuple of data
	@param buffer The buffer of values (a array, table, or unpacked list)
	@LuaMethod METHOD
	@name M:submit
*/
int lua_shader_attr_submit(lua_State *L) {
	ShaderAttributeWrapper *s = Glue<ShaderAttributeWrapper>::checkto(L, 1);
	if(s->attr) {
		AlloArray *lat = lua_array_to(L, 2);
		if(lat) {
			s->attr->submit(lat);
		}
		else {
			float v[MAX_SHADER_ATTR_VALS];
			if(lua::to_vec<float>(L, 2, 4, v)) {
				s->attr->submit(v);
			}
		}
	}
	else {
		luaL_error(L, "ShaderAttribute.submit: invalid ShaderAttribute");
	}
	
	return 0;
}

/*! Get the name
	@ret The name
	@LuaMethod GET
	@name M.name
*/
int lua_shader_attr_name(lua_State *L) {
	ShaderAttributeWrapper *s = Glue<ShaderAttributeWrapper>::checkto(L, 1);
	if(s->attr) {
		lua::push<const char *>(L, s->attr->name());
		return 1;
	}
	else {
		luaL_error(L, "ShaderAttribute.name: invalid ShaderAttribute");
	}
	
	return 0;
}

/*! Get the location
	@ret The location
	@LuaMethod GET
	@name M.location
*/
int lua_shader_attr_location(lua_State *L) {
	ShaderAttributeWrapper *s = Glue<ShaderAttributeWrapper>::checkto(L, 1);
	if(s->attr) {
		lua::push<int>(L, s->attr->get_location());
		return 1;
	}
	else {
		luaL_error(L, "ShaderAttribute.location: invalid ShaderAttribute");
	}
	
	return 0;
}

/*! Get the size in numbers of values per vertex
	@ret The size
	@LuaMethod GET
	@name M.size
*/
int lua_shader_attr_size(lua_State *L) {
	ShaderAttributeWrapper *s = Glue<ShaderAttributeWrapper>::checkto(L, 1);
	if(s->attr) {
		lua::push<int>(L, s->attr->size());
		return 1;
	}
	else {
		luaL_error(L, "ShaderAttribute.size: invalid ShaderAttribute");
	}
	
	return 0;
}



template<> const char * Glue<ShaderAttributeWrapper>::usr_name() { return "ShaderAttribute"; }
template<> bool Glue<ShaderAttributeWrapper>::usr_has_index() { return true; }

template<> void Glue<ShaderAttributeWrapper>::usr_push(lua_State * L, ShaderAttributeWrapper * u) {
	Glue<ShaderAttributeWrapper>::usr_attr_push(L);
}

template<> void Glue<ShaderAttributeWrapper>::usr_index(lua_State * L, ShaderAttributeWrapper * u) {
	Glue<ShaderAttributeWrapper>::usr_attr_index(L);
}

template<> void Glue<ShaderAttributeWrapper>::usr_newindex(lua_State * L, ShaderAttributeWrapper * u) {
	Glue<ShaderAttributeWrapper>::usr_attr_newindex(L);
}

template<> int Glue<ShaderAttributeWrapper>::usr_tostring(lua_State * L, ShaderAttributeWrapper * u) {
	lua_pushfstring(L, "%s: %p", Glue<ShaderAttributeWrapper>::usr_name(), u); 
	return 1;
}

template<> void Glue<ShaderAttributeWrapper>::usr_gc(lua_State * L, ShaderAttributeWrapper * u) { 
	delete u;
}

template<> void Glue<ShaderAttributeWrapper>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "submit", lua_shader_attr_submit },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "name", lua_shader_attr_name },
		{ "location", lua_shader_attr_location },
		{ "size", lua_shader_attr_size },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<ShaderAttributeWrapper>::usr_attr_mt(L, methods, getters, setters);
}




#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opengl_Shader(lua_State *L) {
	Glue<ShaderAttributeWrapper>::define(L);
	Glue<ShaderAttributeWrapper>::register_class(L);

	Glue<Shader>::define(L);
	Glue<Shader>::register_class(L);
	luaL_getmetatable(L, Glue<Shader>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif
