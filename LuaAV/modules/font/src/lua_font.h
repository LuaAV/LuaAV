#ifndef LUA_FONT_H
#define LUA_FONT_H 1

#if defined (__APPLE__) || defined (OSX)

	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_OUTLINE_H
	#include FT_GLYPH_H
	
	#define FONT_OSX 1

#elif defined(__linux__)

	#include <ft2build.h>
	#include <freetype/freetype.h>
	#include <freetype/ftoutln.h>
	#include <freetype/ftglyph.h>

#elif defined WIN32

	#include <windows.h>
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_OUTLINE_H
	#include FT_GLYPH_H

#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>

extern FT_Library front_freetype_library;

#ifdef __cplusplus
}
#endif

#endif // LUA_FONT_H
