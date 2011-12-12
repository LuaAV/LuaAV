#ifndef AL_OPENCL_KERNEL_H
#define AL_OPENCL_KERNEL_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLProgram.hpp"
#include "al_OpenCLMemoryBuffer.hpp"
#include <vector>
#include <list>
#include <string>

using std::vector;
using std::string;

namespace al {
namespace cl {

class OpenCLKernel : public OpenCLResource<OpenCLProgram> {
protected:
	OpenCLKernel(cl_kernel kernel=0)
	:	mKernel(kernel)
	{}

public:	
	virtual ~OpenCLKernel() {
		destroy();
	}
	
	static OpenCLKernel * create(OpenCLProgram &prog, const char *name);
	static void create(OpenCLProgram &prog, vector<OpenCLKernel *> &kers);
	
	cl_kernel get_kernel() const {return mKernel;}
	
	template<typename T>
	void set_argument(int idx, T *arg) {
		set_argument(idx, sizeof(T), (void *)arg);
	}
	void set_argument(cl_uint idx, OpenCLMemoryBuffer &mem);
	void set_argument(cl_uint idx, size_t size, void *arg);
	void destroy();

protected:
	cl_kernel mKernel;
};

}	// cl::
}	// al::


#endif // AL_OPENCL_KERNEL_H
