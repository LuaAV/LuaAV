#ifndef FONT_QT_H
#define FONT_QT_H 1

#include <QFont>
#include <QFontInfo>
#include <QFontDatabase>
#include "Font.h"

class FontQtImpl;

class FontQt : public Font {
public:
	FontQt(FontQtImpl *impl, int font_size, bool anti_aliased=true);
	virtual ~FontQt();
	
	static FontQtImpl * create_font(const char *name, int size);
	
	al::ArrayWrapper * get_ascii_chars();

	// returns the width of a text string in pixels
	float width(const char *text);

	// returns the "above-line" height of the font in pixels
	float get_ascender();
	
	// returns the "below-line" height of the font in pixels
	float get_descender();

	int outline(int idx, AlloArray *vertex, AlloArray *index);
	
protected:
	FontQtImpl	*mImpl;;
};


#endif // FONT_QT_H
