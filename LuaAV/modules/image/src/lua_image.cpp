#include "lua_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_glue.h"
#include "lua_array.h"
#include "Image.h"


#define LUA_IMAGE_ARRAY_FIELD	"__LUA_IMAGE_ARRAY_FIELD"

using image::Image;

/*! Image object for loading and saving image files

	Supported formats include:
	bmp, chead, cut, dcx, dds, doom, doomFlat, exif, gif, hdr, ico, jasc_pal, jpg,
	lbm, lif, mdl, pcd, pcx, pic, pix, png, pnm, psd, psp, pxr, raw, sgi, tgo, tif,
	wal, xpm as well as OpenEXR and rgb
	
	@module Image
*/

/*! Load an image file
	@param filename The image filename
	@name M:load
*/
int lua_image_load(lua_State *L) {
	Image *s = Glue<Image>::checkto(L, 1);
	if(s && lua_type(L, 2) == LUA_TSTRING) {
		const char *filename = lua_tostring(L, 2);
		s->load(filename);
	}
	else {
		luaL_error(L, "Image.load: invalid arguments");
	}
	return 1;
}

/*! Load an image file in its own thread
	load_threaded should be used with care.  Only try to access the internal 
	matrix after the image file is fully loaded by using Image.loaded.

	@param filename The image filename
	@see Image:loading
	@name M:load_threaded
*/
int lua_image_load_threaded(lua_State *L) {
	Image *s = Glue<Image>::checkto(L, 1);
	if(s && lua_type(L, 2) == LUA_TSTRING) {
		const char *filename = lua_tostring(L, 2);
		s->load_threaded(filename);
	}
	else {
		luaL_error(L, "Image.load_threaded: invalid arguments");
	}
	return 1;
}

/*! Check to see if the the image file has loaded
	@ret True/False for whether the file has loaded
	@name M:loading
*/
int lua_image_loading(lua_State *L) {
	Image *s = Glue<Image>::checkto(L, 1);
	if(s) {
		lua::push(L, s->loading());
	}
	else {
		luaL_error(L, "Image.loading: invalid arguments");
	}
	return 1;
}

/*! Save the internal image data to a file
	Supported formats for saving are JPEG (.jpg), JPEG2000 (.jp2), PNG (.png), 
	TIFF (.tif), BMP (.bmp), and GIF (.gif)
	@param filename The filename to use when saving the internal image data
	@name M:save
*/
int lua_image_save(lua_State *L) {
	Image *s = Glue<Image>::checkto(L, 1);
	if(s && lua_type(L, 2) == LUA_TSTRING) {
		const char *filename = lua_tostring(L, 2);
		s->save(filename);
	}
	else {
		luaL_error(L, "Image.save: invalid arguments");
	}
	return 1;
}

/*! Set the internal data with an Array
	@param array The array
	@name M:fromarray
*/
int lua_image_fromarray(lua_State *L) {
	Image *s = Glue<Image>::checkto(L, 1);
	AlloArray *lat = lua_array_checkto(L, 2);
	if(s && lat) {
		s->from_matrix(lat);
	}
	else {
		luaL_error(L, "Image.fromarray: invalid arguments");
	}
	return 1;
}

/*! Get the internal Array data
	@ret The array
	@name M:array
*/
int lua_image_array(lua_State *L) {
	Image *s = Glue<Image>::checkto(L, 1);
	if(s) {
		lua_getfenv(L, 1);
		lua_pushstring(L, LUA_IMAGE_ARRAY_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Image.array: invalid arguments");
	}
	return 1;
}


template<> const char * Glue<Image>::usr_name() { return "Image"; }
template<> bool Glue<Image>::usr_has_index() { return true; }

template<> void Glue<Image>::usr_push(lua_State * L, Image * u) {
	Glue<Image>::usr_attr_push(L);
	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_IMAGE_ARRAY_FIELD);
	Glue<al::ArrayWrapper>::push(L, u->wrapper());
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

template<> Image * Glue<Image>::usr_new(lua_State * L) {
	Image *img = new Image();
	if(lua_type(L, 1) == LUA_TSTRING) {
		const char *filename = lua_tostring(L, 1);
		img->load(filename);
	}
	return img;
}

template<> void Glue<Image>::usr_index(lua_State * L, Image * u) {
	Glue<Image>::usr_attr_index(L);
}

template<> void Glue<Image>::usr_newindex(lua_State * L, Image * u) {
	Glue<Image>::usr_attr_newindex(L);
}

template<> int Glue<Image>::usr_tostring(lua_State * L, Image * u) {
	lua_pushfstring(L, "%s: %p", Glue<Image>::usr_name(), u); 
	return 1;
}

template<> void Glue<Image>::usr_gc(lua_State * L, Image * u) { 
	delete u;
}

template<> void Glue<Image>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{"load", lua_image_load},
		{"load_threaded", lua_image_load_threaded},
		{"loading", lua_image_loading},
		{"save", lua_image_save},
		{"fromarray", lua_image_fromarray},
		{"array", lua_image_array},
		{ NULL, NULL}
	};
	Glue<Image>::usr_attr_mt(L, methods);
}



#ifdef __cplusplus
extern "C" {
#endif

int luaopen_Image(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	
	Glue<Image>::define(L);
	Glue<Image>::register_class(L);
	Glue<Image>::register_module(L, libname);

	return 1;
}


#ifdef __cplusplus
}
#endif
