#include "ofVideoRecorderImpl.h"
#include <string>

using std::string;


namespace video {

ofVideoRecorderImpl :: ofVideoRecorderImpl()
:	VideoRecorderImpl(VideoRecorderImpl::OF)
{}

ofVideoRecorderImpl :: ~ofVideoRecorderImpl() {}

VideoError ofVideoRecorderImpl :: open(const char *filename, int w, int h) {
	mVideoSaver.setup(w, h, string(filename));
	return VIDEO_ERROR_NONE;
}

VideoError ofVideoRecorderImpl :: close() {
	mVideoSaver.finishMovie();
	return VIDEO_ERROR_NONE;
}

VideoError ofVideoRecorderImpl :: save_frame(AlloArray *m) {
	if(
		m->header.components == 3 && 
		m->header.type == AlloUInt8Ty && 
		m->header.dimcount == 2 && 
		m->header.dim[0] == mVideoSaver.get_width() && 
		m->header.dim[1] == mVideoSaver.get_height()		
	) {
		mVideoSaver.addFrame((unsigned char *)(m->data.ptr));
	}
	else {
		return VIDEO_ERROR_GENERIC;
	}
	return VIDEO_ERROR_NONE;
}

} // video::
