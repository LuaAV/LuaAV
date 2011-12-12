#include "al_OpenCLDevice.hpp"
#include "al_OpenCLPlatform.hpp"

namespace al {
namespace cl {

bool OpenCLDevice :: has_extension(const char *ext) {
	return strstr(mExtensions.c_str(), ext) != NULL;
}

void OpenCLDevice :: get_devices(const OpenCLPlatform &plat, vector<OpenCLDevice> &devs) {
	cl_device_id devices[MAX_DEVICES];
	cl_uint num_devices = 0;
	cl_int res = clGetDeviceIDs(
		plat.get_platform(),
		CL_DEVICE_TYPE_ALL,
		MAX_DEVICES,
		devices,
		&num_devices
	);
	
	if(opencl_error(res, "clGetDeviceIDs error querying platform devices")) {
		return;
	}
	
	static cl_device_info param_names[] = {
		CL_DEVICE_PLATFORM,
		CL_DEVICE_NAME,
		CL_DEVICE_VENDOR,
		CL_DEVICE_VENDOR_ID,
		CL_DEVICE_VERSION,
		CL_DRIVER_VERSION,
		CL_DEVICE_EXTENSIONS,
		CL_DEVICE_TYPE,
		CL_DEVICE_PROFILE,
		CL_DEVICE_AVAILABLE,

		//	capabilities
		CL_DEVICE_EXECUTION_CAPABILITIES,
		CL_DEVICE_QUEUE_PROPERTIES,
		
		// key properties
		CL_DEVICE_COMPILER_AVAILABLE,
		CL_DEVICE_ENDIAN_LITTLE,
		
		// misc. properties
		CL_DEVICE_ERROR_CORRECTION_SUPPORT,
		
		// compute environment constants
		CL_DEVICE_MAX_COMPUTE_UNITS,
		CL_DEVICE_MAX_CONSTANT_ARGS,
		CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
		CL_DEVICE_MAX_MEM_ALLOC_SIZE,
		CL_DEVICE_MAX_PARAMETER_SIZE,
		CL_DEVICE_MAX_READ_IMAGE_ARGS,
		CL_DEVICE_MAX_SAMPLERS,
		CL_DEVICE_MAX_WORK_GROUP_SIZE,
		CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
		CL_DEVICE_MAX_WORK_ITEM_SIZES,
		CL_DEVICE_MAX_WRITE_IMAGE_ARGS,
		
		// numerics
		CL_DEVICE_SINGLE_FP_CONFIG,
		CL_DEVICE_DOUBLE_FP_CONFIG,
		CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,
		CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
		CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
		CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,
		CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
		CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,
		
		// alignment
		CL_DEVICE_MEM_BASE_ADDR_ALIGN,
		CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,
		
		// memory
		CL_DEVICE_ADDRESS_BITS,
		CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
		CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
		CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
		CL_DEVICE_GLOBAL_MEM_SIZE,
		CL_DEVICE_LOCAL_MEM_SIZE,
		CL_DEVICE_LOCAL_MEM_TYPE,
		
		// images
		CL_DEVICE_IMAGE_SUPPORT,
		CL_DEVICE_IMAGE2D_MAX_HEIGHT,
		CL_DEVICE_IMAGE2D_MAX_WIDTH,
		CL_DEVICE_IMAGE3D_MAX_DEPTH,
		CL_DEVICE_IMAGE3D_MAX_HEIGHT,
		CL_DEVICE_IMAGE3D_MAX_WIDTH,
		
		// timing
		CL_DEVICE_PROFILING_TIMER_RESOLUTION,
		CL_DEVICE_MAX_CLOCK_FREQUENCY
	};
	
	
	// clear out any previous devices
	devs.clear();
	
	int n_param_names = sizeof(param_names)/sizeof(cl_device_info);
	for(int j=0; j < num_devices; j++) {
		OpenCLDevice device(devices[j]);
		for(int i=0; i < n_param_names; i++) {
		
			char param_value[PLATFORM_PARAM_SIZE];
			size_t param_value_size_ret = 0;
			cl_int res = clGetDeviceInfo(
				devices[j],
				param_names[i],
				PLATFORM_PARAM_SIZE,
				param_value,
				&param_value_size_ret
			);
			
			if(opencl_error(res, "clGetDeviceInfo error")) {
				return;
			}

			switch(param_names[i]) {
				case CL_DEVICE_PLATFORM:
					device.set_platform(opencl_param<string>(param_value));
					break;
					
				case CL_DEVICE_NAME:
					device.set_name(opencl_param<string>(param_value));
					break;
					
				case CL_DEVICE_VENDOR:
					device.set_vendor(opencl_param<string>(param_value));
					break;
					
				case CL_DEVICE_VENDOR_ID:
					device.set_vendor_id(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_VERSION:
					device.set_version(opencl_param<string>(param_value));
					break;
					
				case CL_DRIVER_VERSION:
					device.set_driver_version(opencl_param<string>(param_value));
					break;
					
				case CL_DEVICE_EXTENSIONS:
					device.set_extensions(opencl_param<string>(param_value));
					break;
					
				case CL_DEVICE_TYPE:
					device.set_device_type(opencl_param<cl_device_type>(param_value));
					break;
					
				case CL_DEVICE_PROFILE:
					device.set_profile(opencl_param<Profile>(param_value));
					break;
				
				case CL_DEVICE_AVAILABLE:
					device.set_available(opencl_param<cl_bool>(param_value));
					break;
					
				//	capabilities
				case CL_DEVICE_EXECUTION_CAPABILITIES:
					device.set_exec_capabilities(opencl_param<cl_device_exec_capabilities>(param_value));
					break;
					
				case CL_DEVICE_QUEUE_PROPERTIES:
					device.set_queue_properties(opencl_param<cl_command_queue_properties>(param_value));
					break;
				
				// key properties
				case CL_DEVICE_COMPILER_AVAILABLE:
					device.set_compiler_available(opencl_param<cl_bool>(param_value));
					break;
					
				case CL_DEVICE_ENDIAN_LITTLE:
					device.set_endian_little(opencl_param<cl_bool>(param_value));
					break;
				
				// misc. properties
				case CL_DEVICE_ERROR_CORRECTION_SUPPORT:
					device.set_error_correction_support(opencl_param<cl_bool>(param_value));
					break;
				
				// compute environment constants
				case CL_DEVICE_MAX_COMPUTE_UNITS:
					device.set_max_compute_units(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_MAX_CONSTANT_ARGS:
					device.set_max_constant_args(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
					device.set_max_constant_buffer_size(opencl_param<cl_ulong>(param_value));
					break;
					
				case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
					device.set_max_mem_alloc_size(opencl_param<cl_ulong>(param_value));
					break;
					
				case CL_DEVICE_MAX_PARAMETER_SIZE:
					device.set_max_parameter_size(opencl_param<size_t>(param_value));
					break;
					
				case CL_DEVICE_MAX_READ_IMAGE_ARGS:
					device.set_max_read_image_args(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_MAX_SAMPLERS:
					device.set_max_samplers(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_MAX_WORK_GROUP_SIZE:
					device.set_max_work_group_size(opencl_param<size_t>(param_value));
					break;
					
				case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
					device.set_max_work_item_dimensions(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_MAX_WORK_ITEM_SIZES:
					device.set_max_work_item_sizes(opencl_param<size_t *>(param_value));
					break;
					
				case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:
					device.set_max_write_image_args(opencl_param<cl_uint>(param_value));
					break;
				
				// numerics
				/*case CL_DEVICE_SINGLE_FP_CONFIG:
				case CL_DEVICE_DOUBLE_FP_CONFIG:
				case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
				case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
				case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
				case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
				case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
				case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
				*/
				// alignment
				case CL_DEVICE_MEM_BASE_ADDR_ALIGN:
					//device.max_write_image_args = PARAM_CAST(int, cl_uint, param_value);
					break;
					
				case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
					break;
				
				// memory
				case CL_DEVICE_ADDRESS_BITS:
					device.set_device_address_bits(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
					device.set_global_mem_cache_size(opencl_param<cl_ulong>(param_value));
					break;
					
				case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
					device.set_global_mem_cache_type(opencl_param<cl_device_mem_cache_type>(param_value));
					break;
					
				case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
					device.set_global_mem_cacheline_size(opencl_param<cl_uint>(param_value));
					break;
					
				case CL_DEVICE_GLOBAL_MEM_SIZE:
					device.set_global_mem_size(opencl_param<cl_ulong>(param_value));
					break;
					
				case CL_DEVICE_LOCAL_MEM_SIZE:
					device.set_local_mem_size(opencl_param<cl_ulong>(param_value));
					break;
					
				case CL_DEVICE_LOCAL_MEM_TYPE:
					device.set_local_mem_type(opencl_param<cl_device_local_mem_type>(param_value));
					break;

				
				// images
				case CL_DEVICE_IMAGE_SUPPORT:
					break;
					
				case CL_DEVICE_IMAGE2D_MAX_HEIGHT:
					break;
					
				case CL_DEVICE_IMAGE2D_MAX_WIDTH:
					break;
					
				case CL_DEVICE_IMAGE3D_MAX_DEPTH:
					break;
					
				case CL_DEVICE_IMAGE3D_MAX_HEIGHT:
					break;
					
				case CL_DEVICE_IMAGE3D_MAX_WIDTH:
					break;
				
				// timing
				case CL_DEVICE_PROFILING_TIMER_RESOLUTION:
					device.set_profiling_timer_resolution(opencl_param<size_t>(param_value));
					break;
					
				case CL_DEVICE_MAX_CLOCK_FREQUENCY:
					device.set_max_clock_frequency(opencl_param<cl_uint>(param_value));
					break;
					
				default:
					break;
			}
		}
		devs.push_back(device);
	}
}

}	// cl::
}	// al::