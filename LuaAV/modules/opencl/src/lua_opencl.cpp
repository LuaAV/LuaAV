#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "lua_opencl_constants.h"
#include "lua_opencl_util.h"
#include "al_OpenCL.hpp"
#include "lua_array.h"
#include "lua_utility.h"
#include "lua_glue.h"

using namespace al;
using namespace cl;

static OpenCLEngine s_engine;
//const char *opencl_Kernel = "opencl.Kernel";
//const char *opencl_MemoryBuffer = "opencl.MemoryBuffer";


/*!	OpenCL Kernel
	Kernel objects represents compiled compiled OpenCL code with a single __kernel.  The 
	high-level interface always compiles programs for every device, so kernels retrieved 
	from opencl.get_kernel will run on any system device.
	
	@module opencl.Kernel
*/

/*! Set the argument to a kernel
	@param idx The argument index (starts from 0)
	@param val The argument value

	@name M.argument
*/
int lua_opencl_kernel_argument(lua_State *L) {
	OpenCLKernel *ker = Glue<OpenCLKernel>::checkto(L, 1);
	if(
		ker && 
		lua::is<cl_uint>(L, 2) && 
		lua_type(L, 3) != LUA_TNIL
	) {
		cl_uint idx = lua::to<cl_uint>(L, 2);
		OpenCLMemoryBuffer *mem = Glue<OpenCLMemoryBuffer>::to(L, 3);
		if(mem) {
			ker->set_argument(idx, *mem);
		}
		else {
			//??	
		}
	}
	else {
		luaL_error(L, "Kernel.argument: invalid arguments");
	}
	return 0;
}

template<> const char * Glue<OpenCLKernel>::usr_name() { return "OpenCLKernel"; }
template<> bool Glue<OpenCLKernel>::usr_has_index() { return true; }

template<> void Glue<OpenCLKernel>::usr_push(lua_State * L, OpenCLKernel * u) {
	Glue<OpenCLKernel>::usr_attr_push(L);
}

template<> OpenCLKernel * Glue<OpenCLKernel>::usr_new(lua_State * L) {
	return NULL;
}

template<> void Glue<OpenCLKernel>::usr_index(lua_State * L, OpenCLKernel * u) {
	Glue<OpenCLKernel>::usr_attr_index(L);
}

template<> void Glue<OpenCLKernel>::usr_newindex(lua_State * L, OpenCLKernel * u) {
	Glue<OpenCLKernel>::usr_attr_newindex(L);
}

template<> int Glue<OpenCLKernel>::usr_tostring(lua_State * L, OpenCLKernel * u) {
	lua_pushfstring(L, "%s: %p", Glue<OpenCLKernel>::usr_name(), u); 
	return 1;
}

template<> void Glue<OpenCLKernel>::usr_gc(lua_State * L, OpenCLKernel * u) { 
	delete u;
}

template<> void Glue<OpenCLKernel>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "argument", lua_opencl_kernel_argument },
		{ NULL, NULL}
	};
	
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<OpenCLKernel>::usr_attr_mt(L, methods, getters, setters);
}



template<> const char * Glue<OpenCLMemoryBuffer>::usr_name() { return "OpenCLMemoryBuffer"; }
template<> bool Glue<OpenCLMemoryBuffer>::usr_has_index() { return true; }

template<> void Glue<OpenCLMemoryBuffer>::usr_push(lua_State * L, OpenCLMemoryBuffer * u) {
	Glue<OpenCLMemoryBuffer>::usr_attr_push(L);
}

template<> OpenCLMemoryBuffer * Glue<OpenCLMemoryBuffer>::usr_new(lua_State * L) {
	return NULL;
}

template<> void Glue<OpenCLMemoryBuffer>::usr_index(lua_State * L, OpenCLMemoryBuffer * u) {
	Glue<OpenCLMemoryBuffer>::usr_attr_index(L);
}

template<> void Glue<OpenCLMemoryBuffer>::usr_newindex(lua_State * L, OpenCLMemoryBuffer * u) {
	Glue<OpenCLMemoryBuffer>::usr_attr_newindex(L);
}

