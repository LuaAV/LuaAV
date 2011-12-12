#ifndef FONT_COCOA_H
#define FONT_COCOA_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_utility.h"
#include "Font.h"

class FontCocoaImpl;

class FontCocoa : public Font {
public:
	
	FontCocoa(FontCocoaImpl *impl, int font_size, bool anti_aliased=true);
	virtual ~FontCocoa();
	
	static FontCocoaImpl * create_font(const char *name, int size);
	
	al::ArrayWrapper * get_ascii_chars();

	// returns the width of a text string in pixels
	float width(const char *text);

	// returns the "above-line" height of the font in pixels
	float get_ascender();
	
	// returns the "below-line" height of the font in pixels
	float get_descender();

	int outline(int idx, AlloArray *vertex, AlloArray *index);
	
protected:
	FontCocoaImpl *mImpl;
	
};



#endif // FONT_COCOA_H
