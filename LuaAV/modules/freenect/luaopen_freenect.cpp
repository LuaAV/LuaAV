/*
See README.md

Copyright (c) 2009-2012 Graham Wakefield & Wesley Smith
https://github.com/grrrwaaa/luafreenect/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "lua.hpp"
#include "lua_glue.h"
#include "lua_utility.h"
#include "libfreenect.h"

#include <vector>

//class Freenect {
//public:
//	static Freenect& get();
//	
//	struct Device {
//		freenect_device * dev;
//		
//		bool close() {
//			return check("close", freenect_close_device(dev));
//		}
//		
//		void set_user(void * user) {
//			freenect_set_user(dev, user);
//		}
//		
//		void * get_user() { return freenect_get_user(dev); }
//		
//		Device() : dev(0) {}
//	};
//	
//	static bool check(const char * what, int code) {
//		if (code < 0) {
//			printf("error (%s): %d\n", what, code);
//			return false;
//		}
//		return true;
//	}
//	
//	Device& open(int idx) {
//		check("open", freenect_open_device(ctx, &devices[idx].dev, idx));
//		return devices[idx];
//	}
//
//private:
//	Freenect() {
//		// TODO: handle multiple contexts?
//		freenect_usb_context * usb_ctx = NULL;
//		int res = freenect_init(&ctx, usb_ctx);
//		if (res < 0) {
//			printf("error: failed to initialize libfreenect\n");
//			exit(0);
//		}
//		
//		int numdevs = freenect_num_devices(ctx);
//		printf("%d devices\n", numdevs);
//		devices.resize(numdevs);
//	}
//	
//	Freenect * singleton;
//	freenect_context * ctx;
//	
//	std::vector<Device> devices;
//};


// singleton:
static freenect_context * ctx = 0;
static std::vector<freenect_device *> devices;

int lua_push_frame_mode(lua_State * L, const freenect_frame_mode mode) {
	lua_newtable(L);
	lua_pushinteger(L, mode.resolution); lua_setfield(L, -2, "resolution");
	lua_pushinteger(L, mode.video_format); lua_setfield(L, -2, "video_format");
	lua_pushinteger(L, mode.depth_format); lua_setfield(L, -2, "depth_format");
	lua_pushinteger(L, mode.bytes); lua_setfield(L, -2, "bytes");
	lua_pushinteger(L, mode.width); lua_setfield(L, -2, "width");
	lua_pushinteger(L, mode.height); lua_setfield(L, -2, "height");
	lua_pushinteger(L, mode.data_bits_per_pixel); lua_setfield(L, -2, "data_bits_per_pixel");
	lua_pushinteger(L, mode.padding_bits_per_pixel); lua_setfield(L, -2, "padding_bits_per_pixel");
	lua_pushinteger(L, mode.framerate); lua_setfield(L, -2, "framerate");
	lua_pushinteger(L, mode.is_valid); lua_setfield(L, -2, "is_valid");
	return 1;
}

template<> const char * Glue<freenect_device>::usr_name() { return "freenect_device"; }
template<> void Glue<freenect_device>::usr_gc(lua_State * L, freenect_device * dev) { 
	int res = freenect_close_device(dev);
	if (res < 0) luaL_error(L, "error (%d): freenect_close_device\n", res);
}

// set/get userdata:
int lua_freenect_set_user(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	void * user = lua_touserdata(L, 2);
	freenect_set_user(dev, user);
	return 0;
}
int lua_freenect_get_user(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	lua_pushlightuserdata(L, freenect_get_user(dev));
	return 0;
}

void lua_freenect_depth_cb(freenect_device *dev, void *depth, uint32_t timestamp) {
	printf("depth...");
}

void lua_freenect_video_cb(freenect_device *dev, void *video, uint32_t timestamp) {
	printf("video...");
}

int lua_freenect_start_depth(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	freenect_set_depth_callback(dev, lua_freenect_depth_cb);
	int res = freenect_start_depth(dev);
	if (res < 0) luaL_error(L, "error (%d): freenect_start_depth\n", res);
	return 0;
}

int lua_freenect_start_video(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	freenect_set_video_callback(dev, lua_freenect_video_cb);
	int res = freenect_start_video(dev);
	if (res < 0) luaL_error(L, "error (%d): freenect_start_video\n", res);
	return 0;
}

int lua_freenect_stop_depth(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	int res = freenect_stop_depth(dev);
	if (res < 0) luaL_error(L, "error (%d): freenect_stop_depth\n", res);
	return 0;
}

int lua_freenect_stop_video(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	int res = freenect_stop_video(dev);
	if (res < 0) luaL_error(L, "error (%d): freenect_stop_video\n", res);
	return 0;
}

int lua_freenect_update_tilt_state(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	int res = freenect_update_tilt_state(dev);
	if (res < 0) luaL_error(L, "error (%d): freenect_update_tilt_state\n", res);
	return 0;
}

int lua_freenect_get_tilt_state(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	freenect_raw_tilt_state* tilt = freenect_get_tilt_state(dev);
	if (tilt == 0) luaL_error(L, "error: freenect_get_tilt_state\n");
	lua_newtable(L);
	lua_pushinteger(L, tilt->accelerometer_x); lua_setfield(L, -2, "accelerometer_x");
	lua_pushinteger(L, tilt->accelerometer_y); lua_setfield(L, -2, "accelerometer_y");
	lua_pushinteger(L, tilt->accelerometer_z); lua_setfield(L, -2, "accelerometer_z");
	lua_pushinteger(L, tilt->tilt_angle); lua_setfield(L, -2, "tilt_angle");
	lua_pushinteger(L, tilt->tilt_status); lua_setfield(L, -2, "tilt_status");
	return 1;
}

int lua_freenect_get_tilt_degs(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	freenect_raw_tilt_state* tilt = freenect_get_tilt_state(dev);
	if (tilt == 0) luaL_error(L, "error: freenect_get_tilt_degs\n");
	lua_pushnumber(L, freenect_get_tilt_degs(tilt));
	return 1;
}

int lua_freenect_set_tilt_degs(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	int res = freenect_set_tilt_degs(dev, luaL_optnumber(L, 2, 0));
	if (res < 0) luaL_error(L, "error (%d): freenect_set_tilt_degs\n", res);
	return 0;
}

int lua_freenect_get_tilt_status(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	freenect_raw_tilt_state* tilt = freenect_get_tilt_state(dev);
	freenect_tilt_status_code res = freenect_get_tilt_status(tilt);
	lua_pushinteger(L, res);	
	return 1;
}

int lua_freenect_set_led(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	freenect_led_options option;
	int res = freenect_set_led(dev, option);
	if (res < 0) luaL_error(L, "error (%d): freenect_set_led\n", res);
	return 0;
}



int lua_freenect_get_current_video_mode(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	const freenect_frame_mode mode = freenect_get_current_video_mode(dev);
	return lua_push_frame_mode(L, mode);
}

int lua_freenect_get_current_depth_mode(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	const freenect_frame_mode mode = freenect_get_current_depth_mode(dev);
	return lua_push_frame_mode(L, mode);
}

int lua_freenect_set_video_mode(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	freenect_frame_mode mode;
	
	lua_getfield(L, 2, "resolution"); mode.resolution = (freenect_resolution)luaL_optinteger(L, -1, FREENECT_RESOLUTION_MEDIUM); lua_pop(L, 1);
	lua_getfield(L, 2, "video_format"); mode.video_format = (freenect_video_format)luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	//lua_getfield(L, 2, "depth_format"); mode.depth_format = (freenect_depth_format)luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "bytes"); mode.bytes = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "width"); mode.width = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "height"); mode.height = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "data_bits_per_pixel"); mode.data_bits_per_pixel = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "padding_bits_per_pixel"); mode.padding_bits_per_pixel = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "framerate"); mode.framerate = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "is_valid"); mode.is_valid = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
		
	int res = freenect_set_video_mode(dev, mode);
	if (res < 0) luaL_error(L, "error (%d): freenect_set_video_mode\n", res);
	return 0;
}
int lua_freenect_set_depth_mode(lua_State * L) {
	freenect_device * dev = Glue<freenect_device>::checkto(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	freenect_frame_mode mode;
	
	lua_getfield(L, 2, "resolution"); mode.resolution = (freenect_resolution)luaL_optinteger(L, -1, FREENECT_RESOLUTION_MEDIUM); lua_pop(L, 1);
	//lua_getfield(L, 2, "video_format"); (freenect_video_format)mode.video_format = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "depth_format"); mode.depth_format = (freenect_depth_format)luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "bytes"); mode.bytes = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "width"); mode.width = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "height"); mode.height = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "data_bits_per_pixel"); mode.data_bits_per_pixel = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "padding_bits_per_pixel"); mode.padding_bits_per_pixel = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "framerate"); mode.framerate = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
	lua_getfield(L, 2, "is_valid"); mode.is_valid = luaL_optinteger(L, -1, 0); lua_pop(L, 1);
		
	int res = freenect_set_depth_mode(dev, mode);
	if (res < 0) luaL_error(L, "error (%d): freenect_set_depth_mode\n", res);
	return 0;
}

	
template<> void Glue<freenect_device>::usr_mt(lua_State * L) {
	struct luaL_reg lib[] = {
		{ "close", Glue<freenect_device>::gc },
		{ "set_user", lua_freenect_set_user },
		{ "get_user", lua_freenect_get_user },
		{ "start_depth", lua_freenect_start_depth },
		{ "start_video", lua_freenect_start_video },
		{ "stop_depth", lua_freenect_stop_depth },
		{ "stop_video", lua_freenect_stop_video },
		{ "update_tilt_state", lua_freenect_update_tilt_state },
		{ "get_tilt_state", lua_freenect_get_tilt_state },
		{ "get_tilt_degs", lua_freenect_get_tilt_degs },
		{ "set_tilt_degs", lua_freenect_set_tilt_degs },
		{ "get_tilt_status", lua_freenect_get_tilt_status },
		{ "set_led", lua_freenect_set_led },
		{ "get_current_video_mode", lua_freenect_get_current_video_mode },
		{ "get_current_depth_mode", lua_freenect_get_current_depth_mode },
		{ "set_video_mode", lua_freenect_set_video_mode },
		{ "set_depth_mode", lua_freenect_set_depth_mode },
		{NULL, NULL},
	};
	luaL_register(L, NULL, lib);
}

int lua_freenect_shutdown(lua_State * L) {
	int res = freenect_shutdown(ctx);
	lua_pushinteger(L, res);
	if (res < 0) luaL_error(L, "error (%d): freenect_shutdown\n", res);
	ctx = 0;
	return 0;
}

/**
 * Set the log level for the specified freenect context
 *
 * @param ctx context to set log level for
 * @param level log level to use (see freenect_loglevel enum)
 /// Enumeration of message logging levels
typedef enum {
	FREENECT_LOG_FATAL = 0,     //< Log for crashing/non-recoverable errors 
	FREENECT_LOG_ERROR,         //< Log for major errors 
	FREENECT_LOG_WARNING,       //< Log for warning messages 
	FREENECT_LOG_NOTICE,        //< Log for important messages 
	FREENECT_LOG_INFO,          //< Log for normal messages
	FREENECT_LOG_DEBUG,         //< Log for useful development messages 
	FREENECT_LOG_SPEW,          //< Log for slightly less useful messages 
	FREENECT_LOG_FLOOD,         //< Log EVERYTHING. May slow performance. 
} freenect_loglevel;
 */
