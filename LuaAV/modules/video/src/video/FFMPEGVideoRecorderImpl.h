#ifndef LUA_MURO_FFMPEGVIDEOCAMERAIMPL_H
#define LUA_MURO_FFMPEGVIDEOCAMERAIMPL_H 1

#include "VideoRecorderImpl.h"
#include "FFMPEGUtils.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <avcodec.h>
#include <avformat.h>
#include <swscale.h>

#ifdef __cplusplus
}
#endif

using std::string;
using muro::ffmpeg::ffmpeg_frame;

namespace muro {

class MURO_API FFMPEGVideoRecorderImpl:  public VideoRecorderImpl {
public:
	FFMPEGVideoRecorderImpl();
	virtual ~FFMPEGVideoRecorderImpl();
	
	virtual MuroError open(const char *filename, int w, int h);
	virtual MuroError close();

	virtual MuroError nextFrame(Matrix *mat);
	
protected:
	MuroError openVideo();
//	AVFrame * alloc_picture(int pix_fmt, int width, int height);
	AVStream * add_video_stream(AVFormatContext *oc, CodecID codec_id, int w, int h);
	MuroError write_video_frame(AVFormatContext *oc, AVStream *st);
	
protected:
	AVFormatContext		*mAVformatCtx;	///< Format environment of output video file
	AVOutputFormat		*mAVoutputFmt;
	AVStream			*mAVstream;		///< Video stream
	ffmpeg_frame		mRGBFrame;		///< Frame of uncompressed RGB output video
	ffmpeg_frame		mOutFrame;		///< Frame of compressed output video
	double				mVideoPts;
	int					mFrameCount;
	SwsContext			*mAVswsCtx;		///< Codec conversion environment for transcoding codecs

};

}	// muro::

#endif //LUA_MURO_FFMPEGVIDEOCAMERAIMPL_H
