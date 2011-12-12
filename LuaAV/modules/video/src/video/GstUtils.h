#ifndef VIDEO_GST_UTLS_H
#define VIDEO_GST_UTLS_H 1

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

namespace video {
	
	const char * name_for_gst_message_type(GstMessageType type);
	const char * meaning_for_gst_message(GstMessageType type);
	const char * name_for_gst_state(GstState state);
	const char * name_for_gst_video_format(GstVideoFormat format);
	const char * name_for_gst_state_change_return(GstStateChangeReturn change);

}	// video::

#endif	// VIDEO_GST_UTLS_H
