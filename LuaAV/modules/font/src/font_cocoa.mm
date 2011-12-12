#import "font_cocoa.h"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include "lua_array.h"
#include "allocore/types/al_Array.hpp"

void render_font(NSFont *font, AlloArray &mat, Font::FontCharacter *chars);

#ifdef __cplusplus
extern "C" {
#endif

int font_names(lua_State *L) {
	NSFontManager *font_manager = [NSFontManager sharedFontManager];
	NSArray *names = [font_manager availableFonts];
	
	lua_newtable(L);
	for(int i=0; i < [names count]; i++) {
		NSString *name = [names objectAtIndex:i];
		const char *cname = [name cStringUsingEncoding: NSASCIIStringEncoding];
		lua_pushstring(L, cname);
		lua_rawseti(L, -2, i+1);
	}
	
	return 1;
}

int font_families(lua_State *L) {
	NSFontManager *font_manager = [NSFontManager sharedFontManager];
	NSArray *names = [font_manager availableFontFamilies];
	
	lua_newtable(L);
	for(int i=0; i < [names count]; i++) {
		NSString *name = [names objectAtIndex:i];
		const char *cname = [name cStringUsingEncoding: NSASCIIStringEncoding];
		lua_pushstring(L, cname);
		lua_rawseti(L, -2, i+1);
	}
	
	return 1;
}

/*
int font_test(lua_State *L) {
	AlloArray *mat = lua_array_checkto(L, 3);
	if(
		lua::is<const char *>(L, 1) && 
		lua::is<int>(L, 2) &&
		mat
	) {
		NSString *font_name = [NSString stringWithCString:lua::to<const char *>(L, 1) encoding:NSASCIIStringEncoding];
		int size = lua::to<int>(L, 2);
		
		NSFont *font = [NSFont fontWithName:font_name size:(CGFloat)size];
		if(font) {
			Font::FontCharacter chars[ASCII_SIZE];
			render_font(font, *mat, chars);
		}
	}
	return 0;
}
*/

#ifdef __cplusplus
}
#endif

class FontCocoaImpl {
public:
	
	FontCocoaImpl(NSFont *font)
	:	font(font)
	{}
	
	~FontCocoaImpl() {}
	
	
	NSFont *font;
};

FontCocoa :: FontCocoa(FontCocoaImpl *impl, int font_size, bool anti_aliased)
:	Font(font_size, anti_aliased),
	mImpl(impl)
{}

FontCocoa :: ~FontCocoa() {
	delete mImpl;
}

FontCocoaImpl * FontCocoa::create_font(const char *name, int size) {
	NSString *font_name = [NSString stringWithCString:name encoding:NSASCIIStringEncoding];
	NSFont *font = [NSFont fontWithName:font_name size:(CGFloat)size];
	if(font) {
		return new FontCocoaImpl(font);
	}
	else {
		return 0;
	}
}

NSGraphicsContext * create_rendering_context(AlloArray &mat) {
	unsigned char *planes[] = {(unsigned char *)mat.data.ptr};
	NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc]
		initWithBitmapDataPlanes: planes
		pixelsWide: mat.header.dim[0]
		pixelsHigh: mat.header.dim[1]
		bitsPerSample: 8*allo_type_size(mat.header.type)
		samplesPerPixel: 3//mat.header.components
		hasAlpha: NO
		isPlanar: NO
		colorSpaceName: NSCalibratedRGBColorSpace
		bytesPerRow: mat.header.stride[1]
		bitsPerPixel: 8*mat.header.stride[0] 
	];
	
	NSGraphicsContext *ctx = [NSGraphicsContext graphicsContextWithBitmapImageRep:bitmap];
	[bitmap release];
	return ctx;
}


#define ASCII_SIZE 256	// number of characters to use
#define GLYPHS_PER_ROW 16	// number of glyphs to pack in a row

