#include "graphics/al_ShaderBackendGLSL.hpp"
#include "protocol/al_GraphicsBackendOpenGL.hpp"
#include <string.h>

namespace al {
namespace gfx {

GLSLBackend::GLSLAttribute :: GLSLAttribute(ShaderBackend *shader, const char *name, ShaderAttribute::Type type)
: ShaderAttribute(shader, name, type)
{}

GLSLBackend::GLSLAttribute :: GLSLAttribute(ShaderBackend *shader, const char *name, int location, ShaderAttribute::Type type)
: ShaderAttribute(shader, name, location, type)
{}

void GLSLBackend::GLSLAttribute :: submit(float *v) {
	switch(mDataSize) {
	case 1:	glVertexAttrib1fv(mLocation, v); break;
	case 2:	glVertexAttrib2fv(mLocation, v); break;
	case 3:	glVertexAttrib3fv(mLocation, v); break;
	case 4:	glVertexAttrib4fv(mLocation, v); break;
	}
}

void GLSLBackend::GLSLAttribute :: submit(int *v) {
	switch(mDataSize) {
//	case 1:	glVertexAttrib1iv(mLocation, v); break;
//	case 2:	glVertexAttrib2iv(mLocation, v); break;
//	case 3:	glVertexAttrib3iv(mLocation, v); break;
	case 4:	glVertexAttrib4iv(mLocation, v); break;
	}
}

void GLSLBackend::GLSLAttribute :: submit(AlloArray *v) {
	GLenum type = GraphicsBackendOpenGL::type_for_array_type(v->header.type);
	
	// check total bytes with mBaseType and planecount
	
	glEnableVertexAttribArray(mLocation);
	glVertexAttribPointer(mLocation, mDataSize, type, GL_FALSE, 0, (GLvoid *)(v->data.ptr));
	mShader->enabled_attribute(this);
}

/*
void GLSLBackend::GLSLAttribute :: submit(VBO *v) {
	v->bind();
	glEnableVertexAttribArray(mLocation);
	glVertexAttribPointer(mLocation, mDataSize, v->type(), GL_FALSE, 0, 0);
	mShader->enabledAttribute(this);
}
*/

void GLSLBackend::GLSLAttribute :: disable() {
	glDisableVertexAttribArray(mLocation);
}

// not working right now
void GLSLBackend::GLSLAttribute :: bind_location() {
	/*if(mUpdate && mShader->getID() > INVALID_SHADER) {
		glBindAttribLocation(mShader->getID(), mLocation, mName.c_str());
		mUpdate = false;
	}*/
}


GLSLBackend :: GLSLBackend(Shader *p)
:	ShaderBackend(p, GLSL)
{

}

GLSLBackend :: ~GLSLBackend() {

}

int GLSLBackend :: compile() {
	int err = 0;

	// compile programs
	for(int i=0; i < MAX_PROGRAM_COUNT; i++) {
		if(mPrograms[i].has_data()) {
			int program_id = 0;
			int err = compile_program(
						glenum_from_program_type(i),
						mPrograms[i].get_program_data(), program_id
					);

			if(err) {
				// custom error function?
				printf("Invalid Program\n");
				err = 1;
				mPrograms[i].set_program_id(INVALID_PROGRAM);
				break;
			}
			else {
				mPrograms[i].set_program_id(program_id);
			}
		}
	}

	GraphicsBackendOpenGL::gl_error("Compiling GLSL Shader");

	// return any errors
	return err;
}

int GLSLBackend :: link() {
	int err = 0;

	// check if already has one, may not need to recompile
	GLuint shader = glCreateProgram();
	mShader = (int)shader;

	for(int i=0; i < MAX_PROGRAM_COUNT; i++) {
		if( mPrograms[i].get_program_id() != INVALID_PROGRAM) {
			glAttachShader(shader, (GLuint)mPrograms[i].get_program_id());

			if(i == ShaderProgram::GEOMETRY_PROGRAM) {
				GLenum inprim = primitive_for_name( mPrograms[i].get_input_type().c_str() );
				GLenum outprim = primitive_for_name( mPrograms[i].get_output_type().c_str() );

				glProgramParameteriEXT(shader, GL_GEOMETRY_VERTICES_OUT_EXT, mPrograms[i].get_vertices_out());
				glProgramParameteriEXT(shader, GL_GEOMETRY_INPUT_TYPE_EXT, inprim);
				glProgramParameteriEXT(shader, GL_GEOMETRY_OUTPUT_TYPE_EXT, outprim);
				GraphicsBackendOpenGL::gl_error("Setting Geometry Program Parameters");
			}
		}
	}

	// bind attributes to locations
	map<string, ShaderAttribute *>::iterator ita = mAttributes.begin();
	for(; ita != mAttributes.end(); ita++) {
		(*ita).second->bind_location();
	}

	// Link the program object and print out the info log.
	GLint shaders_linked = GL_FALSE;
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &shaders_linked );

