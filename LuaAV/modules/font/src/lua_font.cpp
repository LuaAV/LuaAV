#include "lua_font.h"
#include "Font.h"
#include "lua_glue.h"
#include "lua_utility.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FONT_OSX) || defined(FONT_QT)
	#define FONT_SYSTEM_FONTS 1
#endif

FT_Library front_freetype_library = 0;

#ifdef FONT_SYSTEM_FONTS

extern int font_names(lua_State *L);
extern int font_families(lua_State *L);

#endif






static const luaL_reg font_lib[] = {
#ifdef FONT_SYSTEM_FONTS
	{"names", font_names},
	{"families", font_families},
#endif
	{NULL, NULL}
};


extern int luaopen_font_font(lua_State *L);


/*! Font loading and rendering module based on FreeType
	The font module provides a high-level binding to the FreeType 
	library to simplify font rendering.  The font module itself does 
	not render fonts but calculates the geometry and texture data 
	needed to render the fonts using Meshes, Textures, or even low-level 
	OpenGL commands.
	
	@module font
*/
extern int luaopen_font(lua_State *L) {
	if(! front_freetype_library) {
		FT_Error err = FT_Init_FreeType(&front_freetype_library);
		if(err) {
			fprintf(stderr, "font: error initializing FreeType library\n");
		}
	}
	
	const char *libname = lua_tostring(L, -1);
	luaL_register(L, libname, font_lib);
	
	
	lua::preloadlib(L, "font.Font", luaopen_font_font);

	return 1;
}

#ifdef __cplusplus
}
#endif