int lua_freenect_set_log_level(lua_State * L) {
	freenect_set_log_level(ctx, freenect_loglevel(luaL_optinteger(L, 1, FREENECT_LOG_INFO)));
	return 0;
}

/**
 * Calls the platform specific usb event processor
 *
 * @param ctx context to process events for
 *
 * @return 0 on success, other values on error, platform/library dependant
 */
int lua_freenect_process_events(lua_State * L) {
	printf("before\n");
	int res = freenect_process_events(ctx);
	printf("after\n");
	lua_pushinteger(L, res);
	if (res < 0) luaL_error(L, "error (%d): freenect_process_events\n", res);
	return 1;
}

/**
 * Return the number of kinect devices currently connected to the
 * system
 *
 * @param ctx Context to access device count through
 *
 * @return Number of devices connected, < 0 on error
 */
int lua_freenect_num_devices(lua_State * L) {
	int res = freenect_num_devices(ctx);
	lua_pushinteger(L, res);
	return 1;
}

/**
 * Set which subdevices any subsequent calls to freenect_open_device()
 * should open.  This will not affect devices which have already been
 * opened.  The default behavior, should you choose not to call this
 * function at all, is to open all supported subdevices - motor, cameras,
 * and audio, if supported on the platform.
 *
 * @param ctx Context to set future subdevice selection for
 * @param subdevs Flags representing the subdevices to select
 */