    if(shaders_linked == GL_FALSE ) {
		char str[2048];
		glGetProgramInfoLog( shader, 2048, NULL, str );
		printf("Shader linking error\n %s\n", str);
		err = 1;
	}

	GraphicsBackendOpenGL::gl_error("Linking GLSL Shader");

	// return any errors
	return err;
}

void GLSLBackend :: destroy() {
	if(mShader != INVALID_SHADER) {
		unbind();
		for(int i=0; i < MAX_PROGRAM_COUNT; i++) {
			if(mPrograms[i].get_program_id() != INVALID_PROGRAM) {
				glDetachShader((GLuint)mShader, (GLuint)mPrograms[i].get_program_id());
				glDeleteShader((GLuint)mPrograms[i].get_program_id());
				mPrograms[i].set_program_id(INVALID_PROGRAM);
			}
		}
		GLuint shader = (GLuint)mShader;
		glDeleteProgram(shader);
		mShader = INVALID_SHADER;
	}
	GraphicsBackendOpenGL::gl_error("Destroying GLSL Shader");
}

void GLSLBackend :: get_params() {
	GLuint program = (GLuint)mShader;
	GLint numActiveUniforms = 0;
	GLint numActiveAttributes = 0;

	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttributes);

	for(int j=0; j < numActiveUniforms; j++)
	{
		GLsizei length;
		GLint size;
		GLenum gltype;
		char name[256];


		glGetActiveUniform(program,
							j,
							256,
							&length,
							&size,
							&gltype,
							name);

		// check for array names
		if(name[ strlen(name)-3 ] == '[' && name[ strlen(name)-1 ] == ']') {
			name[ strlen(name)-3 ] = '\0';
		}

		//could already have a param if the user set some values before compiling
		map<string, ShaderParam *>::iterator it = mParameters.find(name);
		if(it != mParameters.end()) {
			ShaderParam::Type type = param_type_from_gltype(gltype);
			ShaderParam &p = *(it->second);
			p.set_active(true);
			p.set_location(j);
			p.set_type(type);
			p.set_count(size);
		}
		/*
		Only use params defined in shader file
		else
		{
			ShaderParam *p = new ShaderParam(name, j, type, size);
			mParameters[ name ] = p;
		}*/
	}

	for(int j=0; j < numActiveAttributes; j++) {
		GLsizei length;
		GLint size;
		GLenum gltype;
		char name[256];	// could query for max char length

		glGetActiveAttrib(program,
							j,
							256,
							&length,
							&size,
							&gltype,
							name);

		map<string, ShaderAttribute *>::iterator it = mAttributes.find(name);
		if(it != mAttributes.end()) {
			// TODO: FIX THIS HACK
			#if defined(MURO_LINUX_VERSION)
			int loc = (j < 0) ? 1 : j+1;
			#else
			int loc = (j <= 0) ? 1 : j;
			#endif
			ShaderParam::Type type = param_type_from_gltype(gltype);
			ShaderAttribute &a = *(it->second);
			a.realize_location(loc);
			a.set_type(type);
			//a.setCount(size);
		}
	}
}

