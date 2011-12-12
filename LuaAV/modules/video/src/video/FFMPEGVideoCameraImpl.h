#ifndef LUA_MURO_FFMPEGVIDEOCAMERAIMPL_H
#define LUA_MURO_FFMPEGVIDEOCAMERAIMPL_H 1

#include "VideoCameraImpl.h"

namespace muro {

class MURO_API FFMPEGVideoCameraImpl:  public VideoCameraImpl {
public:
	FFMPEGVideoCameraImpl();
	virtual ~FFMPEGVideoCameraImpl();
	
	virtual MuroError open(int w, int h);
	virtual MuroError close();

	virtual MuroError nextFrame(Matrix *mat);
	
protected:
//	ofVideoGrabber	*mVideoGrabber;		///< OF vide grabber object
};

}	// muro::

#endif //LUA_MURO_FFMPEGVIDEOCAMERAIMPL_H
