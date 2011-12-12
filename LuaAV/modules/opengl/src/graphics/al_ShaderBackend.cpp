#include "graphics/al_ShaderBackend.hpp"
#include "graphics/al_ShaderBackendGLSL.hpp"
#include <string.h>


namespace al {
namespace gfx {

const char * ShaderBackend::c_languageNames[] = {
	"GLSL",
	"CG",
	NULL
};


ShaderBackend::Type ShaderBackend :: type_for_name(const char *name) {
	if(strcmp(name, "GLSL") == 0) {
		return GLSL;
	}
	else if(strcmp(name, "CG") == 0) {
		return Cg;
	}
	else {
		return NONE;
	}
}


ShaderBackend :: ShaderBackend(Shader *p, Type type)
:	mParent(p),
	mType(type),
	mShader(INVALID_SHADER)
{

}

ShaderBackend :: ~ShaderBackend() {
	//delete paramters
	map<string, ShaderParam *>::iterator it = mParameters.begin();
	for(; it != mParameters.end(); it++) {
		delete it->second;
	}
}

ShaderBackend * ShaderBackend :: create(Shader *p, Type type) {
	switch(type) {
		case GLSL:
			return new GLSLBackend(p);
		default:
			return NULL;
	}
}

ShaderParam * ShaderBackend :: param(const char *name) {
	map<string, ShaderParam *>::iterator it = mParameters.find( name );
	if(it != mParameters.end()) {
		return it->second;
	}
	else {
		// invalid parameter
		return NULL;
	}
}

void ShaderBackend :: report_compilation_error() {
	if(mParent) {
		printf("Error compiling: %s\n", mParent->get_filename().data());
	}
}

ShaderAttribute * ShaderBackend :: attr(const char *name) {
	map<string, ShaderAttribute *>::iterator it = mAttributes.find( name );
	if(it != mAttributes.end()) {
		return it->second;
	}
	else {
		// invalid parameter
		return NULL;
	}
}

void ShaderBackend :: add_param(ShaderParam *p) {
	map<string, ShaderParam *>::iterator it = mParameters.find( p->name() );
	if(it != mParameters.end()) {
		ShaderParam *old_p = it->second;
		mParameters.erase(it);
		delete old_p;
	}
	mParameters[p->name()] = p;
}

ShaderAttribute * ShaderBackend :: add_attribute(const char *name, ShaderAttribute::Type type) {
	ShaderAttribute *a = NULL;
	if(mType == GLSL) {
		a = new GLSLBackend::GLSLAttribute(this, name, type);
	}

	if(a) {
		map<string, ShaderAttribute *>::iterator it = mAttributes.find( a->name() );
		if(it != mAttributes.end()) {
			ShaderAttribute *old_a = it->second;
			mAttributes.erase(it);
			delete old_a;
		}
		mAttributes[a->name()] = a;
	}
	return a;
}

void ShaderBackend :: enabled_attribute(ShaderAttribute *a) {
	mEnabledAttributes.insert(a);
}

}	// gfx::
}	// al::
