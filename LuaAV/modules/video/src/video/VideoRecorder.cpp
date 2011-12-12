#include "VideoRecorder.h"
#include "VideoRecorderImpl.h"

namespace video {

VideoRecorder :: VideoRecorder()
:	mImpl(NULL)
{}

VideoRecorder :: ~VideoRecorder() {
	if(mImpl) {
		Implementation::destroy(mImpl);
		mImpl = NULL;
	}
}

VideoError VideoRecorder :: make_impl() {
	VideoError err = VIDEO_ERROR_NONE;
	
	// create Implementation
	if(!mImpl) {
		mImpl = Implementation::create();
	}
	if(!mImpl) {
		return VIDEO_ERROR_MEMORY;
	}

	return err;
}

VideoError VideoRecorder :: open(const char *filename, int w, int h) {
	VideoError err = VIDEO_ERROR_NONE;
	
	err = make_impl();
	if(err) return err;
	
	err = mImpl->open(filename, w, h);
	return err;
}

VideoError VideoRecorder :: close() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mImpl) {
		err = mImpl->close();
	}
	return err;
}

VideoError VideoRecorder :: save_frame(AlloArray *m) {
	VideoError err = VIDEO_ERROR_NONE;
	if(mImpl) {
		err = mImpl->save_frame(m);
	}
	return err;
}
	
}	// video::
