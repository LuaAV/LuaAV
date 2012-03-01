#include "VideoCamera.h"
#include "VideoCameraImpl.h"

namespace video {

VideoCamera :: VideoCamera()
:	mImpl(NULL),
	mFrame(NULL),
	mFormat(PIX_FMT_RGB),
	mWidth(1),
	mHeight(1),
	mOpen(false)
{
	mFrame = new al::ArrayWrapper(1);
	// default header
	AlloArrayHeader h;
	h.components = 4;
	h.type = AlloUInt8Ty;
	h.dimcount = 2;
	h.dim[0] = 16;
	h.dim[1] = 16;
	allo_array_setstride(&h, 1);
	allo_array_adapt(mFrame, &h);
	memset(mFrame->data.ptr, '\0', allo_array_size(mFrame));
}

VideoCamera :: ~VideoCamera() {
	if(mImpl) {
		Implementation::destroy(mImpl);
		mImpl = NULL;
	}
	
	mFrame->release();
}

VideoError VideoCamera :: make_impl() {
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

#if defined (__APPLE__) || defined (OSX)
VideoError VideoCamera :: open(const char *uid, int w, int h) {
	VideoError err = VIDEO_ERROR_NONE;
	
	mWidth = w;
	mHeight = h;
	
	// create Implementation
	if(!mImpl) {
		mImpl = Implementation::create(uid);
	}
	if(!mImpl) {
		return VIDEO_ERROR_MEMORY;
	}
	
	err = mImpl->open(w, h, mFormat);
	mOpen = true;
	return err;
}
#endif

VideoError VideoCamera :: open(int w, int h) {
	VideoError err = VIDEO_ERROR_NONE;
	
	mWidth = w;
	mHeight = h;
	
	err = make_impl();
	if(err) return err;
	
	err = mImpl->open(w, h, mFormat);
	mOpen = true;
	return err;
}

VideoError VideoCamera :: settings() {
	VideoError err = VIDEO_ERROR_NONE;
	err = mImpl->settings();
	return err;
}

VideoError VideoCamera :: close() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mImpl) {
		err = mImpl->close();
		mOpen = false;
	}
	return err;
}

VideoError VideoCamera :: next_frame() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mImpl) {
		err = mImpl->next_frame(mFrame);
	}
	return err;
}

void VideoCamera :: set_pixel_format(PixelFormat fmt) {
	if(mFormat != fmt) {
		mFormat = fmt;
		
		if(mOpen) {
			close();
			open(mWidth, mHeight);
		}
	}
}

VideoError VideoCamera :: list_devices(vector<Device> &devices) {
	VideoError err = VIDEO_ERROR_NONE;
#if defined (__APPLE__) || defined (OSX)
	VideoCameraImpl::list_devices(devices);
#endif
	return err;
}
	
}	// video::