void render_font(NSFont *font, AlloArray &mat, Font::FontCharacter *chars) {
	CGFloat size = [font pointSize];
//	CGFloat descender = [font descender];

	AlloArrayHeader h;
	h.components = 4;
	h.type = AlloUInt8Ty;
	h.dim[0] = ((int)size+2)*GLYPHS_PER_ROW;
	h.dim[1] = ((int)size+2)*(ASCII_SIZE/GLYPHS_PER_ROW);
	h.dimcount = 2;
	allo_array_setstride(&h, 1);
	
	al::Array arr(h);
	NSGraphicsContext *ctx = create_rendering_context(arr);
	if(!ctx) return;
	
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:ctx];

	
	char letter[2];
	letter[1] = '\0';
	
	NSDictionary *attrs = [NSDictionary
		dictionaryWithObjectsAndKeys:
			[NSColor whiteColor], NSForegroundColorAttributeName, 
			font, NSFontAttributeName, 
			nil
	];
	
	NSAffineTransform *xform = [NSAffineTransform transform];
//	[xform translateXBy:0. yBy:mat.header.dim[1]];
//	[xform scaleXBy:1. yBy:-1.];
	[xform concat];
	
	NSLayoutManager *layout = [[NSLayoutManager alloc] init];
	NSTextContainer *textContainer = [[NSTextContainer alloc] init];
	[layout addTextContainer:textContainer];
	[textContainer release];
	[layout setUsesScreenFonts:NO];
	
	[[NSColor whiteColor] set];
	for(int i=0; i < ASCII_SIZE; i++) {
		letter[0] = i;
		
		
		float x = (float)((size+2)*(i % GLYPHS_PER_ROW));
		float y = (float)(arr.header.dim[1] - (size+2)*(1+i/(ASCII_SIZE/GLYPHS_PER_ROW)));
		//float y = (float)((size+2)*(i/(ASCII_SIZE/GLYPHS_PER_ROW)));
		//printf("y: %d %f\n", i, y);
		
		
		NSString *text = [NSString stringWithCString:letter encoding:NSASCIIStringEncoding];
		NSTextStorage *storage = [[NSTextStorage alloc] initWithString:text];
		[storage setFont:font];
		[storage addLayoutManager:layout];
		
		
		
		NSRange grange = [layout glyphRangeForTextContainer:textContainer];
		NSGlyph g = (NSGlyph)i;
		if(grange.length >= 1) {
			g = [layout glyphAtIndex:0];
		}
		[storage removeLayoutManager:layout];
		
		NSRect rect = [font boundingRectForGlyph:g];
		float offset = -rect.origin.y;
		[text 
			drawWithRect:NSMakeRect(x+1, y+1+offset, 0, 0) 
			options:NSStringDrawingUsesFontLeading|NSStringDrawingDisableScreenFontSubstitution
			attributes:attrs
		];
		/*
		rect.origin.x += x+1;
		rect.origin.y += y+1+offset;
		[NSBezierPath strokeRect:rect];
		*/
		
		NSSize size = [font advancementForGlyph:g];
		chars[i].width = (int)size.width;
		chars[i].y_offset = (int)-offset;
	}
	
	[layout release];
	
	[NSGraphicsContext restoreGraphicsState];
	
	h.components = 1;
	allo_array_setstride(&h, 1);
	allo_array_adapt(&mat, &h);
	
	int pincr = arr.header.stride[0];
	for(int j=0; j < arr.header.dim[1]; j++) {
		char *src = arr.data.ptr + j*arr.header.stride[1];
		char *dst = mat.data.ptr + j*mat.header.stride[1];
		
		for(int i=0; i < arr.header.dim[0]; i++) {
			*dst++ = src[0];
			src += pincr;
		}
	}
}

al::ArrayWrapper * FontCocoa :: get_ascii_chars() {
	if(! mAsciiChars) {
		mAsciiChars = new al::ArrayWrapper(1);
		render_font(mImpl->font, *mAsciiChars, mChars);
	}
	return mAsciiChars;
}

float FontCocoa :: width(const char *text) {
	int w = 0;
	int len = strlen(text);
	for(int i=0; i < len; i++) {
		w += mChars[ text[i] ].width;
	}
	return w;
}

float FontCocoa :: get_ascender() {
	return [mImpl->font ascender];
}

float FontCocoa :: get_descender() {
	return [mImpl->font descender];
}

int FontCocoa :: outline(int idx, AlloArray *vertex, AlloArray *index) {
	return 0;
}