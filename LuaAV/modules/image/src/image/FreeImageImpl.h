#ifndef LUA_IMAGE_FREEIMAGEIMPL_H
#define LUA_IMAGE_FREEIMAGEIMPL_H 1

#include "Image.h"
#include "ImageImpl.h"
#include "FreeImage.h"
#include <string>

using std::string;

namespace image {

/*!
	\class FreeImageImpl

	Loads and saves images using the FreeImage library.  Supported formats include:

		bmp, chead, cut, dcx, dds, doom, doomFlat, exif, gif, hdr, ico, jasc_pal, jpg,
		lbm, lif, mdl, pcd, pcx, pic, pix, png, pnm, psd, psp, pxr, raw, sgi, tgo, tif,
		wal, xpm
*/

class FreeImageImpl : public ImageImpl {
public:
	/*!
		Basic constructor.  Takes a fully qualified filename.
	*/
	FreeImageImpl();
	virtual ~FreeImageImpl();
	
	/*!
		Convert the image to a Matrix.  The Matrix will adapt to the image dimensions.
		Image channels will be appropriately mapped to the standard Matrix channel mappings.
	*/
	void to_matrix(AlloArray *mat);

	/*!
		Convert a Matrix into an image
	*/
	void from_matrix(AlloArray *mat);

	
	virtual ImageError load(const char *filename, AlloArray &mat);
	virtual ImageError save(const char *filename, AlloArray &mat);

	/*!
		Loads an image.  By default, image type is not specified, but guessed
		from the file extension.
	*/
	ImageError load(const char *filename, FREE_IMAGE_FORMAT type=FIF_UNKNOWN);

	/*!
		Saves an image.  By default, image type is not specified, but guessed
		from the file extension.
	*/
//		void save(const char *filename, ILenum type = IL_TYPE_UNKNOWN);

	/*!
		Returns with width and height of the image.
	*/
	void get_dim(uint32_t &w, uint32_t &h);


	/*!
		Returns the number of planes
	*/
	int get_planes();

	/*!
		Returns a byte pointer to the image data
	*/
	virtual char * get_data();

	/*!
		returns the Matrix::Type of the image
	*/
	AlloTy matrix_type();

	/*!
		returns the Matrix::Format of the planecount
	*/
	static ArrayFormat matrix_format(int planes);

protected:
	static void initialize();
	void destroy();

protected:
	FIBITMAP		*mImage;
	static bool	c_initialized;
};

}	// image::

#endif	// LUA_IMAGE_FREEIMAGEIMPL_H
