#ifndef INCLUDE_AL_GRAPHICS_BACKEND_HPP
#define INCLUDE_AL_GRAPHICS_BACKEND_HPP

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


#include "math/al_Vec.hpp"
#include "types/al_Buffer.hpp"
#include "types/al_Color.hpp"
#include "protocol/al_Graphics.hpp"
#include "graphics/al_Texture.hpp"
#include "graphics/al_Surface.hpp"

namespace al{
namespace gfx{

class Slab;

class GraphicsBackend {
public:

	GraphicsBackend() {}
	virtual ~GraphicsBackend() {}
	
	// Render State
	virtual void enableBlending(bool enable, BlendFunc src, BlendFunc dst) = 0;
	virtual void enableLighting(bool enable) = 0;
	virtual void enableLight(bool enable, int idx) = 0;
	virtual void enableDepthTesting(bool enable) = 0;
	virtual void setPolygonMode(PolygonMode mode) = 0;
	virtual void setAntialiasing(AntiAliasMode::t mode) = 0;
	virtual void color(const Color &c) = 0;
	virtual void pointSize(double v) = 0;
	virtual void lineWidth(double v) = 0;
	
	// Frame
	virtual void clear(int attribMask) = 0;
	virtual void clearColor(float r, float g, float b, float a) = 0;
	
	// Coordinate Transforms
	virtual void viewport(int x, int y, int width, int height) = 0;
	virtual void setProjectionMatrix(Matrix4d &m) = 0;
	virtual void setModelviewMatrix(Matrix4d &m) = 0;
	
	// Textures
	Texture * textureNew() { return new Texture(this); }
	virtual void textureCreate(Texture *tex) = 0;
	virtual void textureDestroy(Texture *tex) = 0;
	virtual void textureBind(Texture *tex, int unit) = 0;
	virtual void textureUnbind(Texture *tex, int unit) = 0;
	virtual void textureEnter(Texture *tex, int unit) = 0;
	virtual void textureLeave(Texture *tex, int unit) = 0;
	virtual void textureSubmit(Texture *tex) = 0;
	virtual void textureSubSubmit(const Vec3i &offset, Texture *tex) = 0;
	virtual void textureToArray(Texture *tex) = 0;
	
	// surfaces
	virtual Surface * surfaceNew() = 0;
	virtual void surfaceCreate(Surface *surface) = 0;
	virtual void surfaceDestroy(Surface *surface) = 0;
	virtual void surfaceBind(Surface *surface) = 0;
	virtual void surfaceUnbind(Surface *surface) = 0;
	virtual void surfaceEnter(Surface *surface) = 0;
	virtual void surfaceLeave(Surface *surface) = 0;
	virtual void surfaceClear(Surface *surface) = 0;
	virtual void surfaceCopy(Surface *surface, Texture *texture) = 0;
	
	virtual void slabDraw(Slab *slab, int argc, Texture **argv) = 0;
	
	// Buffer drawing
	virtual void draw(const Mesh& v) = 0;
	
//	int dummy;
//	virtual int test() = 0;
//	virtual void test(const Mesh& v) = 0;
};

} // ::al::gfx
} // ::al
	
#endif
	
