#ifndef LUA_VIDEO_OFVIDEOCAMERAIMPL_H
#define LUA_VIDEO_OFVIDEOCAMERAIMPL_H 1

#include "VideoCameraImpl.h"

class ofVideoGrabber;

namespace video {

class ofVideoCameraImpl:  public VideoCameraImpl {
public:
	ofVideoCameraImpl();
	virtual ~ofVideoCameraImpl();
	
	virtual VideoError open(int w, int h, video::PixelFormat);

	virtual VideoError close();
	virtual VideoError settings();

	virtual VideoError next_frame(AlloArray *mat);
	
protected:
	ofVideoGrabber	*mVideoGrabber;		///< OF vide grabber object
	bool			mOpen;
};

}	// video::

#endif // LUA_VIDEO_OFVIDEOCAMERAIMPL_H

