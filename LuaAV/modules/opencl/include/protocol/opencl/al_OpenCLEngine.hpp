#ifndef AL_OPENCL_ENGINE_H
#define AL_OPENCL_ENGINE_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLPlatform.hpp"
#include "al_OpenCLDevice.hpp"
#include "al_OpenCLContext.hpp"
#include "al_OpenCLProgram.hpp"
#include "al_OpenCLMemoryBuffer.hpp"
#include "al_OpenCLKernel.hpp"
#include "al_OpenCLCommandQueue.hpp"
#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;
using std::string;

namespace al {
namespace cl {

class OpenCLEngine {
public:
	OpenCLEngine()
	{}

	~OpenCLEngine();
	
	const vector<OpenCLPlatform> & get_platforms();
	OpenCLPlatform * get_platform(cl_platform_id id);
	
	const OpenCLDevice * get_device(cl_device_id id);
	
	OpenCLDevice * default_device();
	OpenCLDevice * cpu_device();
	OpenCLDevice * gpu_device();
	
	void compile_source(const char *name, const char *source);
	OpenCLKernel * get_kernel(const char *name, const char *kernel_name=NULL);

	OpenCLEvent enqueue_kernel(
		const OpenCLDevice &dev, 
		const OpenCLKernel *ker, 
		cl_uint ndim, 
		size_t *global, 
		size_t *local
	);
	
	OpenCLEvent enqueue_read(
		const OpenCLDevice &dev, 
		OpenCLMemoryBuffer *mem, 
		bool block, 
		size_t offset, 
		size_t size, 
		void *ptr
	);
	
	OpenCLEvent enqueue_read(
		const OpenCLDevice &dev,
		OpenCLMemoryBuffer *mem, 
		bool block, 
		size_t offset, 
		AlloArray *array
	);
	
	OpenCLMemoryBuffer * create_memory_buffer(cl_mem_flags usage, size_t size, void *ptr);
	OpenCLMemoryBuffer * create_memory_buffer(cl_mem_flags usage, AlloArray *arrayArray);
	
	template <typename A1>
	OpenCLEvent execute(
		const OpenCLDevice &dev, 
		OpenCLKernel *ker, 
		cl_uint ndim,
		size_t *global,
		A1 arg1
	);
	
	template <typename A1, typename A2>
	OpenCLEvent execute(
		const OpenCLDevice &dev, 
		OpenCLKernel *ker, 
		cl_uint ndim,
		size_t *global,
		A1 arg1,
		A2 arg2
	);

protected:
	bool create_context();
	OpenCLCommandQueue * queue_for_device(const OpenCLDevice &dev);

protected:

	vector<OpenCLPlatform> mPlatforms;
	OpenCLContext mContext;
	map<string, OpenCLProgram *> mPrograms;
	map<cl_device_id, OpenCLCommandQueue *>	mDeviceQueues;
};

template <typename A1>
OpenCLEvent OpenCLEngine :: execute(
	const OpenCLDevice &dev, 
	OpenCLKernel *ker, 
	cl_uint ndim,
	size_t *global,
	A1 arg1
) {
	size_t local[] = {1, 1};
	ker->set_argument(0, arg1);
	return enqueue_kernel(dev, ker, ndim, global, local);
}

template <typename A1, typename A2>
OpenCLEvent OpenCLEngine :: execute(
	const OpenCLDevice &dev, 
	OpenCLKernel *ker, 
	cl_uint ndim,
	size_t *global,
	A1 arg1,
	A2 arg2
) {
	size_t local[] = {1, 1};
	ker->set_argument(0, arg1);
	ker->set_argument(1, arg2);
	return enqueue_kernel(dev, ker, ndim, global, local);
}


}	// cl::
}	// al::


#endif // AL_OPENCL_ENGINE_H
