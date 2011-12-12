#include "al_OpenCLExtensions.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//PFNCLGETGLCONTEXTINFOKHRPROC al_clGetGLContextInfoKHR = NULL;
PFNCLGETGLCONTEXTINFOAPPLEPROC al_clGetGLContextInfoAPPLE = NULL;
PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC al_clSetMemObjectDestructorAPPLE = NULL;
PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC al_clLogMessagesToSystemLogAPPLE = NULL;
PFNCLLOGMESSAGESTOSTDOUTAPPLE al_clLogMessagesToStdoutAPPLE = NULL;
PFNCLLOGMESSAGESTOSTDERRAPPLE al_clLogMessagesToStderrAPPLE = NULL;

#ifdef __cplusplus
}
#endif

namespace al {
namespace cl {

void load_extension_functions() {
//	al_clGetGLContextInfoKHR = (PFNCLGETGLCONTEXTINFOKHRPROC)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
	al_clGetGLContextInfoAPPLE = (PFNCLGETGLCONTEXTINFOAPPLEPROC)clGetExtensionFunctionAddress("clGetGLContextInfoAPPLE");
	al_clSetMemObjectDestructorAPPLE = (PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC)clGetExtensionFunctionAddress("clSetMemObjectDestructorAPPLE");
	al_clLogMessagesToSystemLogAPPLE = (PFNCLSETMEMOBJECTDESTRUCTORAPPLEPROC)clGetExtensionFunctionAddress("clLogMessagesToSystemLogAPPLE");
	al_clLogMessagesToStdoutAPPLE = (PFNCLLOGMESSAGESTOSTDOUTAPPLE)clGetExtensionFunctionAddress("clLogMessagesToStdoutAPPLE");
	al_clLogMessagesToStderrAPPLE = (PFNCLLOGMESSAGESTOSTDERRAPPLE)clGetExtensionFunctionAddress("clLogMessagesToStderrAPPLE");
}

} // cl::
} // al::
