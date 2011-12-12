#include "GstUtils.h"

namespace video {

const char * name_for_gst_message_type(GstMessageType type) {
	switch(type) {
		case GST_MESSAGE_UNKNOWN: return "GST_MESSAGE_UNKNOWN";
		case GST_MESSAGE_EOS: return "GST_MESSAGE_EOS";
		case GST_MESSAGE_ERROR: return "GST_MESSAGE_ERROR";
		case GST_MESSAGE_WARNING: return "GST_MESSAGE_WARNING";
		case GST_MESSAGE_INFO: return "GST_MESSAGE_INFO";
		case GST_MESSAGE_TAG: return "GST_MESSAGE_TAG";
		case GST_MESSAGE_BUFFERING: return "GST_MESSAGE_BUFFERING";
		case GST_MESSAGE_STATE_CHANGED: return "GST_MESSAGE_STATE_CHANGED";
		case GST_MESSAGE_STATE_DIRTY: return "GST_MESSAGE_STATE_DIRTY";
		case GST_MESSAGE_STEP_DONE: return "GST_MESSAGE_STEP_DONE";
		case GST_MESSAGE_CLOCK_PROVIDE: return "GST_MESSAGE_CLOCK_PROVIDE";
		case GST_MESSAGE_CLOCK_LOST: return "GST_MESSAGE_CLOCK_LOST";
		case GST_MESSAGE_NEW_CLOCK: return "GST_MESSAGE_NEW_CLOCK";
		case GST_MESSAGE_STRUCTURE_CHANGE: return "GST_MESSAGE_STRUCTURE_CHANGED";
		case GST_MESSAGE_STREAM_STATUS: return "GST_MESSAGE_STREAM_STATUS";
		case GST_MESSAGE_APPLICATION: return "GST_MESSAGE_APPLICATION";
		case GST_MESSAGE_ELEMENT: return "GST_MESSAGE_ELEMENT";
		case GST_MESSAGE_SEGMENT_START: return "GST_MESSAGE_SEGMENT_START";
		case GST_MESSAGE_SEGMENT_DONE: return "GST_MESSAGE_SEGMENT_DONE";
		case GST_MESSAGE_DURATION: return "GST_MESSAGE_DURATION";
		case GST_MESSAGE_LATENCY: return "GST_MESSAGE_LATENCY";
		case GST_MESSAGE_ASYNC_START: return "GST_MESSAGE_ASYNC_START";
		case GST_MESSAGE_ASYNC_DONE: return "GST_MESSAGE_ASYNC_DONE";
	//	case GST_MESSAGE_REQUEST_STATE: return "GST_MESSAGE_REQUEST_STATE";
	//	case GST_MESSAGE_STEP_START: return "GST_MESSAGE_STEP_START";
		default: return "<unknown message>";
	}
}

const char * meaning_for_gst_message(GstMessageType type) {
	switch(type) {		
		case GST_MESSAGE_UNKNOWN:
			return "an undefined message";

		case GST_MESSAGE_EOS:
			return "end-of-stream reached in a pipeline. The application will only receive this message in the PLAYING state and every time it sets a pipeline to PLAYING that is in the EOS state. The application can perform a flushing seek in the pipeline, which will undo the EOS state again.";

		case GST_MESSAGE_ERROR:
			return "an error occured. Whe the application receives an error message it should stop playback of the pipeline and not assume that more data will be played.";

		case GST_MESSAGE_WARNING:
			return "a warning occured.";

		case GST_MESSAGE_INFO:
			return "an info message occured";

		case GST_MESSAGE_TAG:
			return "a tag was found.";

		case GST_MESSAGE_BUFFERING:
			return "the pipeline is buffering. When the application receives a buffering message in the PLAYING state for a non-live pipeline it must PAUSE the pipeline until the buffering completes, when the percentage field in the message is 100%. For live pipelines, no action must be performed and the buffering percentage can be used to inform the user about the progress.";

		case GST_MESSAGE_STATE_CHANGED:
			return "a state change happened";

		case GST_MESSAGE_STATE_DIRTY:
			return "an element changed state in a streaming thread. This message is deprecated.";

		case GST_MESSAGE_STEP_DONE:
			return "a framestep finished. This message is not yet implemented.";

		case GST_MESSAGE_CLOCK_PROVIDE:
			return "an element notifies its capability of providing a clock. This message is used internally and never forwarded to the application.";

		case GST_MESSAGE_CLOCK_LOST:
			return "The current clock as selected by the pipeline became unusable. The pipeline will select a new clock on the next PLAYING state change.";

		case GST_MESSAGE_NEW_CLOCK:
			return "a new clock was selected in the pipeline.";

		case GST_MESSAGE_STRUCTURE_CHANGE:
			return "the structure of the pipeline changed. This message is used internally and never forwarded to the application.";

		case GST_MESSAGE_STREAM_STATUS:
			return "status about a stream, emitted when it starts, stops, errors, etc..";

		case GST_MESSAGE_APPLICATION:
			return "message posted by the application, possibly via an application-specific element.";

		case GST_MESSAGE_ELEMENT:
			return "element-specific message, see the specific element's documentation";

		case GST_MESSAGE_SEGMENT_START:
			return "pipeline started playback of a segment. This message is used internally and never forwarded to the application.";

		case GST_MESSAGE_SEGMENT_DONE:
			return "pipeline completed playback of a segment. This message is forwarded to the application after all elements that posted GST_MESSAGE_SEGMENT_START posted a GST_MESSAGE_SEGMENT_DONE message.";

		case GST_MESSAGE_DURATION:
			return "The duration of a pipeline changed. The application can get the new duration with a duration query.";

		case GST_MESSAGE_LATENCY:
			return "Posted by elements when their latency changes. The pipeline will calculate and distribute a new latency. Since: 0.10.12";

		case GST_MESSAGE_ASYNC_START:
			return "Posted by elements when they start an ASYNC state change. This message is not forwarded to the application but is used internally. Since: 0.10.13.";

		case GST_MESSAGE_ASYNC_DONE:
			return "Posted by elements when they complete an ASYNC state change. The application will only receive this message from the toplevel pipeline. Since: 0.10.13";
/*
		case GST_MESSAGE_REQUEST_STATE:
			return "Posted by elements when they want the pipeline to change state. This message is a suggestion to the application which can decide to perform the state change on (part of) the pipeline. Since: 0.10.23.";

		case GST_MESSAGE_STEP_START:
			return"A stepping operation was started.";
*/
		default: return "<unknown message>";
	}
}

const char * name_for_gst_state(GstState state) {
	switch(state){
		case GST_STATE_VOID_PENDING: return "GST_STATE_VOID_PENDING";
		case GST_STATE_NULL: return "GST_STATE_NULL";
		case GST_STATE_READY: return "GST_STATE_READY";
		case GST_STATE_PAUSED: return "GST_STATE_PAUSED";
		case GST_STATE_PLAYING: return "GST_STATE_PLAYING";
		default: return "<unknown state>";
	}
}

const char * name_for_gst_video_format(GstVideoFormat format) {
	switch(format) {
		case GST_VIDEO_FORMAT_UNKNOWN: return "GST_VIDEO_FORMAT_UNKNOWN";
		case GST_VIDEO_FORMAT_I420: return "GST_VIDEO_FORMAT_I420";
		case GST_VIDEO_FORMAT_YV12: return "GST_VIDEO_FORMAT_YV12";
		case GST_VIDEO_FORMAT_YUY2: return "GST_VIDEO_FORMAT_YUY2";
		case GST_VIDEO_FORMAT_UYVY: return "GST_VIDEO_FORMAT_UYVY";
		case GST_VIDEO_FORMAT_AYUV: return "GST_VIDEO_FORMAT_AYUV";
		case GST_VIDEO_FORMAT_RGBx: return "GST_VIDEO_FORMAT_RGBx";
		case GST_VIDEO_FORMAT_BGRx: return "GST_VIDEO_FORMAT_BGRx";
		case GST_VIDEO_FORMAT_xRGB: return "GST_VIDEO_FORMAT_xRGB";
		case GST_VIDEO_FORMAT_xBGR: return "GST_VIDEO_FORMAT_xBGR";
		case GST_VIDEO_FORMAT_RGBA: return "GST_VIDEO_FORMAT_RGBA";
		case GST_VIDEO_FORMAT_BGRA: return "GST_VIDEO_FORMAT_BGRA";
		case GST_VIDEO_FORMAT_ARGB: return "GST_VIDEO_FORMAT_ARGB";
		case GST_VIDEO_FORMAT_ABGR: return "GST_VIDEO_FORMAT_ABGR";
		case GST_VIDEO_FORMAT_RGB: return "GST_VIDEO_FORMAT_RGB";
		case GST_VIDEO_FORMAT_BGR: return "GST_VIDEO_FORMAT_BGR";
		case GST_VIDEO_FORMAT_Y41B: return "GST_VIDEO_FORMAT_Y41B";
		case GST_VIDEO_FORMAT_Y42B: return "GST_VIDEO_FORMAT_Y42B";
//		case GST_VIDEO_FORMAT_YVYU: return "GST_VIDEO_FORMAT_YVYU";
//		case GST_VIDEO_FORMAT_Y444: return "GST_VIDEO_FORMAT_Y444";
//		case GST_VIDEO_FORMAT_v210: return "GST_VIDEO_FORMAT_v210";
//		case GST_VIDEO_FORMAT_v216: return "GST_VIDEO_FORMAT_v216";
//		case GST_VIDEO_FORMAT_NV12: return "GST_VIDEO_FORMAT_NV12";
//		case GST_VIDEO_FORMAT_NV21: return "GST_VIDEO_FORMAT_NV21";
		default: return "<unknown video format>";
	}
}

const char * name_for_gst_state_change_return(GstStateChangeReturn change) {
	switch(change) {
		case GST_STATE_CHANGE_SUCCESS: return "GST_STATE_CHANGE_SUCCESS"; break;
		case GST_STATE_CHANGE_FAILURE: return "GST_STATE_CHANGE_FAILURE"; break;
		case GST_STATE_CHANGE_ASYNC: return "GST_STATE_CHANGE_ASYNC"; break;
		default: return "<unknown state change return>";
	}
}

}	// video::

