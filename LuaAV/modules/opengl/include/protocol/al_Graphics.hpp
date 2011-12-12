#ifndef INCLUDE_AL_GRAPHICS_HPP
#define INCLUDE_AL_GRAPHICS_HPP

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

/*
	Example code:
	
	Graphics gl();
	
	gl.begin(gl.POINTS);
	gl.vertex3d(0, 0, 0);
	gl.end();
*/

#include "math/al_Vec.hpp"
#include "math/al_Matrix4.hpp"
#include "types/al_Buffer.hpp"
#include "types/al_Color.hpp"

#include <stack>
using std::stack;

namespace al{
namespace gfx{

class Mesh;

enum BlendFunc {
	SRC_ALPHA = 0,
	SRC_COLOR,
	DST_ALPHA,
	DST_COLOR,
	ZERO,
	ONE,
	SRC_ALPHA_SATURATE
};

enum PolygonMode {
	POINT = 0,
	LINE,
	FILL
};

enum MatrixMode {
	MODELVIEW = 0,
	PROJECTION
};

enum Primitive {
	POINTS = 0,
	LINES,
	LINE_STRIP,
	LINE_LOOP,
	TRIANGLES,
	QUADS,
	POLYGON
};

enum Face {
	FRONT = 0,
	BACK,
	FRONT_AND_BACK
};

namespace AntiAliasMode {
	enum t{
		DontCare	= 1<<0,	
		Fastest		= 1<<1,
		Nicest		= 1<<2
	};
	inline t operator| (const t& a, const t& b){ return t(int(a) | int(b)); }
	inline t operator& (const t& a, const t& b){ return t(int(a) & int(b)); }
}

namespace AttributeBit {
	enum t{
		ColorBuffer	= 1<<0,		/**< Color-buffer bit */
		DepthBuffer	= 1<<1,		/**< Depth-buffer bit */
		Enable		= 1<<2,		/**< Enable bit */
		Viewport	= 1<<3		/**< Viewport bit */
	};
	inline t operator| (const t& a, const t& b){ return t(int(a) | int(b)); }
	inline t operator& (const t& a, const t& b){ return t(int(a) & int(b)); }
}

struct StateChange {
	StateChange()
	:	blending(false),
		lighting(false),
		depth_testing(false),
		polygon_mode(false),
		antialiasing(false)
	{}
	
	~StateChange(){}

	bool blending;
	bool lighting;
	bool depth_testing;
	bool polygon_mode;
	bool antialiasing;
};


struct State {
	State()
	:	blend_enable(false),
		blend_src(SRC_COLOR),
		blend_dst(DST_COLOR),
		lighting_enable(false),
		depth_enable(true),
		polygon_mode(FILL),
		antialias_mode(AntiAliasMode::DontCare)
	{}
	
	~State() {}
	

	// Blending
	bool blend_enable;
	BlendFunc blend_src;
	BlendFunc blend_dst;
	
	// Lighting
	bool lighting_enable;
	
	// Depth Testing
	bool depth_enable;
	
	// Polygon Mode
	PolygonMode polygon_mode;
	
	// Anti-Aliasing
	AntiAliasMode::t antialias_mode;
	
};


class GraphicsBackend;

/*
	Graphics knows how to draw GraphicsData
	It also owns a GraphicsData, to simulate immediate mode (where it draws its own data)
*/	
class Graphics {
public:

//	Graphics(gfx::Backend::type backend = gfx::Backend::AutoDetect);
	Graphics(GraphicsBackend *backend);
	~Graphics();
	
	// Rendering State
	void pushState(State &state);
	void popState();

	// Frame
	void clear(int attribMask);
	void clearColor(float r, float g, float b, float a);
	void clearColor(const Color& color) { clearColor(color.r, color.g, color.b, color.a); }

	// Coordinate Transforms
	void viewport(int x, int y, int width, int height);
	void matrixMode(MatrixMode mode);
	MatrixMode matrixMode();
	void pushMatrix();
	void popMatrix();
	void loadIdentity();
	void loadMatrix(const Matrix4d &m);
	void multMatrix(const Matrix4d &m);
	void translate(double x, double y, double z);
	void translate(const Vec3d& v) { translate(v[0], v[1], v[2]); }
	void translate(const Vec3f& v) { translate(v[0], v[1], v[2]); }
	void rotate(double angle, double x, double y, double z);
	void rotate(double angle, const Vec3d& v) { rotate(angle, v[0], v[1], v[2]); }
	void scale(double x, double y, double z);
	void scale(double s) { scale(s, s, s); }
	void scale(const Vec3d& v) { scale(v[0], v[1], v[2]); }
	void scale(const Vec3f& v) { scale(v[0], v[1], v[2]); }
	
	// Immediate Mode
	void begin(Primitive mode);
	void end();
	

	void vertex(double x, double y, double z=0.);
	void vertex(const Vec3d& v) { vertex(v[0], v[1], v[2]); }
	void vertex(const Vec3f& v) { vertex(v[0], v[1], v[2]); }
	void texcoord(double u, double v);
	void normal(double x, double y, double z=0.);
	void normal(const Vec3d& v) { normal(v[0], v[1], v[2]); }
	void normal(const Vec3f& v) { normal(v[0], v[1], v[2]); }
	void color(double r, double g, double b, double a=1.);
	void color(const Vec3d& v, double a=1.) { color(v[0], v[1], v[2], a); }
	void color(const Vec3f& v, double a=1.) { color(v[0], v[1], v[2], a); }

	// Other state
	void pointSize(double v);
	void lineWidth(double v);
	
	// Buffer drawing
	void draw(const Mesh& v);
	void draw();
	
	GraphicsBackend * backend() { return mBackend; }
	Mesh& data() { return *mGraphicsData; }
	
	
protected:
	void compareState(State &prev_state, State &state);
	void enableState();
	void drawBegin();
	void drawEnd();
	
	stack<Matrix4d> & matrixStackForMode(MatrixMode mode);
	
	
protected:
	GraphicsBackend	*	mBackend;			// graphics API implementation
	Mesh				*mGraphicsData;		// used for immediate mode style rendering
	bool				mInImmediateMode;	// flag for whether or not in immediate mode
	MatrixMode			mMatrixMode;		// matrix stack to use
	stack<Matrix4d>		mProjectionMatrix;	// projection matrix stack
	stack<Matrix4d>		mModelViewMatrix;	// modelview matrix stack
	StateChange			mStateChange;		// state difference to mark changes
	stack<State>		mState;				// state stack

};

/*
	Abstract base class for any object that can be rendered via Graphics:
*/	
class Drawable {
public:
	virtual void draw(Graphics& gl) = 0;

};

} // ::al::gfx
} // ::al
	
#endif
	
