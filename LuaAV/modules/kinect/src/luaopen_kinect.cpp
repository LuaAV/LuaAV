extern "C" {
	#include "lua.h"
	#include "lauxlib.h"

	#include "libusb.h"
	#include "libfreenect.h"
}

#include "luaav.h"
#include "lua_glue.h"
#include "lua_array.h"
#include "allocore/types/al_Array.hpp"
using al::ArrayWrapper;


freenect_context *f_ctx;
freenect_device *f_dev;
ArrayWrapper depthArray;
ArrayWrapper rgbArray;

void depth_cb(freenect_device *dev, freenect_depth *depth, uint32_t timestamp)
{
	for (int h=0; h<FREENECT_FRAME_H; h++) {
		int row = h*FREENECT_FRAME_W;
		for (int w=0; w<FREENECT_FRAME_W; w++) {
			depthArray.write(depth+row+w, w, h);
		}
	}
}

void rgb_cb(freenect_device *dev, freenect_pixel *rgb, uint32_t timestamp)
{
	memcpy(rgbArray.data.ptr, rgb, FREENECT_RGB_SIZE);
}


int poll(lua_State * L) {
	lua_pushinteger(L, freenect_process_events(f_ctx));
	return 1;
}

int depth(lua_State * L) {
	Glue<ArrayWrapper>::push(L, &depthArray);
	return 1;
}

int rgb(lua_State * L) {
	Glue<ArrayWrapper>::push(L, &rgbArray);
	return 1;
}

/* require */
extern "C" int luaopen_kinect(lua_State * L) {

	depthArray.format(1, AlloUInt16Ty, FREENECT_FRAME_W, FREENECT_FRAME_H);
	rgbArray.format(3, AlloUInt8Ty, FREENECT_FRAME_W, FREENECT_FRAME_H);
	
//	freenect_start_depth(f_dev);
	//freenect_start_rgb(f_dev);
	
//	Kinect::sDepthPixels.format(1, AlloUInt16Ty, 640, 480);
//	Kinect::sDepthPixels.format(3, AlloUInt8Ty, 640, 480);
	
	// define the module table 
	luaL_Reg lib[] = {
		{ "depth", depth },
		{ "rgb", rgb },
		{ "poll", poll },
		{ NULL, NULL }
	};
	luaL_register(L, "kinect", lib);
	
	if (freenect_init(&f_ctx, NULL) < 0) {
		luaL_error(L, "freenect_init() failed");
	}

	if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
		luaL_error(L, "Could not open device");
	}

	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_rgb_callback(f_dev, rgb_cb);
	freenect_set_rgb_format(f_dev, FREENECT_FORMAT_RGB);
	
//	Glue<Kinect>::define(L);
//	Glue<Kinect>::register_ctor(L);
	
	return 1;
}
