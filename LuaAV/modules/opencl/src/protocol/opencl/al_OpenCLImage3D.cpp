#include "al_OpenCLImage3D.hpp"
#include "al_OpenCLCommandQueue.hpp"
#include "al_OpenCLImageFormat.hpp"

namespace al {
namespace cl {

void OpenCLImage3D :: create(
	OpenCLContext &ctx, 
	cl_mem_flags usage, 
	const cl_image_format *format, 
	size_t width, 
	size_t height, 
	size_t depth, 
	size_t rowstride, 
	size_t planestride, 
	void *ptr
) {
	destroy();
	detach();

	usage = OpenCLMemoryBuffer::check_memory_flags(usage, ptr);
	
	cl_int res = CL_SUCCESS;
	cl_mem mem = clCreateImage3D(
		ctx.get_context(),
		usage,
		format,
		width,
		height,
		depth,
		rowstride,
		planestride,
		ptr,
		&res
	);
	
	if(opencl_error(res, "clCreateImage3D error creating buffer")) {
		return;
	}
	
	mMem = mem;
	ctx.attach_resource(this);
}

void OpenCLImage3D :: create(
	OpenCLContext &ctx, 
	cl_mem_flags usage, 
	AlloArray *array
) {
	destroy();
	detach();

	usage = OpenCLMemoryBuffer::check_memory_flags(usage, array->data.ptr);
	
	bool at_least_2d = array->header.dimcount >= 2;
	bool at_least_3d = array->header.dimcount >= 3;
	size_t width = array->header.dim[0];
	size_t height = at_least_2d ? array->header.dim[1] : 1;
	size_t rowstride = at_least_2d ? array->header.stride[1] : allo_array_size(array);
	size_t depth = at_least_3d ? array->header.dim[2] : 1;
	size_t planestride = at_least_3d ? array->header.stride[2] : allo_array_size(array);
	
	cl_image_format format = OpenCLImageFormat::format_from_array(array);
	
	cl_int res = CL_SUCCESS;
	cl_mem mem = clCreateImage3D(
		ctx.get_context(),
		usage,
		&format,
		width,
		height,
		depth,
		rowstride,
		planestride,
		array->data.ptr,
		&res
	);
	
	if(opencl_error(res, "clCreateImage3D error creating buffer")) {
		return;
	}
	
	mMem = mem;
	ctx.attach_resource(this);
}

OpenCLEvent OpenCLImage3D :: enqueue_read(
	OpenCLCommandQueue &queue, 
	bool block, 
	size_t offset, 
	AlloArray *array
) {
	bool at_least_2d = array->header.dimcount >= 2;
	bool at_least_3d = array->header.dimcount >= 3;
	size_t rowstride = at_least_2d ? array->header.stride[1] : allo_array_size(array);
	size_t planestride = at_least_3d ? array->header.stride[2] : allo_array_size(array);
	
	size_t width = array->header.dim[0];
	size_t height = at_least_2d ? array->header.dim[1] : 1;
	size_t depth = at_least_3d ? array->header.dim[2] : 1;
	
	size_t zcells = offset/planestride;
	size_t ycells = (offset - (planestride*zcells))/rowstride;
	size_t xcells = (offset - (planestride*zcells) - (ycells*rowstride))/(array->header.stride[0]);
	
	size_t origin[] = {
		xcells, 
		ycells, 
		zcells
	};
	size_t region[] = {
		width,
		height,
		depth
	};
	
	return enqueue_read(queue, block, origin, region,  rowstride, planestride, array->data.ptr);
}

OpenCLEvent OpenCLImage3D :: enqueue_read(
	OpenCLCommandQueue &queue, 
	bool block, 
	const size_t origin[3], 
	const size_t region[3], 
	size_t rowstride,
	size_t planestride,
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
