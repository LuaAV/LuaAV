#include "stdio.h"
#include "protocol/al_GraphicsBackendOpenGL.hpp"
#include "protocol/al_GraphicsBackendOpenGLPBO.h"
#include "graphics/al_Slab.hpp"
#include "graphics/al_Mesh.hpp"

namespace al{
namespace gfx{

GraphicsBackendOpenGL::SurfaceData::SurfaceData()
:	depth_id(0),
	pbo(new PBO())
{}

GraphicsBackendOpenGL::SurfaceData::~SurfaceData() {
	delete pbo;
}

void GraphicsBackendOpenGL::gl_error(const char *msg) {
	GLenum err = glGetError();

	switch(err) {
		case GL_INVALID_ENUM:
			printf("%s:\n %s\n", msg, "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_INVALID_VALUE:
			printf("%s:\n %s\n", msg, "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_INVALID_OPERATION:
			printf("%s:\n %s\n", msg, "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_STACK_OVERFLOW:
			printf("%s:\n %s\n", msg, "This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_STACK_UNDERFLOW:
			printf("%s:\n %s\n", msg, "This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_OUT_OF_MEMORY:
			printf("%s:\n %s\n", msg, "There is not enough memory left to execute the command.  The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
			break;

		case GL_TABLE_TOO_LARGE:
			printf("%s:\n %s\n", msg, "The specified table exceeds the implementation's maximum supported table size.  The offending command is ignored and has no other side effect than to set the error flag.");
			break;

		case GL_NO_ERROR:
			break;

		default:
			break;
	}
}


GLenum gl_antialias_mode(AntiAliasMode::t mode) {
	switch (mode) {
		case AntiAliasMode::Nicest:		return GL_NICEST;
		case AntiAliasMode::Fastest:	return GL_FASTEST;
		default:
			return GL_DONT_CARE;
	}
}

GLenum gl_blend_func(BlendFunc bf) {
	switch (bf) {
		case SRC_COLOR:	return GL_SRC_COLOR;
		case DST_COLOR:	return GL_DST_COLOR;
		case SRC_ALPHA:	return GL_SRC_ALPHA;
		case DST_ALPHA:	return GL_DST_ALPHA;
		case ZERO:		return GL_ZERO;
		case ONE:		return GL_ONE;
		case SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
		
		default:
			return GL_SRC_COLOR;
	}
}

GLenum gl_polygon_mode(PolygonMode mode) {
	switch(mode) {
		case POINT:	return GL_POINT;
		case LINE:	return GL_LINE;
		case FILL:
		default:
			return GL_FILL;
	}
}

GLenum GraphicsBackendOpenGL::gl_primitive(Primitive prim) {
	switch(prim) {
		case POINTS:		return GL_POINTS;
		case LINES:			return GL_LINES;
		case LINE_STRIP:	return GL_LINE_STRIP;
		case LINE_LOOP:		return GL_LINE_LOOP;
		case TRIANGLES:		return GL_TRIANGLES;
		case QUADS:			return GL_QUADS;
		case POLYGON:		return GL_POLYGON;
		
		default: return GL_POINTS;
	}
}

Primitive GraphicsBackendOpenGL::primitive(GLenum prim) {
	switch(prim) {
		case GL_POINTS:		return POINTS;
		case GL_LINES:		return LINES;
		case GL_LINE_STRIP:	return LINE_STRIP;
		case GL_LINE_LOOP:	return LINE_LOOP;
		case GL_TRIANGLES:	return TRIANGLES;
		case GL_QUADS:		return QUADS;
		case GL_POLYGON:	return POLYGON;
		
		default: return POINTS;
	}
}


GraphicsBackendOpenGL::GraphicsBackendOpenGL()
:	GraphicsBackend()
{}

GraphicsBackendOpenGL::~GraphicsBackendOpenGL(){
}

// Render State
void GraphicsBackendOpenGL::enableBlending(bool enable, BlendFunc src, BlendFunc dst) {
	if(enable) {
		glEnable(GL_BLEND);
		glBlendFunc(
			gl_blend_func(src),
			gl_blend_func(dst)
		);
	}
	else {
		glDisable(GL_BLEND);
	}
}

void GraphicsBackendOpenGL::enableLighting(bool enable) {
	if(enable) {
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
	}
}

void GraphicsBackendOpenGL::enableLight(bool enable, int idx) {
	if(enable) {
		glEnable(GL_LIGHT0+idx);
	}
	else {
		glDisable(GL_LIGHT0+idx);
	}
}

void GraphicsBackendOpenGL::enableDepthTesting(bool enable) {
	if(enable) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

void GraphicsBackendOpenGL::setPolygonMode(PolygonMode mode) {
	glPolygonMode(GL_FRONT_AND_BACK, gl_polygon_mode(mode));
}

void GraphicsBackendOpenGL::setAntialiasing(AntiAliasMode::t mode) {
	GLenum m = gl_antialias_mode(mode);
	glEnable(GL_POINT_SMOOTH_HINT);
	glEnable(GL_LINE_SMOOTH_HINT);
	glEnable(GL_POLYGON_SMOOTH_HINT);
	glHint(GL_POINT_SMOOTH_HINT, m);
	glHint(GL_LINE_SMOOTH_HINT, m);
	glHint(GL_POLYGON_SMOOTH_HINT, m);
	if (m!=AntiAliasMode::Fastest) {
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
	} else {
		glDisable(GL_POLYGON_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
	}
}

void GraphicsBackendOpenGL::color(const Color &c) {
	glColor4fv(c.to_ptr());
}

void GraphicsBackendOpenGL::pointSize(double v) {
	glPointSize(v);
}

void GraphicsBackendOpenGL::lineWidth(double v) {
	glLineWidth(v);
}

// Frame
void GraphicsBackendOpenGL::clear(int attribMask) {
	int bits = 
		(attribMask & AttributeBit::ColorBuffer ? GL_COLOR_BUFFER_BIT : 0) |
		(attribMask & AttributeBit::DepthBuffer ? GL_DEPTH_BUFFER_BIT : 0) |
		(attribMask & AttributeBit::Enable ? GL_ENABLE_BIT : 0) |
		(attribMask & AttributeBit::Viewport ? GL_VIEWPORT_BIT : 0);
	glClear(bits);
}

void GraphicsBackendOpenGL::clearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

// Coordinate Transforms
void GraphicsBackendOpenGL::viewport(int x, int y, int width, int height) {
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height);
	glViewport(x, y, width, height);
}

void GraphicsBackendOpenGL::setProjectionMatrix(Matrix4d &m) {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(m.elems);
}

void GraphicsBackendOpenGL::setModelviewMatrix(Matrix4d &m) {
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m.elems);
}

GLenum GraphicsBackendOpenGL::type_for_array_type(AlloTy type) {
	switch(type) {
		case AlloFloat32Ty: return GL_FLOAT;
		case AlloFloat64Ty: return GL_DOUBLE;

		case AlloSInt8Ty:	return GL_BYTE;
		case AlloSInt16Ty:	return GL_SHORT;
		case AlloSInt32Ty:	return GL_INT;
		
		case AlloUInt8Ty:	return GL_UNSIGNED_BYTE;
		case AlloUInt16Ty:	return GL_UNSIGNED_SHORT;
		case AlloUInt32Ty:	return GL_UNSIGNED_INT;
	}
	
	return GL_BYTE;
}

AlloTy GraphicsBackendOpenGL::array_type_for_type(GLenum type) {
	switch(type) {
		case GL_FLOAT:	return AlloFloat32Ty;
		case GL_DOUBLE: return AlloFloat64Ty;

		case GL_BYTE:	return AlloSInt8Ty;
		case GL_SHORT:	return AlloSInt16Ty;
		case GL_INT:	return AlloSInt32Ty;
		
		case GL_UNSIGNED_BYTE:	return AlloUInt8Ty;
		case GL_UNSIGNED_SHORT:	return AlloUInt16Ty;
		case GL_UNSIGNED_INT:	return AlloUInt32Ty;
	}
	
	return AlloSInt8Ty;
}

int GraphicsBackendOpenGL::components_from_format(GLenum format) {
	switch(format) {
		case GL_COLOR_INDEX:
		case GL_STENCIL_INDEX:
		case GL_DEPTH_COMPONENT:
		case GL_RED:
		case GL_GREEN:
		case GL_BLUE:
		case GL_ALPHA:
		case GL_LUMINANCE:			return 1;

		case GL_LUMINANCE_ALPHA:	return 2;
		case GL_RGB:				return 3;
		case GL_RGBA:
		default:					return 4;
	}
}

int GraphicsBackendOpenGL::bytes_from_type(GLenum type) {
	switch(type) {
		/*
		case GL_BITMAP:	??????
			type = sizeof(GLbitfield);
			break;
		*/

		case GL_UNSIGNED_SHORT:	return sizeof(GLushort);
		case GL_SHORT:			return sizeof(GLshort);
		case GL_UNSIGNED_INT:	return sizeof(GLuint);
		case GL_INT:			return sizeof(GLint);
		case GL_FLOAT:			return sizeof(GLfloat);
		case GL_BYTE:			return sizeof(GLbyte);

		case GL_UNSIGNED_BYTE:
		default:				return sizeof(GLubyte);
	}
}

GLsizeiptrARB GraphicsBackendOpenGL::size_from_buffer_parameters(GLenum format, GLenum type, int *dim) {
	int components = components_from_format(format);
	int bpp = bytes_from_type(type);

	return components*bpp*dim[0]*dim[1];
}

Texture::Target GraphicsBackendOpenGL::check_target(GraphicsBackendOpenGL *backend, Texture::Target target) {
	switch(target) {
		case Texture::TEXTURE_RECT:
			/*if(! Glob::extensions().haveRectTexture()) {
				fprintf(stderr, "Rectangular textures not supported on hard, reverting to 2D textures\n");
				return GL_TEXTURE_2D;
			}
			else return mTarget;
			*/
			return Texture::TEXTURE_RECT;

		case Texture::TEXTURE_3D:
			/*
			if(! Glob::extensions().have3DTexture()) {
				fprintf(stderr, "3D textures not supported on hard, reverting to 2D textures\n");
				return GL_TEXTURE_2D;
			}
			else return mTarget;
			*/
			return Texture::TEXTURE_3D;
		
		case Texture::TEXTURE_1D:
		case Texture::TEXTURE_2D:
		default:
			return target;
	}
}

Texture::Target GraphicsBackendOpenGL::texture_target_from_target(GraphicsBackendOpenGL *backend, GLenum target) {
	switch(target) {
		case GL_TEXTURE_1D:	return Texture::TEXTURE_1D;
		case GL_TEXTURE_2D:	return Texture::TEXTURE_2D;
		case GL_TEXTURE_RECTANGLE_ARB:	return Texture::TEXTURE_RECT;
		case GL_TEXTURE_3D:	return Texture::TEXTURE_3D;
	}
	return Texture::TEXTURE_2D;
}

GLenum GraphicsBackendOpenGL::target_from_texture_target(GraphicsBackendOpenGL *backend, Texture::Target target) {
	switch(target) {
		case Texture::TEXTURE_1D:	return GL_TEXTURE_1D;
		case Texture::TEXTURE_2D:	return GL_TEXTURE_2D;
		case Texture::TEXTURE_RECT:	return GL_TEXTURE_RECTANGLE_ARB;
		case Texture::TEXTURE_3D:	return GL_TEXTURE_3D;
	}
	
	return GL_TEXTURE_2D;
}

void GraphicsBackendOpenGL::clamp_texture_dimensions(GraphicsBackendOpenGL *backend, Texture::Target target, int &w, int &h, int &d) {
	/*
	switch(mTarget) {
		case GL_TEXTURE_RECTANGLE_ARB: {
			int sz = Glob::extensions().maxRectTextureSize();
			mWidth = (mWidth > sz) ? sz : mWidth;
			mHeight = (mHeight > sz) ? sz : mHeight;
		} break;
		
		case GL_TEXTURE_3D: {
			int sz = Glob::extensions().max3DTextureSize();
			mWidth = (mWidth > sz) ? sz : mWidth;
			mHeight = (mHeight > sz) ? sz : mHeight;
			mDepth = (mDepth > sz) ? sz : mDepth;
		} break;
		
		default: {
			int sz = Glob::extensions().maxTextureSize();
			mWidth = (mWidth > sz) ? sz : mWidth;
			mHeight = (mHeight > sz) ? sz : mHeight;
			mDepth = (mDepth > sz) ? sz : mDepth;
		} break;
	}
	*/
}

Texture::Wrap GraphicsBackendOpenGL::texture_wrap_from_wrap(GraphicsBackendOpenGL *backend, Texture::Target target, GLenum wrap) {
	switch(wrap) {
		case GL_CLAMP_TO_EDGE:		return Texture::CLAMP_TO_EDGE;
		case GL_CLAMP_TO_BORDER:	return Texture::CLAMP_TO_BORDER;
		case GL_CLAMP:				return Texture::CLAMP;
		case GL_MIRRORED_REPEAT:	return Texture::MIRRORED_REPEAT;
		case GL_REPEAT:				return Texture::REPEAT;
	}
	
	return Texture::CLAMP_TO_BORDER;
}

GLenum GraphicsBackendOpenGL::wrap_from_texture_wrap(GraphicsBackendOpenGL *backend, Texture::Target target, Texture::Wrap wrap) {
	if(target == Texture::TEXTURE_RECT) {
		switch(wrap) {
			case Texture::CLAMP_TO_EDGE:	return GL_CLAMP_TO_EDGE;
			case Texture::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
			case Texture::CLAMP:			return GL_CLAMP;
			//case Texture::REPEAT:
			default:
				return GL_CLAMP_TO_BORDER;
		}
	}
	else {
		switch(wrap) {
			case Texture::CLAMP_TO_EDGE:	return GL_CLAMP_TO_EDGE;
			case Texture::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
			case Texture::CLAMP:			return GL_CLAMP;
			case Texture::MIRRORED_REPEAT:	return GL_MIRRORED_REPEAT;
			case Texture::REPEAT:			return GL_REPEAT;
		}
	}
	
	return GL_CLAMP_TO_BORDER;
}

Texture::Filter GraphicsBackendOpenGL::texture_filter_from_filter(GraphicsBackendOpenGL *backend, GLenum filter) {
	switch(filter) {
		case GL_NEAREST:					return Texture::NEAREST;
		case GL_LINEAR:						return Texture::LINEAR;
		case GL_NEAREST_MIPMAP_NEAREST:		return Texture::NEAREST_MIPMAP_NEAREST;
		case GL_LINEAR_MIPMAP_NEAREST:		return Texture::LINEAR_MIPMAP_NEAREST;
		case GL_NEAREST_MIPMAP_LINEAR:		return Texture::NEAREST_MIPMAP_LINEAR;
		case GL_LINEAR_MIPMAP_LINEAR:		return Texture::LINEAR_MIPMAP_LINEAR;
	}
	
	return Texture::LINEAR;
}

GLenum GraphicsBackendOpenGL::filter_from_texture_filter(GraphicsBackendOpenGL *backend, Texture::Filter filter) {
	switch(filter) {
		case Texture::NEAREST:					return GL_NEAREST;
		case Texture::LINEAR:					return GL_LINEAR;
		case Texture::NEAREST_MIPMAP_NEAREST:	return GL_NEAREST_MIPMAP_NEAREST;
		case Texture::LINEAR_MIPMAP_NEAREST:	return GL_LINEAR_MIPMAP_NEAREST;
		case Texture::NEAREST_MIPMAP_LINEAR:	return GL_NEAREST_MIPMAP_LINEAR;
		case Texture::LINEAR_MIPMAP_LINEAR:		return GL_LINEAR_MIPMAP_LINEAR;
	}
	
	return GL_LINEAR;
}

Texture::Format GraphicsBackendOpenGL::texture_format_from_format(GraphicsBackendOpenGL *backend, GLenum format) {
	switch(format) {
		case GL_LUMINANCE: return Texture::LUMINANCE;
		case GL_ALPHA: return Texture::ALPHA;
		case GL_LUMINANCE_ALPHA: return Texture::LUMALPHA;
		case GL_RGB: return Texture::RGB;
		case GL_BGR: return Texture::BGR;
		case GL_RGBA: return Texture::RGBA;
		case GL_BGRA: return Texture::BGRA;
	}
	
	return Texture::RGBA;
}

GLenum GraphicsBackendOpenGL::format_from_texture_format(GraphicsBackendOpenGL *backend, Texture::Format format) {
	switch(format) {
		case Texture::LUMINANCE: return GL_LUMINANCE;
		case Texture::ALPHA:	return GL_ALPHA;
		case Texture::LUMALPHA: return GL_LUMINANCE_ALPHA;
		case Texture::RGB:		return GL_RGB;
		case Texture::BGR:		return GL_RGB;
		case Texture::RGBA:		return GL_RGBA;
		case Texture::BGRA:		return GL_RGBA;
	}
	
	return GL_RGBA;
}

Texture::Type GraphicsBackendOpenGL::texture_type_from_type(GraphicsBackendOpenGL *backend, GLenum type) {
	switch(type) {
		case GL_UNSIGNED_BYTE:	return Texture::UCHAR;
		case GL_INT:			return Texture::INT;
		case GL_UNSIGNED_INT:	return Texture::UINT;
		case GL_FLOAT:			return Texture::FLOAT32;
	}
	
	return Texture::UCHAR;
}

GLenum GraphicsBackendOpenGL::type_from_texture_type(GraphicsBackendOpenGL *backend, Texture::Type type) {
	switch(type) {
		case Texture::UCHAR:	return GL_UNSIGNED_BYTE;
		case Texture::INT:		return GL_INT;
		case Texture::UINT:		return GL_UNSIGNED_INT;
		case Texture::FLOAT32:	return GL_FLOAT;
	}
	
	return GL_UNSIGNED_BYTE;
}

GLenum GraphicsBackendOpenGL::internal_format_from_format(GraphicsBackendOpenGL *backend, Texture::Format format, Texture::Type type) {
	if(type == Texture::FLOAT32) {// check for extension && mType == GL_FLOAT) {
		switch(format) {
			case Texture::LUMINANCE: return GL_LUMINANCE32F_ARB;
			case Texture::ALPHA: return GL_ALPHA32F_ARB;
			case Texture::LUMALPHA: return GL_LUMINANCE_ALPHA32F_ARB;
			case Texture::RGB: return GL_RGB32F_ARB;
			case Texture::BGR: return GL_RGB32F_ARB;
			case Texture::RGBA: return GL_RGBA32F_ARB;
			case Texture::BGRA: return GL_RGBA32F_ARB;
		}
	}
	else {
		switch(format) {
			case Texture::LUMINANCE: return GL_LUMINANCE;
			case Texture::ALPHA: return GL_ALPHA;
			case Texture::LUMALPHA: return GL_LUMINANCE_ALPHA;
			case Texture::RGB: return GL_RGB;
			case Texture::BGR: return GL_RGB;
			case Texture::RGBA: return GL_RGBA;
			case Texture::BGRA: return GL_RGBA;
		}
	}
	
	return GL_RGBA;
}


void GraphicsBackendOpenGL::textureCreate(Texture *tex) {
	GLuint texid = 0;
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texid);
	
	if(texid <= 0) return;
	
	tex->id((int)texid);
	

	// get the OpenGL texture target
	tex->target(
		check_target(this, tex->target())
	);
	GLenum gltarget = target_from_texture_target(
		this,
		tex->target()
	);
	

	glBindTexture(gltarget, texid);
	

	// check for valid dimensions
	int w, h, d;
	tex->getDimensions(w, h, d);
	clamp_texture_dimensions(this, tex->target(), w, h, d);

	
	// set the texcoord wrapping
	GLenum glwrap = wrap_from_texture_wrap(
		this,
		tex->target(),
		tex->wrap()
	);
	if(gltarget == GL_TEXTURE_RECTANGLE_ARB || gltarget == GL_TEXTURE_2D) {
		glTexParameteri(gltarget, GL_TEXTURE_WRAP_S, glwrap);
		glTexParameteri(gltarget, GL_TEXTURE_WRAP_T, glwrap);
	}
	else {
		glTexParameteri(gltarget, GL_TEXTURE_WRAP_S, glwrap);
		glTexParameteri(gltarget, GL_TEXTURE_WRAP_T, glwrap);
		glTexParameteri(gltarget, GL_TEXTURE_WRAP_R, glwrap);
	}
	
	if(gltarget == GL_TEXTURE_3D) {
		glTexParameteri(gltarget, GL_TEXTURE_WRAP_R, glwrap);

		#ifdef MURO_APPLE_VERSION	//this is due to a bug in the nvidia drivers on osx for z-slice updating
		glTexParameteri(gltarget, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_CACHED_APPLE);
		#endif
	}

	
	// set the filters and border color
	GLenum glmagfilter = filter_from_texture_filter(this, tex->magFilter());
	GLenum glminfilter = filter_from_texture_filter(this, tex->minFilter());
	glTexParameteri(gltarget, GL_TEXTURE_MAG_FILTER, glmagfilter);
	glTexParameteri(gltarget, GL_TEXTURE_MIN_FILTER, glminfilter);
	glTexParameterfv(gltarget, GL_TEXTURE_BORDER_COLOR, tex->borderColor().to_ptr());


	GLenum glformat = format_from_texture_format(this, tex->format());
	GLenum gltype = type_from_texture_type(this, tex->type());
	GLenum iformat = internal_format_from_format(this, tex->format(), tex->type());
	
	switch(gltarget) {
		case GL_TEXTURE_1D:
			glTexImage1D(gltarget, 0, iformat, w, 0, glformat, gltype, NULL);
			break;
			
		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE_ARB:
			glTexImage2D(gltarget, 0, iformat, w, h, 0, glformat, gltype, NULL);
			break;

		case GL_TEXTURE_3D:
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexImage3D(gltarget, 0, iformat, w, h, d, 0, glformat, gltype, NULL);
			break;
	}


	glBindTexture(gltarget, 0);
	glDisable(gltarget);
	
	//	mRebuild = false;
	//	mUpdate = false;

	/*
	if(pixels == 0 && mStartClear) {
		err = clear();
		if(err) return err;
	}
	*/
	gl_error("creating texture");
}

void GraphicsBackendOpenGL::textureDestroy(Texture *tex) {
	if(tex->created()) {
		GLuint tex_id = (GLuint)tex->id();
		glDeleteTextures(1, &tex_id);
		tex->id(0);
	}
	
	gl_error("destroying texture");
}

void GraphicsBackendOpenGL::textureBind(Texture *tex, int unit) {
	// multitexturing
	glActiveTextureARB( GL_TEXTURE0_ARB+unit );
	//printf("bind active unit: %d -> %d\n", unit, tex->id());

	GLenum gltarget = target_from_texture_target(
		this, 
		tex->target()
	);
	glEnable(gltarget);
	glBindTexture(gltarget, tex->id());
	
	gl_error("binding texture");
}

void GraphicsBackendOpenGL::textureUnbind(Texture *tex, int unit) {
	GLenum gltarget = target_from_texture_target(
		this, 
		tex->target()
	);
	glBindTexture(gltarget, 0);
	glDisable(gltarget);
	
	gl_error("unbinding texture");
}

void GraphicsBackendOpenGL::textureEnter(Texture *tex, int unit) {
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	
	if(tex->target() == Texture::TEXTURE_RECT) {
		glScalef(tex->width(), tex->height(), 1.);
	}
	glMatrixMode(GL_MODELVIEW);

	// set texture environment
//	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, mCoordReplace);

	gl_error("entering texture");
}

void GraphicsBackendOpenGL::textureLeave(Texture *tex, int unit) {
	// multitexturing unwind
	glActiveTextureARB( GL_TEXTURE0_ARB+unit );
	//printf("unbind active unit: %d <- %d\n", unit, tex->id());

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	
	gl_error("leaving texture");
}

void GraphicsBackendOpenGL::textureSubmit(Texture *tex) {
	GLvoid *data = (GLvoid *)tex->getData();
	if(data) {
		GLenum gltarget = target_from_texture_target(
			this, 
			tex->target()
		);
		GLenum glformat = format_from_texture_format(this, tex->format());
		GLenum gltype = type_from_texture_type(this, tex->type());
		
		int alignment = tex->getRowStride() % 4;
		if(alignment == 0) {
			alignment = 4;
		}
		if(gltarget == GL_TEXTURE_1D) {
			alignment = 4;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		gl_error("submitting texture");
		switch(gltarget) {
			case GL_TEXTURE_1D:
				glTexSubImage1D(
					gltarget, 
					0, 0, 
					tex->width(), 
					glformat, 
					gltype, 
					data
				);
				break;
			case GL_TEXTURE_2D:
			case GL_TEXTURE_RECTANGLE_ARB:
				glTexSubImage2D(
					gltarget, 
					0, 0, 0, 
					tex->width(), tex->height(), 
					glformat, 
					gltype, 
					data
				);
				break;
			case GL_TEXTURE_3D:
				glTexSubImage3D(
					gltarget, 
					0, 0, 0, 0, 
					tex->width(), tex->height(), tex->depth(), 
					glformat, 
					gltype, 
					data
				);
				break;

			default:
				break;
		}
	}
	
	gl_error("submitting texture");
}

void GraphicsBackendOpenGL::textureSubSubmit(const Vec3i &offset, Texture *tex) {
	GLvoid *data = (GLvoid *)tex->getSubData();
	if(data) {
		GLenum gltarget = target_from_texture_target(
			this, 
			tex->target()
		);
		GLenum glformat = format_from_texture_format(this, tex->format());
		GLenum gltype = type_from_texture_type(this, tex->type());
		
		
		int alignment = tex->getRowStride() % 4;
		if(alignment == 0) {
			alignment = 4;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		
		switch(gltarget) {
			case GL_TEXTURE_1D:
				glTexSubImage1D(
					gltarget, 
					0, 
					offset[0], 
					tex->width(), 
					glformat, 
					gltype, 
					data
				);
				break;
			case GL_TEXTURE_2D:
			case GL_TEXTURE_RECTANGLE_ARB:
				glTexSubImage2D(
					gltarget, 
					0, 
					offset[0], offset[1],
					tex->width(), tex->height(), 
					glformat, 
					gltype, 
					data
				);
				break;
			case GL_TEXTURE_3D:
				glTexSubImage3D(
					gltarget, 
					0, 
					offset[0], offset[1], offset[2],
					tex->width(), tex->height(), tex->depth(), 
					glformat, 
					gltype, 
					data
				);
				break;

			default:
				break;
		}
	}
	
	gl_error("sub-submitting texture");
}

void GraphicsBackendOpenGL::textureToArray(Texture *tex) {
	if(tex->mode() == Texture::SURFACE) {
		Surface *surface = tex->surface();
		if(surface) {
			SurfaceData *surface_data = (SurfaceData *)surface->surfaceData();
			PBO *pbo = surface_data->pbo;
			pbo->bind();
			pbo->fromTexture(tex, format_from_texture_format(this, tex->format()));
			pbo->unbind();
			pbo->toArray(tex->array());
		}
	}
	else {
		// glTexImage
	}
}

GLenum check_fbo_status(Surface *surface) {
	GLint fboid;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fboid);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)surface->id());

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status) {
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
			
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			printf("framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");
			/* you gotta choose different formats */
			break;
			
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			printf("framebuffer INCOMPLETE_ATTACHMENT\n");
			break;
		
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			printf("framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");
			break;
		
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			printf("framebuffer FRAMEBUFFER_DIMENSIONS\n");
			break;
		
		case 0x8CD8: //GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
			printf("framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n");
			break;
			
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			printf("framebuffer INCOMPLETE_FORMATS\n");
			break;
			
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			printf("framebuffer INCOMPLETE_DRAW_BUFFER\n");
			break;
			
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			printf("framebuffer INCOMPLETE_READ_BUFFER\n");
			break;
			
		case GL_FRAMEBUFFER_BINDING_EXT:
			printf("framebuffer BINDING_EXT\n");
			break;
			
		case 0x8CDE: // GL_FRAMEBUFFER_STATUS_ERROR_EXT:
			printf("framebuffer STATUS_ERROR\n");
			break;
			
		default:
			/* programming error; will fail on all hardware */
			//exit(0);
			break;
	}
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)fboid);
	
	return status;
}

Surface * GraphicsBackendOpenGL::surfaceNew() {
	SurfaceData *surface_data = new SurfaceData();
	return new Surface(this, surface_data);
}

void GraphicsBackendOpenGL::surfaceAttachTexture(Surface *surface, Texture *tex) {
	// in case there's already an FBO bound, we want it rebound after this operation
	GLint fbo_id;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fbo_id);
	
	// attach the texture to the surface
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, surface->id());
	int unit = tex->unit();
	tex->bind(unit);
		glFramebufferTexture2DEXT(
			GL_FRAMEBUFFER_EXT, 
			GL_COLOR_ATTACHMENT0_EXT, 
			target_from_texture_target(
				this, 
				tex->target()
			), 
			tex->id(), 
			0
		);
	tex->unbind(unit);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)fbo_id);
	
	gl_error("attaching texture to surface");
}

