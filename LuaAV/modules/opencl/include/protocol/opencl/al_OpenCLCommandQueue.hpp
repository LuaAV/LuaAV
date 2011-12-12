#ifndef AL_OPENCL_COMMAND_QUEUE_H
#define AL_OPENCL_COMMAND_QUEUE_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLContext.hpp"
#include "al_OpenCLKernel.hpp"
#include "al_OpenCLEvent.hpp"
#include "allocore/types/al_Array.hpp"
#include <vector>
#include <string>


using std::vector;
using std::string;

namespace al {
namespace cl {

class OpenCLMemoryBuffer;

class OpenCLCommandQueue : public OpenCLResource<OpenCLContext> {
public:	
	OpenCLCommandQueue(cl_command_queue command_queue=0)
	:	mCommandQueue(command_queue)
	{}


	virtual ~OpenCLCommandQueue() {}
	
	cl_command_queue get_command_queue() const {return mCommandQueue;}
	void create(OpenCLContext &ctx, const OpenCLDevice &dev, bool ordered=true, bool profiling=false);
	OpenCLEvent enqueue_kernel(const OpenCLKernel &ker, cl_uint ndim, size_t *global, size_t *local);
	OpenCLEvent enqueue_read(OpenCLMemoryBuffer &mem, bool block, size_t offset, size_t size, void *ptr);
	OpenCLEvent enqueue_read(OpenCLMemoryBuffer &mem, bool block, size_t offset, AlloArray *array);
	void destroy();

protected:
	cl_command_queue mCommandQueue;
};

}	// cl::
}	// al::


#endif // AL_OPENCL_COMMAND_QUEUE_H
