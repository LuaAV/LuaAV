#ifndef VIDEO_VIDEO_VIDEOCAMERA_H
#define VIDEO_VIDEO_VIDEOCAMERA_H 1

#include "allocore/types/al_Array.hpp"
#include "video_common.h"
#include <string>

using std::string;

namespace video {

class VideoCameraImpl;

class VideoCamera {
private:
	friend class VideoCameraImpl;
	
	typedef VideoCamera		Self;
	typedef VideoCameraImpl	Implementation;

public:
	
	VideoCamera();
	virtual ~VideoCamera();

	VideoError open(int w, int h);
	VideoError close();
	VideoError settings();
	
	VideoError next_frame();
	al::ArrayWrapper * wrapper() {return mFrame;}
	
	void set_pixel_format(PixelFormat fmt);
	PixelFormat get_pixel_format() {return mFormat;}

protected:
	VideoError make_impl();

protected:
	Implementation			*mImpl;		///< Implementation (library binding)
	al::ArrayWrapper		*mFrame;	///< Current video frame
	PixelFormat				mFormat;	///< Pixel format
	int						mWidth;
	int						mHeight;
	bool					mOpen;
};

}	// video::

#endif	// VIDEO_VIDEO_VIDEOCAMERA_H
