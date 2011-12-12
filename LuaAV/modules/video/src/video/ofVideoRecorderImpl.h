#ifndef OF_VIDEO_RECORDER_IMPL_H
#define OF_VIDEO_RECORDER_IMPL_H 1

#include "VideoRecorderImpl.h"
#include "ofxQtVideoSaver.h"

namespace video {

class ofVideoRecorderImpl : public VideoRecorderImpl {
public:

	ofVideoRecorderImpl();
	virtual ~ofVideoRecorderImpl();
	
	VideoError open(const char *filename, int w, int h);
	VideoError close();
	VideoError save_frame(AlloArray *m);

protected:
	ofxQtVideoSaver	mVideoSaver;

};

} // video::

#endif // OF_VIDEO_RECORDER_IMPL_H