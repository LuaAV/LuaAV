#ifndef LUA_VIDEO_QTKITVIDEOIMPL_H
#define LUA_VIDEO_QTKITVIDEOIMPL_H 1

#include "Video.h"
#include "VideoImpl.h"



namespace video {

class QtKitVideoImpl : public VideoImpl {
public:
	QtKitVideoImpl();
	virtual ~QtKitVideoImpl();
	
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
	void *mVideo;	///< QtKit video
	float mSpeed;	///< Playback speed
};

}	// video::

#endif	// LUA_VIDEO_QTKITVIDEOIMPL_H
