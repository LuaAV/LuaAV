#include "font_qt.h"
#include <QStringList>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>

#include "lua_array.h"
#include "allocore/types/al_Array.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"


int font_names(lua_State *L) {
	QFontDatabase database;

	lua_newtable(L);
	int idx = 1;
	foreach(QString family, database.families()) {
		foreach(QString style, database.styles(family)) {
			QString name(family);
			name.append("-");
			name.append(style);

			lua_pushstring(L, name.toStdString().data());
			lua_rawseti(L, -2, idx);
			idx++;	
		}
	}

	return 1;
}

int font_families(lua_State *L) {
	QFontDatabase database;

	lua_newtable(L);
	int idx = 1;
	foreach(QString family, database.families()) {
		lua_pushstring(L, family.toStdString().data());
		lua_rawseti(L, -2, idx);
		idx++;
	}
	return 1;
}

#ifdef __cplusplus
}
#endif

#define PT_TO_PIX (10.f/13.f)

void render_font(
	int pixsize, 
	QFont *font, 
	AlloArray &mat, 
	Font::FontCharacter *chars
) {
	AlloArrayHeader h;
	h.components = 4;
	h.type = AlloUInt8Ty;
	h.dim[0] = ((int)pixsize+2)*GLYPHS_PER_ROW;
	h.dim[1] = ((int)pixsize+2)*(ASCII_SIZE/GLYPHS_PER_ROW);
	h.dimcount = 2;
	allo_array_setstride(&h, 1);
	allo_array_adapt(&mat, &h);
	
//	al::Array arr(h);
	AlloArray &arr = mat;

	QImage image(
		(unsigned char *)arr.data.ptr,
		arr.header.dim[0],
		arr.header.dim[1],
		arr.header.stride[1],
		QImage::Format_ARGB32
//		QImage::Format_RGB888
	);

	image.fill(0x000000);

	QPainter painter(&image);
	painter.setPen(Qt::white);
	QFontDatabase database;
	painter.setFont(*font);

	QFontMetrics metrics(painter.font());

	char str[2];
	str[1] = '\0';

	int ww = h.dim[0]/GLYPHS_PER_ROW;
	int hh = h.dim[1]/(ASCII_SIZE/GLYPHS_PER_ROW);

	int tw = ww-2;
	int th = hh+2;
	for(int i=0; i < ASCII_SIZE; i++) {
		str[0] = (char)i;

		//if(i>250 && i < 255) {
		//if(i==70) {
		painter.setPen(Qt::white);
		QRect bounds = metrics.boundingRect(QChar(i));
		/*printf("%c %d: %d %d %d %d %d\n", (char)i, i, 
			metrics.leftBearing(QChar(i)),
			bounds.x(), bounds.y(), bounds.width(), bounds.height()
		);
		*/

		int x = (ww)*(i%GLYPHS_PER_ROW);
		int y = (hh)*(i/(ASCII_SIZE/GLYPHS_PER_ROW));


		int tx = x+1;
		int ty = y-1;

		int y1 = ty+hh - metrics.descent() + bounds.y()+1;
		int ymid = ty+hh - metrics.descent()+1;
		int y2 = y1+bounds.height();

		int shift = 0;
		if(y2 > y+hh-2) {
			shift = (y+hh-2)-y2;
		}
		else if(y1 < ty+2) {
			shift = (ty+2)-y1;
		}
		
		ty += shift;
		y1 += shift;
		y2 += shift;
		ymid += shift;
		

		chars[i].width = bounds.width();
		chars[i].y_offset = shift+metrics.descent();

		painter.drawText(
			QRect((float)tx, (float)ty, (float)tw, (float)th), 
			Qt::AlignLeft, 
			QString(str)
		);

		
		/*

		painter.setPen(Qt::blue);

		
		painter.drawLine(
			bounds.x() + tx, y1, 
			bounds.x() + tx + bounds.width(), y1
		);

		painter.setPen(Qt::red);
		painter.drawLine(
			bounds.x() + x+1, y2, 
			bounds.x() + x+1 + bounds.width(), y2
		);
		painter.setPen(Qt::blue);*/
		/*painter.drawLine(
			bounds.x() + x+1, 0,
			bounds.x() + x+1, h.dim[1]
		);

		painter.drawLine(
			bounds.width() + bounds.x() + x+1, 0,
			bounds.width() + bounds.x() + x+1, h.dim[1]
		);
		*/
		/*painter.setPen(Qt::red);
		painter.drawLine(0, y, h.dim[0], y);
		painter.drawLine(x, 0, x, h.dim[1]);
		
		
		painter.drawLine(0, hh+y, h.dim[0], hh+y);
		painter.drawLine(ww+x, 0, ww+x, h.dim[1]);
*/
		/*painter.setPen(Qt::yellow);
		painter.drawLine(
			tx, ymid, 
			tx+tw, ymid
		);*/
		//}
	}

	

	painter.end();

/*	h.components = 1;
	allo_array_setstride(&h, 1);
	allo_array_adapt(&mat, &h);
	
	int pincr = arr.header.stride[0];
	for(int j=0; j < arr.header.dim[1]; j++) {
		char *src = arr.data.ptr + j*arr.header.stride[1];
		char *dst = mat.data.ptr + j*mat.header.stride[1];
		
		for(int i=0; i < arr.header.dim[0]; i++) {
			*dst++ = src[1];
			src += pincr;
		}
	}
*/
}


class FontQtImpl{
public:
	FontQtImpl(QFont *font)
	:	font(font),
		metrics(*font)
	{}

	~FontQtImpl() {
		delete font;
	}

	QFont *font;
	QFontMetrics metrics;
};


FontQt :: FontQt(FontQtImpl *impl, int font_size, bool anti_aliased)
:	Font(font_size, anti_aliased),
	mImpl(impl)
{}

FontQt :: ~FontQt() {
	delete mImpl;
}

FontQtImpl * FontQt :: create_font(const char *name, int size) {
	QFontDatabase database;
	char ename[256];
	strcpy(ename, name);
	char *dash = strchr(ename, '-');
	if(dash) {
		dash[0] = '\0';	
		QString family(ename);
		QString style(dash+1);
		QFont font = database.font(
			family, 
			style, 
			(int)(((float)size)*PT_TO_PIX)
		);
		return new FontQtImpl(new QFont(font));
	}
	else {
		return NULL;
	}
}
	
al::ArrayWrapper * FontQt :: get_ascii_chars() {
	if(!mAsciiChars) {
		mAsciiChars = new al::ArrayWrapper(1);
		render_font(mFontSize, mImpl->font, *mAsciiChars, mChars);
	}
	return mAsciiChars;
}

float FontQt :: width(const char *text) {
	return 0.;
}

float FontQt :: get_ascender() {
	return (float)mImpl->metrics.ascent();
}
	
float FontQt :: get_descender() {
	return (float)mImpl->metrics.descent();
}

int FontQt :: outline(int idx, AlloArray *vertex, AlloArray *index) {
	return 0;
}
