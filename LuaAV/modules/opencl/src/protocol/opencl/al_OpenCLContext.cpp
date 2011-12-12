#include "al_OpenCLContext.hpp"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

namespace al {
namespace cl {

void OpenCLContext :: create(const OpenCLDevice& dev) {
	create(vector<OpenCLDevice>(1, dev));
}

void create_context_cb(const char *errinfo, const void *private_info, size_t cb, void *user_data) {
	opencl_error(USER_OPENCL_ERROR, errinfo);
	//clLogMessagesToStderrAPPLE
}

void OpenCLContext :: create(const vector<OpenCLDevice> &devs) {
	destroy();

	int ndevices = devs.size();
	if(ndevices <= 0) {
		opencl_error(USER_OPENCL_ERROR, "No OpenCL devices to create context with");
		return;
	}
	
	cl_device_id devices[MAX_DEVICES];
	cl_int res = CL_SUCCESS;
	for(int i=0; i < MIN(MAX_DEVICES, ndevices); i++) {
		devices[i] = devs[i].get_device();
		
		if(!devs[i].get_available()) {
			char msg[256];
			sprintf(msg, "OpenCL Device %s is not available", devs[i].get_name().c_str());
			opencl_error(USER_OPENCL_ERROR, msg);
			return;
		}
	}
	
	cl_context context = clCreateContext(
		0,	// platform to use (only one for now)
		ndevices,
		devices,
		create_context_cb,
		(void *)this,
		&res
	);
	
	if(opencl_error(res, "clCreateContext error creating context")) {
		return;
	}
	
	mContext = context;
	mDevices = devs;
}

void OpenCLContext :: destroy() {
	if(mContext) {
		list<OpenCLResource<OpenCLContext> *>::iterator it = mResources.begin();
		while(it != mResources.end()) {
			(*it)->destroy();
			(*it)->detach();
			it = mResources.begin();
		}
	
		cl_int res = clReleaseContext(mContext);
		mContext = 0;
		mDevices.clear();
		
		opencl_error(res, "clReleaseContext error releasing context");
	}
}

void OpenCLContext :: attach_resource(OpenCLResource<OpenCLContext> *resource) {
	resource->attach(this);
	mResources.push_back(resource);
}

void OpenCLContext :: detach_resource(OpenCLResource<OpenCLContext> *resource) {
	list<OpenCLResource<OpenCLContext> *>::iterator it = mResources.begin();
	list<OpenCLResource<OpenCLContext> *>::iterator ite = mResources.end();
	for(; it != ite; ++it) {
		if((*it) == resource) {
			mResources.erase(it);
			break;
		}
	}
}

}	// cl::
}	// al::