#ifndef AL_SLAB_H
#define AL_SLAB_H 1

#include "graphics/al_Shader.hpp"
#include "graphics/al_Texture.hpp"
#include "graphics/al_GPUObject.hpp"

#define SLAB_MAX_RENDER_TARGETS	8

using al::gfx::Shader;

namespace al {
namespace gfx {

class Slab : public GPUObject {
public:
	Slab(GraphicsBackend *backend);
	virtual ~Slab();
	
	void draw(int argc, Texture **argv);
	void draw(Texture *tex);
	void draw(Texture *tex1, Texture *tex2);
	void draw(Slab *slab);
	void draw(Slab *slab1, Slab *slab2);
	

	Texture * get_texture(int i=0) {return mTex[i];}
	Surface * get_surface(int i=0) {return mSurface[i];}
	
	void get_dim(int width, int height);

	int get_width() {return mWidth;}
	int get_height() {return mHeight;}
	
	Shader * get_shader() {return mShader;}

protected:
	void onDestroy();
	void onCreate() {
		// dummy id for proxy destruction of internal objects
		id(1);
	}

protected:
	GraphicsBackend *		mBackend;				///< The backend
	Shader *				mShader;				///< The shader	
	int						mNumActiveTextures;		///< number of active render target textures
	Texture					*mTex[SLAB_MAX_RENDER_TARGETS];		///< Output texture
	Surface					*mSurface[SLAB_MAX_RENDER_TARGETS];	///< Output surface
	int						mWidth;					///< Output width of Slab
	int						mHeight;				///< Output height of Slab
};

} // gfx::
} // al::

#endif // AL_SLAB_H
