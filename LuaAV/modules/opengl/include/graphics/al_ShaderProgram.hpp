#ifndef AL_SHADER_PROGRAM_H
#define AL_SHADER_PROGRAM_H 1

#include "graphics/al_Common.hpp"
#include <string>

using std::string;

namespace al {
namespace gfx {

#define MAX_PROGRAM_COUNT	3
#define INVALID_PROGRAM		-1

/*!
	\class ShaderProgram

	A Shader program.  ShaderProgram in language independent.  It contains data used by
	language-specific backends to manage shader programs.
*/
class ShaderProgram {
public:

	/*!
		The progam types
	*/
	enum Type {
		NONE = -1,			///< Invalid program type
		VERTEX_PROGRAM,		///< A Vertex program
		GEOMETRY_PROGRAM,	///< A Geometry program
		FRAGMENT_PROGRAM,	///< A Fragment program
	};

public:
	ShaderProgram();
	ShaderProgram(const char *program_data);

	~ShaderProgram();

	void set_program_data(const char *program_data);
	const char * get_program_data() {return mProgramData.data();}

	void set_program_entry(const char *entry);
	const char * set_program_entry() {return mEntry.data();}

	bool has_data();
	void set_program_id(int program_id) {mProgramID = program_id;}
	int get_program_id() {return mProgramID;}


	void set_vertices_out(int vo);
	int get_vertices_out() {return mVerticesOut;}

	void set_input_type(string s);
	string & get_input_type() {return mInputType;}

	void set_output_type(string s);
	string & get_output_type() {return mOutputType;}

	void set_targets(int targets) {mTargets = targets;}
	int get_targets() {return mTargets;}

	static Type type_for_name(const char *name);

protected:

	std::string mProgramData;	///< The shader program code
	std::string mEntry;			///< The entry point to the program code
	int mProgramID;				///< The program ID

	// Geometry Shader parameters
	int mVerticesOut;			///< Number of vertices emitted
	string mInputType;			///< Input primitive
	string mOutputType;			///< Output primitive
	int mTargets;				///< Number of targets (for fragment program)
};

} // gfx::
} // al::

#endif	// AL_SHADER_PROGRAM_H