int lua_freenect_select_subdevices(lua_State * L) {
	int flags = luaL_optinteger(L, 1, FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA | FREENECT_DEVICE_AUDIO);
	freenect_select_subdevices(ctx, freenect_device_flags(flags));
	return 0;
}

/**
 * Opens a kinect device via a context. Index specifies the index of
 * the device on the current state of the bus. Bus resets may cause
 * indexes to shift.
 *
 * @param ctx Context to open device through
 * @param dev Device structure to assign opened device to
 * @param index Index of the device on the bus
 *
 * @return 0 on success, < 0 on error
 */
int lua_freenect_open_device(lua_State * L) {
	int index = luaL_optinteger(L, 1, 0);
	devices.resize(index+1);
	int res = freenect_open_device(ctx, &devices[index], index);
	if (res < 0) luaL_error(L, "error (%d): freenect_open_device\n", res);
	Glue<freenect_device>::push(L, devices[index]);
	return 1;
}

int lua_freenect_get_video_mode_count(lua_State * L) {
	lua_pushinteger(L, freenect_get_video_mode_count());
	return 1;
}

int lua_freenect_get_depth_mode_count(lua_State * L) {
	lua_pushinteger(L, freenect_get_depth_mode_count());
	return 1;
}

int lua_freenect_get_video_mode(lua_State * L) {
	const freenect_frame_mode mode = freenect_get_video_mode(luaL_optinteger(L, 1, 0));
	return lua_push_frame_mode(L, mode);
}

