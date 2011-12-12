#include "GstVideoImpl.h"
#include <cstring>

namespace video {

GstVideoImpl :: GstVideoImpl()
:	VideoImpl(VideoImpl::GST),
	mPipeline(0),
	mVideoSink(0),
	mAudioSink(0),
	mDuration(0),
	mDurationSeconds(0),
	mWidth(0),
	mHeight(0),
	mNFrames(0),
	mFps(0),
	mNanoPerFrame(0),
	mFrameData(0),
	mPaused(false),
	mNewPixels(false),
	mSpeed(1),
	mFrame(0)
{
	static bool initialized = false;
	if(!initialized) {
		gst_init(NULL, NULL);	// no argc/argv settings
		initialized = true;
	}

	mFrameData = new AlloArray();
	allo_array_clear(mFrameData);
}

GstVideoImpl :: ~GstVideoImpl() {
	close();
	destroy();
	delete mFrameData;
}

void GstVideoImpl :: destroy() {
	// kill pipeline
	gst_object_unref(mPipeline);
	mPipeline = 0;
	mVideoSink = 0;
	mAudioSink = 0;
}

VideoError GstVideoImpl :: open(const char *filename) {
	VideoError err = close();
	if(err) return err;

	printf("open\n");

	// create a new pipeline if we don't have one
	if(!mPipeline) {
		// http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-playbin.html
		mPipeline = gst_element_factory_make("playbin","player");

		// create video sink and set data format
		mVideoSink = gst_element_factory_make("appsink", NULL);
	
			// 3-channel char image
			GstCaps *caps = gst_caps_new_simple("video/x-raw-rgb", NULL);
			gst_app_sink_set_caps(GST_APP_SINK(mVideoSink), caps);
			gst_caps_unref(caps);
			gst_base_sink_set_sync(GST_BASE_SINK(mVideoSink), FALSE);
	
		g_object_set(G_OBJECT(mPipeline),"video-sink", mVideoSink, (void*)NULL);

		// create audio sink
	//	mAudioSink = gst_element_factory_make("gconfaudiosink", NULL);
	//	g_object_set(G_OBJECT(mPipeline), "audio-sink", mAudioSink,(void*)NULL);
	}

	// make filename into a valid URI
	std::string uri("");
	if(! strstr(filename, "://")) {
		uri.assign("file://");
	}
	uri.append(filename);

	g_object_set(G_OBJECT(mPipeline), "uri", uri.data(), (void*)NULL);

	err = start();
	return err;
}

VideoError GstVideoImpl :: start() {
	printf("start\n");

	VideoError err = VIDEO_ERROR_NONE;

	// start-up pipeline
	if(gst_element_set_state(GST_ELEMENT(mPipeline), GST_STATE_PAUSED) == 
			GST_STATE_CHANGE_FAILURE)
	{
		close();
		return VIDEO_ERROR_GENERIC;
	}

	// set callbacks
	g_object_set(G_OBJECT(mVideoSink), "emit-signals", TRUE, "sync", TRUE, NULL);
	g_signal_connect(mVideoSink, "new-buffer", 
						G_CALLBACK(new_buffer), 
						(gpointer)this);

	// verify pipeline start
	GstState state = GST_STATE_PAUSED;
	GstStateChangeReturn change = gst_element_get_state(mPipeline, 
														&state, NULL, 
														2*GST_SECOND);

	if(change == GST_STATE_CHANGE_FAILURE) {
		close();
		return VIDEO_ERROR_GENERIC;
	}

	// query video duration
	GstFormat format = GST_FORMAT_TIME;
	if(! gst_element_query_duration(mPipeline, &format, &mDuration)) {
		close();
		return VIDEO_ERROR_GENERIC;
	}
	mDurationSeconds = ((double)mDuration)*10e-9;

	// query video dimensions and frame rate
	if(GstPad* pad = gst_element_get_static_pad(mVideoSink, "sink")){
		if(! gst_video_get_size(pad, &mWidth, &mHeight)){
			close();
			return VIDEO_ERROR_GENERIC;
		}

		const GValue *framerate = gst_video_frame_rate(pad);
		if(framerate && GST_VALUE_HOLDS_FRACTION(framerate)){
			gint fps_n = gst_value_get_fraction_numerator(framerate);
			gint fps_d = gst_value_get_fraction_denominator(framerate);
			mFps = ((float)fps_n)/((float)fps_d);
			mNFrames = (gint64)(mDuration / GST_SECOND) * (float)fps_n/(float)fps_d;
			mNanoPerFrame = ((double)mDuration)/((double)mNFrames);
		}
		else{
			close();
			return VIDEO_ERROR_GENERIC;
		}
	}
	else {
		close();
		return VIDEO_ERROR_GENERIC;
	}

	return err;
}

void GstVideoImpl :: loop() {
	printf("loop\n");

	GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT);
	if(mSpeed > 1. || mSpeed < -1.) {
		flags = (GstSeekFlags)(flags | GST_SEEK_FLAG_SKIP);
	}

