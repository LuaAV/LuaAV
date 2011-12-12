#include "graphics/al_ShaderParam.hpp"
#include <string.h>

namespace al {
namespace gfx {

ShaderParam :: ShaderParam(const char *name, Type type, int count)
	: mActive(false),
	mLocation(INVALID_LOCATION),
	mType(type)
{
	mName.assign(name);

	mIdataSize = DEFAULT_SIZE;
	mIdata = new int[DEFAULT_SIZE];

	mFdataSize = DEFAULT_SIZE;
	mFdata = new float[DEFAULT_SIZE];

	// will alloc enough memory
	set_count(count);
}

ShaderParam :: ShaderParam(const char *name, int location, Type type, int count)
	: mActive(false),
	mLocation(location),
	mType(type)
{
	mName.assign(name);

	mIdataSize = DEFAULT_SIZE;
	mIdata = new int[DEFAULT_SIZE];

	mFdataSize = DEFAULT_SIZE;
	mFdata = new float[DEFAULT_SIZE];

	// will alloc enough memory
	set_count(count);
}

ShaderParam :: ~ShaderParam() {
	delete[] mIdata;
	delete[] mFdata;
}

void ShaderParam :: set(int i) {
	mIdata[0] = i;
}

void ShaderParam :: set(int ac, int *av) {
	if(ac > mFdataSize) {
		delete[] mFdata;
		mIdata = new int[ac];
	}

	for(int i=0; i < ac; i++) {
		mIdata[i] = av[i];;
	}
}

void ShaderParam :: set(float f) {
	mFdata[0] = f;
}

// is there a max size?
void ShaderParam :: set(int ac, float *av) {
	if(ac > mFdataSize) {
		delete[] mFdata;
		mFdata = new float[ac];
	}

	for(int i=0; i < ac; i++) {
		mFdata[i] = av[i];;
	}
}

void ShaderParam :: set_count(int count) {
	mCount = (count <= 0) ? 1 : count;	// min value is 1

	int unit_size = size_for_type(mType);
	int total_size = unit_size*mCount;

	switch( data_type_for_type(mType) ) {
		case FLOAT: {
			if(mFdataSize < total_size) {
				delete[] mFdata;
				mFdata = new float[total_size];
			}
		} break;

		case INT: {
			if(mIdataSize < total_size) {
				delete[] mIdata;
				mIdata = new int[total_size];
			}
		} break;

		default:
		break;
	}
}

int ShaderParam :: size_for_type(Type type)
{
	int size = 1;
	switch(type) {
		case FLOAT:	size = 1;	break;
		case VEC2:	size = 2;	break;
		case VEC3:	size = 3;	break;
		case VEC4:	size = 4;	break;
		case INT:	size = 1;	break;
		case INT2:	size = 2;	break;
		case INT3:	size = 3;	break;
		case INT4:	size = 4;	break;
		case BOOL:	size = 1;	break;
		case BOOL2: size = 2;	break;
		case BOOL3: size = 3;	break;
		case BOOL4: size = 4;	break;
		case MAT22: size = 4;	break;
		case MAT33: size = 9;	break;
		case MAT44: size = 16;	break;

		default: break;
	}
	return size;
}

ShaderParam::Type ShaderParam :: data_type_for_type(Type type) {
	switch(type) {
		case FLOAT:
		case VEC2:
		case VEC3:
		case VEC4:
		case MAT22:
		case MAT33:
		case MAT44:
			return FLOAT;

		case INT:
		case INT2:
		case INT3:
		case INT4:
		case BOOL:
		case BOOL2:
		case BOOL3:
		case BOOL4:
			return INT;

		default:
			return FLOAT;
	}
}

const char * ShaderParam :: name_for_type(ShaderParam::Type type) {
	switch(type) {
		case FLOAT:	return "float";
		case VEC2:	return "vec2";
		case VEC3:	return "vec3";
		case VEC4:	return "vec4";
		
		case INT:	return "int";
		case INT2:	return "int2";
		case INT3:	return "int3";
		case INT4:	return "int4";
		
		case BOOL:	return "bool";
		case BOOL2:	return "bool2";
		case BOOL3:	return "bool3";
		case BOOL4:	return "bool4";
		
		case MAT22:	return "mat22";
		case MAT33:	return "mat33";
		case MAT44:	return "mat44";
		default:	return "";
	}
}

string & string_upper2(string &s) {
	for(int i=0; i < s.length(); i++) {
		s[i] = toupper(s[i]);
	}
	return s;
}

ShaderParam::Type ShaderParam :: type_for_name(const char *name) {
	string n(name);
	string_upper2(n);
	
	if(strcmp(n.c_str(), "FLOAT") == 0) {
		return FLOAT;
	}
	else if(strcmp(n.c_str(), "VEC2") == 0) {
		return VEC2;
	}
	else if(strcmp(n.c_str(), "VEC3") == 0) {
		return VEC3;
	}
	else if(strcmp(n.c_str(), "VEC4") == 0) {
		return VEC4;
	}
	if(strcmp(n.c_str(), "INT") == 0) {
		return INT;
	}
	else if(strcmp(n.c_str(), "INT2") == 0) {
		return INT2;
	}
	else if(strcmp(n.c_str(), "INT3") == 0) {
		return INT3;
	}
	else if(strcmp(n.c_str(), "INT4") == 0) {
		return INT4;
	}
	if(strcmp(n.c_str(), "BOOL") == 0) {
		return BOOL;
	}
	else if(strcmp(n.c_str(), "BOOL2") == 0) {
		return BOOL2;
	}
	else if(strcmp(n.c_str(), "BOOL3") == 0) {
		return BOOL3;
	}
	else if(strcmp(n.c_str(), "BOOL4") == 0) {
		return BOOL4;
	}
	else if(strcmp(n.c_str(), "MAT22") == 0) {
		return MAT22;
	}
	else if(strcmp(n.c_str(), "MAT33") == 0) {
		return MAT33;
	}
	else if(strcmp(n.c_str(), "MAT44") == 0) {
		return MAT44;
	}

	return NONE;
}

ShaderParam::State ShaderParam :: state_for_name(const char *name) {
	string n(name);
	string_upper2(n);
	
	if(strcmp(n.c_str(), "MODELVIEW_MATRIX") == 0) {
		return MODELVIEW_MATRIX;
	}
	else if(strcmp(n.c_str(), "PROJECTION_MATRIX") == 0) {
		return PROJECTION_MATRIX;
	}
	else if(strcmp(n.c_str(), "TEXTURE_MATRIX") == 0) {
		return TEXTURE_MATRIX;
	}
	else if(strcmp(n.c_str(), "MODELVIEW_PROJECTION_MATRIX") == 0) {
		return MODELVIEW_PROJECTION_MATRIX;
	}

	return NOSTATE;
}

ShaderParam::Transform ShaderParam :: transform_for_name(const char *name) {
	string n(name);
	string_upper2(n);
	
	if(strcmp(n.c_str(), "IDENTITY") == 0) {
		return IDENTITY;
	}
	else if(strcmp(n.c_str(), "TRANSPOSE") == 0) {
		return TRANSPOSE;
	}
	else if(strcmp(n.c_str(), "INVERSE") == 0) {
		return INVERSE;
	}
	else if(strcmp(n.c_str(), "INVERSE_TRANSPOSE") == 0) {
		return INVERSE_TRANSPOSE;
	}

	return NOTRANSFORM;
}

} // gfx::
} // al::

