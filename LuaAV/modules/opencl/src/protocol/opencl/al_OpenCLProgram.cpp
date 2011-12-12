#include "al_OpenCLProgram.hpp"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

namespace al {
namespace cl {


void OpenCLProgram :: create(OpenCLContext &ctx, const char *source) {
	destroy();
	detach();

	cl_int res = CL_SUCCESS;
	cl_program program = clCreateProgramWithSource(
		ctx.get_context(),
		1,
		&source,
		NULL,
		&res
	);
	
	if(opencl_error(res, "clCreateProgramWithSource error creating program")) {
		return;
	}
	
	mProgram = program;
	ctx.attach_resource(this);
}

void OpenCLProgram :: build(const OpenCLDevice &dev) {
	build(vector<OpenCLDevice>(1, dev));
}

void OpenCLProgram :: build(const vector<OpenCLDevice> &devs) {
	int ndevices = devs.size();
	cl_device_id devices[MAX_DEVICES];
	for(int i=0; i < MIN(MAX_DEVICES, ndevices); i++) {
		devices[i] = devs[i].get_device();
		
		if(!devs[i].get_available()) {
			char msg[256];
			sprintf(msg, "OpenCL Device %s is not available", devs[i].get_name().c_str());
			opencl_error(USER_OPENCL_ERROR, msg);
			return;
		}
	}
	
	cl_int res = clBuildProgram(
		mProgram,
		ndevices,
		devices,
		"",
		NULL,
		NULL
	);
	
	if(opencl_error(res, "clBuildProgram error building program")) {
		/*  INSTEAD OF CALLBACKS:
		clGetProgramBuildInfo(
      program,              // the program object being queried
      device_id,            // the device for which the OpenCL code was built
      CL_PROGRAM_BUILD_LOG, // specifies that we want the build log
      sizeof(buffer),       // the size of the buffer
      buffer,               // on return, holds the build log
      &len);                // on return, the actual size in bytes of the
                            //   data returned
		*/
		return;
	}
	
	return;
}

void OpenCLProgram :: destroy() {
	if(mProgram) {
		list<OpenCLResource<OpenCLProgram> *>::iterator it = mResources.begin();
		while(it != mResources.end()) {
			(*it)->destroy();
			(*it)->detach();
			it = mResources.begin();
		}
	
		cl_int res = clReleaseProgram(mProgram);
		mProgram = 0;
		
		opencl_error(res, "clReleaseProgram error releasing program");
	}
}

void OpenCLProgram :: attach_resource(OpenCLResource<OpenCLProgram> *resource) {
	resource->attach(this);
	mResources.push_back(resource);
}

void OpenCLProgram :: detach_resource(OpenCLResource<OpenCLProgram> *resource) {
	list<OpenCLResource<OpenCLProgram> *>::iterator it = mResources.begin();
	list<OpenCLResource<OpenCLProgram> *>::iterator ite = mResources.end();
	for(; it != ite; ++it) {
		if((*it) == resource) {
			mResources.erase(it);
			break;
		}
	}
}
	
	
}	// cl::
}	// al::
