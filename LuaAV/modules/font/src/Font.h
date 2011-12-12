#ifndef FONT_FONT_H
#define FONT_FONT_H

#include "allocore/types/al_Array.hpp"

#include <string>
#include <map>
#include <stdio.h>

using std::string;
using std::map;
using std::pair;


#define ASCII_SIZE 256	// number of characters to use
#define GLYPHS_PER_ROW 16	// number of glyphs to pack in a row
#define PIX_TO_EM (64.f)


class Font {
public:
	
	struct FontCharacter{
		FontCharacter()
		:	width(10),
			y_offset(0)
		{}
		
		~FontCharacter() {}

		int width;
		int y_offset;
	};
	

	Font(int font_size, bool anti_aliased=true);
	virtual ~Font();
	
	virtual al::ArrayWrapper * get_ascii_chars() = 0;
	void render(const char *text, AlloArray *vertex, AlloArray *texcoord);

	const FontCharacter & get_char(int idx) { return mChars[idx]; }

	// returns the width of a text string in pixels
	virtual float width(const char *text) = 0;

	// returns the "above-line" height of the font in pixels
	virtual float get_ascender() = 0;
	
	// returns the "below-line" height of the font in pixels
	virtual float get_descender() = 0;

	// returns the total height of the font in pixels
	int get_font_size() { return mFontSize; }

	
	virtual int outline(int idx, AlloArray *vertex, AlloArray *index) = 0;
	
protected:

	FontCharacter mChars[ASCII_SIZE];
	int mFontSize;
	bool mAntiAliased;
	al::ArrayWrapper *mAsciiChars;
};

#endif // FONT_FONT_H
