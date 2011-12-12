#include "ofVideoImpl.h"
#include "ofVideoPlayer.h"

namespace video {

ofVideoImpl :: ofVideoImpl()
:	VideoImpl(VideoImpl::OF),
	mVideoPlayer(NULL)
{
	mVideoPlayer = new ofVideoPlayer();
}

ofVideoImpl :: ~ofVideoImpl() {
	close();
	delete mVideoPlayer;
}

VideoError ofVideoImpl :: open(const char *filename) {
	VideoError err = VIDEO_ERROR_NONE;

	bool success = mVideoPlayer->loadMovie(filename);
	if(! success) {
		return VIDEO_ERROR_GENERIC;
	}
	
	return err;
}

VideoError ofVideoImpl :: close() {
	VideoError err = VIDEO_ERROR_NONE;
	mVideoPlayer->closeMovie();
	return err;
}

VideoError ofVideoImpl :: play() {
	VideoError err = VIDEO_ERROR_NONE;
	mVideoPlayer->play();
	mVideoPlayer->idleMovie();
	return err;
}

VideoError ofVideoImpl :: next_frame(AlloArray *mat) {
	VideoError err = VIDEO_ERROR_NONE;

	mVideoPlayer->idleMovie();

	AlloArray array;
	array.header.components = 3;
	array.header.type = AlloUInt8Ty;
	array.header.dimcount = 2;
	array.header.dim[0] = mVideoPlayer->width;
	array.header.dim[1] = mVideoPlayer->height;
	allo_array_setstride(&(array.header), 1);
	array.data.ptr = (char *)mVideoPlayer->getPixels();
	
	allo_array_copy(mat, &array);

	return err;
}

VideoError ofVideoImpl :: stop() {
	VideoError err = VIDEO_ERROR_NONE;
	mVideoPlayer->stop();
	return err;
}

VideoError ofVideoImpl :: pause(bool b) {
	VideoError err = VIDEO_ERROR_NONE;
	mVideoPlayer->setPaused(b);
	return err;
}


void ofVideoImpl :: set_position(float pct) {
	mVideoPlayer->setPosition(pct);
}

float ofVideoImpl :: get_position() {
	return mVideoPlayer->getPosition();
}
	
void ofVideoImpl :: set_speed(float speed) {
	mVideoPlayer->setSpeed(speed);
}

float ofVideoImpl :: get_speed() {
	return mVideoPlayer->getSpeed();
}

void ofVideoImpl :: set_frame(int f) {
	mVideoPlayer->setFrame(f);
}

int ofVideoImpl :: get_frame() {
	return mVideoPlayer->getCurrentFrame();
}

int ofVideoImpl :: get_nframes() {
	return mVideoPlayer->getTotalNumFrames();
}
	
float ofVideoImpl :: get_duration() {
	return mVideoPlayer->getDuration();
}

void ofVideoImpl :: get_dim(int &w, int &h) {
	w = mVideoPlayer->width;
	h = mVideoPlayer->height;
}

}	// video::