template<> int Glue<OpenCLMemoryBuffer>::usr_tostring(lua_State * L, OpenCLMemoryBuffer * u) {
	lua_pushfstring(L, "%s: %p", Glue<OpenCLMemoryBuffer>::usr_name(), u); 
	return 1;
}

template<> void Glue<OpenCLMemoryBuffer>::usr_gc(lua_State * L, OpenCLMemoryBuffer * u) { 
	delete u;
}

template<> void Glue<OpenCLMemoryBuffer>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ NULL, NULL}
	};
	
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<OpenCLMemoryBuffer>::usr_attr_mt(L, methods, getters, setters);
}

/**********************************************************************************************
*/

/*!	OpenCL bindings
	The OpenCL module contains both high-level and low-level bindings.  The low-level bindings 
	provide wrappers around OpenCL API functions while the high-level functions enable the use 
	of OpenCL with minimal amounts of code.  With high-level functions the OpenCL context and 
	its resources are automatically managed.
	
	@module opencl
*/


/*! Get the platform ids on the system
	@ret List of platform ids

	@name platforms
*/
int lua_opencl_platforms(lua_State *L) {
	cl_platform_id platforms[MAX_PLATFORMS];
	cl_uint num_platforms = 0;
	cl_int res = clGetPlatformIDs(
		MAX_PLATFORMS,
		platforms,
		&num_platforms
	);
	
	if(opencl_error(res, "clGetPlatformIDs error querying platforms")) {
		return 0;
	}
	
	lua::push_vec_t<cl_platform_id>(L, (int)num_platforms, platforms);
	
	return 1;
}

/*! Get information on a particular platform
	@param id The platform id
	@param param The name of the parameter to get
	@ret The parameter value

	@name platform_info
*/
int lua_opencl_platform_info(lua_State *L) {
	if(	
		lua::is<cl_platform_id>(L, 1) && 
		lua::is<cl_platform_info>(L, 2)
	) {
		cl_platform_id pid = lua::to<cl_platform_id>(L, 1);
		cl_platform_info param_name = lua::to<cl_platform_info>(L, 2);
		
		char param_value[PLATFORM_PARAM_SIZE];
		size_t param_value_size_ret = 0;
		cl_int res = clGetPlatformInfo(
			pid,
			param_name,
			PLATFORM_PARAM_SIZE,
			param_value,
			&param_value_size_ret
		);
		
		if(opencl_error(res, "clGetPlatformInfo getting platform information")) {
			return 0;
		}
		
		switch(param_name) {
			case CL_PLATFORM_VENDOR:
			case CL_PLATFORM_NAME:
			case CL_PLATFORM_VERSION:
			case CL_PLATFORM_PROFILE:
			case CL_PLATFORM_EXTENSIONS:
				 lua::push<const char *>(L, param_value);
				 break;
				 
			default:
				luaL_error(L, "invalid parameter name");
				break;
		}
		
	}
	else {
		luaL_error(L, "opencl.platform_info: invalid arguments");
	}
	return 1;
}

/*! Get the devices on a particular platform
	@param id The platform id
	@ret The list of device ids

	@name devices
*/
int lua_opencl_devices(lua_State *L) {
	if(lua::is<cl_platform_id>(L, 1)) {
		cl_platform_id pid = lua::to<cl_platform_id>(L, 1);
	
		lua_newtable(L);
	
		cl_device_id devices[MAX_DEVICES];
		cl_uint num_devices = 0;
		cl_int res = clGetDeviceIDs(
			pid,
			CL_DEVICE_TYPE_ALL,
			MAX_DEVICES,
			devices,
			&num_devices
		);
		
		if(opencl_error(res, "clGetDeviceIDs error querying platform devices")) {
			return 0;
		}
	
		lua::push_vec_t<cl_device_id>(L, (int)num_devices, devices);
	}
	else {
		luaL_error(L, "opencl.devices: missing platform id argument");
	}
	
	return 1;
}

