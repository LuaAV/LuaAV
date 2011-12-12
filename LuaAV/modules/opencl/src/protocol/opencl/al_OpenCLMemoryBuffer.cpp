#include "al_OpenCLMemoryBuffer.hpp"
#include "al_OpenCLCommandQueue.hpp"


namespace al {
namespace cl {

void OpenCLMemoryBuffer :: create(OpenCLContext &ctx, cl_mem_flags usage, size_t size, void *ptr) {
	destroy();
	detach();
	
	usage = check_memory_flags(usage, ptr);
	
	cl_int res = CL_SUCCESS;
	cl_mem mem = clCreateBuffer(
		ctx.get_context(),
		usage,
		size,
		ptr,
		&res
	);
	
	if(opencl_error(res, "clCreateBuffer error creating buffer")) {
		return;
	}
	
	mMem = mem;
	ctx.attach_resource(this);
}

void OpenCLMemoryBuffer :: create(OpenCLContext &ctx, cl_mem_flags usage, AlloArray *array) {
	destroy();
	detach();
	
	usage = check_memory_flags(usage, array->data.ptr);
	
	cl_int res = CL_SUCCESS;
	cl_mem mem = clCreateBuffer(
		ctx.get_context(),
		usage,
		allo_array_size(array),
		array->data.ptr,
		&res
	);
	
	if(opencl_error(res, "clCreateBuffer error creating buffer")) {
		return;
	}
	
	mMem = mem;
	ctx.attach_resource(this);
}

void OpenCLMemoryBuffer :: destroy() {
	if(mMem) {
		cl_int res = clReleaseMemObject(mMem);
		mMem = 0;
		
		opencl_error(res, "clReleaseMemObject error releasing memory");
	}
}

OpenCLEvent OpenCLMemoryBuffer :: enqueue_read(
	OpenCLCommandQueue &queue, 
	bool block, 
	size_t offset, 
	size_t size, 
	void *ptr
) {
	cl_event event = 0;
	cl_int res = clEnqueueReadBuffer(
		queue.get_command_queue(),
		mMem,
		block ? CL_TRUE : CL_FALSE,
		offset,
		size,
		ptr,
		0,
		NULL,
		&event
	);
	
	if(opencl_error(res, "clEnqueueReadBuffer error enqueuing read event")) {
		return OpenCLEvent();
	}
	
	return OpenCLEvent(event);
}

OpenCLEvent OpenCLMemoryBuffer :: enqueue_read(
	OpenCLCommandQueue &queue, 
	bool block, 
	size_t offset, 
	AlloArray *array
) {
	cl_event event = 0;
	cl_int res = clEnqueueReadBuffer(
		queue.get_command_queue(),
		mMem,
		block ? CL_TRUE : CL_FALSE,
		offset,
		allo_array_size(array),
		array->data.ptr,
		0,
		NULL,
		&event
	);
	
	if(opencl_error(res, "clEnqueueReadBuffer error enqueuing read event")) {
		return OpenCLEvent();
	}
	
	return OpenCLEvent(event);
}


// CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR initializes gost memory
// CL_MEM_COPY_HOST_PTR | CL_MEM_USE_HOST_PTR are mutually exclusive
// CL_MEM_ALLOC_HOST_PTR | CL_MEM_USE_HOST_PTR are mutually exclusive
cl_mem_flags OpenCLMemoryBuffer :: check_memory_flags(cl_mem_flags usage, void *ptr) {
	if(GET_FLAG(CL_MEM_COPY_HOST_PTR, usage) && GET_FLAG(CL_MEM_USE_HOST_PTR, usage)) {
		opencl_error(USER_OPENCL_ERROR, "OpenCLMemoryBuffer::create CL_MEM_COPY_HOST_PTR | CL_MEM_USE_HOST_PTR are mutually exclusive");
	}
	if(GET_FLAG(CL_MEM_ALLOC_HOST_PTR, usage) && GET_FLAG(CL_MEM_USE_HOST_PTR, usage)) {
		opencl_error(USER_OPENCL_ERROR, "OpenCLMemoryBuffer::create CL_MEM_ALLOC_HOST_PTR | CL_MEM_USE_HOST_PTR are mutually exclusive");
	}
	
	usage |= (ptr ? CL_MEM_USE_HOST_PTR : 0);
	
	return usage;
}

}	// cl::
}	// al::
