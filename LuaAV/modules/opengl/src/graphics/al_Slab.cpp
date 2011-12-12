#include "graphics/al_Slab.hpp"

namespace al {
namespace gfx {

Slab :: Slab(GraphicsBackend *backend)
:	mBackend(backend),
	mShader(new Shader(backend)),
	mNumActiveTextures(0),
	mWidth(512),
	mHeight(512)
{
	for(int i=0; i < SLAB_MAX_RENDER_TARGETS; i++) {
		mTex[i] = mBackend->textureNew();
		mSurface[i] = mBackend->surfaceNew();
		mSurface[i]->attach(mTex[i]);
	}
}

Slab :: ~Slab() {
	delete mShader;
	for(int i=0; i < SLAB_MAX_RENDER_TARGETS; i++) {
		delete mTex[i];
		delete mSurface[i];
	}
}

void Slab :: draw(int argc, Texture **argv) {
	if(! created()) {
		create();
	}

	mBackend->slabDraw(this, argc, argv);
}

void Slab :: draw(Texture *tex) {
	draw(1, &tex);
}

void Slab :: draw(Texture *tex1, Texture *tex2) {
	Texture *texs[] = {tex1, tex2};
	draw(2, texs);
}

void Slab :: draw(Slab *slab) {
	Texture *texs[] = {slab->get_texture(0)};
	draw(1, texs);
}

void Slab :: draw(Slab *slab1, Slab *slab2) {
	Texture *texs[] = {
		slab1->get_texture(0),
		slab2->get_texture(0)
	};
	draw(2, texs);
}

void Slab :: onDestroy() {
	mShader->destroy();
	for(int i=0; i < SLAB_MAX_RENDER_TARGETS; i++) {
		mTex[i]->destroy();
		mSurface[i]->destroy();
	}
}


} // gfx::
} // al::
