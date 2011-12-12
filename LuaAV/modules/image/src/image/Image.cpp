#include "Image.h"
#include "ImageImpl.h"

namespace image {

ImageError g_lastError = IMAGE_ERROR_NONE;
string g_LastErrorMsg;

ImageError set_error(ImageError err, const char *msg) {
	g_lastError = err;
	g_LastErrorMsg.assign(msg);
	return g_lastError;
}

Image :: Image()
:	mImpl(0),
	mData(0),
	mLoaded(false),
	mTaskData(NULL)
{
	mTaskData.o = this;
	mData = new al::ArrayWrapper(1);
}

Image :: ~Image() {
	mTask.stop();

	if(mImpl) Implementation::destroy(mImpl);
	mImpl = NULL;
	
	mData->release();
}

ImageError Image :: make_impl(const char *filename, bool loading) {
	ImageError err = IMAGE_ERROR_NONE;
	
	// check if we need to switch Implementations
	if(mImpl) {
		if(Implementation::type_for_filename(filename, loading) != mImpl->type()) {
			Implementation::destroy(mImpl);
			mImpl = NULL;
		}
	}
	
	// create Implementation if necessary
	if(!mImpl) {
		mImpl = Implementation::create(filename, loading);
	}
	if(!mImpl) {
		return set_error(IMAGE_ERROR_MEMORY, "Image::makeImpl(): couldn't create Image Implementation");
	}
	return err;
}

ImageError Image :: load(const char *filename) {
	ImageError err = IMAGE_ERROR_NONE;
	mLoaded = false;
	
	// make sure we have the appropriate backend
	err = make_impl(filename, true);
	if(err) return err;
	
	// load the file
	err = mImpl->load(filename, *mData);
	if(err) return err;
	
	mFilename.assign(filename);
	mLoaded = true;
	
	return err;
}

ImageError Image :: load_threaded(const char *filename) {
	ImageError err = IMAGE_ERROR_NONE;
	
	// check if task is active
	if(mImpl && !loaded()) return err;
	
	// reset thread task
	mTask.clear();
	
	// perform task
	mTaskData.filename.assign(filename);
	mTaskData.active = true;
	mTask.start(&mTaskData);

	return err;
}

void Image :: task() {
	mTaskData.err = load(mTaskData.filename.c_str());
}

ImageError Image :: save(const char *filename) {
	ImageError err = IMAGE_ERROR_NONE;
	
	// make sure we have the appropriate backend
	err = make_impl(filename, false);
	if(err) return err;
	
	// save the file
	err = mImpl->save(filename, *mData);
	if(err) return err;
	
	mFilename.assign(filename);
	
	return err;
}

void Image :: from_matrix(AlloArray *mat) {
	allo_array_copy(mData, mat);
}

}	// image::