ShaderParam::Type GLSLBackend :: param_type_from_gltype(GLenum gltype) {
	ShaderParam::Type type = ShaderParam::NONE;

	switch(gltype) {
		case GL_FLOAT:			type = ShaderParam::FLOAT;	break;
		case GL_FLOAT_VEC2:		type = ShaderParam::VEC2;	break;
		case GL_FLOAT_VEC3:		type = ShaderParam::VEC3;	break;
		case GL_FLOAT_VEC4:		type = ShaderParam::VEC4;	break;

		case GL_INT:			type = ShaderParam::INT;	break;
		case GL_INT_VEC2:		type = ShaderParam::INT2;	break;
		case GL_INT_VEC3:		type = ShaderParam::INT3;	break;
		case GL_INT_VEC4:		type = ShaderParam::INT4;	break;

		case GL_BOOL:			type = ShaderParam::BOOL;	break;
		case GL_BOOL_VEC2:		type = ShaderParam::BOOL2;	break;
		case GL_BOOL_VEC3:		type = ShaderParam::BOOL3;	break;
		case GL_BOOL_VEC4:		type = ShaderParam::BOOL4;	break;

		case GL_FLOAT_MAT2:		type = ShaderParam::MAT22;	break;
		case GL_FLOAT_MAT3:		type = ShaderParam::MAT33;	break;
		case GL_FLOAT_MAT4:		type = ShaderParam::MAT44;	break;

		case GL_SAMPLER_1D:		type = ShaderParam::SAMPLER_1D;	break;
		case GL_SAMPLER_2D:		type = ShaderParam::SAMPLER_2D;	break;
		case GL_SAMPLER_2D_RECT_ARB: type = ShaderParam::SAMPLER_RECT; break;
		case GL_SAMPLER_3D:		type = ShaderParam::SAMPLER_3D;	break;
		case GL_SAMPLER_CUBE:	type = ShaderParam::SAMPLER_CUBE; break;
		case GL_SAMPLER_1D_SHADOW: type = ShaderParam::SAMPLER_1D_SHADOW; break;
		case GL_SAMPLER_2D_SHADOW: type = ShaderParam::SAMPLER_2D_SHADOW; break;
	}

	return type;
}

GLenum GLSLBackend :: glenum_from_program_type(int type) {
	GLenum glEnum = GL_VERTEX_SHADER_ARB;

	switch(type) {
		case 0: glEnum = GL_VERTEX_SHADER_ARB;		break;
		case 1: glEnum = GL_GEOMETRY_SHADER_EXT;	break;
		case 2: glEnum = GL_FRAGMENT_SHADER_ARB;	break;
	}

	return glEnum;
}

void GLSLBackend :: bind() {
	if(mShader == INVALID_SHADER) {
		compile();
		link();
		get_params();
	}

	if(mShader != INVALID_SHADER) {
		GLuint shader = (GLuint)mShader;
		glUseProgram(shader);
		submit();
	}
	GraphicsBackendOpenGL::gl_error("Binding GLSL Shader");
}

void GLSLBackend :: unbind() {
	disable_attributes();

	if(mShader != INVALID_SHADER) {
		glUseProgram(0);
	}
	GraphicsBackendOpenGL::gl_error("Unbinding GLSL Shader");
}

void GLSLBackend :: disable_attributes() {
	set<ShaderAttribute *>::iterator it = mEnabledAttributes.begin();
	set<ShaderAttribute *>::iterator ite = mEnabledAttributes.end();
	for(; it != ite; ++it) {
		(*it)->disable();
	}
	mEnabledAttributes.clear();
	GraphicsBackendOpenGL::gl_error("Disabling GLSL Shader Attributes");
}