void GraphicsBackendOpenGL::surfaceAttachDepthbuffer(Surface *surface) {
	int width = surface->width();
	int height = surface->height();
	
	SurfaceData *surface_data = (SurfaceData *)surface->surfaceData();
	
	// generate a new renderbuffer
	GLuint depth_id = 0;
	glGenRenderbuffersEXT(1, &depth_id);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_id);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	surface_data->depth_id = depth_id;

	// in case there's already an FBO bound, we want it rebound after this operation
	GLint fbo_id = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fbo_id);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, surface->id());
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_id);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)fbo_id);
	
	gl_error("attaching depth buffer to surface");
}

void GraphicsBackendOpenGL::surfaceCreate(Surface *surface) {
	GLuint fbo_id = 0;
	glGenFramebuffersEXT(1, &fbo_id);
	surface->id(fbo_id);
	
	surfaceAttachTexture(surface, surface->texture());

	// attach depth buffer
	surfaceAttachDepthbuffer(surface);
	
	// see if we're FBO complete
	check_fbo_status(surface);
	
	gl_error("creating surface");
}

void GraphicsBackendOpenGL::surfaceDestroy(Surface *surface) {
	if(surface->created()) {
		GLuint fbo_id = (GLuint)surface->id();
		glDeleteFramebuffersEXT(1, &fbo_id);
		surface->id(0);
		
		SurfaceData *surface_data = (SurfaceData *)surface->surfaceData();
		surface_data->depth_id = 0;
		surface_data->pbo->destroy();
	}
	
	gl_error("destroying surface");
}