	if(! gst_element_seek(
			mPipeline, 
			mSpeed, 
			GST_FORMAT_TIME,
			flags,
			GST_SEEK_TYPE_SET,
			0,
			GST_SEEK_TYPE_SET,
			-1.
		)
	){
		// ERROR
	}
}

void GstVideoImpl :: new_buffer(GstElement *e, GstVideoImpl *gst) {
	GstBuffer *buffer = gst_app_sink_pull_buffer(GST_APP_SINK(e));
	guint size = GST_BUFFER_SIZE(buffer);

	gst->fromBuffer((const char *)GST_BUFFER_DATA(buffer), size);
	GstClockTime now = GST_BUFFER_TIMESTAMP(buffer);
	if(now != GST_CLOCK_TIME_NONE) {
		gst->updateTime(now);
	}

	gst_buffer_unref (buffer);
}

void GstVideoImpl :: fromBuffer(const char *data, guint size) {
	printf("from buffer\n");
	if(mFrameData->header.dim[0] != mWidth || 
		mFrameData->header.dim[1] != mHeight)
	{
		int planes = size/(mWidth*mHeight);
		allo_array_adapt2d(mFrameData, planes, AlloUInt8Ty, mWidth, mHeight, 1);
	}

	memcpy(mFrameData->data.ptr, data, size);
	mNewPixels = true;
}

void GstVideoImpl :: updateTime(GstClockTime now) {
	mFrame = (int)(now/mNanoPerFrame);
	mPosition = (float)((float)now)/((float)mDuration);
}

VideoError GstVideoImpl :: close() {
	if(!mPipeline) return VIDEO_ERROR_NONE;

	// close video and free pixels
	gst_element_set_state(GST_ELEMENT(mPipeline), GST_STATE_NULL);

	// free pixels
	allo_array_destroy(mFrameData);

	// reset all video-dependent variables
	mDuration = 0;
	mDurationSeconds = 0;
	mWidth = 0;
	mHeight = 0;
	mNFrames = 0;
	mFps = 0;
	mNanoPerFrame = 0;
	mPaused = true;
	mNewPixels = false;
	mSpeed = 1;
	mFrame = 0;
	mPosition = 0;

	return VIDEO_ERROR_NONE;
}
	
VideoError GstVideoImpl :: play() {
	printf("play\n");
	if(mPipeline) {
		gst_element_set_state(mPipeline, GST_STATE_PLAYING);
		mPaused = false;
	}
	return VIDEO_ERROR_NONE;
}

VideoError GstVideoImpl :: next_frame(AlloArray *mat) {
	// process any messages
	handleMessages();

	GstClock *clock = gst_element_get_clock(mVideoSink);
	GstClockTime time = gst_clock_get_internal_time(clock);
	printf("time: %d\n", (int)time);

	if(mNewPixels) {
		printf("nextFrame\n");
		allo_array_adapt(mat, &(mFrameData->header));
		memcpy(mat->data.ptr, mFrameData->data.ptr, allo_array_size(mFrameData));
		mNewPixels = false;
	}
	return VIDEO_ERROR_NONE;
}

VideoError GstVideoImpl :: stop() {
	printf("stop\n");
	if(mPipeline) {
		gst_element_set_state(mPipeline, GST_STATE_PAUSED);
	}
	return VIDEO_ERROR_NONE;
}

VideoError GstVideoImpl :: pause(bool b) {
	printf("pause\n");
	if(mPipeline) {
		if(b) {
			gst_element_set_state(mPipeline, GST_STATE_PAUSED);
		}
		else {
			gst_element_set_state(mPipeline, GST_STATE_PLAYING);
		}
		mPaused = b;
	}
	return VIDEO_ERROR_NONE;
}
	
void GstVideoImpl :: set_position(float pct) {
	printf("position\n");
	if(mPipeline) {
		gint64 pos = (gint64)(pct*mDuration);
		GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH |  
											GST_SEEK_FLAG_ACCURATE);

		if(mSpeed > 1. || mSpeed < -1.) {
			flags = (GstSeekFlags)(flags | GST_SEEK_FLAG_SKIP);
		}

		gint64 start_pos, end_pos;
		if(mSpeed > 0) {
			start_pos = pos;
			end_pos = -1;
		}
		else {
			start_pos = 0;
			end_pos = pos;
		}
	
		GstFormat format = GST_FORMAT_TIME;
		if(! gst_element_seek(mPipeline, 
							mSpeed, 
							format,
							flags,
							GST_SEEK_TYPE_SET,
							start_pos,
							GST_SEEK_TYPE_SET,
							end_pos))
		{
			//muro::setError(MURO_ERROR_GENERIC, 
			//					"GstVideoImpl::position unable to seek");
		}
	}
}

