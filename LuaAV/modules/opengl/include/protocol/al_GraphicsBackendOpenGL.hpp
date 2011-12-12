#ifndef INCLUDE_AL_GRAPHICS_BACKEND_OPENGL_HPP
#define INCLUDE_AL_GRAPHICS_BACKEND_OPENGL_HPP

/*
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/


#include "protocol/al_GraphicsBackend.hpp"

namespace al{
namespace gfx{

class PBO;

class GraphicsBackendOpenGL : public GraphicsBackend {
public:
	static void gl_error(const char *msg);
	static GLenum gl_primitive(Primitive prim);
	static Primitive primitive(GLenum prim);
	static GLenum type_for_array_type(AlloTy type);
	static AlloTy array_type_for_type(GLenum type);
	static int components_from_format(GLenum format);
	static int bytes_from_type(GLenum type);
	static GLsizeiptrARB size_from_buffer_parameters(GLenum format, GLenum type, int *dim);

	static Texture::Target check_target(GraphicsBackendOpenGL *backend, Texture::Target target);
	static Texture::Target texture_target_from_target(GraphicsBackendOpenGL *backend, GLenum target);
	static GLenum target_from_texture_target(GraphicsBackendOpenGL *backend, Texture::Target target);
	static void clamp_texture_dimensions(GraphicsBackendOpenGL *backend, Texture::Target target, int &w, int &h, int &d);
	static Texture::Wrap texture_wrap_from_wrap(GraphicsBackendOpenGL *backend, Texture::Target target, GLenum wrap);
	static GLenum wrap_from_texture_wrap(GraphicsBackendOpenGL *backend, Texture::Target target, Texture::Wrap wrap);
	static Texture::Filter texture_filter_from_filter(GraphicsBackendOpenGL *backend, GLenum filter);
	static GLenum filter_from_texture_filter(GraphicsBackendOpenGL *backend, Texture::Filter filter);
	static Texture::Format texture_format_from_format(GraphicsBackendOpenGL *backend, GLenum format);
	static GLenum format_from_texture_format(GraphicsBackendOpenGL *backend, Texture::Format format);
	static Texture::Type texture_type_from_type(GraphicsBackendOpenGL *backend, GLenum type);
	static GLenum type_from_texture_type(GraphicsBackendOpenGL *backend, Texture::Type type);
	static GLenum internal_format_from_format(GraphicsBackendOpenGL *backend, Texture::Format format, Texture::Type type);

public:

	struct SurfaceData{
		SurfaceData();
		~SurfaceData();
	
		int depth_id;
		PBO *pbo;
	};
	
	static Graphics * makeGraphics() {
		return new Graphics(new GraphicsBackendOpenGL());
	}

	GraphicsBackendOpenGL();
	virtual ~GraphicsBackendOpenGL();
	
	// Render State
	virtual void enableBlending(bool enable, BlendFunc src, BlendFunc dst);
	virtual void enableLighting(bool enable);
	virtual void enableLight(bool enable, int idx);
	virtual void enableDepthTesting(bool enable);
	virtual void setPolygonMode(PolygonMode mode);
	virtual void setAntialiasing(AntiAliasMode::t mode);
	virtual void color(const Color &c);
	virtual void pointSize(double v);
	virtual void lineWidth(double v);
	
	// Frame
	virtual void clear(int attribMask);
	virtual void clearColor(float r, float g, float b, float a);
	
	// Coordinate Transforms
	virtual void viewport(int x, int y, int width, int height);
	virtual void setProjectionMatrix(Matrix4d &m);
	virtual void setModelviewMatrix(Matrix4d &m);
	
	// Textures
	virtual void textureCreate(Texture *tex);
	virtual void textureDestroy(Texture *tex);
	virtual void textureBind(Texture *tex, int unit);
	virtual void textureUnbind(Texture *tex, int unit);
	virtual void textureEnter(Texture *tex, int unit);
	virtual void textureLeave(Texture *tex, int unit);
	virtual void textureSubmit(Texture *tex);
	virtual void textureSubSubmit(const Vec3i &offset, Texture *tex);
	virtual void textureToArray(Texture *tex);
	
	// surfaces
	virtual Surface * surfaceNew();

protected:
	void surfaceAttachTexture(Surface *surface, Texture *tex);
	void surfaceAttachDepthbuffer(Surface *surface);

public:	
	virtual void surfaceCreate(Surface *surface);
	virtual void surfaceDestroy(Surface *surface);
	virtual void surfaceBind(Surface *surface);
	virtual void surfaceUnbind(Surface *surface);
	virtual void surfaceEnter(Surface *surface);
	virtual void surfaceLeave(Surface *surface);
	virtual void surfaceClear(Surface *surface);
	virtual void surfaceCopy(Surface *surface, Texture *texture);
	
	
	virtual void slabDraw(Slab *slab, int argc, Texture **argv);
	
	// Buffer drawing
	virtual void draw(const Mesh& v);
	
//	virtual int test();
//	virtual void test(const Mesh& v);
};

} // ::al::gfx
} // ::al
	
#endif
	
