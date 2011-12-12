#ifndef LUA_IMAGE_IMAGE_H
#define LUA_IMAGE_IMAGE_H 1

#include "allocore/types/al_Array.hpp"
#include "ThreadTask.h"
#include <string>

using std::string;


namespace image {


enum ImageError{
	IMAGE_ERROR_NONE = 0,
	IMAGE_ERROR_GENERIC,
	IMAGE_ERROR_OPENGL,
	IMAGE_ERROR_MEMORY,
	IMAGE_ERROR_INVALID_ENUM,
	IMAGE_ERROR_INVALID_OPERATION
};

enum ArrayFormat{
	FORMAT_ALPHA = 1,
	FORMAT_LUMALPHA = 2,
	FORMAT_RGB = 3,
	FORMAT_RGBA = 4
};

template <typename T>
struct RGBpix {
	T r;
	T g;
	T b;
};

template <typename T>
struct RGBApix {
	T r;
	T g;
	T b;
	T a;
};


ImageError set_error(ImageError err, const char *msg);

class ImageImpl;

/*!
	\class Image

	Loads and saves images.  Supported formats include:

		bmp, chead, cut, dcx, dds, doom, doomFlat, exif, gif, hdr, ico, jasc_pal, jpg,
		lbm, lif, mdl, pcd, pcx, pic, pix, png, pnm, psd, psp, pxr, raw, sgi, tgo, tif,
		wal, xpm
*/
class Image {
//private:
//	friend class ImageImpl;

protected:
	typedef ImageImpl		Implementation;

public:
	Image();
	~Image();
	
	
	struct TaskData {
		TaskData(Image *o)
		:	o(o),
			err(IMAGE_ERROR_NONE),
			active(false)
		{}
	
		Image *o;
		string filename;
		bool active;
		ImageError err;
	};


	/*!
		Loads an image.  By default, image type is not specified, but guessed
		from the file extension.
	*/
	ImageError load(const char *filename);
	
	ImageError load_threaded(const char *filename);
	void task();

	/*!
		Saves an image.
	*/
	ImageError save(const char *filename);

	void from_matrix(AlloArray *mat);
	AlloArray * matrix() {return mData;}
	al::ArrayWrapper * wrapper() {return mData;}
	
	bool loading() {
		return mTaskData.active;
	}
	
	bool loaded() {
		return !loading() && mLoaded;
	}

protected:
	ImageError make_impl(const char *filename, bool loading);

protected:
	Implementation			*mImpl;		///< Implementation (library binding)
	al::ArrayWrapper		*mData;		///< Pixel data
	string					mFilename;	///< Filename of loaded/saved image
	bool					mLoaded;	///< File loaded flag, useful for threaded loader
	ThreadTask<TaskData>	mTask;		///< Task for threaded file loading
	TaskData				mTaskData;	///< Data for threaded file loading
};

}	// image::

#endif	// LUA_IMAGE_IMAGE_H
