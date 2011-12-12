#include "lua_font.h"
#include "Font.h"
#include "font_freetype.h"
#include "lua_utility.h"
#include "lua_glue.h"
#include "lua_array.h"

#ifdef FONT_OSX
#include "font_cocoa.h"
#endif

#ifdef FONT_QT
#include "font_qt.h"
#endif

#define LUA_FONT_ASCII_CHARS_FIELD "__LUA_FONT_ASCII_CHARS_FIELD"

/*! A font face at a given size
	The Font object models a font face at a given size.  It calculates all of the 
	geometry and texture data required to render the specific font.
	
	@module font.Font
*/

/*! Get metrics of a particular character
	@param char The character
	@ret The character metrics
	@LuaMethod METHOD
	@name M:char
*/
int lua_font_char(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	if(s && lua::is<int>(L, 2)) {
		int idx = MAX(MIN(lua::to<int>(L, 2), 255), 0);
		const Font::FontCharacter &c = s->get_char(idx);
		lua_newtable(L);
		lua::pushfield(L, -1, "width", c.width);
		lua::pushfield(L, -1, "y_offset", c.y_offset);
	}
	else {
		luaL_error(L, "Font.char: invalid arguments");
	}
	return 1;
}

/*! The a bitmap of the font's ASCII characters
	The a bitmap of the font's ASCII characters in a 16x16 grid
	@ret The bitmap as an Array
	@LuaMethod METHOD
	@name M:ascii_chars
*/
int lua_font_ascii_chars(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	if(s) {
		lua_getfenv(L, 1);
		lua_pushstring(L, LUA_FONT_ASCII_CHARS_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Font.ascii_chars: invalid arguments");
	}
	return 1;
}

/*! Render text geometry
	Render text into geometry for drawing a string of text using the bitmap 
	returned by ascii_chars.  Render expects the vertex and texcoord buffers 
	to be at least as big as the text length * 4 since each character is 
	rendered by a quad.

	Example usage:
	<pre>
		local tex:fromarray(font:ascii_chars())
	
		local text = "jabberwocky"
		font:render(text, vertex, texcoord)
		mesh:vertex(vertex)
		mesh:texcoord(texcoord)
		
		tex:bind()
		mesh:draw()
		tex:unbind()
	</pre>
	
	@param text The text
	@param vertex The buffer to hold the vertex data
	@param texcoord The buffer to hold the texcoord data
	@LuaMethod METHOD
	@name M:render
*/
int lua_font_render(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	AlloArray *v = lua_array_checkto(L, 3);
	AlloArray *tc = lua_array_checkto(L, 4);
	if(
		s && 
		lua::is<const char *>(L, 2) && 
		v && 
		tc
	) {
		const char *text = lua::to<const char *>(L, 2);
		s->render(text, v, tc);
	}
	else {
		luaL_error(L, "Font.render: invalid arguments");
	}
	return 0;
}

int lua_font_outline(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	AlloArray *vertex = lua_array_checkto(L, 3);
	AlloArray *index = lua_array_checkto(L, 4);
	if(lua::is<int>(L, 2) ) {
		s->outline(lua::to<int>(L, 2), vertex, index);
	}
	else {
		luaL_error(L, "Font.outline: invalid arguments");
	}
	return 0;
}

/*! Get the width of text in the font
	@param text The text
	@ret The width of the text
	@LuaMethod METHOD
	@name M:width
*/
int lua_font_width(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	if(s && lua::is<const char *>(L, 2)) {
		const char *text = lua::to<const char *>(L, 2);
		lua::push<float>(L, s->width(text));
	}
	else {
		luaL_error(L, "Font.width: invalid arguments");
	}
	return 1;
}

int lua_font_ascender(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	if(s) {
		lua::push<float>(L, s->get_ascender());
	}
	else {
		luaL_error(L, "Font.ascender: invalid arguments");
	}
	return 1;
}

int lua_font_descender(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	if(s) {
		lua::push<float>(L, s->get_descender());
	}
	else {
		luaL_error(L, "Font.descender: invalid arguments");
	}
	return 1;
}

/*! Get the size of the font
	@LuaMethod GET
	@name M.size
*/
int lua_font_size(lua_State *L) {
	Font *s = Glue<Font>::checkto(L, 1);
	if(s) {
		lua::push<int>(L, s->get_font_size());
	}
	else {
		luaL_error(L, "Font.size: invalid arguments");
	}
	return 1;
}

template<> const char * Glue<Font>::usr_name() { return "Font"; }
template<> bool Glue<Font>::usr_has_index() { return true; }

template<> void Glue<Font>::usr_push(lua_State * L, Font * u) {
	Glue<Font>::usr_attr_push(L);
	Glue<Font>::usr_attr_prototype(L);

	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_FONT_ASCII_CHARS_FIELD);
	Glue<al::ArrayWrapper>::push(L, u->get_ascii_chars());
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

template<> Font * Glue<Font>::usr_new(lua_State * L) {
	if(lua::is<const char *>(L, 1)) {
		const char *filename = lua::to<const char *>(L, 1);
		int size = lua::opt<int>(L, 2, 10);
		bool anti_aliased = lua::opt<bool>(L, 3, true);
		FT_Face face = FontFreetype::open_face(filename);
		
		Font *font = NULL;
		
		if(! face) {
			#ifdef FONT_OSX
				FontCocoaImpl *ci = FontCocoa::create_font(filename, size);
				if(ci) {
					font = new FontCocoa(ci, size, anti_aliased);
				}
			#endif

			#ifdef FONT_QT
				if(!font) {
					FontQtImpl *qi = FontQt::create_font(filename, size);
					font = new FontQt(qi, size, anti_aliased);
				}
			#endif
			
			if(!font) {
				luaL_error(L, "Font(): error opening font %s", filename);
			}
		}
		else {
			font = new FontFreetype(face, size, anti_aliased);
		}
		
		return font;
	}
	else {
		luaL_error(L, "Font(): missing font filename");
	}
	return NULL;
}

template<> void Glue<Font>::usr_index(lua_State * L, Font * u) {
	Glue<Font>::usr_attr_index(L);
}

template<> void Glue<Font>::usr_newindex(lua_State * L, Font * u) {
	Glue<Font>::usr_attr_newindex(L);
}

template<> int Glue<Font>::usr_tostring(lua_State * L, Font * u) {
	lua_pushfstring(L, "%s: %p", Glue<Font>::usr_name(), u); 
	return 1;
}

template<> void Glue<Font>::usr_gc(lua_State * L, Font * u) { 
	delete u;
}

template<> void Glue<Font>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "char", lua_font_char },
		{ "ascii_chars", lua_font_ascii_chars },
		{ "render", lua_font_render },
		{ "outline", lua_font_outline },
		{ "width", lua_font_width},
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "ascender", lua_font_ascender },
		{ "descender", lua_font_descender },
		{ "size", lua_font_size },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<Font>::usr_attr_mt(L, methods, getters, setters);
}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_font_font(lua_State *L) {
	Glue<Font>::define(L);
	Glue<Font>::register_class(L);
	luaL_getmetatable(L, Glue<Font>::mt_name(L));

	return 1;
}

#ifdef __cplusplus
}
#endif
