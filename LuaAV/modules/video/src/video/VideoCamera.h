#ifndef VIDEO_VIDEO_VIDEOCAMERA_H
#define VIDEO_VIDEO_VIDEOCAMERA_H 1

#include "allocore/types/al_Array.hpp"
#include "video_common.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace video {


class Device{
public:
	string name;
	string uid;
};


class VideoCameraImpl;

class VideoCamera {
private:
	friend class VideoCameraImpl;
	
	typedef VideoCamera		Self;
	typedef VideoCameraImpl	Implementation;

public:
	
	VideoCamera();
	virtual ~VideoCamera();

#if defined (__APPLE__) || defined (OSX)
	VideoError open(const char *uid, int w, int h);
#endif
	VideoError open(int w, int h);
	VideoError close();
	VideoError settings();
	
	VideoError next_frame();
	al::ArrayWrapper * wrapper() {return mFrame;}
	
	void set_pixel_format(PixelFormat fmt);
	PixelFormat get_pixel_format() {return mFormat;}
	static VideoError list_devices(vector<Device> &devices);

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
