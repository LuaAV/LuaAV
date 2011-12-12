#include "protocol/al_GraphicsBackendOpenGLPBO.h"
#include <cmath>
#include <string.h>

namespace al {
namespace gfx {

PBO :: PBO()
:	mPboid(0),
	mTarget(GL_PIXEL_PACK_BUFFER_ARB),
	mUsage(GL_STREAM_READ_ARB),
	mFormat(GL_RGBA),
	mType(GL_UNSIGNED_BYTE),
	mSize(0),
	mResubmit(false)
{
	mOrigin[0] = 0;
	mOrigin[1] = 0;
	mDim[0] = 0;
	mDim[1] = 0;
	
	allo_array_clear(&mData);
}

PBO :: ~PBO() {
	destroy();
}

void PBO :: create() {
	glGenBuffersARB(1, &mPboid);
}

void PBO :: destroy() {
	if(mPboid > 0) {
		glDeleteBuffers(1, &mPboid);
		mPboid = 0;
		
		mDim[0] = 0;
		mDim[1] = 0;
	}
}

void PBO :: bind() {
	if(mPboid <= 0) {
		create();
	}
	if(mResubmit) {
		submit(mSize, 0);
	}
	glBindBuffer(mTarget, mPboid);
}

void PBO :: unbind() {
	glBindBuffer(mTarget, 0);
}

void PBO :: getData() {
	bind();
	
	mData.header.type = GraphicsBackendOpenGL::array_type_for_type(mType);
	mData.header.components = GraphicsBackendOpenGL::components_from_format(mFormat);
	mData.header.dimcount = 2;
	mData.header.dim[0] = mDim[0];
	mData.header.dim[1] = mDim[1];
	allo_array_setstride(&(mData.header), 1);
	
	mData.data.ptr = (char *)glMapBuffer(mTarget, GL_READ_ONLY_ARB);
	unbind();
	GraphicsBackendOpenGL::gl_error("PBO getting data");
}

void PBO :: leaveData() {
	bind();
	glUnmapBuffer(mTarget);
	unbind();
	allo_array_clear(&mData);
	GraphicsBackendOpenGL::gl_error("PBO leaving data");
}


void PBO :: fromTexture(Texture *t, GLenum format) {
	Surface *surface = t->surface();
	if(!surface) {
		return;
	}

	// generate buffer if it is not there or doesn't have the correct size
	if(t->width() != mDim[0] || t->height() != mDim[1]) {
		int dim[] = {t->width(), t->height()};
		submit(
			format, 
			GraphicsBackendOpenGL::type_from_texture_type((GraphicsBackendOpenGL *)t->backend(), t->type()), 
			dim, 
			0
		);
	}

	GLenum prevTarget = mTarget;
	target(GL_PIXEL_PACK_BUFFER_ARB);

	bool autoclear = surface->autoClear();
	surface->autoClear(false);

	surface->enter();
		bind();
			readBuffer(GL_COLOR_ATTACHMENT0_EXT);
		unbind();
	surface->leave();

	surface->autoClear(autoclear);
	target(prevTarget);

	GraphicsBackendOpenGL::gl_error("PBO from Texture");
}

void PBO :: toArray(AlloArray *m) {
	getData();
	if(mData.data.ptr) {
		allo_array_adapt(m, &mData.header);
		allo_array_copy(m, &mData);
	}
	leaveData();
	GraphicsBackendOpenGL::gl_error("PBO to Matrix");
}


void PBO :: submit(GLenum format, GLenum type, int *dim, const GLvoid *data) {
	mResubmit = false;

	// this will validate the input parameters
	// if any are invalid, size will be 0
	mSize = GraphicsBackendOpenGL::size_from_buffer_parameters(format, type, dim);

	if(mSize) {
		mFormat = format;
		mType = type;
		for(int i=0; i < 2; i++) {
			mDim[i] = dim[i];
		}

		bind();
		glBufferDataARB(mTarget, mSize, data, mUsage);
		unbind();
	}
	GraphicsBackendOpenGL::gl_error("Submitting PBO data");
}

void PBO :: submit(GLsizeiptrARB size, const GLvoid *data) {
	mResubmit = false;
	mSize = size;

	if(size) {
		// guess at dims in case RTVBO is called (estimate an NxN size)
		int components = GraphicsBackendOpenGL::components_from_format(mFormat);
		int bpp = GraphicsBackendOpenGL::bytes_from_type(mType);
		if(components && bpp) {
			int numpixels = ((int)size)/(bpp*components);
			mDim[0] = (int)floor( sqrt((float)numpixels) );
			mDim[1] = numpixels/mDim[0];
		}
	}
	else {
		mDim[0] = 0;
		mDim[1] = 0;
	}

	bind();
	glBufferDataARB(mTarget, size, data, mUsage);
	unbind();
	GraphicsBackendOpenGL::gl_error("Submitting PBO data");
}

void PBO :: sendData() {
	// start to copy from PBO to texture object ///////
	// bind the texture and PBO
//	glBindTexture(GL_TEXTURE_2D, textureId);
//	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[index]);

	// copy pixels from PBO to texture object
	// Use offset instead of ponter.
//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);

// bind PBO to update pixel values
//        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);

        // map the buffer object into client's memory
        // Note that glMapBufferARB() causes sync issue.
        // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferDataARB() with NULL pointer before glMapBufferARB().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBufferARB() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
     //   glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB);
    //    GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    //    if(ptr)
  //      {
            // update data directly on the mapped buffer
    //        updatePixels(ptr, DATA_SIZE);
  //          glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
//        }

        ///////////////////////////////////////////////////

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
//        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void PBO :: readBuffer(GLenum mode) {
	// must be pixel a pixel pack buffer
	glReadBuffer(mode);
	glReadPixels(
		mOrigin[0], mOrigin[1],
		mDim[0], mDim[1],
		mFormat, mType,
		0
	);
	
	GraphicsBackendOpenGL::gl_error("PBO Buffer Readback");
}

void PBO :: target(GLenum target) {
	mTarget = target;
}

}	// gfx::
}	// al::
