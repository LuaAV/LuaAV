#ifndef AL_OPENCL_IMAGE3D_H
#define AL_OPENCL_IMAGE3D_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLContext.hpp"
#include "al_OpenCLMemoryBuffer.hpp"
#include <vector>
#include <string>

using std::vector;
using std::list;
using std::string;

namespace al {
namespace cl {

class OpenCLImage3D : public OpenCLMemoryBuffer {
public:
	OpenCLImage3D(cl_mem mem=0)
	:	OpenCLMemoryBuffer(mem)
	{}
	
	virtual ~OpenCLImage3D() {}
	
	void create(
		OpenCLContext &ctx, 
		cl_mem_flags usage, 
		const cl_image_format *format, 
		size_t width, 
		size_t height, 
		size_t depth, 
		size_t rowstride, 
		size_t planestride, 
		void *ptr
	);
	
	virtual void create(
		OpenCLContext &ctx, 
		cl_mem_flags usage, 
		AlloArray *array
	);
	
	OpenCLEvent enqueue_read(
		OpenCLCommandQueue &queue, 
		bool block, 
		size_t offset, 
		size_t size, 
		void *ptr
	) { printf("NOT IMPLEMENTED for IMAGE2D\n"); return OpenCLEvent(0); }
	
	virtual OpenCLEvent enqueue_read(
		OpenCLCommandQueue &queue, 
		bool block, 
		size_t offset, 
		AlloArray *array
	);
	
	OpenCLEvent enqueue_read(
		OpenCLCommandQueue &queue, 
		bool block, 
		const size_t origin[3], 
		const size_t region[3], 
		size_t rowstride,
		size_t planestride,
		void *ptr
	);

protected:

};

}	// cl::
}	// al::


#endif // AL_OPENCL_IMAGE3D_H
