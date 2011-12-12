#ifndef VIDEO_RECORDER_H
#define VIDEO_RECORDER_H 1

#include "allocore/types/al_Array.hpp"
#include "video_common.h"
#include <string>

using std::string;

namespace video {

class VideoRecorderImpl;

class VideoRecorder {
private:
	friend class VideoRecorderImpl;
	
	typedef VideoRecorder		Self;
	typedef VideoRecorderImpl	Implementation;

public:
	VideoRecorder();
	virtual ~VideoRecorder();

	VideoError open(const char *filename, int w, int h);
	VideoError close();
	
	VideoError save_frame(AlloArray *m);

protected:
	VideoError make_impl();

protected:
	Implementation			*mImpl;		///< Implementation (library binding)
};

}	// video::

#endif	// VIDEO_RECORDER_H
