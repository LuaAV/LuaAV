#ifndef LUA_VIDEO_VIDEO_H
#define LUA_VIDEO_VIDEO_H 1

#include "allocore/types/al_Array.hpp"
#include "video_common.h"
#include <string>

using std::string;

namespace video {

class VideoImpl;


class Video {
private:
	friend class VideoImpl;
	
	typedef Video		Self;
	typedef VideoImpl	Implementation;

public:
	Video();
	virtual ~Video();

	bool open(const char *filename);

	void close();
	
	void play();
	void stop();
	void pause(bool b);
	void next_frame();
	
	// attrs
	void set_position(float pct);
	void set_speed(float speed);
	void set_frame(int f);
	
	float get_position();
	float get_speed();
	int get_frame();
	int get_nframes();
	float get_duration();
	
	void get_dim(int &w, int &h);
	
	
	al::ArrayWrapper * wrapper() {return mFrame;}

protected:
	VideoError make_impl(const char *filename, bool loading);

protected:
	Implementation		*mImpl;		///< Implementation (library binding)
	al::ArrayWrapper	*mFrame;	///< Current video frame
	string				mFilename;
};

}	// video::

#endif	// LUA_VIDEO_VIDEO_H
