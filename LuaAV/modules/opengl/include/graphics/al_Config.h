#ifndef INCLUDE_AL_GRAPHICS_CONFIG_H
#define INCLUDE_AL_GRAPHICS_CONFIG_H

#include "allocore/system/al_Config.h"

#ifdef WIN32
	#include <GL/glew.h>
#else
	#include "glew.h"
#endif

// OpenGL platform-dependent includes

#if defined(AL_OSX)
	#define AL_GRAPHICS_USE_OPENGL
	
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
	
	#define AL_GRAPHICS_INIT_CONTEXT\
		/* prevents tearing */ \
		{	GLint MacHackVBL = 1;\
			CGLContextObj ctx = CGLGetCurrentContext();\
			CGLSetParameter(ctx,  kCGLCPSwapInterval, &MacHackVBL); }
			
#elif defined(AL_LINUX)
	#define AL_GRAPHICS_USE_OPENGL
	
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <time.h>
	
	#define AL_GRAPHICS_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if (GLEW_OK != err){\
  				/* Problem: glewInit failed, something is seriously wrong. */\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}
#elif defined(AL_WIN32)
	#define AL_GRAPHICS_USE_OPENGL
	
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )
	
	#define AL_GRAPHICS_INIT_CONTEXT
	
#else

	#ifdef __IPHONE_2_0
		#define AL_GRAPHICS_USE_OPENGLES1
		
		#import <OpenGLES/ES1/gl.h>
		#import <OpenGLES/ES1/glext.h>
	#endif

	#ifdef __IPHONE_3_0
		#define AL_GRAPHICS_USE_OPENGLES2
		
		#import <OpenGLES/ES2/gl.h>
		#import <OpenGLES/ES2/glext.h>
	#endif

#endif

#endif
