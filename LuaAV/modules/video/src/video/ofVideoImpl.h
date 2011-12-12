#ifndef LUA_VIDEO_OFVIDEOIMPL_H
#define LUA_VIDEO_OFVIDEOIMPL_H 1

#include "Video.h"
#include "VideoImpl.h"
#include <string>

using std::string;

class ofVideoPlayer;

namespace video {

class ofVideoImpl : public VideoImpl {
public:
	ofVideoImpl();
	virtual ~ofVideoImpl();
	
	virtual VideoError open(const char *filename);
	virtual VideoError close();
	
	virtual VideoError play();
	virtual VideoError next_frame(AlloArray *mat);

	virtual VideoError stop();
	virtual VideoError pause(bool b);
	
	virtual void set_position(float pct);
	virtual void set_speed(float speed);
	virtual void set_frame(int f);
	
	virtual float get_position();
	virtual float get_speed();
	virtual int get_frame();
	virtual int get_nframes();
	virtual float get_duration();
	virtual void get_dim(int &w, int &h);

protected:
	ofVideoPlayer	*mVideoPlayer;	///< OF video player object
};

}	// video::

#endif	// LUA_VIDEO_OFVIDEOIMPL_H
