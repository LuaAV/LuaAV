#include "al_OpenCLKernel.hpp"

namespace al {
namespace cl {

OpenCLKernel * OpenCLKernel :: create(OpenCLProgram &prog, const char *name) {
	cl_int res = CL_SUCCESS;
	cl_kernel kernel = clCreateKernel(
		prog.get_program(),
		name,
		&res
	);
	
	if(opencl_error(res, "clCreateKernel error creating kernel")) {
		return NULL;
	}
	
	OpenCLKernel *ker = new OpenCLKernel(kernel);
	prog.attach_resource(ker);
	return ker;
}

void OpenCLKernel :: create(OpenCLProgram &prog, vector<OpenCLKernel *> &kers) {
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels = 0;
	cl_int res = clCreateKernelsInProgram(
		prog.get_program(),
		MAX_KERNELS,
		kernels,
		&num_kernels
	);

	if(opencl_error(res, "clCreateKernelsInProgram error creating kernels")) {
		return;
	}
	
	for(int i=0; i < num_kernels; i++) {
		OpenCLKernel *ker = new OpenCLKernel(kernels[i]);
		prog.attach_resource(ker);
		kers.push_back(ker);
	}
}

void OpenCLKernel :: set_argument(cl_uint idx, OpenCLMemoryBuffer &mem) {
	cl_mem memory = mem.get_memory();
	set_argument(idx, sizeof(cl_mem), (void *)(&memory));
}

void OpenCLKernel :: set_argument(cl_uint idx, size_t size, void *arg) {
	cl_int res = clSetKernelArg(
		mKernel,
		idx,
		size,
		arg
	);

	if(opencl_error(res, "clSetKernelArg error setting argument")) {
		return;
	}
}

void OpenCLKernel :: destroy() {
	if(mKernel) {
		cl_int res = clReleaseKernel(mKernel);
		mKernel = 0;
		
		opencl_error(res, "clReleaseKernel error releasing kernel");
	}
}

}	// cl::
}	// al::