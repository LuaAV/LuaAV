#include "FFMPEGVideoRecorderImpl.h"

namespace muro {

FFMPEGVideoRecorderImpl :: FFMPEGVideoRecorderImpl()
:	VideoRecorderImpl(VideoRecorderImpl::FFMPEG),
	mAVformatCtx(NULL),
	mAVoutputFmt(NULL),
	mAVstream(NULL),
//	mAVrgbframe(NULL),
//	mAVoutframe(NULL),
//	mAVoutdata(NULL),
	mVideoPts(0.),
	mFrameCount(0),
	mAVswsCtx(NULL)
{
	ffmpeg::initialize();
}

FFMPEGVideoRecorderImpl :: ~FFMPEGVideoRecorderImpl() {
	close();
}

// adapted from http://cekirdek.pardus.org.tr/~ismail/ffmpeg-docs/output__example_8c-source.html
// see link for license
MuroError FFMPEGVideoRecorderImpl :: open(const char *filename, int w, int h) {
	MuroError err = MURO_ERROR_NONE;

	// auto detect the output format from the name. default is mpeg
	mAVoutputFmt = guess_format(NULL, filename, NULL);

	// allocate and initialize the output media context
	mAVformatCtx = av_alloc_format_context();
	if(!mAVformatCtx) {
		return muro::setError(MURO_ERROR_MEMORY, "FFMPEGVideoRecorderImpl.open: Couldn't allocate AVFormatContext");
	}
	mAVformatCtx->oformat = mAVoutputFmt;
	snprintf(mAVformatCtx->filename, sizeof(mAVformatCtx->filename), "%s", filename);

	// add the audio and video streams using the default format codecs and initialize the codecs
	if(mAVoutputFmt->video_codec != CODEC_ID_NONE) {
		mAVstream = add_video_stream(mAVformatCtx, mAVoutputFmt->video_codec, w, h);
	}

	// set the output parameters (must be done even if no parameters).
	if(av_set_parameters(mAVformatCtx, NULL) < 0) {
		return muro::setError(MURO_ERROR_GENERIC, "FFMPEGVideoRecorderImpl.open: Invalid output format parameters");
	}

	// now that all the parameters are set, we can open the audio and video codecs and allocate
	// the necessary encode buffers
	if(mAVstream) {
		err = openVideo();
		if(err) return err;
	}

	// open the output file, if needed
	if(! (mAVformatCtx->flags & AVFMT_NOFILE)) {
		if(url_fopen(&mAVformatCtx->pb, filename, URL_WRONLY) < 0) {
			return muro::setError(MURO_ERROR_GENERIC, "FFMPEGVideoRecorderImpl.open: Couldn't open file");
		}
	}

	// write the stream header, if any
	av_write_header(mAVformatCtx);

	return err;
}

// TODO: add pointer check
MuroError FFMPEGVideoRecorderImpl :: openVideo() {
	MuroError err = MURO_ERROR_NONE;

	AVCodecContext *codecCtx = mAVstream->codec;

	// find the video encoder
	AVCodec *codec = avcodec_find_encoder(codecCtx->codec_id);
	if (!codec) {
		return muro::setError(MURO_ERROR_GENERIC, "FFMPEGVideoRecorderImpl.openVideo: codec not found");
	}

	// open the codec
	if(avcodec_open(codecCtx, codec) < 0) {
		return muro::setError(MURO_ERROR_GENERIC, "FFMPEGVideoRecorderImpl.openVideo: couldn't open codec");
	}

	/* allocate the encoded raw picture */
	//mAVoutframe = alloc_picture(codecCtx->pix_fmt, codecCtx->width, codecCtx->height);
	err = mOutFrame.alloc(codecCtx->pix_fmt, codecCtx->width, codecCtx->height);
	if(err) {
		return muro::setError(err, "FFMPEGVideoRecorderImpl.openVideo: couldn't allocate picture");
	}

	if(codecCtx->pix_fmt != PIX_FMT_RGB24) {
		err = mRGBFrame.alloc(PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
		if(err) {
			return muro::setError(err, "FFMPEGVideoRecorderImpl.openVideo: couldn't allocate x picture");
		}
	}

	return err;
}

#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */
AVStream * FFMPEGVideoRecorderImpl :: add_video_stream(AVFormatContext *oc, CodecID codec_id, int w, int h) {
	AVStream *st = av_new_stream(oc, 0);
	if(!st) {
		muro::setError(MURO_ERROR_GENERIC, "FFMPEGVideoRecorderImpl.add_video_stream: Couldn't create video stream");
		return NULL;
	}

	AVCodecContext *c = st->codec;
	c->codec_id = codec_id;
	c->codec_type = CODEC_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	c->width = w;
	c->height = h;
	/* time base: this is the fundamental unit of time (in seconds) in terms
	of which frame timestamps are represented. for fixed-fps content,
	timebase should be 1/framerate and timestamp increments should be
	identically 1. */
	c->time_base.den = STREAM_FRAME_RATE;
	c->time_base.num = 1;
	c->gop_size = 12; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = STREAM_PIX_FMT;
	if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
		/* just for testing, we also add B frames */
		c->max_b_frames = 2;
	}
	if (c->codec_id == CODEC_ID_MPEG1VIDEO){
		/* Needed to avoid using macroblocks in which some coeffs overflow.
		This does not happen with normal video, it just happens here as
		the motion of the chroma plane does not match the luma plane. */
		c->mb_decision = 2;
	}
	// some formats want stream headers to be separate
	if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") || !strcmp(oc->oformat->name, "3gp"))
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return st;
}

MuroError FFMPEGVideoRecorderImpl :: close() {
	MuroError err = MURO_ERROR_NONE;

	 av_write_trailer(mAVformatCtx);

	// free the streams
	for(unsigned int i=0; i < mAVformatCtx->nb_streams; i++) {
		av_freep(&mAVformatCtx->streams[i]->codec);
		av_freep(&mAVformatCtx->streams[i]);
	}

	if (! (mAVoutputFmt->flags & AVFMT_NOFILE)) {
		// close the output file
		url_fclose(mAVformatCtx->pb);
	}

	// free the stream
	av_free(mAVformatCtx);
	mAVformatCtx = NULL;
	mAVoutputFmt = NULL;

	mRGBFrame.free();
	mOutFrame.free();

	return err;
}

MuroError FFMPEGVideoRecorderImpl :: nextFrame(Matrix *mat) {
	MuroError err = MURO_ERROR_NONE;

//	if(!mAVstream) return error;

	int src_rowstride;	mat->getRowSize(src_rowstride);
	int dst_rowstride = mRGBFrame.frame->linesize[0];
	for(int j=0; j < mat->header.dim[1]; j++) {
		uint8_t *src = mat->data.ptr + j*src_rowstride;
		uint8_t *dst = mRGBFrame.frame->data[0] + j*dst_rowstride;
		for(int i=0; i < mat->header.dim[0]; i++) {
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];

			dst += 3;
			src += mat->header.planes;
		}
	}

