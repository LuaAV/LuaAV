#include "ImageImpl.h"
#include "RGBImageImpl.h"
#include "allocore/system/al_Config.h"

#ifdef AL_OSX
#include "ImageIOImpl.h"
#else
#include "FreeImageImpl.h"
#endif

namespace image {

ImageImpl :: ImageImpl(Type type)
:	mType(type)
{}

ImageImpl :: ~ImageImpl()
{}
	
ImageImpl * ImageImpl :: create(const char *filename, bool loading) {
	ImageImpl *impl = NULL;
	switch(type_for_filename(filename, loading)) {
	
#ifdef IMAGE_USE_OPENEXR
	case OPENEXR: 
		impl = new OpenEXRImpl();
		break;
#endif

	case RGB:
		impl = new RGBImpl();
		break;

#ifdef AL_OSX
	case IMAGEIO:
	default:
		impl = new ImageIOImpl();
		break;
#else
	case FREEIMAGE: 
	default:
		impl = new FreeImageImpl();
		break;
#endif
	}
	
	return impl;
}

void ImageImpl :: destroy(ImageImpl *impl) {
	delete impl;
}

ImageImpl::Type ImageImpl :: type_for_filename(const char *filename, bool loading) {
	if(loading) {
		if(strncmp(filename+strlen(filename)-4, ".rgb", 4) == 0) {
			return RGB;
		}
#ifndef AL_OSX
		else if(strncmp(filename+strlen(filename)-4, ".exr", 4) == 0) {
			return OPENEXR;
		}
#endif

	}
#ifdef AL_OSX
	return IMAGEIO;
#else
	return FREEIMAGE;
#endif
}

}	// image::