/*! Get information on a particular device
	@param id The device id
	@param param The name of the parameter to get
	@ret The parameter value

	@name device_info
*/
int lua_opencl_device_info(lua_State *L) {
	if(	
		lua::is<cl_device_id>(L, 1) && 
		lua::is<cl_device_info>(L, 2)
	) {
		cl_device_id pid = lua::to<cl_device_id>(L, 1);
		cl_device_info param_name = lua::to<cl_device_info>(L, 2);
		
		char param_value[PLATFORM_PARAM_SIZE];
		size_t param_value_size_ret = 0;
		cl_int res = clGetDeviceInfo(
			pid,
			param_name,
			PLATFORM_PARAM_SIZE,
			param_value,
			&param_value_size_ret
		);
	
		if(opencl_error(res, "clGetDeviceInfo getting platform information")) {
			return 0;
		}
		
		switch(param_name) {
			case CL_DEVICE_ADDRESS_BITS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_AVAILABLE: 
				lua::push<cl_bool>(L, opencl_param<cl_bool>(param_value));
				break;
				
			case CL_DEVICE_COMPILER_AVAILABLE: 
				lua::push<cl_bool>(L, opencl_param<cl_bool>(param_value));
				break;
				
			case CL_DEVICE_DOUBLE_FP_CONFIG: 
				lua::push<cl_device_fp_config>(L, opencl_param<cl_device_fp_config>(param_value));
				break;
				
			case CL_DEVICE_ENDIAN_LITTLE: 
				lua::push<cl_bool>(L, opencl_param<cl_bool>(param_value));
				break;
				
			case CL_DEVICE_ERROR_CORRECTION_SUPPORT: 
				lua::push<cl_bool>(L, opencl_param<cl_bool>(param_value));
				break;
				
			case CL_DEVICE_EXECUTION_CAPABILITIES: 
				lua::push<cl_device_exec_capabilities>(L, opencl_param<cl_device_exec_capabilities>(param_value));
				break;
				
			case CL_DEVICE_EXTENSIONS: 
				lua::push<const char *>(L, param_value);
				break;
				
			case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: 
				lua::push<cl_ulong>(L, opencl_param<cl_ulong>(param_value));
				break;
				
			case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: 
				lua::push<cl_device_mem_cache_type>(L, opencl_param<cl_device_mem_cache_type>(param_value));
				break;
				
			case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
			
			case CL_DEVICE_GLOBAL_MEM_SIZE: 
				lua::push<cl_ulong>(L, opencl_param<cl_ulong>(param_value));
				break;
			
			/*
			case CL_DEVICE_HALF_FP_CONFIG: 
				lua_push<cl_device_fp_config>(L, opencl_param<cl_device_fp_config>(param_value));
				break;
			*/
			
			case CL_DEVICE_IMAGE_SUPPORT: 
				lua::push<cl_bool>(L, opencl_param<cl_bool>(param_value));
				break;
				
			case CL_DEVICE_IMAGE2D_MAX_HEIGHT: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				
			case CL_DEVICE_IMAGE2D_MAX_WIDTH: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
			
			case CL_DEVICE_IMAGE3D_MAX_DEPTH: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				
			case CL_DEVICE_IMAGE3D_MAX_HEIGHT: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				
			case CL_DEVICE_IMAGE3D_MAX_WIDTH: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				
			case CL_DEVICE_LOCAL_MEM_SIZE: 
				lua::push<cl_ulong>(L, opencl_param<cl_ulong>(param_value));
				break;
				
			case CL_DEVICE_LOCAL_MEM_TYPE: 
				lua::push<cl_device_local_mem_type>(L, opencl_param<cl_device_local_mem_type>(param_value));
				break;
				
			case CL_DEVICE_MAX_CLOCK_FREQUENCY: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MAX_COMPUTE_UNITS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MAX_CONSTANT_ARGS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: 
				lua::push<cl_ulong>(L, opencl_param<cl_ulong>(param_value));
				break;
			
			case CL_DEVICE_MAX_MEM_ALLOC_SIZE: 
				lua::push<cl_ulong>(L, opencl_param<cl_ulong>(param_value));
				break;
				
			case CL_DEVICE_MAX_PARAMETER_SIZE: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				
			case CL_DEVICE_MAX_READ_IMAGE_ARGS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MAX_SAMPLERS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MAX_WORK_GROUP_SIZE: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				 
			case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MAX_WORK_ITEM_SIZES: 
				lua::push_vec_t<size_t>(
					L, 
					param_value_size_ret/sizeof(size_t), 
					opencl_param<size_t *>(param_value)
				);
				break;
				
			case CL_DEVICE_MAX_WRITE_IMAGE_ARGS: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_MEM_BASE_ADDR_ALIGN: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;

			case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_NAME: 
				lua::push<const char *>(L, param_value);
				break;
				
			case CL_DEVICE_PLATFORM: 
				lua::push<cl_platform_id>(L, opencl_param<cl_platform_id>(param_value));
				break;
				
			case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: 
			case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: 
			case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: 
			case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: 
			case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: 
			case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: 
				lua::push<cl_uint>(L, opencl_param<cl_uint>(param_value));
				break;
				
			case CL_DEVICE_PROFILE: 
				lua::push<const char *>(L, param_value);
				break;
				
			case CL_DEVICE_PROFILING_TIMER_RESOLUTION: 
				lua::push<size_t>(L, opencl_param<size_t>(param_value));
				break;
				
			case CL_DEVICE_QUEUE_PROPERTIES: 
				lua::push<cl_command_queue_properties>(L, opencl_param<cl_command_queue_properties>(param_value));
				break;
				
			case CL_DEVICE_SINGLE_FP_CONFIG: 
				lua::push<cl_device_fp_config>(L, opencl_param<cl_device_fp_config>(param_value));
				break;
				
			case CL_DEVICE_TYPE: 
				lua::push<cl_device_type>(L, opencl_param<cl_device_type>(param_value));
				break;
				
			case CL_DEVICE_VENDOR: 
				lua::push<const char *>(L, param_value);
				break;
				
			case CL_DEVICE_VERSION: 
				lua::push<const char *>(L, param_value);
				break;
				
			case CL_DRIVER_VERSION: 
				lua::push<const char *>(L, param_value);
				break;
				
				
			default:
				luaL_error(L, "invalid parameter name");
				break;
		}
		
	}
	else {
		luaL_error(L, "opencl.device_info: invalid arguments");
	}
	return 1;
}