int lua_freenect_get_depth_mode(lua_State * L) {
	const freenect_frame_mode mode = freenect_get_depth_mode(luaL_optinteger(L, 1, 0));
	return lua_push_frame_mode(L, mode);
}

int lua_freenect_find_video_mode(lua_State * L) {
	freenect_resolution res = freenect_resolution(luaL_optinteger(L, 1, 0));
	freenect_video_format fmt = freenect_video_format(luaL_optinteger(L, 2, 0));
	const freenect_frame_mode mode = freenect_find_video_mode(res, fmt);
	return lua_push_frame_mode(L, mode);
}	

int lua_freenect_find_depth_mode(lua_State * L) {
	freenect_resolution res = freenect_resolution(luaL_optinteger(L, 1, 0));
	freenect_depth_format fmt = freenect_depth_format(luaL_optinteger(L, 2, 0));
	const freenect_frame_mode mode = freenect_find_depth_mode(res, fmt);
	return lua_push_frame_mode(L, mode);
}	

extern "C" int luaopen_freenect(lua_State * L) {
	if (ctx == 0) {
		freenect_usb_context * usb_ctx = NULL;
		int res = freenect_init(&ctx, usb_ctx);
		if (res < 0) luaL_error(L, "error (%d): freenect_init\n", res);
	}
	
	const char * libname = luaL_optstring(L, 1, "freenect");
	struct luaL_reg lib[] = {
		{ "set_log_level", lua_freenect_set_log_level },
		{ "process_events", lua_freenect_process_events },
		{ "num_devices", lua_freenect_num_devices },
		{ "select_subdevices", lua_freenect_select_subdevices },
		{ "open_device", lua_freenect_open_device },
		{ "get_video_mode_count", lua_freenect_get_video_mode_count },
		{ "get_video_mode", lua_freenect_get_video_mode },
		{ "find_video_mode", lua_freenect_find_video_mode },
		{ "get_depth_mode_count", lua_freenect_get_depth_mode_count },
		{ "get_depth_mode", lua_freenect_get_depth_mode },
		{ "find_depth_mode", lua_freenect_find_depth_mode },
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);
	
	#define FREENECT_BITFIELD(x) lua_pushinteger(L, FREENECT_##x); lua_setfield(L, -2, #x);	
	#define BITFIELD(x) lua_pushinteger(L, x); lua_setfield(L, -2, #x);	
	
	lua_newtable(L);
	FREENECT_BITFIELD(DEVICE_MOTOR);	
	FREENECT_BITFIELD(DEVICE_CAMERA);	
	FREENECT_BITFIELD(DEVICE_AUDIO);	
	lua_setfield(L, -2, "device_flags");
	
	lua_newtable(L);
	FREENECT_BITFIELD(RESOLUTION_LOW);  /**< QVGA - 320x240 */
	FREENECT_BITFIELD(RESOLUTION_MEDIUM)  /**< VGA  - 640x480 */
	FREENECT_BITFIELD(RESOLUTION_HIGH)  /**< SXGA - 1280x1024 */
	FREENECT_BITFIELD(RESOLUTION_DUMMY) 
	lua_setfield(L, -2, "resolution");
	
	lua_newtable(L);
	FREENECT_BITFIELD(VIDEO_RGB) //             = 0, /**< Decompressed RGB mode (demosaicing done by libfreenect) */
	FREENECT_BITFIELD(VIDEO_BAYER) //           = 1, /**< Bayer compressed mode (raw information from camera) */
	FREENECT_BITFIELD(VIDEO_IR_8BIT) //         = 2, /**< 8-bit IR mode  */
	FREENECT_BITFIELD(VIDEO_IR_10BIT) //        = 3, /**< 10-bit IR mode */
	FREENECT_BITFIELD(VIDEO_IR_10BIT_PACKED) // = 4, /**< 10-bit packed IR mode */
	FREENECT_BITFIELD(VIDEO_YUV_RGB) //         = 5, /**< YUV RGB mode */
	FREENECT_BITFIELD(VIDEO_YUV_RAW) //         = 6, /**< YUV Raw mode */
	FREENECT_BITFIELD(VIDEO_DUMMY) //  
	lua_setfield(L, -2, "video_format");
	
	lua_newtable(L);
	FREENECT_BITFIELD(DEPTH_11BIT) //        = 0, /**< 11 bit depth information in one uint16_t/pixel */
	FREENECT_BITFIELD(DEPTH_10BIT) //        = 1, /**< 10 bit depth information in one uint16_t/pixel */
	FREENECT_BITFIELD(DEPTH_11BIT_PACKED) // = 2, /**< 11 bit packed depth information */
	FREENECT_BITFIELD(DEPTH_10BIT_PACKED) // = 3, /**< 10 bit packed depth information */
	FREENECT_BITFIELD(DEPTH_DUMMY) //  
	lua_setfield(L, -2, "depth_format");
	
	lua_newtable(L);
	BITFIELD(LED_OFF);
	BITFIELD(LED_RED);
	BITFIELD(LED_YELLOW);
	BITFIELD(LED_BLINK_GREEN);
	BITFIELD(LED_BLINK_RED_YELLOW);
	lua_setfield(L, -2, "led_options");
	
	lua_newtable(L);
	BITFIELD(TILT_STATUS_STOPPED); /**< Tilt motor is stopped */
	BITFIELD(TILT_STATUS_LIMIT); /**< Tilt motor has reached movement limit */
	BITFIELD(TILT_STATUS_MOVING);
	lua_setfield(L, -2, "tilt_status_code");
	
	lua_newtable(L);
	FREENECT_BITFIELD(LOG_FATAL) // = 0,     /**< Log for crashing/non-recoverable errors */
	FREENECT_BITFIELD(LOG_ERROR) //,         /**< Log for major errors */
	FREENECT_BITFIELD(LOG_WARNING) //,       /**< Log for warning messages */
	FREENECT_BITFIELD(LOG_NOTICE) //,        /**< Log for important messages */
	FREENECT_BITFIELD(LOG_INFO) //,          /**< Log for normal messages */
	FREENECT_BITFIELD(LOG_DEBUG) //,         /**< Log for useful development messages */
	FREENECT_BITFIELD(LOG_SPEW) //,          /**< Log for slightly less useful messages */
	FREENECT_BITFIELD(LOG_FLOOD) //,   
	lua_setfield(L, -2, "loglevel"); 
	
	
	
	Glue<freenect_device>::define(L);
	
	// attach a close handler:
	lua::gc_sentinel(L, -1, lua_freenect_shutdown);
	
	return 1;
}
