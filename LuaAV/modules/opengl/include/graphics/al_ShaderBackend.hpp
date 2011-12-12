#ifndef AL_SHADER_BACKEND_H
#define AL_SHADER_BACKEND_H 1

#include "graphics/al_ShaderProgram.hpp"
#include "graphics/al_ShaderAttribute.hpp"
#include "graphics/al_ShaderParam.hpp"
#include <map>
#include <string>
#include <set>

#define INVALID_SHADER	-1

using namespace std;

namespace al {
namespace gfx {

class Shader;

/*!
	\class ShaderBackend

	The abstract base class for shader language backends.  Shader backends must implement
	the basic functionality for compiling, linking, and binding shader code as well as
	enumerating and submitting shader parameter data.
*/
class ShaderBackend {
public:

	/*!
		Type of shader backend
	*/
	enum Type {
		NONE = -1,
		GLSL,		///< OpenGL GLSL shader
		Cg			///< Nvidia Cg shader
	};
	
	
	typedef map<string, ShaderParam *>::iterator	ShaderParamIter;

protected:
	ShaderBackend(Shader *p, Type type);

public:
	virtual ~ShaderBackend();

	// Get a shader type for a string representation of that type.
	static Type type_for_name(const char *name);

	// create a backend
	static ShaderBackend * create(Shader *p, Type type);

	
	virtual int compile() = 0;
	virtual int link() = 0;
	virtual void destroy() = 0;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void submit() = 0;

	void report_compilation_error();

	ShaderProgram * program(ShaderProgram::Type programType) {
		return mPrograms + programType;
	}
	
	
	int get_id() {return mShader;}

	Type type() {return mType;}

	ShaderParam * param(const char *name);
	ShaderParamIter param_begin() {return mParameters.begin();}
	ShaderParamIter param_end() {return mParameters.end();}

	
	ShaderAttribute * attr(const char *name);
	void enabled_attribute(ShaderAttribute *a);

	void add_param(ShaderParam *p);

	ShaderAttribute * add_attribute(const char *name, ShaderAttribute::Type type);
	map<string, ShaderAttribute *> * attributes() {return &mAttributes;}

	static const char *		c_languageNames[];

protected:
	Shader							*mParent;						///< Pointer to parent shader object
	Type							mType;							///< The type of the shader
	int								mShader;						///< The shader's ID
	ShaderProgram					mPrograms[MAX_PROGRAM_COUNT];	///< The shader programs
	map<string, ShaderParam *>		mParameters;					///< The shader parameters
	map<string, ShaderAttribute *>	mAttributes;					///< The shader attributes
	set<ShaderAttribute *>			mEnabledAttributes;				///< Enabled Attributes during a bind
};


} // gfx::
} // al::

#endif	// AL_SHADER_BACKEND_H
