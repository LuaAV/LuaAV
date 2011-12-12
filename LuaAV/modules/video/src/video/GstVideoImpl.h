#ifndef VIDEO_GST_VIDEO_IMPL_H
#define VIDEO_GST_VIDEO_IMPL_H 1

#include "VideoImpl.h"
#include <string>

#include "GstUtils.h"

using std::string;

class ofVideoPlayer;

namespace video {

class GstVideoImpl : public VideoImpl {
public:
	GstVideoImpl();
	virtual ~GstVideoImpl();

	void destroy();
	
	VideoError start();
	void loop();
	
	static void new_buffer(GstElement *e, GstVideoImpl *gst);
	void fromBuffer(const char *data, guint size);
	void updateTime(GstClockTime now);

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

	void handleMessages();
	
	// gstreamer pipeline elements
	GstElement *mPipeline;
	GstElement *mVideoSink;
	GstElement *mAudioSink;
	
	// video properties
	gint64 mDuration;
	float mDurationSeconds;
	gint mWidth;
	gint mHeight;
	gint64 mNFrames;
	float mFps;
	double mNanoPerFrame;

	// video data
	AlloArray *mFrameData;

	// playback state
	bool mPaused;

	// playback properties
	bool mNewPixels;
	float mSpeed;
	int mFrame;
	float mPosition;
};

}	// video::

#endif	// VIDEO_GST_VIDEO_IMPL_H
