#include "FFMPEGVideoCameraImpl.h"

namespace muro {

FFMPEGVideoCameraImpl :: FFMPEGVideoCameraImpl()
:	VideoCameraImpl(VideoCameraImpl::FFMPEG)
{

}

FFMPEGVideoCameraImpl :: ~FFMPEGVideoCameraImpl() {
	close();
}

MuroError FFMPEGVideoCameraImpl :: open(int w, int h) {
	MuroError err = MURO_ERROR_NONE;
	/*
	// destroy previously open file if necessary
	destroy();

	// Open video file (last 3 params do autodetect of (AVInputFormat *fmt,int buf_size, AVFormatParameters *ap)
	if(av_open_input_stream(&mAVformatCtx, filename, NULL, 0, NULL) != 0) {
		return muro::setError(MURO_ERROR_GENERIC, "FFMPEGImpl.open: unable to open file");
	}

	// Retrieve stream information
	if(av_find_stream_info(mAVformatCtx) < 0) {
		return muro::setError(MURO_ERROR_GENERIC, "FFMPEGImpl.open: couldn't find stream information");
	}

	printInfo(filename);
	*/
	return err;
}

MuroError FFMPEGVideoCameraImpl :: close() {
	MuroError err = MURO_ERROR_NONE;

	return err;
}

MuroError FFMPEGVideoCameraImpl :: nextFrame(Matrix *mat) {
	MuroError err = MURO_ERROR_NONE;

	return err;
}


}	// muro::

