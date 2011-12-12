#include "al_OpenCLPlatform.hpp"

namespace al {
namespace cl {


void OpenCLPlatform :: get_platforms(vector<OpenCLPlatform> &plats) {
	cl_platform_id platforms[MAX_PLATFORMS];
	cl_uint num_platforms = 0;
	cl_int res = clGetPlatformIDs(
		MAX_PLATFORMS,
		platforms,
		&num_platforms
	);
	
	if(opencl_error(res, "clGetPlatformIDs error querying platforms")) {
		return;
	}
	
	
	if(num_platforms <= 0) {
		opencl_error(USER_OPENCL_ERROR, "clGetPlatformIDs no OpenCL platforms detected");
		return;
	}
	
	
	// clear out any previous platforms
	plats.clear();
	
	static cl_platform_info param_names[] = {
		CL_PLATFORM_VENDOR,
		CL_PLATFORM_NAME,
		CL_PLATFORM_VERSION,
		CL_PLATFORM_PROFILE,
		CL_PLATFORM_EXTENSIONS
	};
	
	int n_param_names = sizeof(param_names)/sizeof(cl_platform_info);
	for(int j=0; j < num_platforms; j++) {
		OpenCLPlatform plat(platforms[j]);
		
		for(int i=0; i < n_param_names; i++) {
			char param_value[PLATFORM_PARAM_SIZE];
			size_t param_value_size_ret = 0;
			cl_int res = clGetPlatformInfo(
				platforms[j],
				param_names[i],
				PLATFORM_PARAM_SIZE,
				param_value,
				&param_value_size_ret
			);
			
			if(opencl_error(res, "clGetPlatformInfo getting platform information")) {
				return;
			}
		
			switch(param_names[i]) {
				case CL_PLATFORM_VENDOR:
					plat.set_vendor(opencl_param<string>(param_value));
					break;
					
				case CL_PLATFORM_NAME:
					plat.set_name(opencl_param<string>(param_value));
					break;
					
				case CL_PLATFORM_VERSION:
					plat.set_version(opencl_param<string>(param_value));
					break;
					
				case CL_PLATFORM_PROFILE:
					plat.set_profile(opencl_param<Profile>(param_value));
					break;
				
				case CL_PLATFORM_EXTENSIONS:
					plat.set_extension(opencl_param<string>(param_value));
					break;
					
				default:
					break;
			}
		}
		
		plats.push_back(plat);
	}
}

const vector<OpenCLDevice> & OpenCLPlatform :: get_devices() {
	if(mDevices.size() <= 0) {
		OpenCLDevice::get_devices(*this, mDevices);
	}
	return mDevices;
}

OpenCLDevice * OpenCLPlatform :: default_device() {
	vector<OpenCLDevice>::iterator it = mDevices.begin();
	vector<OpenCLDevice>::iterator ite = mDevices.end();
	for(; it != ite; ++it) {
		OpenCLDevice &device = *it;
		if(GET_FLAG(CL_DEVICE_TYPE_DEFAULT, device.get_device_type())) {
			return &device;
		}
	}
	return NULL;
}

OpenCLDevice * OpenCLPlatform :: cpu_device() {
	vector<OpenCLDevice>::iterator it = mDevices.begin();
	vector<OpenCLDevice>::iterator ite = mDevices.end();
	for(; it != ite; ++it) {
		OpenCLDevice &device = *it;
		if(GET_FLAG(CL_DEVICE_TYPE_CPU, device.get_device_type())) {
			return &device;
		}
	}
	return NULL;
}

OpenCLDevice * OpenCLPlatform :: gpu_device() {
	vector<OpenCLDevice>::iterator it = mDevices.begin();
	vector<OpenCLDevice>::iterator ite = mDevices.end();
	for(; it != ite; ++it) {
		OpenCLDevice &device = *it;
		if(GET_FLAG(CL_DEVICE_TYPE_GPU, device.get_device_type())) {
			return &device;
		}
	}
	return NULL;
}

}	// cl::
}	// al::