void GraphicsBackendOpenGL::surfaceBind(Surface *surface) {
	glPushAttrib(GL_VIEWPORT_BIT);
	glDisable(GL_SCISSOR_TEST);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	
	/*
	GLint fboid;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fboid);
	mPrevfboid = (GLuint)fboid;
	*/
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, surface->id());
}

void GraphicsBackendOpenGL::surfaceUnbind(Surface *surface) {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
}

void GraphicsBackendOpenGL :: surfaceEnter(Surface *surface) {
	glViewport(0, 0, surface->width(), surface->height());
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();	
	
	if(surface->autoClear()) {
		Color &c = surface->clearColor();
		glClearColor(c.r, c.g, c.b, c.a);
		glClearDepth(surface->clearDepth());
		
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		/*
		for(int i=0; i < mNumAttachments; i++) {
			glDrawBuffer(bufferAttachPoint(i));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		*/
	}
	
	gl_error("entering surface");
}

void GraphicsBackendOpenGL::surfaceLeave(Surface *surface) {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	gl_error("leaving surface");
}

void GraphicsBackendOpenGL::surfaceClear(Surface *surface) {
	surfaceBind(surface);
	surfaceEnter(surface);
	surfaceLeave(surface);
	surfaceUnbind(surface);
	
	gl_error("clearing surface");
}

