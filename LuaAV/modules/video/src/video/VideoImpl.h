#ifndef LUA_VIDEO_VIDEOIMPL_H
#define LUA_VIDEO_VIDEOIMPL_H 1

#include "Video.h"

namespace video {

class VideoImpl {
public:
	
	enum Type {
		FFMPEG = 0,	///< ffmpeg backend
		QT,			///< Quicktime backend
		GST,		///< gstreamer backend
		OF,			///< OpenFrameworks backend
		GMERLIN,	///< gmerlin backend
		QTKIT		///< QtKit backend
	};

protected:
	VideoImpl(Type type);
	virtual ~VideoImpl();
	
public:
	static VideoImpl * create(const char *filename, bool loading);
	static void destroy(VideoImpl *impl);
	
	virtual VideoError open(const char *filename) = 0;
	virtual VideoError close() = 0;
	
	virtual VideoError play() = 0;
	virtual VideoError next_frame(AlloArray *mat) = 0;
	
	virtual VideoError stop() = 0;
	
	virtual VideoError pause(bool b) = 0;
	
	virtual void set_position(float pct) = 0;
	virtual void set_speed(float speed) = 0;
	virtual void set_frame(int f) = 0;
	
	virtual float get_position() = 0;
	virtual float get_speed() = 0;
	virtual int get_frame() = 0;
	virtual int get_nframes() = 0;
	virtual float get_duration() = 0;
	virtual void get_dim(int &w, int &h) = 0;
	
	Type type() {return mType;}
	static Type type_for_filename(const char *filename, bool loading);
	
protected:
	Type	mType;		///< Backend Type
};

}	// video::

#endif // LUA_VIDEO_VIDEOIMPL_H
