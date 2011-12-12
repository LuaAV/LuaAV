#include "al_OpenCLImage2D.hpp"
#include "al_OpenCLCommandQueue.hpp"
#include "al_OpenCLImageFormat.hpp"

namespace al {
namespace cl {

void OpenCLImage2D :: create(
	OpenCLContext &ctx, 
	cl_mem_flags usage, 
	const cl_image_format *format, 
	size_t width, 
	size_t height, 
	size_t rowstride, 
	void *ptr
) {
	destroy();
	detach();

	usage = OpenCLMemoryBuffer::check_memory_flags(usage, ptr);
	
	cl_int res = CL_SUCCESS;
	cl_mem mem = clCreateImage2D(
		ctx.get_context(),
		usage,
		format,
		width,
		height,
		rowstride,
		ptr,
		&res
	);
	
	if(opencl_error(res, "clCreateImage2D error creating buffer")) {
		return;
	}
	
	mMem = mem;
	ctx.attach_resource(this);
}

void OpenCLImage2D :: create(
	OpenCLContext &ctx, 
	cl_mem_flags usage, 
	AlloArray *array
) {
	destroy();
	detach();

	usage = OpenCLMemoryBuffer::check_memory_flags(usage, array->data.ptr);
	
	bool at_least_2d = array->header.dimcount >= 2;
	size_t width = array->header.dim[0];
	size_t height = at_least_2d ? array->header.dim[1] : 1;
	size_t rowstride = at_least_2d ? array->header.stride[1] : allo_array_size(array);
	cl_image_format format = OpenCLImageFormat::format_from_array(array);
	
	cl_int res = CL_SUCCESS;
	cl_mem mem = clCreateImage2D(
		ctx.get_context(),
		usage,
		&format,
		width,
		height,
		rowstride,
		array->data.ptr,
		&res
	);
	
	if(opencl_error(res, "clCreateImage2D error creating buffer")) {
		return;
	}
	
	mMem = mem;
	ctx.attach_resource(this);
}

OpenCLEvent OpenCLImage2D :: enqueue_read(
	OpenCLCommandQueue &queue, 
	bool block, 
	size_t offset, 
	AlloArray *array
) {
	bool at_least_2d = array->header.dimcount >= 2;
	size_t rowstride = at_least_2d ? array->header.stride[1] : allo_array_size(array);
	
	size_t width = array->header.dim[0];
	size_t height = at_least_2d ? array->header.dim[1] : 1;
	
	size_t ycells = offset/rowstride;
	size_t xcells = (offset - (ycells*rowstride))/(array->header.stride[0]);

	size_t origin[] = {
		xcells, 
		ycells, 
		0
	};
	size_t region[] = {
		width,
		height,
		0
	};
	
	return enqueue_read(queue, block, origin, region, rowstride, array->data.ptr);
}

OpenCLEvent OpenCLImage2D :: enqueue_read(
	OpenCLCommandQueue &queue, 
	bool block, 
	const size_t origin[3], 
	const size_t region[3], 
	size_t rowstride,
	void *ptr
) {
	cl_event event = 0;
	cl_int res = clEnqueueReadImage(
		queue.get_command_queue(),
		mMem,
		block ? CL_TRUE : CL_FALSE,
		origin,
		region,
		rowstride,
		0,
		ptr,
		0,
		NULL,
		&event
	);
	
	if(opencl_error(res, "clEnqueueReadImage error enqueuing read event")) {
		return OpenCLEvent();
	}
	
	return OpenCLEvent(event);
}

}	// cl::
}	// al::
