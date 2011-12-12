#include "al_OpenCLCommandQueue.hpp"
#include "al_OpenCLMemoryBuffer.hpp"

namespace al {
namespace cl {

void OpenCLCommandQueue :: create(OpenCLContext &ctx, const OpenCLDevice &dev, bool ordered, bool profiling) {
	destroy();
	detach();
	
	cl_command_queue_properties properties = 0;
	if(profiling) {
		properties |= CL_QUEUE_PROFILING_ENABLE;
	}
	if(! ordered) {
		if(GET_FLAG(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, dev.get_queue_properties())) {
			properties |= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
		}
		else {
			opencl_error(USER_OPENCL_ERROR, "Device doesn't support out of order execution ... disabling");
		}
	}
	
	cl_int res = CL_SUCCESS;
	cl_command_queue command_queue = clCreateCommandQueue(
		ctx.get_context(),
		dev.get_device(),
		properties,
		&res
	);
	
	if(opencl_error(res, "clCreateCommandQueue error creating command queue")) {
		return;
	}
	
	mCommandQueue = command_queue;
	ctx.attach_resource(this);
}

OpenCLEvent OpenCLCommandQueue :: enqueue_kernel(const OpenCLKernel &ker, cl_uint ndim, size_t *global, size_t *local) {
	cl_event event = 0;
	cl_int res = clEnqueueNDRangeKernel(
		mCommandQueue,
		ker.get_kernel(),
		ndim,
		NULL,
		global,
		local,
		0,
		NULL,
		&event
	);
	
	if(opencl_error(res, "clEnqueueNDRangeKernel error enqueuing kernel event")) {
		return OpenCLEvent();
	}
	return OpenCLEvent(event);
}

OpenCLEvent OpenCLCommandQueue :: enqueue_read(OpenCLMemoryBuffer &mem, bool block, size_t offset, size_t size, void *ptr) {
	return mem.enqueue_read(*this, block, offset, size, ptr);
}

OpenCLEvent OpenCLCommandQueue :: enqueue_read(OpenCLMemoryBuffer &mem, bool block, size_t offset, AlloArray *array) {
	return mem.enqueue_read(*this, block, offset, array);
}

void OpenCLCommandQueue :: destroy() {
	if(mCommandQueue) {
		cl_int res = clReleaseCommandQueue(mCommandQueue);
		mCommandQueue = 0;
		
		opencl_error(res, "clReleaseCommandQueue error releasing command queue");
	}
}

}	// cl::
}	// al::