	mVideoPts = (double)mAVstream->pts.val * mAVstream->time_base.num / mAVstream->time_base.den;
	err = write_video_frame(mAVformatCtx, mAVstream);
	if(err) return err;

	return err;
}

#ifndef INT64_C
#define INT64_C(v)   (v ## LL)
#endif

#define STREAM_DURATION   5.0
#define STREAM_NB_FRAMES  ((int)(STREAM_DURATION * STREAM_FRAME_RATE))
static int sws_flags = SWS_BICUBIC;
MuroError FFMPEGVideoRecorderImpl :: write_video_frame(AVFormatContext *oc, AVStream *st) {
	MuroError err = MURO_ERROR_NONE;
	AVCodecContext *c = st->codec;

	if(mFrameCount >= STREAM_NB_FRAMES) {
		/*	no more frame to compress. The codec has a latency of a few
			frames if using B frames, so we get the last frames by
			passing the same picture again */
	}
	else {
		if(c->pix_fmt != PIX_FMT_RGB24) {
			// as we only generate a YUV420P picture, we must convert it to the codec pixel format if needed
			if(mAVswsCtx == NULL) {
				mAVswsCtx = sws_getContext(c->width, c->height,
					PIX_FMT_RGB24,
					c->width, c->height,
					c->pix_fmt,
					sws_flags, NULL, NULL, NULL);

				if (mAVswsCtx == NULL) {
					return muro::setError(MURO_ERROR_NONE, "FFMPEGVideoRecorderImpl.write_video_frame: Cannot initialize the conversion context");
				}
			}

			sws_scale(	mAVswsCtx,
						mRGBFrame.frame->data, mRGBFrame.frame->linesize,
						0, c->height,
						mOutFrame.frame->data, mOutFrame.frame->linesize);
		}
		else {

		}
	}

	int out_size, ret;
	if(oc->oformat->flags & AVFMT_RAWPICTURE) {
		//raw video case. The API will change slightly in the near future for that
		AVPacket pkt;
		av_init_packet(&pkt);

		pkt.flags |= PKT_FLAG_KEY;
		pkt.stream_index = st->index;
		pkt.data = (uint8_t *)(mOutFrame.frame);
		pkt.size = sizeof(AVPicture);

		ret = av_write_frame(oc, &pkt);
	}
	else {
		/* encode the image */
		out_size = avcodec_encode_video(c, mOutFrame.data, mOutFrame.size, mOutFrame.frame);

		/* if zero size, it means the image was buffered */
		if(out_size > 0) {
			AVPacket pkt;
			av_init_packet(&pkt);

			if(c->coded_frame->pts != AV_NOPTS_VALUE)
				pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);

			if(c->coded_frame->key_frame)
				pkt.flags |= PKT_FLAG_KEY;

			pkt.stream_index = st->index;
			pkt.data = mOutFrame.data;
			pkt.size = mOutFrame.size;

			/* write the compressed frame in the media file */
			ret = av_write_frame(oc, &pkt);
		}
		else {
			ret = 0;
		}
	}
	if(ret != 0) {
		return muro::setError(MURO_ERROR_GENERIC, "FFMPEGVideoRecorderImpl.write_video_frame: Error while writing video frame");
	}
	mFrameCount++;

	return err;
}

}	// muro::

