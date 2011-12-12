#ifndef AL_SHADER_BACKEND_GLSL_H
#define AL_SHADER_BACKEND_GLSL_H 1

#include "graphics/al_Shader.hpp"
#include <string>

using namespace std;

namespace al {
namespace gfx {

/*!
	\class GLSLBackend

	A GLSL shader backend for the Shader class.  Use of this backend is transparent to the user.
	If the GLSL language is specified in a .shl file, then a GLSLBackend will be created.
*/
class GLSLBackend : public ShaderBackend {
public:
	class GLSLAttribute : public ShaderAttribute {
	public:
		GLSLAttribute(ShaderBackend *shader, const char *name, ShaderAttribute::Type type);
		GLSLAttribute(ShaderBackend *shader, const char *name, int location, ShaderAttribute::Type type);

		virtual void submit(float *v);
		virtual void submit(int *v);
		virtual void submit(AlloArray *v);
//		virtual void submit(VBO *v);
		virtual void disable();
		virtual void bind_location();
	};

public:
	GLSLBackend(Shader *p);
	virtual ~GLSLBackend();

	virtual int compile();
	virtual int link();
	virtual void destroy();
	virtual void bind();
	virtual void unbind();
	virtual void submit();

	static ShaderParam::Type param_type_from_gltype(GLenum gltype);
	static GLenum glenum_from_program_type(int type);
	static GLenum primitive_for_name(const char *name);

protected:
	int compile_program(GLenum program_type, const char *program_code, int &program_id);
	void get_params();
	void disable_attributes();
};

}	// gfx::
}	// al::

#endif	// AL_SHADER_BACKEND_GLSL_H
