#ifndef AL_OPENCL_MEMORY_BUFFER_H
#define AL_OPENCL_MEMORY_BUFFER_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLContext.hpp"
#include "al_OpenCLEvent.hpp"
#include "allocore/types/al_Array.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace al {
namespace cl {

class OpenCLCommandQueue;

class OpenCLMemoryBuffer : public OpenCLResource<OpenCLContext> {
public:
	OpenCLMemoryBuffer(cl_mem mem=0)
	:	mMem(mem)
	{}
	
	virtual ~OpenCLMemoryBuffer() {}
	
	cl_mem get_memory() const {return mMem;}
	void create(OpenCLContext &ctx, cl_mem_flags usage, size_t size, void *ptr);
	virtual void create(
		OpenCLContext &ctx, 
		cl_mem_flags usage, 
		AlloArray *array
	);
	void destroy();
	
	virtual OpenCLEvent enqueue_read(
		OpenCLCommandQueue &queue, 
		bool block, 
		size_t offset, 
		size_t size, 
		void *ptr
	);
	
	virtual OpenCLEvent enqueue_read(
		OpenCLCommandQueue &queue, 
		bool block, 
		size_t offset, 
		AlloArray *array
	);
	
	static cl_mem_flags check_memory_flags(cl_mem_flags usage, void *ptr);

protected:
	cl_mem mMem;
};

}	// cl::
}	// al::


#endif // AL_OPENCL_MEMORY_BUFFER_H
