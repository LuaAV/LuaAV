#ifndef MURO_FFMPEG_UTILS_H
#define MURO_FFMPEG_UTILS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <avcodec.h>
#include <avformat.h>
#include <swscale.h>

#ifdef __cplusplus
}
#endif

#ifndef INT64_C
#define INT64_C(v)   (v ## LL)
#endif

/*
	#define AVERROR_UNKNOWN     AVERROR(EINVAL)  //< unknown error
	#define AVERROR_IO          AVERROR(EIO)     //< I/O error
	#define AVERROR_NUMEXPECTED AVERROR(EDOM)    //< Number syntax expected in filename
	#define AVERROR_INVALIDDATA AVERROR(EINVAL)  //< invalid data found
	#define AVERROR_NOMEM       AVERROR(ENOMEM)  //< not enough memory
	#define AVERROR_NOFMT       AVERROR(EILSEQ)  //< unknown format
	#define AVERROR_NOTSUPP     AVERROR(ENOSYS)  //< Operation not supported
	#define AVERROR_NOENT       AVERROR(ENOENT)  //< No such file or directory
*/

/* Notes on FFMPEG:
	av_free is a light wrapper around free, so any structs with attached pointers freed in this manner
		will not have those attached pointers freed since they're not part of the immediate memory block.

	avcodec_alloc_frame is a light wrapper around av_malloc that also initializes the AVFrame struct to
		default values.
*/

namespace muro {
namespace ffmpeg {

/*! Basic FFMPEG framebuffer structure
	This is needed because FFMPEG can slice up a the framebuffer memory
	block associated with a frame according to the codec it represents.  
	For example, YCbCr formatted codecs will be split into 3 blocks for 
	the different channels.  Keeping a copt of the pointer outside of 
	the AVFrame data structure makes it easy to free later.
*/
struct ffmpeg_frame {
public:
	ffmpeg_frame()
	: frame(NULL), size(0), data(NULL)
	{}
	
	~ffmpeg_frame() {
		free();
	}
	
	/*! Allocates a frame of video
		@param pix_fmt - pixel format enum/define
		@param width - width of frame
		@param height - height of frame
	*/
	MuroError alloc(int pix_fmt, int width, int height) {
		MuroError err = MURO_ERROR_NONE;

		// allocate frame struct
		frame = avcodec_alloc_frame();
		if(!(frame)) {
			return muro::setError(MURO_ERROR_GENERIC, "ffmpeg_frame::alloc_frame: Couldn't allocate AVFrame");
		}
		
		// allocate framebuffer
		size = avpicture_get_size(pix_fmt, width, height);
		data = (uint8_t *)av_malloc(size);
		if(!(data)) {
			av_free(frame);
			frame = NULL;
			return muro::setError(MURO_ERROR_GENERIC, "ffmpeg_frame::alloc_frame: Couldn't allocate AVFrame framebuffer");
		}
		
		// associate data buffer with frame
		avpicture_fill((AVPicture *)(frame), data, pix_fmt, width, height);

		return err;
	}

	void free() {
		if(frame) av_free(frame);
		frame = NULL;
		
		if(data) av_free(data);
		size = 0; data = NULL;
	}

	AVFrame *frame;
	int size;
	uint8_t *data;
};


/*! Initializes the FFMPEG library
*/	
static void initialize() {
	static bool inited = false;
	if(!inited) {
		printf("Initalizing FFMPEG\n");
		av_register_all();
		inited = true;
	}
}

	
}	// ffmpeg:
}	// muro::

#endif	// MURO_FFMPEG_UTILS_H
