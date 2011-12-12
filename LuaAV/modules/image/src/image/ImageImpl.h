#ifndef LUA_IMAGE_IMAGEIMPL_H
#define LUA_IMAGE_IMAGEIMPL_H 1

#include "Image.h"

namespace image {

class ImageImpl {
public:
	/*!
		Different Image file I/O library backends available
	*/
	enum Type {
		FREEIMAGE = 0,	///< FreeImage backend
		OPENEXR,		///< OpenEXR backend
		RGB,			///< RGB backend
		IMAGEIO,		///< OSX ImageIO backend
	};

protected:
	ImageImpl(Type type);
	virtual ~ImageImpl();
	
public:
	static ImageImpl * create(const char *filename, bool loading);
	static void destroy(ImageImpl *impl);
	
	virtual ImageError load(const char *filename, AlloArray &mat) = 0;
	virtual ImageError save(const char *filename, AlloArray &mat) = 0;
	
	Type type() {return mType;}
	static Type type_for_filename(const char *filename, bool loading);
	
protected:
	Type	mType;		///< Backend Type
};

}	// image::

#endif // LUA_IMAGE_IMAGEIMPL_H
