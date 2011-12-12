#include "ofVideoCameraImpl.h"
#include "ofVideoGrabber.h"

namespace video {

ofVideoCameraImpl :: ofVideoCameraImpl()
:	VideoCameraImpl(VideoCameraImpl::OF),
	mVideoGrabber(NULL),
	mOpen(false)
{
	mVideoGrabber = new ofVideoGrabber();
}

ofVideoCameraImpl :: ~ofVideoCameraImpl() {
	close();
	if(mVideoGrabber) delete mVideoGrabber;
}
	
VideoError ofVideoCameraImpl :: open(int w, int h, video::PixelFormat) {
	VideoError err = VIDEO_ERROR_NONE;
	
	w = (w <= 0) ? 1 : w;	//set min val
	h = (h <= 0) ? 1 : h;	//set min val

	bool isok = mVideoGrabber->initGrabber(w, h);
	if(!isok) {
		err = VIDEO_ERROR_GENERIC;
		mOpen = false;
	}
	else {
		mOpen = true;
	}
	return err;
}

VideoError ofVideoCameraImpl :: settings() {
	VideoError err = VIDEO_ERROR_NONE;
	mVideoGrabber->videoSettings();
	return err;
}


VideoError ofVideoCameraImpl :: close() {
	VideoError err = VIDEO_ERROR_NONE;
	if(mOpen) {
		mVideoGrabber->close();
	}
	mOpen = false;
	return err;
}

VideoError ofVideoCameraImpl :: next_frame(AlloArray *mat) {
	VideoError err = VIDEO_ERROR_NONE;
	//mVideoGrabber->grabFrame(mat);
	mVideoGrabber->grabFrame();
	if(mVideoGrabber->isFrameNew()) {
		AlloArray array;
		array.header.components = 3;
		array.header.type = AlloUInt8Ty;
		array.header.dimcount = 2;
		array.header.dim[0] = mVideoGrabber->width;
		array.header.dim[1] = mVideoGrabber->height;
		allo_array_setstride(&(array.header), 1);
		array.data.ptr = (char *)mVideoGrabber->getPixels();
		
		allo_array_copy(mat, &array);
	}
	
	return err;
}

}	// video::
