#ifndef AL_SHADER_H
#define AL_SHADER_H 1

#include <string>
#include <map>
#include <vector>

#include "protocol/al_GraphicsBackend.hpp"
#include "graphics/al_Common.hpp"
#include "graphics/al_GPUObject.hpp"
#include "allocore/types/al_Array.hpp"
#include "graphics/al_ShaderAttribute.hpp"
#include "graphics/al_ShaderProgram.hpp"
#include "graphics/al_ShaderParam.hpp"
#include "graphics/al_ShaderBackend.hpp"

// Lua here is completely internal, never coming
// into contact with potential host lua_States
#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef __cplusplus
}
#endif

using std::string;
using std::map;
using std::vector;

namespace al {
namespace gfx {


class Shader : public GPUObject {
public:
	typedef ShaderBackend::ShaderParamIter		ShaderParamIter;

public:
	Shader(GraphicsBackend *backend);
	virtual ~Shader();

	
	// load .shl shader source
	void source(const char *shlsrc);

	// load a .shl shader file
	void set_filename(const char *file);
	string get_filename() {return mFilename;}

	// Get a shader parameter.  Will return NULL if the parameter
	ShaderParam * param(const char *name);
	ShaderParamIter param_begin();
	ShaderParamIter param_end();

	// Get a shader attribute.  Will return NULL if the parameter
	ShaderAttribute * attr(const char *name);
	map<string, ShaderAttribute *> * attributes();

	void set_shader_backend(ShaderBackend *b) {mShaderBackend = b;}
	ShaderBackend * get_shader_backend() {return mShaderBackend;}

	void bind();
	void unbind();

protected:

	void init_shl();
	void run_shl();

	// Shl Shader construction functions (called by embedded Lua script)
	static int create_backend(lua_State *L);
	static int create_program(lua_State *L);
	static int create_parameter(lua_State *L);
	static int create_attribute(lua_State *L);

protected:
	void onDestroy();
	void onCreate() {
		// dummy id for proxy destruction of internal objects
		id(1);
	}

protected:

	GraphicsBackend *			mBackend;			///< The backend
	bool						mHasFilename;		///< Flag for if a file has been loaded
	string						mFilename;			///< Shader filename
	string						mFileData;			///< String representation of the shader file

	ShaderBackend *				mShaderBackend;		///< Active shader backend

	lua_State					*L;					///< Internal Lua state for loading .shl files
};

}	// gfx::
}	// al::

#endif	// AL_SHADER_H