void GraphicsBackendOpenGL::surfaceCopy(Surface *surface, Texture *texture) {
	glPushAttrib(GL_COLOR_BUFFER_BIT);	// to save blending state
	glDisable(GL_BLEND);

	surface->enter();
	texture->bind(0);

	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -100, 100);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

				glLoadIdentity();
				glBegin(GL_QUADS);
					glColor4f(1., 1., 1., 1.);
					glTexCoord2f(0., 1.);
					glVertex3f(-1., 1., 0.);

					glTexCoord2f(1., 1.);
					glVertex3f(1., 1., 0.);

					glTexCoord2f(1., 0.);
					glVertex3f(1., -1., 0.);

					glTexCoord2f(0., 0.);
					glVertex3f(-1., -1., 0.);

				glEnd();

			glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	texture->unbind(0);
	surface->leave();

	glPopAttrib();	// to restore blending state
	
	gl_error("copying surface");
}

void tex_coords(int unit, float x, float y) {
	for(int i=0; i <= unit; i++) {
		glMultiTexCoord2d(GL_TEXTURE0_ARB+i, x, y);
	}
}

void GraphicsBackendOpenGL::slabDraw(Slab *slab, int argc, Texture **argv) {
	glPushAttrib(GL_COLOR_BUFFER_BIT);	//to save blending state
	glDisable(GL_BLEND);
	
	// trigger texture rebuild before trying to use to avoid texture unit smashing
	for(int i=0; i < argc; i++) {
		if(argv[i]->needsRebuild()) {
			argv[i]->bind(i);
			argv[i]->unbind(i);
		}
	}

	slab->get_surface(0)->enter();
	
	for(int i=0; i < argc; i++) {
		argv[i]->bind(i);
	}
	
	slab->get_shader()->bind();
	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -100, 100);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

				glLoadIdentity();
				glBegin(GL_QUADS);
					//tex coords for rect textures are scaled by texture matrix
					glColor4f(1., 1., 1., 1.);
					
					tex_coords(argc, 0., 1.);
					glVertex3f(-1., 1., 0.);
					
					tex_coords(argc, 1., 1.);
					glVertex3f(1., 1., 0.);
					
					tex_coords(argc, 1., 0.);
					glVertex3f(1., -1., 0.);
					
					tex_coords(argc, 0., 0.);
					glVertex3f(-1., -1., 0.);
					
				glEnd();
				
			glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
			
	glMatrixMode(GL_MODELVIEW);	
	slab->get_shader()->unbind();
	
	for(int i=0; i < argc; i++) {
		argv[i]->unbind(i);
	}
	
	slab->get_surface(0)->leave();
	
	glPopAttrib();	// restores blend/color state
	
	gl_error("drawing slab");
}

