#ifndef LUA_IMAGE_IMAGEIOIMPL_H
#define LUA_IMAGE_IMAGEIOIMPL_H 1

#include "Image.h"
#include "ImageImpl.h"
#include <ApplicationServices/ApplicationServices.h>

namespace image {

class ImageIOImpl : public ImageImpl {
public:
	ImageIOImpl();
	virtual ~ImageIOImpl();
	
	virtual ImageError load(const char *filename, AlloArray &mat);
	virtual ImageError save(const char *filename, AlloArray &mat);

protected:
	void render_image(AlloArray &mat);
	void get_image_ref(const char *filename);

protected:
	CGImageRef	mImage;
};


} // image::

#endif // LUA_IMAGE_IMAGEIOIMPL_H