#ifndef INCLUDE_AL_GRAPHICS_GPUOBJECT_H
#define INCLUDE_AL_GRAPHICS_GPUOBJECT_H

#include "allocore/system/al_Config.h"

namespace al {
namespace gfx{

class GPUObject{
public:
	GPUObject(): mID(0){}

	virtual ~GPUObject(){}

	/// Returns the assigned object id
	const uint32_t id() const { return mID; }
	void id(uint32_t v) {mID = v;}
	
	bool created(){ return id()!=0; }

	/// Creates object on GPU
	void create(){
		if(created()){ destroy(); }
		onCreate();
	}
	
	/// Destroys object on GPU
	void destroy(){ 
		if(created()) {
			onDestroy(); mID=0; 
		}
	}

protected:
	uint32_t mID;
	virtual void onCreate() = 0;
	virtual void onDestroy() = 0;
};


} // ::al::gfx
} // ::al

#endif