/*! The the system's GPU device id
	@ret The id of the GPU device

	@name gpu_device
*/
int lua_opencl_gpu_device(lua_State *L) {
	OpenCLDevice *dev = s_engine.gpu_device();
	if(dev) {
		lua::push<cl_device_id>(L, dev->get_device());
	}
	else {
		luaL_error(L, "opencl.gpu_device: No GPU device available");
	}
	return 1;
}

/*! The the system's CPU device id
	@ret The id of the CPU device

	@name cpu_device
*/
int lua_opencl_cpu_device(lua_State *L) {
	OpenCLDevice *dev = s_engine.cpu_device();
	if(dev) {
		lua::push<cl_device_id>(L, dev->get_device());
	}
	else {
		luaL_error(L, "opencl.cpu_device: No CPU device available");
	}
	return 1;
}

/*! Compile OpenCL source code
	A high-level function associating a name with compiled source code.  
	The resulting cl_program is managed internally.  The kernels in the 
	program can be retrieved with the <b>get_kernel</b> function by using 
	the same name parameter.
	
	@param name The name of the source code
	@param soruce The source code

	@see get_kernel
	@name compile_source
*/
int lua_opencl_compile_source(lua_State *L) {
	if(
		lua::is<const char *>(L, 1) &&
		lua::is<const char *>(L, 2)
	) {
		const char *name = lua::to<const char *>(L, 1);
		const char *source = lua::to<const char *>(L, 2);
		s_engine.compile_source(name, source);
	}
	else {
		luaL_error(L, "opencl.compile_source: invalid arguments");
	}
	return 0;
}

