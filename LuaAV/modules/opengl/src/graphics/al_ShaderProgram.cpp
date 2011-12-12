#include "graphics/al_ShaderProgram.hpp"
#include <string.h>

namespace al {
namespace gfx {

ShaderProgram :: ShaderProgram()
:	mProgramID(INVALID_PROGRAM),
	mVerticesOut(0),
	mInputType("TRIANGLES"),
	mOutputType("TRIANGLE_STRIPS"),
	mTargets(1)
{}

ShaderProgram :: ShaderProgram(const char *program_data)
:	mProgramID(INVALID_PROGRAM),
	mInputType("TRIANGLES"),
	mOutputType("TRIANGLE_STRIPS"),
	mTargets(1)
{
	set_program_data(program_data);
}

ShaderProgram :: ~ShaderProgram() {
}

bool ShaderProgram :: has_data() {
	string s;
	return s != mProgramData;
}

void ShaderProgram :: set_program_data(const char *program_data) {
	mProgramData.assign(program_data);
}

void ShaderProgram :: set_program_entry(const char *entry) {
	mEntry.assign(entry);
}

void ShaderProgram :: set_vertices_out(int vo) {
	mVerticesOut = (vo < 0) ? 0 : vo;
}

string & string_upper(string &s) {
	for(int i=0; i < s.length(); i++) {
		s[i] = toupper(s[i]);
	}
	return s;
}

void ShaderProgram :: set_input_type(string s) {
	mInputType.assign(string_upper(s));
}

void ShaderProgram :: set_output_type(string s) {
	mOutputType.assign(string_upper(s));
}

ShaderProgram::Type ShaderProgram :: type_for_name(const char *name) {
	string n(name);
	string_upper(n);

	if(strcmp(n.c_str(), "VERTEX") == 0) {
		return VERTEX_PROGRAM;
	}
	else if(strcmp(n.c_str(), "GEOMETRY") == 0) {
		return GEOMETRY_PROGRAM;
	}
	else if(strcmp(n.c_str(), "FRAGMENT") == 0) {
		return FRAGMENT_PROGRAM;
	}

	return NONE;
}

} // gfx::
} // al::
