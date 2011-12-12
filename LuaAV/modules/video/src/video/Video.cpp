#include "Video.h"
#include "VideoImpl.h"

namespace video {


Video :: Video()
:	mImpl(NULL),
	mFrame(NULL)
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

Video :: ~Video() {
	if(mImpl) {
		Implementation::destroy(mImpl);
		mImpl = NULL;
	}
	mFrame->release();
}

VideoError Video :: make_impl(const char *filename, bool loading) {
	VideoError err = VIDEO_ERROR_NONE;
	
	// check if we need to switch Implementations
	if(mImpl) {
		if(Implementation::type_for_filename(filename, loading) != mImpl->type()) {
			Implementation::destroy(mImpl);
			mImpl = NULL;
		}
	}
	
	// create Implementation if necessary
	if(!mImpl) {
		mImpl = Implementation::create(filename, loading);
	}
	if(!mImpl) {
		return VIDEO_ERROR_GENERIC;
	}
	return err;
}

bool Video :: open(const char *filename) {
	VideoError err = VIDEO_ERROR_NONE;

	err = make_impl(filename, true);
	if(err) return false;
	
	err = mImpl->open(filename);
	if(err) return false;
	
	return true;
}


void Video :: close() {
	if(mImpl) {
		mImpl->close();
	}
}

void Video :: play() {
	if(mImpl) mImpl->play();
}

void Video :: stop() {
	if(mImpl) mImpl->stop();
}

void Video :: pause(bool b) {
	if(mImpl) mImpl->pause(b);
}

void Video :: next_frame() {
	if(mImpl) mImpl->next_frame(mFrame);
}

void Video :: set_position(float pct) {
	if(mImpl) mImpl->set_position(pct);
}

float Video :: get_position() {
	if(mImpl) return mImpl->get_position();
	else return 0.;
}

void Video :: set_speed(float speed) {
	if(mImpl) mImpl->set_speed(speed);
}

float Video :: get_speed() {
	if(mImpl) return mImpl->get_speed();
	else return 0.;
}

void Video :: set_frame(int f) {
	if(mImpl) mImpl->set_frame(f);
}

int Video :: get_frame() {
	if(mImpl) return mImpl->get_frame();
	else return 0;
}

int Video :: get_nframes() {
	if(mImpl) return mImpl->get_nframes();
	else return 0;
}

float Video :: get_duration() {
	if(mImpl) return mImpl->get_duration();
	else return 0.;
}

void Video :: get_dim(int &w, int &h) {
	if(mImpl) {
		mImpl->get_dim(w, h);
	}
	else {
		w = h = 0;
	}
}

}	// video::
