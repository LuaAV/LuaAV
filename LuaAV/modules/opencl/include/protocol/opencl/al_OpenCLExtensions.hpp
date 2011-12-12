#ifndef AL_OPENCL_EXTENSIONS_H
#define AL_OPENCL_EXTENSIONS_H 1

#include "al_OpenCLInternal.hpp"

namespace al {
namespace cl {

extern void load_extension_functions();

} // cl::
} // al::


#ifdef __cplusplus
extern "C" {
#endif

#define AL_CLEXT_EXPORT extern
#define CLAPIENTRY


// cl_khr_fp64
#define CL_DEVICE_DOUBLE_FP_CONFIG          0x1032

// cl_khr_byte_addressable_store 
// cl_khr_global_int32_base_atomics 
// cl_khr_global_int32_extended_atomics  
// cl_khr_local_int32_base_atomics 
// cl_khr_local_int32_extended_atomics 

// cl_khr_gl_sharing
/*
//cl_int clGetGLContextInfoKHR(
typedef cl_int (CLAPIENTRY * PFNCLGETGLCONTEXTINFOKHRPROC) (
	const cl_context_properties *properties,
	cl_gl_context_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret
);
*/
// cl_APPLE_gl_sharing
#define CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE        0x10000000
#define CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE 		0x10000002
#define CL_CGL_DEVICES_FOR_SUPPORTED_VIRTUAL_SCREENS_APPLE	0x10000003
#define CL_INVALID_GL_CONTEXT_APPLE                         -1000      

//cl_int clGetGLContextInfoAPPLE (
typedef cl_int (CLAPIENTRY * PFNCLGETGLCONTEXTINFOAPPLEPROC) (
	cl_context /* context */,
	void * /* platform_gl_ctx */,
	cl_gl_platform_info /* param_name */,
	size_t /* param_value_size */,
	void * /* param_value */,
	size_t * /* param_value_size_ret */
);


// cl_APPLE_SetMemObjectDestructor 
//cl_int clSetMemObjectDestructorAPPLE(
typedef cl_int (CLAPIENTRY * PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC) (
	cl_mem /* memobj */, 
	void (* /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/), 
	void * /*user_data */
);


// cl_APPLE_ContextLoggingFunctions
//void clLogMessagesToSystemLogAPPLE(
typedef void (CLAPIENTRY * PFNCLLOGMESSAGESTOSYSTEMLOGAPPLE) (
	const char * /* errstr */, 
	const void * /* private_info */, 
	size_t       /* cb */, 
	void *       /* user_data */
);


//void clLogMessagesToStdoutAPPLE(
typedef void (CLAPIENTRY * PFNCLLOGMESSAGESTOSTDOUTAPPLE) (
	const char * /* errstr */, 
	const void * /* private_info */, 
	size_t       /* cb */, 
	void *       /* user_data */
);

//void clLogMessagesToStderrAPPLE(
typedef void (CLAPIENTRY * PFNCLLOGMESSAGESTOSTDERRAPPLE) (
	const char * /* errstr */, 
	const void * /* private_info */, 
	size_t       /* cb */, 
	void *       /* user_data */
);


//AL_CLEXT_EXPORT PFNCLGETGLCONTEXTINFOKHRPROC al_clGetGLContextInfoKHR;
AL_CLEXT_EXPORT PFNCLGETGLCONTEXTINFOAPPLEPROC al_clGetGLContextInfoAPPLE;
AL_CLEXT_EXPORT PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC al_clSetMemObjectDestructorAPPLE;
AL_CLEXT_EXPORT PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC al_clLogMessagesToSystemLogAPPLE;
AL_CLEXT_EXPORT PFNCLLOGMESSAGESTOSTDOUTAPPLE al_clLogMessagesToStdoutAPPLE;
AL_CLEXT_EXPORT PFNCLLOGMESSAGESTOSTDERRAPPLE al_clLogMessagesToStderrAPPLE;


#ifdef __cplusplus
}
#endif

#endif AL_OPENCL_EXTENSIONS_H
