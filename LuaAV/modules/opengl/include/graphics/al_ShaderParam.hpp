#ifndef AL_SHADER_PARAM_H
#define AL_SHADER_PARAM_H 1

#include "graphics/al_Common.hpp"
#include <string>

using namespace std;

/*
Assume that a shader with the following
variables is being used:
uniform float specIntensity;
uniform vec4 specColor;
uniform float t[2];
uniform vec4 colors[3];

In the application, the code for setting the variables could be:
GLint loc1,loc2,loc3,loc4;
float specIntensity = 0.98;
float sc[4] = {0.8,0.8,0.8,1.0};
float threshold[2] = {0.5,0.25};
float colors[12] = {0.4,0.4,0.8,1.0, 0.2,0.2,0.4,1.0, 0.1,0.1,0.1,1.0};
loc1 = glGetUniformLocationARB(p,"specIntensity");
glUniform1fARB(loc1,specIntensity);
loc2 = glGetUniformLocationARB(p,"specColor");
glUniform4fvARB(loc2,1,sc);
loc3 = glGetUniformLocationARB(p,"t");
glUniform1fvARB(loc3,2,threshold);
loc4 = glGetUniformLocationARB(p,"colors");
glUniform4fvARB(loc4,3,colors);

*/

/*

The type argument will return a pointer to the attribute variable's data type.
The symbolic constants GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_FLOAT_MAT2,
GL_FLOAT_MAT3, GL_FLOAT_MAT4 may be returned. The size argument will return the
size of the attribute, in units of the type returned in type .


Can allocate appropriate amount of memory in setSize function

*/

namespace al {
namespace gfx {

#define DEFAULT_SIZE		16
#define INVALID_LOCATION	0

/*
#ifdef WIN32
typedef int		gloint;
#elif __APPLE__
typedef GLint	gloint;
#else
typedef GLint   gloint;
#endif
*/
/*!
	\class ShaderParam

	A shader parameter.  ShaderParam is language independent and contains all the necessary data
	for managing parameter values.  During the submission process, ShaderBackends convert the
	data to language specific function calls.
*/
class ShaderParam {
public:

	/*!
		The basic parameter types
	*/
	enum Type {
		NONE = 0,	//uninitialized type

		FLOAT,		///< A single float value
		VEC2,		///< Two float values
		VEC3,		///< Three float values
		VEC4,		///< Four float values

		INT,		///< A single int value
		INT2,		///< Two int values
		INT3,		///< Three int values
		INT4,		///< Four int values

		BOOL,		///< A single bool value
		BOOL2,		///< Two bool values
		BOOL3,		///< Three bool values
		BOOL4,		///< Four bool values

		MAT22,		///< A 2x2 matrix
		MAT33,		///< A 3x3 matrix
		MAT44,		///< A 4x4 matrix

		SAMPLER_1D,			///< A 1D texture
		SAMPLER_2D,			///< A 2D texture
		SAMPLER_RECT,		///< A rectangular texture
		SAMPLER_3D,			///< A 3D texture
		SAMPLER_CUBE,		///< A cubemap texture
		SAMPLER_1D_SHADOW,	///< A 1D depth texture
		SAMPLER_2D_SHADOW	///< A 2D depth texture

		//textures? non square matrices? attributes?
	};

	/*!
		Parameter state.  This is mostly used by the Cg backend
		for 4x4 matrices right now.
	*/
	enum State {
		NOSTATE = 0,					///< Invalid state
		MODELVIEW_MATRIX,				///< The ModelView Matrix
		PROJECTION_MATRIX,				///< The Projection Matrix
		TEXTURE_MATRIX,					///< The Texture Matrix
		MODELVIEW_PROJECTION_MATRIX,	///< The combined ModelView and Projection matrices
	};

	/*!
		Transformation.  This is mostly used by the Cg backend
		for 4x4 matrices right now.
	*/
	enum Transform {
		NOTRANSFORM = 0,		///< Invalid transformation
		IDENTITY,				///< Identity transformation
		TRANSPOSE,				///< Transpose transformation
		INVERSE,				///< Inverse transformation
		INVERSE_TRANSPOSE,		///< Inverse Transpose transformation
	};

public:
	ShaderParam(const char *name, Type type, int count);
	ShaderParam(const char *name, int location, Type type, int count);
	~ShaderParam();

	void set(int i);
	void set(int ac, int *av);

	void set(float f);
	void set(int ac, float *av);

	void set_location(ptrdiff_t location) {mLocation = location;}
	ptrdiff_t get_location() {return mLocation;}

	void set_active(bool active) {mActive = active;}
	bool get_active() {return mActive;}

	void set_type(Type type) {mType = type;}
	Type get_type() {return mType;}

	int * idata() {return mIdata;}
	float * fdata() {return mFdata;}

	void set_transpose(int transpose) {mTranspose = transpose;}
	int get_transpose() {return mTranspose;}

	void set_count(int count);
	int get_count() {return mCount;}

	void set_state(State state) {mState = state;}
	State get_state() {return mState;}

	void set_transform(Transform transform) {mTransform = transform;}
	Transform get_transform() {return mTransform;}


	const char * name() {return mName.data();}

	static int size_for_type(Type type);
	static Type data_type_for_type(Type type);
	static const char * name_for_type(ShaderParam::Type type);
	static Type type_for_name(const char *name);
	static State state_for_name(const char *name);
	static Transform transform_for_name(const char *name);

protected:
	void initialize();

	bool		mActive;		///< Whether the parameter is active or not (could be optimized away)
	Type		mType;			///< Primitive type (i.e. FLOAT, VEC4, etc.)
	string		mName;			///< Name of parameter
	ptrdiff_t	mLocation;		///< Location in program

	int			mCount;			///< Number of elements in parameter array (if not an array, this is 1)

	int			mIdataSize;		///< Integer array size
	int			*mIdata;		///< Integer array values

	int			mFdataSize;		///< Float array size
	float		*mFdata;		///< Float array values

	State		mState;			///< State of parameter (mostly for Cg 4x4 matrices)
	Transform	mTransform;		///< Transform of parameter (mostly for Cg 4x4 matrices)

	int			mTranspose;		///< For Matrix transpose in GLSL
};

} // gfx::
} // al::

#endif	// AL_SHADER_PARAM_H
