#ifndef LUA_OPENGL_CONFIG_H
#define LUA_OPENGL_CONFIG_H 1

#include "graphics/al_GPUObject.hpp"
#include "protocol/al_GraphicsBackendOpenGL.hpp"

#ifdef WIN32
	#include <GL/glew.h>
#else
	#include "glew.h"
#endif


#if defined(WIN32) || defined(__WINDOWS_MM__)
    // Windows
	#include <windows.h>

	#include <gl/gl.h>
	#include <gl/glu.h>
#elif defined( __APPLE__ ) && defined( __MACH__ ) || defined (OSX)
    // OSX
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
#else
    // Linux
    #include <GL/gl.h>
	#include <GL/glu.h>
#endif

//useful macros
#ifndef MAX
	#define MAX(a, b) (a > b ? a : b)
#endif

#ifndef MIN
	#define MIN(a, b) (a < b ? a : b)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void lua_opengl_error(const char *msg);
extern void lua_opengl_register(const char *ctx, al::gfx::GPUObject *o);
extern void lua_opengl_unregister(al::gfx::GPUObject *o);
extern al::gfx::GraphicsBackendOpenGL * lua_opengl_get_backend(const char *ctx);

#ifdef __cplusplus
}
#endif


#endif // LUA_OPENGL_CONFIG_H