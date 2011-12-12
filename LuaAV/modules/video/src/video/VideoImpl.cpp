#include "VideoImpl.h"

#ifdef VIDEO_USE_OF
#include "ofVideoImpl.h"
#endif

#ifdef VIDEO_USE_FFMPEG
#include "FFMPEGVideoImpl.h"
#endif

#ifdef VIDEO_USE_GST
#include "GstVideoImpl.h"
#endif

#if defined (__APPLE__) || defined (OSX)
#include "QtKitVideoImpl.h"
#endif

#include <stdio.h>

namespace video {

VideoImpl :: VideoImpl(Type type)
:	mType(type)
{}

VideoImpl :: ~VideoImpl()
{}
	
VideoImpl * VideoImpl :: create(const char *filename, bool loading) {
	VideoImpl *impl = NULL;
	
//	impl = new QtKitVideoImpl();
//	if(impl) return impl;
	
#ifdef VIDEO_USE_FFMPEG
	impl = new FFMPEGVideoImpl();
#elif VIDEO_USE_GST
	impl = new GstVideoImpl();
#elif VIDEO_USE_OF
	impl = new ofVideoImpl();
#else
	fprintf(stderr, "no video for muro enabled\n");
#endif
	return impl;
}

void VideoImpl :: destroy(VideoImpl *impl) {
	delete impl;
}

VideoImpl::Type VideoImpl :: type_for_filename(const char *filename, bool loading) {
#ifdef VIDEO_USE_FFMPEG
	return FFMPEG;
#elif VIDEO_USE_GST
	return GST;
#elif VIDEO_USE_OF
	return OF;
#else
	return OF;
#endif
}

}	// video::