/*! Get an OpenCL kernel associated with previously compiled code
	If the kernel_name is not provided, the first kernel found will 
	be returned.
	
	@param name The name of the compiled code
	@param [kerenel_name] Optional name of the kernel
	@ret The kernel

	@see compile_source
	@name get_kernel
*/
int lua_opencl_get_kernel(lua_State *L) {
	if(lua::is<const char *>(L, 1)) {
		const char *name = lua::to<const char *>(L, 1);
		const char *kernel_name = lua::is<const char *>(L, 2) ? 
										lua::to<const char *>(L, 2) : 
										NULL
									;
		OpenCLKernel *ker = s_engine.get_kernel(name, kernel_name);
		if(ker) {
			Glue<OpenCLKernel>::push(L, ker);
		}
	}
	else {
		luaL_error(L, "opencl.get_kernel: invalid arguments");
	}
	return 1;
}

/*! Enqueue an OpenCL kernel for processing

	@param device_id The id of the device to use
	@param kernel The kernel to run
	@param ndim The dimensionality of the domain
	@param global The size of the global work area
	@param local The size of the local work area
	@ret The resulting event

	@name enqueue_kernel
*/
int lua_opencl_enqueue_kernel(lua_State *L) {
	OpenCLKernel *ker = Glue<OpenCLKernel>::checkto(L, 2);
	size_t global[] = {0, 0, 0};
	size_t local[] = {0, 0, 0};
	
	if(
		lua::is<cl_device_id>(L, 1) && 
		ker && 
		lua::is<cl_uint>(L, 3) && 
		lua::to_vec_t<size_t>(L, 4, 3, global) && 
		lua::to_vec_t<size_t>(L, 5, 3, local)
	) {
		cl_device_id pid = lua::to<cl_device_id>(L, 1);
		cl_uint ndim = lua::to<cl_uint>(L, 3);
		
		const OpenCLDevice *dev = s_engine.get_device(pid);
		if(! dev) {
			luaL_error(L, "opencl.enqueue_kernel: invalid device id");
		}
		
		OpenCLEvent e = s_engine.enqueue_kernel(
			*dev,
			ker,
			ndim,
			global,
			local
		);
		
		lua::push(L, e);
	}
	else {
		luaL_error(L, "opencl.enqueue_kernel: invalid arguments");
	}
	return 1;
}

/*! Enqueue a read on an OpenCL memory buffer

	@param device_id The id of the device to use
	@param mem The memory
	@param block The blocking flag
	@param global The size of the global work area
	@param array The array to read back the data into
	@ret The resulting event

	@name enqueue_read
*/
int lua_opencl_enqueue_read(lua_State *L) {
	OpenCLMemoryBuffer *mem = Glue<OpenCLMemoryBuffer>::checkto(L, 2);
	AlloArray *array = lua_array_checkto(L, 5);
	if(
		lua::is<cl_device_id>(L, 1) && 
		mem && 
		lua::is<bool>(L, 3) && 
		lua::is<size_t>(L, 4) && 
		array
	) {
		cl_device_id pid = lua::to<cl_device_id>(L, 1);
		bool block = lua::to<bool>(L, 3);
		size_t offset = lua::to<size_t>(L, 4);
		
		const OpenCLDevice *dev = s_engine.get_device(pid);
		if(! dev) {
			luaL_error(L, "opencl.enqueue_read: invalid device id");
		}
		
		OpenCLEvent e = s_engine.enqueue_read(
			*dev,
			mem,
			block,
			offset,
			array
		);
		
		lua::push(L, e);
	}
	else {
		luaL_error(L, "opencl.enqueue_read: invalid arguments");
	}
	return 1;
}

