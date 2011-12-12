#ifndef LUA_MURO_FFMPEGVIDEOIMPL_H
#define LUA_MURO_FFMPEGVIDEOIMPL_H 1

#include "Muro.h"
#include "VideoImpl.h"
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

/*!
	\class FFMPEGVideoImpl
*/
class MURO_API FFMPEGVideoImpl : public VideoImpl {
public:
	FFMPEGVideoImpl();
	~FFMPEGVideoImpl();

	virtual MuroError open(const char *filename);
	virtual MuroError close();
	virtual MuroError save(const char *filename);

	virtual MuroError play();
	virtual MuroError nextFrame(Matrix *mat);

	void printInfo(const char *filename);

protected:
	void destroy();
	MuroError openCodec();
	MuroError nextFrame();
	void printStreamFormat(AVFormatContext *ic, int i, int index, int is_output);

protected:
	AVFormatContext		*mAVformatCtx;	///< Format environment of input video file
	AVStream			*mAVstream;		///< Video stream
	AVCodecContext		*mAVcodecCtx;	///< Codec environment if input video stream
	int					mStreamIdx;		///< Index in input file of video stream
	AVFrame				*mAVframe;		///< Frame of raw input video
	ffmpeg_frame		mRGBFrame;		///< RGB frame for uncompressed frame
	struct SwsContext	*mAVswsCtx;		///< Codec conversion environment for transcoding codecs
};

}	// muro::

#endif	//LUA_MURO_FFMPEGVIDEOIMPL_H
