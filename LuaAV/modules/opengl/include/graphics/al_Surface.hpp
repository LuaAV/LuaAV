#ifndef INCLUDE_AL_GRAPHICS_SURFACE_HPP
#define INCLUDE_AL_GRAPHICS_SURFACE_HPP

#include "graphics/al_Texture.hpp"
#include "graphics/al_Common.hpp"
#include "graphics/al_GPUObject.hpp"
#include "allocore/types/al_Array.hpp"
#include "types/al_Color.hpp"

namespace al {
namespace gfx{

class GraphicsBackend;

class Surface : public GPUObject {
public:

	Surface(GraphicsBackend *backend, void *surface_data);
	virtual ~Surface();
	
	void * surfaceData();
	
	void attach(Texture *tex);
	Texture * texture();
	
	void enter();
	void leave();
	
	void copy(Texture *tex);
	
	void clearColor(Color &c);
	Color& clearColor();
	
	float clearDepth();
	void clearDepth(float v);
	
	bool autoClear();
	void autoClear(bool v);
	
	int width();
	int height();
	
	void clear();
	
	bool creating() {return mCreating;}
	
protected:

	virtual void onCreate();
	virtual void onDestroy();
	
	
	GraphicsBackend	* mBackend;
	void *			mSurfaceData;
	Texture *		mTexture;
	Color			mClearColor;
	float			mClearDepth;
	bool			mAutoclear;
	bool			mCreating;
};

} // ::al::gfx
} // ::al

#endif
