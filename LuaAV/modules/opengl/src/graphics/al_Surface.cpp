#include <stdlib.h>
#include "protocol/al_GraphicsBackend.hpp"
#include "graphics/al_Config.h"
#include "graphics/al_Surface.hpp"

namespace al {
namespace gfx{

Surface::Surface(GraphicsBackend *backend, void *surface_data)
:	GPUObject(),
	mBackend(backend),
	mSurfaceData(surface_data),
	mTexture(0),
	mClearColor(0., 0., 0., 1.),
	mClearDepth(1.),
	mAutoclear(true),
	mCreating(false)
{}

Surface::~Surface() {
	destroy();
}

void * Surface::surfaceData() {
	return mSurfaceData;
}

void Surface::attach(Texture *tex) {
	mTexture = tex;
	mTexture->attach(this);
}

Texture * Surface::texture() {
	return mTexture;
}

void Surface::enter() {
	if(mTexture) {
		if(! created()) {
			create();
		}
	
		mBackend->surfaceBind(this);
		mBackend->surfaceEnter(this);
	}
}

void Surface::leave() {
	mBackend->surfaceLeave(this);
	mBackend->surfaceUnbind(this);
}

void Surface::copy(Texture *tex) {
	mBackend->surfaceCopy(this, tex);
}

void Surface::clearColor(Color &c) {
	mClearColor = c;
}

Color& Surface::clearColor() {
	return mClearColor;
}

float Surface::clearDepth() {
	return mClearDepth;
}

void Surface::clearDepth(float v) {
	mClearDepth = v;
}

bool Surface::autoClear() {
	return mAutoclear;
}

void Surface::autoClear(bool v) {
	mAutoclear = v;
}

int Surface::width() {
	return mTexture ? mTexture->width() : 0;
}

int Surface::height() {
	return mTexture ? mTexture->height() : 0;
}

void Surface::clear() {
	mBackend->surfaceClear(this);
}


void Surface::onCreate() {
	if(! mCreating) {
		mCreating = true;
		mBackend->surfaceCreate(this);
		mCreating = false;
	}
}

void Surface::onDestroy() {
	mBackend->surfaceDestroy(this);
}

} // ::al::gfx
} // ::al
