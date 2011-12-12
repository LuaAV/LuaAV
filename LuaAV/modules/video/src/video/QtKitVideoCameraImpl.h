#ifndef LUA_VIDEO_QTKITVIDEOCAMERAIMPL_H
#define LUA_VIDEO_QTKITVIDEOCAMERAIMPL_H 1

#include "VideoCameraImpl.h"

namespace video {

class QtKitVideoCameraImpl:  public VideoCameraImpl {
public:
	QtKitVideoCameraImpl();
	virtual ~QtKitVideoCameraImpl();
	
	virtual VideoError open(int w, int h, PixelFormat fmt);
	virtual VideoError close();
	virtual VideoError settings();

	virtual VideoError next_frame(AlloArray *mat);
	
protected:
	void	*mVideoGrabber;		///< QtKit vide grabber object
	bool	mOpen;
};

}	// video::

#endif // LUA_VIDEO_QTKITVIDEOCAMERAIMPL_H
