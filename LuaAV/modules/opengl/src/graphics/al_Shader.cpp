#include "graphics/al_Shader.hpp"
#include "lua_utility.h"
#include <fstream>

namespace al {
namespace gfx {

// a Lua script that puts loaded shaders in global namespace
// assigned by language type names in upper case
// i.e. a GLSL shader will be located in _G["GLSL"]

#define SHADER_FILENAME		"SHADER_FILENAME"

// may want to put these functions in the registry in a special table?
const char * c_shlLoader[] = {
"local",
"function readProgramFile(file, shaderFile)",
	"local programFilePath",
	"if(file[1] ~= \"/\") then",
		"--relative path",
		"local r_shaderfile = string.reverse(shaderFile)",
		"local ridx = string.find(r_shaderfile, \"/\")",
		"local idx = #shaderFile - ridx",
		"local path = string.sub(shaderFile, 1, idx+1)",
		"programFilePath = path .. file",
	"else",
		"programFilePath = file",
	"end",

	"local programFile = io.open(programFilePath)",
	"if(programFile) then",
		"local programString = programFile:read(\"*a\")"
		"programFile:close()",

		"return programString",
	"else",
		"--error",
		"error(string.format(\"Couldn\'t find program file %s\", programFilePath))",
	"end",
"end",
/*
"function print_t(msg, t)",
	"if(t) then",
	"for k, v in pairs(t) do",
		"print(msg .. ':', k, v)",
	"end",
	"else",
	"print('no table ' .. msg)",
	"end",
"end",
*/
"function Program(t)",
	"local info = debug.getinfo(2, 'lSn')",
//	"print_t('2', info)",
	"t.src_offset = info.currentline",
	
	"local programType = t.type",
	"if(not programType or type(programType) ~= \"string\") then",
		"error(\"Program has no type or is it invalid.\")",
	"end",
	"if(not t[1] and t.file) then",
		"local file = t.file",
		"local shaderFile = _G[\"SHADER_FILENAME\"]",
		"t[1] = readProgramFile(file, shaderFile)",
		"t.src_offset = 0",
	"end",
	"if(not t[1] or type(t[1]) ~= \"string\") then",
		"error(\"Program has no code or is it invalid.\")",
	"end",
	"t.type = string.upper(programType)",
	"t.entry = t.entry or \"main\"",
	"t.targets = t.targets or 1",
	"return t",
"end",

"function Parameter(t)",
	"if(not t.name or type(t.name) ~= \"string\") then",
		"error(\"Parameter has no name or is it invalid\")",
	"end",
	"if(not t.type or type(t.type) ~= \"string\") then",
		"error(\"Parameter has no type or is it invalid\")",
	"end",
	"t.type = string.upper(t.type)",
	"t.default = t.default or {}",

	"if(t.state) then",
		"t.state = string.upper(t.state)",
	"end",

	"if(t.transform) then",
		"t.transform = string.upper(t.transform)",
	"end",

	"return t",
"end",

"function Attribute(t)",
	"if(not t.name or type(t.name) ~= \"string\") then",
		"error(\"Attribute has no name or is it invalid\")",
	"end",
	"if(not t.type or type(t.type) ~= \"string\") then",
		"error(\"Attribute has no type or is it invalid\")",
	"end",
	"t.type = string.upper(t.type)",

	"t.__attribute = true",

	"return t",
"end",

"function Shader(t)",
//	"print(t.language)",
	"local language = t.language or \"GLSL\"",
	"language = string.upper(language)",
	"t.language = language",

	"for i, program in ipairs(t) do",
		"if(type(program) ~= \"table\") then",
			"error(string.format(\"Program %d is invalid\", i))",
		"end",
	"end",
	"if(not t.parameters) then",
		"t.parameters = {}",
	"end",
	"for i, param in ipairs(t.parameters) do",
		"if(type(param) ~= \"table\") then",
			"error(string.format(\"Parameters %d is invalid\", i))",
		"end",
	"end",

	"_G[language] = t",

	"return t",
"end",

"function processShader(shader, lang)",
//	"print(\"processShader\", shader, lang)",
	"if(type(_G[lang]) == \"table\") then --if shader exists",
		"local sh = _G[lang]",
		"local backend = createBackend(shader, lang)",

		"if(backend) then",
			"for i, program in ipairs(sh) do",
				"createProgram(backend, program.type, program[1], program.entry,",
						"program.vertices_out, program.input_type,"
						"program.output_type, program.targets)"
			"end",

			"for i, param in ipairs(sh.parameters) do",
				"if(param.__attribute) then",
					"createAttribute(backend, param.name, param.type)",
				"else",
					"--unpack values into one long array",
					"local vals = {}",
					"if(type(param[1]) == \"number\") then",
						"vals = {param[1]}",
					"elseif(type(param[1]) == \"table\") then",
						"if(type(param[1][1]) == \"number\") then",
							"vals = param[1]",
						"elseif(type(param[1][1]) == \"table\") then",
							"for j, t in ipairs(param[1]) do",
								"if(type(t) == \"table\") then",
									"for jj, tt in ipairs(t) do",
										"if(type(tt) == \"number\") then",
											"vals[#vals+1] = tt",
										"end",
									"end",
								"else",
									"vals[#vals+1] = t[j]",
								"end",
							"end",
						"end",
					"end",

					"createParameter(backend, param.name, param.type, vals,",
									"param.state, param.transform)",
				"end",
			"end",
		"else",
			"--error",
		"end",
	"end",
"end",
NULL};

Shader :: Shader(GraphicsBackend *backend)
:	GPUObject(),
	mBackend(backend),
	mShaderBackend(NULL),
	mHasFilename(false),
	L(NULL)
{
	init_shl();
}

Shader :: ~Shader() {
	lua_close(L);
}

void Shader :: init_shl() {
	if(L) {
		lua_close(L);
		L = NULL;
	}

	L = lua_open();

	static const luaL_Reg libs[] = {
		{"", luaopen_base},
		{LUA_LOADLIBNAME, luaopen_package},
		{LUA_TABLIBNAME, luaopen_table},
		{LUA_IOLIBNAME, luaopen_io},
		{LUA_OSLIBNAME, luaopen_os},
		{LUA_STRLIBNAME, luaopen_string},
		{LUA_MATHLIBNAME, luaopen_math},
		{LUA_DBLIBNAME, luaopen_debug},
		{NULL, NULL}
	};
	lua::loadlibs(L, libs);

	string loader;
	int i=0;
	while(c_shlLoader[i]) {
		loader.append(c_shlLoader[i]);
		loader.append("\n");
		i++;
	}

	int err = luaL_dostring(L, loader.data());
	if(err) {
		printf("error initializing shl loader %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	// add functions for maniuplating the Shader from Lua
	lua_pushcfunction(L, create_backend);
	lua_setglobal(L, "createBackend");

	lua_pushcfunction(L, create_program);
	lua_setglobal(L, "createProgram");

	lua_pushcfunction(L, create_parameter);
	lua_setglobal(L, "createParameter");

	lua_pushcfunction(L, create_attribute);
	lua_setglobal(L, "createAttribute");
}

ShaderParam * Shader :: param(const char *name) {
	return mShaderBackend ? mShaderBackend->param(name) : NULL;
}

Shader::ShaderParamIter Shader :: param_begin() {
	return mShaderBackend->param_begin();
}
Shader::ShaderParamIter Shader :: param_end() {
	return mShaderBackend->param_end();
}

ShaderAttribute * Shader :: attr(const char *name) {
	return mShaderBackend ? mShaderBackend->attr(name) : NULL;
}

map<string, ShaderAttribute *> * Shader :: attributes() {
	return mShaderBackend ? mShaderBackend->attributes() : NULL;
}

void Shader :: onDestroy() {
	if(mShaderBackend) {
		mShaderBackend->destroy();
	}
}

void Shader :: bind() {
	if(mShaderBackend) {
		if(!created()) {
			create();
		}
	
		mShaderBackend->bind();
	}
}

void Shader :: unbind() {
	if(mShaderBackend) {
		mShaderBackend->unbind();
	}
}


int Shader :: create_backend(lua_State *L) {
	if(
		lua_islightuserdata(L, 1) && 
		lua::is<const char *>(L, 2)
	) {
		Shader *s = (Shader *)lua_touserdata(L, 1);
		const char *lang = lua::to<const char *>(L, 2);
		ShaderBackend::Type type = ShaderBackend::type_for_name(lang);
		ShaderBackend *backend = ShaderBackend::create(s, type);
		if(backend) {
			s->set_shader_backend(backend);
			lua_pushlightuserdata(L, (void *)backend);
		}
		else {
			// error
			lua_pushnil(L);
		}
	}
	else {
		// error
		lua_pushnil(L);
	}

	return 1;
}


int Shader :: create_program(lua_State *L) {
	if(
		lua_islightuserdata(L, 1) && 
		lua::is<const char *>(L, 2) &&
		lua::is<const char *>(L, 3) && 
		lua::is<const char *>(L, 4)
	) {
		ShaderBackend *b = (ShaderBackend *)lua_touserdata(L, 1);
		const char *program_type_name = lua::to<const char *>(L, 2);
		const char *program_data = lua::to<const char *>(L, 3);
		const char *program_entry = lua::to<const char *>(L, 4);

		ShaderProgram::Type program_type = ShaderProgram::type_for_name(program_type_name);
		if(program_type != ShaderProgram::NONE)
		{
			b->program(program_type)->set_program_data(program_data);
			b->program(program_type)->set_program_entry(program_entry);

			if(program_type == ShaderProgram::GEOMETRY_PROGRAM) {
				if(
					lua::is<int>(L, 5) && 
					lua::is<const char *>(L, 6) &&
					lua::is<const char *>(L, 7)
				) {
					int vertices_out = lua::to<int>(L, 5);
					string input_type = string(lua::to<const char *>(L, 6));
					string output_type = string(lua::to<const char *>(L, 7));

					b->program(program_type)->set_vertices_out(vertices_out);
					b->program(program_type)->set_input_type(input_type);
					b->program(program_type)->set_output_type(output_type);
				}
				else {
					// error, geometry shader has no program parameters
				}
			}
			else if(program_type == ShaderProgram::FRAGMENT_PROGRAM) {
				int n = lua_gettop(L);
				if(lua_isnumber(L, n)) {
					int targets = (int)lua_tonumber(L, n);
					b->program(program_type)->set_targets(targets);
				}
				else {
					// error, fragment shader has no number of targets
				}
			}
		}
		else {
			// error, invalid program type
		}
	}
	else {
		// error
	}

	return 0;
}

int Shader :: create_parameter(lua_State *L) {
	if(
		lua_islightuserdata(L, 1) && 
		lua::is<const char *>(L, 2) &&
		lua::is<const char *>(L, 3) && 
		lua_istable(L, 4)
	) {
		ShaderBackend *b = (ShaderBackend *)lua_touserdata(L, 1);
		const char *param_name = lua::to<const char *>(L, 2);
		string param_type_name(lua::to<const char *>(L, 3));
		ShaderParam::Type param_type = ShaderParam::type_for_name(param_type_name.c_str());
		int nvals = lua_objlen(L, 4);
		int count = nvals / ShaderParam::size_for_type(param_type);
		ShaderParam *p = new ShaderParam(param_name, param_type, count);
		b->add_param(p);

		if(lua::is<const char *>(L, 5)) {
			string state(lua::to<const char *>(L, 5));
			ShaderParam::State s = ShaderParam::state_for_name(state.c_str());
			p->set_state(s);
		}
		if(lua::is<const char *>(L, 6)) {
			string transform(lua::to<const char *>(L, 6));
			ShaderParam::Transform t = ShaderParam::transform_for_name(transform.c_str());
			p->set_transform(t);
		}

		switch( ShaderParam::data_type_for_type(param_type) ) {
			case ShaderParam::FLOAT: {
				float *vals = new float[nvals];
				for(int i=0; i < nvals; i++) {
					lua_rawgeti(L, 4, i+1);
					vals[i] = lua_tonumber(L, -1);
					lua_pop(L, 1);
				}
				p->set(nvals, vals);
				delete vals;
			} break;

			case ShaderParam::INT: {
				int *vals = new int[nvals];
				for(int i=0; i < nvals; i++) {
					lua_rawgeti(L, 4, i+1);
					vals[i] = (int)lua_tonumber(L, -1);
					lua_pop(L, 1);
				}
				p->set(nvals, vals);
				delete vals;
			} break;

			default: break;
		}
	}
	else {
		//error
	}

	return 0;
}

int Shader :: create_attribute(lua_State *L) {
	if(lua_islightuserdata(L, 1) && 
		lua::is<const char *>(L, 2) &&
		lua::is<const char *>(L, 3)
	) {
		ShaderBackend *b = (ShaderBackend *)lua_touserdata(L, 1);
		const char *attr_name = lua::to<const char *>(L, 2);
		string attr_type_name(lua::to<const char *>(L, 3));
		ShaderAttribute::Type attr_type = ShaderParam::type_for_name(attr_type_name.c_str());
		b->add_attribute(attr_name, attr_type);
	}
	else {
		// error
	}

	return 0;
}

void Shader :: run_shl() {
	if(mShaderBackend) {
		mShaderBackend->destroy();
		delete mShaderBackend;
		mShaderBackend = NULL;
	}
	
	int i=0;
	while(ShaderBackend::c_languageNames[i]) {
		lua_getglobal(L, "processShader");
		if(! lua_isfunction(L, -1)) {
			// error, no function to call to process shader
			lua_pop(L, 1);
			i++;
			continue;
		}
		lua_pushlightuserdata(L, (void *)this);
		lua_pushstring(L, ShaderBackend::c_languageNames[i]);
		int err = lua_pcall(L, 2, 0, 0);
		if(err) {
			printf("error processing %s shader: %s\n", ShaderBackend::c_languageNames[i], lua_tostring(L, -1));
			lua_pop(L, 1);
			return;
		}
		i++;
		
		if(mShaderBackend) break;
	}
}

void Shader :: source(const char *shlsrc) {
	// run the file through the shl loader
	mFileData.assign(shlsrc);
	mHasFilename = false;

	init_shl();
	lua_pushstring(L, shlsrc);
	int err = luaL_dostring(L, shlsrc);
	if(err) {
		fprintf(stderr, "error invalid shl file: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	run_shl();
}

void Shader :: set_filename(const char *file) {
	// run the file through the shl loader
	mFilename.assign(file);
	mHasFilename = true;

	init_shl();
	lua_pushstring(L, file);
	lua_setglobal(L, SHADER_FILENAME);
	int err = luaL_dofile(L, file);
	if(err) {
		fprintf(stderr, "error invalid shl file: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	run_shl();
}

} // gfx::
} // al::
