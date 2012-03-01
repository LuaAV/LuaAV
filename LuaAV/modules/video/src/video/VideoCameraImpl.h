#ifndef LUA_VIDEO_VIDEOCAMERAIMPL_H
#define LUA_VIDEO_VIDEOCAMERAIMPL_H 1

#include "VideoCamera.h"

namespace video {

class VideoCameraImpl {
public:
	/*!
		Different Image file I/O library backends available
	*/
	enum Type {
		FFMPEG = 0,	///< ffmpeg backend
//		QT,			///< Quicktime backend
		OF,			///< OpenFrameworks backend
		QTKIT		///< QtKit Backend
	};

protected:
	VideoCameraImpl(Type type);
	virtual ~VideoCameraImpl();
	
public:
	static VideoCameraImpl * create();
	static void destroy(VideoCameraImpl *impl);
	
	virtual VideoError open(int w, int h, PixelFormat fmt) = 0;
	virtual VideoError close() = 0;
	virtual VideoError settings() = 0;

	virtual VideoError next_frame(AlloArray *mat) = 0;
	
#if defined (__APPLE__) || defined (OSX)
	static VideoCameraImpl * create(const char *uid);
	static void list_devices(vector<Device> &devices);
#endif
	
	Type type() {return mType;}
	
protected:
	Type	mType;		///< Backend Type
};

}	// video::

#endif // LUA_VIDEO_VIDEOCAMERAIMPL_H