/*! Create a memory buffer

	@param usage The memory usage flags
	@param array The array data to associate with the kernel
	@ret The memory buffer

	@name create_memory_buffer
*/
int lua_opencl_create_memory_buffer(lua_State *L) {
	AlloArray *array = lua_array_checkto(L, 2);
	if(
		lua::is<cl_mem_flags>(L, 1) && 
		array
	) {
		cl_mem_flags usage = lua::to<cl_mem_flags>(L, 1);
		OpenCLMemoryBuffer *mem = s_engine.create_memory_buffer(usage, array);
		if(mem) {
			Glue<OpenCLMemoryBuffer>::push(L, mem);
		}
	}
	else {
		luaL_error(L, "opencl.create_memory_buffer: invalid arguments");
	}
	return 1;
}

/*! Execute a kernel on a device with arguments
	Execute will set the arguments for the kernel and then enqueue it on the 
	given device.  It is a wrapper around Kernel.argument and opencl.enqueue_kernel.

	@param device_id The id of the device to use
	@param ker The kerenel to execute
	@param ndim The dimensionality of the domain
	@param global The dimensions of the domain
	@param ... The arguments to the kernel
	@ret The resulting event

	@see Kernel.argument
	@see enqueue_kernel
	@name execute
*/
int lua_opencl_execute(lua_State *L) {
	int top = lua_gettop(L);
	OpenCLKernel *ker = Glue<OpenCLKernel>::checkto(L, 2);
	size_t global[] = {0, 0, 0};
	
	if(
		lua::is<cl_device_id>(L, 1) && 
		ker &&
		lua::is<cl_uint>(L, 3) && 
		lua::to_vec_t<size_t>(L, 4, 3, global) && 
		top >= 5
	) {
		cl_device_id pid = lua::to<cl_device_id>(L, 1);
		cl_uint ndim = lua::to<cl_uint>(L, 3);
		
		const OpenCLDevice *dev = s_engine.get_device(pid);
		if(! dev) {
			luaL_error(L, "opencl.enqueue_kernel: invalid device id");
		}
		
		OpenCLEvent event;
		
		switch(top) {
			case 5: {
				OpenCLMemoryBuffer *mem = Glue<OpenCLMemoryBuffer>::to(L, 5);
				if(mem) {
					event = s_engine.execute(
						*dev,
						ker,
						ndim,
						global,
						*mem
					);
				}
			} break;
			
			case 6:
			default: {
				OpenCLMemoryBuffer *mem1 = Glue<OpenCLMemoryBuffer>::to(L, 5);
				OpenCLMemoryBuffer *mem2 = Glue<OpenCLMemoryBuffer>::to(L, 6);
				if(mem1 && mem2) {
					event = s_engine.execute(
						*dev,
						ker,
						ndim,
						global,
						*mem1,
						*mem2
					);
				}
			} break;
		}
		lua::push(L, event);
	}
	else {
		luaL_error(L, "opencl.execute: invalid arguments");
	}
	return 1;
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_opencl(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	
	struct luaL_reg lib[] = {
		{ "platforms", lua_opencl_platforms },
		{ "platform_info", lua_opencl_platform_info },
		{ "devices", lua_opencl_devices },
		{ "device_info", lua_opencl_device_info },
		
		// high-level functions
		{ "gpu_device", lua_opencl_gpu_device },
		{ "cpu_device", lua_opencl_cpu_device },
		{ "compile_source", lua_opencl_compile_source },
		{ "get_kernel", lua_opencl_get_kernel },
		{ "enqueue_kernel", lua_opencl_enqueue_kernel },
		{ "enqueue_read", lua_opencl_enqueue_read },
		{ "create_memory_buffer", lua_opencl_create_memory_buffer },
		{ "execute", lua_opencl_execute },
		{ NULL, NULL }
	};
	luaL_register(L, libname, lib);
	
	for(int i=0; opencl_enums[i].name; i++) {
		lua_pushinteger(L, (lua_Integer)opencl_enums[i].value);
		lua_setfield(L, -2, opencl_enums[i].name);
	}
	
	Glue<OpenCLKernel>::define(L);
	Glue<OpenCLMemoryBuffer>::define(L);
	
	return 1;
}


#ifdef __cplusplus
}
#endif