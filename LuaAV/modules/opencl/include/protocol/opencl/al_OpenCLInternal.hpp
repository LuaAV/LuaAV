#ifndef AL_OPENCL_INTERNAL_H
#define AL_OPENCL_INTERNAL_H 1

#include <OpenCL/OpenCL.h>
#include <OpenCL/cl_gl.h>
#include <string>

#define GET_FLAG(flag, word) (flag & word)
#define SET_FLAG(flag, word) (flag | word)

#define PARAM_CAST(type, var) (*((type *)var))

#define MAX_PLATFORMS 16
#define MAX_DEVICES 16
#define MAX_KERNELS 16
#define PLATFORM_PARAM_SIZE 2048
#define USER_OPENCL_ERROR (CL_SUCCESS+1)

namespace al {
namespace cl {

// Profiles
enum Profile {
	UNKNOWN_PROFILE  = (-1),
	FULL_PROFILE  = (0),
	EMBEDDED_PROFILE  = (1)
};


extern bool opencl_error(cl_int err, const char *msg);

template <typename T>
T opencl_param(char *param_value) {
	return PARAM_CAST(T, param_value);
}


template <> std::string opencl_param(char *param_value);
template<> Profile opencl_param(char *param_value);
template<> size_t * opencl_param(char *param_value);


template<class T>
class OpenCLResource {
public:
	OpenCLResource() 
	:	mOwner(0)
	{}
	virtual ~OpenCLResource() {
		destroy();
		detach();
	}
	
	T * get_owner() {return mOwner;}
	void attach(T *owner) {mOwner = owner;}
	void detach() {
		if(mOwner) {
			mOwner->detach_resource(this);
			mOwner = 0;
		}
	}
	virtual void destroy() {};
	
protected:
	T *mOwner;
};

}	// cl::
}	// al::

#endif	// AL_OPENCL_INTERNAL_H
