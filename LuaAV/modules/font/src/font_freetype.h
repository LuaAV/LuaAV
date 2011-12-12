#ifndef FONT_FREETYPE_H
#define FONT_FREETYPE_H 1

#include "lua_font.h"
#include "Font.h"

class FontFreetype : public Font {
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
	

	FontFreetype(FT_Face face, int font_size, bool anti_aliased=true);
	virtual ~FontFreetype();
	
	static FT_Face open_face(const char *filename);
	
	al::ArrayWrapper * get_ascii_chars();
	void load_glyph(int idx);
	void set_char_with_glyph(int idx);
	void fill_image(unsigned char *image, int rowstride);
	

	// returns the width of a text string in pixels
	float width(const char *text);

	// returns the "above-line" height of the font in pixels
	float get_ascender() { return mFace->size->metrics.ascender/PIX_TO_EM; }
	
	// returns the "below-line" height of the font in pixels
	float get_descender() { return mFace->size->metrics.descender/PIX_TO_EM; }

	// returns the total height of the font in pixels
	int get_font_size() { return mFontSize; }

	
	int outline(int idx, AlloArray *vertex, AlloArray *index);
	
protected:

	FT_Face mFace;

	void load_glyph(FontCharacter &character, int idx, AlloArray &lat);
};

#endif // FONT_FREETYPE_H