#include "VideoRecorderImpl.h"

#ifdef VIDEO_USE_FFMPEG_VIDEO_RECORDER
	#include "FFMPEGVideoRecorderImpl.h"
#elif defined (__APPLE__) || defined (OSX)
	#define VIDEO_USE_OF_VIDEO_RECORDER 1
	#include "ofVideoRecorderImpl.h"
#endif

namespace video {

VideoRecorderImpl :: VideoRecorderImpl(Type type)
:	mType(type)
{}

VideoRecorderImpl :: ~VideoRecorderImpl()
{}
	
VideoRecorderImpl * VideoRecorderImpl :: create() {
	VideoRecorderImpl *impl = NULL;
	#if  defined(VIDEO_USE_FFMPEG_VIDEO_RECORDER)
	impl = new FFMPEGVideoRecorderImpl();
	#elif defined(VIDEO_USE_OF_VIDEO_RECORDER)
	impl = new ofVideoRecorderImpl();
	#endif
	return impl;
}

void VideoRecorderImpl :: destroy(VideoRecorderImpl *impl) {
	delete impl;
}

}	// video::