// Buffer drawing
void GraphicsBackendOpenGL::draw(const Mesh& v) {

	int Nv = v.vertices().size();
	if(0 == Nv) return;
	
	int Nc = v.colors().size();
	int Nn = v.normals().size();
	int Nt2= v.texCoord2s().size();
	int Nt3= v.texCoord3s().size();
	int Ni = v.indices().size();

	// Enable arrays and set pointers...
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &v.vertices()[0]);
	//glVertexPointer(3, GL_FLOAT, 0, (GLvoid *)v.vertices().elems());

	if(Nn){
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &v.normals()[0]);
	}
	
	if(Nc){
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, &v.colors()[0]);			
	}
	
	if(Nt2 || Nt3){
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if(Nt2) glTexCoordPointer(2, GL_FLOAT, 0, &v.texCoord2s()[0]);
		if(Nt3) glTexCoordPointer(3, GL_FLOAT, 0, &v.texCoord3s()[0]);
	}
	
	if(Ni){
		//unsigned vs=0, ve=Nv;	// range of vertex indices to prefetch
								// NOTE:	if this range exceeds the number of vertices,
								//			expect a segmentation fault...
		unsigned is=0, ie=Ni;	// range of indices to draw

//		glDrawRangeElements(v.primitive(), vs, ve, ie-is, GL_UNSIGNED_INT, &v.indices()[is]);
		glDrawElements(
			gl_primitive(v.primitive()), 
			ie-is, 
			GL_UNSIGNED_INT, 
			&v.indices()[is]
		);
	}
	else{
		glDrawArrays(
			gl_primitive(v.primitive()), 
			0, 
			v.vertices().size()
		);
	}
	


	if(Nc) glDisableClientState(GL_COLOR_ARRAY);
	if(Nt2 || Nt3) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if(Nn) glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

} // ::al::gfx
} // ::al