void GLSLBackend :: submit() {
	map<string, ShaderParam *>::iterator it = mParameters.begin();
	for(; it != mParameters.end(); it++) {
		ShaderParam *p = (*it).second;
		if(p->get_active()) {
			switch( p->get_type() )
			{
				case ShaderParam::FLOAT: glUniform1fv(p->get_location(), p->get_count(), p->fdata()); break;
				case ShaderParam::VEC2: glUniform2fv(p->get_location(), p->get_count(), p->fdata()); break;
				case ShaderParam::VEC3: glUniform3fv(p->get_location(), p->get_count(), p->fdata()); break;

				case ShaderParam::VEC4: glUniform4fv(p->get_location(), p->get_count(), p->fdata()); break;

				case ShaderParam::INT:
				case ShaderParam::SAMPLER_1D:
				case ShaderParam::SAMPLER_2D:
				case ShaderParam::SAMPLER_RECT:
				case ShaderParam::SAMPLER_3D:
				case ShaderParam::SAMPLER_CUBE:
				case ShaderParam::SAMPLER_1D_SHADOW:
				case ShaderParam::SAMPLER_2D_SHADOW: glUniform1iv(p->get_location(), p->get_count(), p->idata()); break;

				case ShaderParam::INT2: glUniform2iv(p->get_location(), p->get_count(), p->idata());  break;
				case ShaderParam::INT3: glUniform3iv(p->get_location(), p->get_count(), p->idata()); break;
				case ShaderParam::INT4: glUniform3iv(p->get_location(), p->get_count(), p->idata()); break;

				case ShaderParam::BOOL: break;
				case ShaderParam::BOOL2: break;
				case ShaderParam::BOOL3: break;
				case ShaderParam::BOOL4: break;

				case ShaderParam::MAT22: glUniformMatrix2fv(p->get_location(), 4, p->get_transpose(), p->fdata()); break;
				case ShaderParam::MAT33: glUniformMatrix3fv(p->get_location(), 9, p->get_transpose(), p->fdata()); break;
				case ShaderParam::MAT44: glUniformMatrix4fv(p->get_location(), 16, p->get_transpose(), p->fdata()); break;

				default: /* error invalid param */ break;
			}
		}
	}
	GraphicsBackendOpenGL::gl_error("Submitting GLSL Shader Uniform Parameters");
}

int GLSLBackend :: compile_program(GLenum program_type, const char *program_code, int &program_id) {
	char str[4096]; // For error messages from the GLSL compiler
	const GLchar *shaderText = program_code;
	int err = 0;

	// create a shader object resource
	GLuint program = glCreateShader(program_type);

	// set its source code and compile it
	glShaderSource(program, 1, &shaderText, NULL);
	glCompileShader(program);

	// check for errors
	GLint shader_compiled = 0;
	glGetShaderiv(program, GL_COMPILE_STATUS, &shader_compiled);

	if(shader_compiled == GL_FALSE) {
		glGetShaderInfoLog(program, sizeof(str), NULL, str);
		report_compilation_error();
		printf("Shader compile error\n %s\n", str);
		err = 1;
	}


	sprintf(str, "Compiling GLSL Program: %s",
		(program_type == GL_VERTEX_SHADER_ARB)	?	"Vertex Shader"		:
		(program_type == GL_GEOMETRY_SHADER_EXT) ?	"Geoemtry Shader"	:
													"Fragment Shader");
	GraphicsBackendOpenGL::gl_error(str);

	program_id = (int)program;
	return err;
}

GLenum GLSLBackend :: primitive_for_name(const char *name) {
	GLenum prim = GL_TRIANGLES;

	if(strcmp(name, "POINTS") == 0) {
		prim = GL_POINTS;
	}
	else if(strcmp(name, "LINES") == 0) {
		prim = GL_LINES;
	}
	else if(strcmp(name, "LINE_STRIP") == 0) {
		prim = GL_LINE_STRIP;
	}
	else if(strcmp(name, "TRIANGLES") == 0) {
		prim = GL_TRIANGLES;
	}
	else if(strcmp(name, "TRIANGLE_STRIP") == 0) {
		prim = GL_TRIANGLE_STRIP;
	}
	else if(strcmp(name, "LINES_ADJACENCY") == 0) {
		prim = GL_LINES_ADJACENCY_EXT;
	}
	else if(strcmp(name, "TRIANGLES_ADJACENCY") == 0) {
		prim = GL_TRIANGLES_ADJACENCY_EXT;
	}

	return prim;
}

} // gfx::
} // al::
