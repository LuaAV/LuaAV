#include "ImageImpl.h"
#include <string>
#include "rgb_io.h"

using std::string;

namespace image {

/*!
	\class RGBImpl

	Loads and saves RGB images
*/
class RGBImpl : public ImageImpl {
public:

	/*!
		Basic constructor.  Takes a fully qualified filename.
	*/
	RGBImpl();
	virtual ~RGBImpl();

	/*!
		Load an RGB image into Matrix mat
	*/
	virtual ImageError load(const char *filename, AlloArray &mat);

	/*!
		Saves an image;
	*/
	virtual ImageError save(const char *filename, AlloArray &mat);


protected:
	AlloArray		mImage;
};

}	// image::