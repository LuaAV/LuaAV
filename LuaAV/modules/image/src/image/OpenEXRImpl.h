#ifndef LUA_IMAGE_OPENEXRIMPL_H
#define LUA_IMAGE_OPENEXRIMPL_H 1

#include "ImageImpl.h"
#include <string>

#include "IlmImf/ImfIO.h"
#include "Iex/Iex.h"
#include "IlmImf/ImfOutputFile.h"
#include "IlmImf/ImfInputFile.h"
#include "IlmImf/ImfRgbaFile.h"
#include "IlmImf/ImfChannelList.h"
#include "IlmImf/ImfRgba.h"
#include "IlmImf/ImfArray.h"
#include "IlmImf/ImfTestFile.h"
#include "Half/half.h"

using std::string;

namespace image {

/*!
	\class OpenEXRImpl

	Loads and saves OpenEXR images
*/

class OpenEXRImpl : public ImageImpl {
public:

	/*!
		Basic constructor.  Takes a fully qualified filename.
	*/
	OpenEXRImpl();
	virtual ~OpenEXRImpl();

	/*!
		Load an OpenEXR image into Matrix mat
	*/
	virtual ImageError load(const char *filename, Matrix &mat);

	/*!
		Saves an image;
	*/
	virtual ImageError save(const char *filename, Matrix &mat);

	/*!
		Returns with width and height of the image.
	*/
	void get_dim(int &w, int &h);


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
	Matrix::Type matrix_type();

	/*!
		returns the Matrix::Format of the image
	*/
	Matrix::Format matrix_format();

	static Imf::PixelType pixel_type_for_matrix_type(Matrix::Type type);

	static bool is_openexr_file(const char *filename) {
		return Imf::isOpenExrFile(filename);
	}

protected:
	void destroy();


protected:
	void load_channels(Imf::InputFile &file, Matrix& mat, int numChannels, const char *channelNames);

	AlloArray		mImage;
	static bool	c_initialized;
};


}	// image::

#endif	// LUA_IMAGE_OPENEXRIMPL_H