float GstVideoImpl :: get_position() {
	return mPosition;
}
	
void GstVideoImpl :: set_speed(float speed) {
	printf("speed\n");
	if(!mPipeline) {
		mSpeed = speed;
		return;
	}

	if(speed == 0 && speed != 0 && !mPaused) {
		gst_element_set_state(mPipeline, GST_STATE_PLAYING);
	}
	mSpeed = speed;

	if(mSpeed == 0) {
		gst_element_set_state(mPipeline, GST_STATE_PAUSED);
		return;
	}

	GstFormat format = GST_FORMAT_TIME;
	gint64 pos = 0;
	if(! gst_element_query_position(GST_ELEMENT(mPipeline), &format, &pos)) {
		//muro::setError(MURO_ERROR_GENERIC, 
		//					"GstVideoImpl::speed unable to get video position");
		return;
	}

	GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH |  
										GST_SEEK_FLAG_ACCURATE);

	if(mSpeed > 1. || mSpeed < -1.) {
		flags = (GstSeekFlags)(flags | GST_SEEK_FLAG_SKIP);
	}

	gint64 start_pos, end_pos;
	if(mSpeed > 0) {
		start_pos = pos;
		end_pos = -1;
	}
	else {
		start_pos = 0;
		end_pos = pos;
	}
	
	if(! gst_element_seek(mPipeline, 
						mSpeed, 
						format,
						flags,
						GST_SEEK_TYPE_SET,
						start_pos,
						GST_SEEK_TYPE_SET,
						end_pos))
	{
		//muro::setError(MURO_ERROR_GENERIC, 
			//				"GstVideoImpl::speed unable to seek");
	}
}

float GstVideoImpl :: get_speed() {
	return mSpeed;
}
	
void GstVideoImpl :: set_frame(int f) {
	if(mPipeline) {
		float pct = ((float)f)/((float)mNFrames);
		set_position(pct);
	}
}

int GstVideoImpl :: get_frame() {
	return mFrame;
}

int GstVideoImpl :: get_nframes() {
	return mNFrames;
}
	
float GstVideoImpl :: get_duration() {
	return mDurationSeconds;
}
	
void GstVideoImpl :: get_dim(int &w, int &h) {
	w = mWidth;
	h = mHeight;
}

void GstVideoImpl :: handleMessages() {
	if(mPipeline) {
		GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(mPipeline));
		while(gst_bus_have_pending(bus)) {
			GstMessage *msg = gst_bus_pop(bus);
		
			switch(GST_MESSAGE_TYPE(msg)) {

				case GST_MESSAGE_STATE_CHANGED: {
					
					if(strcmp(gst_element_get_name(GST_MESSAGE_SRC(msg)), "player") == 0) {
						printf("GST_MESSAGE_STATE_CHANGED\n");
						GstState oldstate, newstate, pendstate;
						gst_message_parse_state_changed(msg, &oldstate, &newstate, &pendstate);

						printf("old: %s new: %s  pend: %s\n",
											name_for_gst_state(oldstate),
											name_for_gst_state(newstate),
											name_for_gst_state(pendstate));

						/*if(newstate == GST_STATE_READY) {
							gint nstreams = 0;
							g_object_get(G_OBJECT(pipeline), "nstreams", &nstreams, NULL);
							printf("nstreams: %d\n", nstreams);
							
							GList *stream_info = 0;
							g_object_get(G_OBJECT(pipeline), "stream-info", &stream_info, NULL);
							g_list_foreach(stream_info, stream_list, NULL);
							g_list_free(stream_info);
						}*/
					}
					
				} break;

				case GST_MESSAGE_ASYNC_DONE:
					break;

				case GST_MESSAGE_ERROR: {
					GError *err = 0;
					gchar *debug = 0;
					gst_message_parse_error(msg, &err, &debug);
			
					printf("gst error: %s %s\n", 
								gst_element_get_name(GST_MESSAGE_SRC(msg)),
								err->message);

					g_error_free(err);
					g_free(debug);
				} break;

				case GST_MESSAGE_EOS:
					this->loop();
				break;

				default:
					/*
					printf("message: %s %s\n%s\n", 
									gst_element_get_name(GST_MESSAGE_SRC(msg)),
									name_for_gst_message_type(GST_MESSAGE_TYPE(msg)),
									meaning_for_gst_message(GST_MESSAGE_TYPE(msg)));
					*/					
					break;
			}
		
			gst_message_unref(msg);
		}
	
		gst_object_unref(GST_OBJECT(bus));
	}
}

}	// video::

