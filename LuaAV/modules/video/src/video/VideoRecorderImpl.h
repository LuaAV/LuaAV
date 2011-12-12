#ifndef VIDEORECORDERIMPL_H
#define VIDEORECORDERIMPL_H 1

#include "VideoRecorder.h"

namespace video {

class VideoRecorderImpl {
public:

	enum Type {
		FFMPEG = 0,	///< ffmpeg backend
//		QT,			///< Quicktime backend
		OF			///< OpenFrameworks backend
	};

protected:
	VideoRecorderImpl(Type type);
	virtual ~VideoRecorderImpl();
	
public:
	static VideoRecorderImpl * create();
	static void destroy(VideoRecorderImpl *impl);
	
	virtual VideoError open(const char *filename, int w, int h) = 0;
	virtual VideoError close() = 0;

	virtual VideoError save_frame(AlloArray *mat) = 0;
	
	Type type() {return mType;}
	
protected:
	Type	mType;		///< Backend Type
};

}	// video::

#endif // VIDEORECORDERIMPL_H
