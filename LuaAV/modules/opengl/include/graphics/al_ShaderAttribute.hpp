#ifndef AL_SHADER_ATTRIBUTE_H
#define AL_SHADER_ATTRIBUTE_H 1

#include "graphics/al_Common.hpp"
#include "graphics/al_ShaderParam.hpp"
#include "allocore/types/al_Array.hpp"
//#include "VBO.h"

namespace al {
namespace gfx {

class ShaderBackend;

#define MAX_SHADER_ATTR_VALS	4

class ShaderAttribute {
public:
	typedef ShaderParam::Type	Type;

	ShaderAttribute(ShaderBackend *shader, const char *name, Type type);
	ShaderAttribute(ShaderBackend *shader, const char *name, int location, Type type);
	~ShaderAttribute();

	virtual void submit(float *v) = 0;
	virtual void submit(int *v) = 0;
	virtual void submit(AlloArray *v) = 0;
//	virtual void submit(VBO *v) = 0;
	virtual void disable() = 0;
	virtual void bind_location() {}

	const char * name() {return mName.data();}

	void set_location(int location);
	void realize_location(int location);
	int get_location() {return mLocation;}

	void set_type(Type type);
	Type type() {return mType;}
	
	int size() {return mDataSize;}

	Type base_type() {return mBaseType;}

protected:

	Type			mType;			///< Primitive type (i.e. FLOAT, VEC4, etc.)
	string			mName;			///< Name of parameter
	bool			mUpdate;		///< Update flag for when location is changed
	int				mLocation;		///< Location in program
	Type			mBaseType;		///< Whether INT or FLOAT
	ShaderBackend	*mShader;		///< Parent shader

	int				mDataSize;		///< Integer array size
};

}	// gfx::
}	// al::

#endif	// AL_SHADER_ATTRIBUTE_H
