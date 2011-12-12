#include <vector>
#include <map>
#include <string>

#include "protocol/al_Graphics.hpp"
#include "protocol/al_GraphicsBackend.hpp"
#include "graphics/al_Mesh.hpp"

namespace al {
namespace gfx{

Graphics::Graphics(GraphicsBackend *backend)
:	mBackend(backend),
	mGraphicsData(new Mesh(backend)),
	mInImmediateMode(false),
	mMatrixMode(MODELVIEW)
{
	mProjectionMatrix.push(Matrix4d::identity());
	mModelViewMatrix.push(Matrix4d::identity());
	State nullstate;
	nullstate.depth_enable = false;
	mState.push(nullstate);
}

Graphics::~Graphics() {
	delete mGraphicsData;
}


// Rendering State
void Graphics::pushState(State &state) {
	compareState(mState.top(), state);
	mState.push(state);
}

void Graphics::popState() {
	State &prev_state = mState.top();
	mState.pop();
	compareState(prev_state, mState.top());
}

void Graphics::compareState(State &prev_state, State &state) {
	// blending
	if(	prev_state.blend_enable != state.blend_enable || 
		prev_state.blend_src != state.blend_src || 
		prev_state.blend_dst != state.blend_dst)
	{
		mStateChange.blending = true;
	}
	
	// lighting
	if(prev_state.lighting_enable != state.lighting_enable) {
		mStateChange.lighting = true;
	}
	
	// depth testing
	if(prev_state.depth_enable != state.depth_enable) {
		mStateChange.depth_testing = true;
	}
	
	// polygon mode
	if(prev_state.polygon_mode != state.polygon_mode) {
		mStateChange.polygon_mode = true;
	}
	
	// anti-aliasing
	if(prev_state.antialias_mode != state.antialias_mode) {
		mStateChange.antialiasing = true;
	}
	
}


// Frame
void Graphics::clear(int attribMask) {
	mBackend->clear(attribMask);
}

void Graphics::clearColor(float r, float g, float b, float a) {
	mBackend->clearColor(r, g, b, a);
}


// Coordinate Transforms
void Graphics::viewport(int x, int y, int width, int height) {
	mBackend->viewport(x, y, width, height);
}

void Graphics::matrixMode(MatrixMode mode) {
	mMatrixMode = mode;
}

MatrixMode Graphics::matrixMode() {
	return mMatrixMode;
}

stack<Matrix4d> & Graphics::matrixStackForMode(MatrixMode mode) {
	if(mode == PROJECTION) {
		return mProjectionMatrix;
	}
	else {
		return mModelViewMatrix;
	}
}

void Graphics::pushMatrix() {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.push(matrix_stack.top());
}

void Graphics::popMatrix() {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.pop();
}

void Graphics::loadIdentity() {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.top() = Matrix4d::identity();
}

void Graphics::loadMatrix(const Matrix4d &m) {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.top() = m;
}

void Graphics::multMatrix(const Matrix4d &m) {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.top() *= m;
}

void Graphics::translate(double x, double y, double z) {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.top() *= Matrix4d::translate(x, y, z);
}

void Graphics::rotate(double angle, double x, double y, double z) {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.top() *= Matrix4d::rotate(angle, Vec3d(x, y, z)); // * matrix_stack.top();
}

void Graphics::scale(double x, double y, double z) {
	stack<Matrix4d> &matrix_stack = matrixStackForMode(mMatrixMode);
	matrix_stack.top() *= Matrix4d::scale(x, y, z); // * matrix_stack.top();
}

// Immediate Mode
void Graphics::begin(Primitive mode) {
	// clear buffers
	mGraphicsData->resetBuffers();

	// start primitive drawing
	mGraphicsData->primitive(mode);
	mInImmediateMode = true;
}

void Graphics::end() {
	draw(*mGraphicsData);
	mInImmediateMode = false;
}

void Graphics::vertex(double x, double y, double z) {
	if(mInImmediateMode) {
		// make sure all buffers are the same size if > 0
		mGraphicsData->addVertex(x, y, z);
		mGraphicsData->equalizeBuffers();
	}
}

void Graphics::texcoord(double u, double v) {
	if(mInImmediateMode) {
		mGraphicsData->addTexCoord(u, v);
	}
}

void Graphics::normal(double x, double y, double z) {
	if(mInImmediateMode) {
		mGraphicsData->addNormal(x, y, z);
	}
}

void Graphics::color(double r, double g, double b, double a) {
	if(mInImmediateMode) {
		mGraphicsData->addColor(r, g, b, a);
	}
	else {
		mBackend->color(Color(r, g, b, a));
	}
}

void Graphics::enableState() {
	State &state = mState.top();
	
	if(mStateChange.blending) {
		mBackend->enableBlending(
			state.blend_enable,
			state.blend_src,
			state.blend_dst
		);
		mStateChange.blending = false;
	}
	
	if(mStateChange.lighting) {
		mBackend->enableLighting(
			state.lighting_enable
		);
		mStateChange.lighting = false;
	}
	
	if(mStateChange.depth_testing) {
		mBackend->enableDepthTesting(
			state.depth_enable
		);
		mStateChange.depth_testing = false;
	}
	
	if(mStateChange.polygon_mode) {
		mBackend->setPolygonMode(
			state.polygon_mode
		);
		mStateChange.polygon_mode = false;
	}
	
	if(mStateChange.antialiasing) {
		mBackend->setAntialiasing(state.antialias_mode);
		mStateChange.antialiasing = false;
	}
}

void Graphics::pointSize(double v) {
	mBackend->pointSize(v);
}

void Graphics::lineWidth(double v) {
	mBackend->lineWidth(v);
}

void Graphics::drawBegin() {
// Draw Begin (generic)
//		update state?
	enableState();
	
	mBackend->setProjectionMatrix(mProjectionMatrix.top());
	mBackend->setModelviewMatrix(mModelViewMatrix.top());
	
	
//		update matrices?
//		update textures?
/*
	vector<Texture *>::iterator tex_it = mTextures.begin();
	vector<Texture *>::iterator tex_ite = mTextures.end();
	for(int i=0; tex_it != tex_ite; ++tex_it, ++i) {
		(*tex_it)->bind(i);
	}
*/
//		update material?
}

void Graphics::drawEnd() {
	// Draw End (generic)
//		unbind materials
//		unbind used textures
/*
	vector<Texture *>::iterator tex_it = mTextures.begin();
	vector<Texture *>::iterator tex_ite = mTextures.end();
	for(int i=0; tex_it != tex_ite; ++tex_it, ++i) {
		(*tex_it)->unbind(i);
	}
*/
}

// Buffer drawing
void Graphics::draw(const Mesh& v) {
	drawBegin();
		mBackend->draw(v);
	drawEnd();
}

void Graphics::draw() {
	draw(*mGraphicsData);
}





/*
static void gl_draw(const GraphicsData& v){}

static void gl_clear(int attribMask){}
static void gl_clearColor(float r, float g, float b, float a){}
static void gl_loadIdentity(){}
static void gl_viewport(int x, int y, int w, int h){}

static void gl_begin(Graphics * g, int mode) {}
static void gl_end(Graphics * g) {}
static void gl_vertex(Graphics * g, double x, double y, double z) {}
static void gl_normal(Graphics * g, double x, double y, double z) {}
static void gl_color(Graphics * g, double x, double y, double z, double a) {}



bool setBackendNone(Graphics * g) {
	g->s_draw = gl_draw;
	g->s_clear = gl_clear;
	g->s_clearColor = gl_clearColor;
	g->s_loadIdentity = gl_loadIdentity;
	g->s_viewport = gl_viewport;

	g->s_begin = gl_begin;
	g->s_end = gl_end;
	g->s_vertex = gl_vertex;
	g->s_normal = gl_normal;
	g->s_color = gl_color;

	g->mBackend = Backend::None;
	
	return true;
}


bool setBackendOpenGLES(Graphics * g) {
	if (setBackendOpenGLES2(g)) {
		return true;
	} else if (setBackendOpenGLES1(g)) {
		return true;
	} else {
		return false;
	}
}

Graphics :: Graphics(Backend::type backend) {
	setBackend(backend);	
}


Graphics :: ~Graphics() {
	
}

bool Graphics :: setBackend(Backend::type backend) {
	if (backend == Backend::AutoDetect) {
		if (setBackendOpenGLES(this)) {
			return true;
		} else if (setBackendOpenGL(this)) {
			return true;
		} else {
			setBackendNone(this);
			return false;
		}
	}
		
	// set function pointers according to backend
	switch (backend) {
		case Backend::OpenGLES2:
			setBackendOpenGLES2(this);
			break;
		case Backend::OpenGLES1:
			setBackendOpenGLES1(this);
			break;
		case Backend::OpenGL:
			setBackendOpenGL(this);
			break;
		default:
			setBackendNone(this);
			return false;
	}
	
	return true;
}
*/
} // ::al::gfx
} // ::al
