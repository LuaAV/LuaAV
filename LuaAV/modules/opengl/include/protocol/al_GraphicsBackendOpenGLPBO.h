#ifndef AL_GRAPHICS_BACKEND_OPENGL_PBO
#define AL_GRAPHICS_BACKEND_OPENGL_PBO 1

#include "protocol/al_GraphicsBackendOpenGL.hpp"
#include "allocore/types/al_Array.hpp"
#include "graphics/al_Texture.hpp"

namespace al{
namespace gfx{

class PBO {
public:
	typedef unsigned char		uchar;

	/*enum Type {
		RGBA
		BGRA
		VTcN
	};*/

public:
	PBO();
	virtual ~PBO();

	void create();

	void destroy();

	void bind();
	void unbind();

	/*!
		Get the pointer to the data in DMA
	*/
	void getData();

	/*!
		This is called leave because we aren't actually releasing memory, just giving back
		access to it.  The pointer is in DMA, so we must make sure to maintain access to only
		for as long as we need it and no longer.
	*/
	void leaveData();


	void submit(GLenum format, GLenum type, int *dim, const GLvoid *data);
	void submit(GLsizeiptrARB size, const GLvoid *data);

	/*!

	*/
	void sendData();

	/*!
		Gets pixels form a framebuffer specified my mode

		@param mode		Specifies a color buffer.  Accepted values are GL_FRONT_LEFT, GL_FRONT_RIGHT,
						GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, and GL_AUXi,
						where i is	between	0 and GL_AUX_BUFFERS-1.
	*/
	void readBuffer(GLenum mode);


	void fromTexture(Texture *t, GLenum format);
	void toArray(AlloArray *m);
	void target(GLenum target);



protected:

	GLuint				mPboid;		///< Unique buffer ID
	GLenum				mTarget;	///< Specifies the target buffer object being mapped
	GLenum				mUsage;		///< Specifies the access policy, indicating whether it will be possible to read from, write to, or both read from and write to the buffer object's mapped data store.
	GLenum				mFormat;	///< Data storage format (channels etc.)
	GLenum				mType;		///< Data basic type (uchar etc. )
	GLsizeiptrARB		mSize;		///< Size of buffer in bytes
	int					mOrigin[2];	///< Origin of the rectangle data view
	int					mDim[2];	///< Extent of the rectangle data view
	bool				mResubmit;	///< Whether to rebuild the GPU buffer
	AlloArray			mData;		///< Mapped data when available
};


} // ::al::gfx
} // ::al

#endif // AL_GRAPHICS_BACKEND_OPENGL_PBO