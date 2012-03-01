#include "Video.h"
#include "VideoCamera.h"
#include "VideoRecorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_glue.h"
#include "lua_array.h"


#define LUA_VIDEO_ARRAY_FIELD	"__LUA_VIDEO_ARRAY_FIELD"

using video::Video;
using video::VideoCamera;
using video::VideoRecorder;


/*! Video file playing and video camera streaming

	@module video
*/


/*! Video file player
	
	@module video.Video
*/

/*! Openg a video file
	@param filename The file to open
	@name M:open
*/
int lua_video_open(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s && lua::is<const char *>(L, 2)) {
		const char *filename = lua::to<const char *>(L, 2);
		s->open(filename);
	}
	else {
		luaL_error(L, "Video.open: invalid arguments");
	}
	return 0;
}

/*! Play the video file
	@name M:play
*/
int lua_video_play(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		s->play();
	}
	else {
		luaL_error(L, "Video.play: invalid arguments");
	}
	return 0;
}

/*! Toggle pausing the video file
	@param [flag] Optional flag to pause / unpause the video (default true)
	@name M:pause
*/
int lua_video_pause(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		bool v = lua::opt<bool>(L, 2, true);
		s->pause(v);
	}
	else {
		luaL_error(L, "Video.pause: invalid arguments");
	}
	return 0;
}

/*! Stop the video file
	@name M:stop
*/
int lua_video_stop(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		s->stop();
	}
	else {
		luaL_error(L, "Video.stop: invalid arguments");
	}
	return 0;
}

/*! Get the internal Array data
	@ret The array
	@name M:array
*/
int lua_video_array(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		s->next_frame();

		lua_getfenv(L, 1);
		lua_pushstring(L, LUA_VIDEO_ARRAY_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Video.array: invalid arguments");
	}
	return 1;
}

/*! Video position (as a percentage)
	@param position The position
	@LuaMethod GETSET
	@name M.pos
*/
int lua_video_pos(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		if(lua::is<float>(L, 2)) {
			float pct = lua::to<float>(L, 2);
			s->set_position(pct);
			return 0;
		}
		else {
			lua::push(L, s->get_position());
			return 1;
		}
	}
	else {
		luaL_error(L, "Video.pos: invalid arguments");
	}
	return 0;
}

/*! Video speed
	@param speed The speed
	@LuaMethod GETSET
	@name M.speed
*/
int lua_video_speed(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		if(lua::is<float>(L, 2)) {
			float speed = lua::to<float>(L, 2);
			s->set_speed(speed);
			return 0;
		}
		else {
			lua::push(L, s->get_speed());
			return 1;
		}
	}
	else {
		luaL_error(L, "Video.speed: invalid arguments");
	}
	return 0;
}

/*! Video frame
	@param frame The frame
	@LuaMethod GETSET
	@name M.frame
*/
int lua_video_frame(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		if(lua::is<int>(L, 2)) {
			float frame = lua::to<int>(L, 2);
			s->set_frame(frame);
			return 0;
		}
		else {
			lua::push(L, s->get_frame());
			return 1;
		}
	}
	else {
		luaL_error(L, "Video.frame: invalid arguments");
	}
	return 0;
}

/*! Total number of video frames
	@ret nframes The number of frames
	@LuaMethod GET
	@name M.nframes
*/
int lua_video_nframes(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		lua::push(L, s->get_nframes());
	}
	else {
		luaL_error(L, "Video.nframes: invalid arguments");
	}
	return 1;
}

/*! Video duration
	@ret dur The duration
	@LuaMethod GET
	@name M.dur
*/
int lua_video_dur(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		lua::push(L, s->get_duration());
	}
	else {
		luaL_error(L, "Video.dur: invalid arguments");
	}
	return 1;
}

/*! Video dimensions
	@ret dim The dimensions
	@LuaMethod GET
	@name M.dim
*/
int lua_video_dim(lua_State *L) {
	Video *s = Glue<Video>::checkto(L, 1);
	if(s) {
		int dim[] = {0, 0};
		s->get_dim(dim[0], dim[1]);
		lua::push_vec_t(L, 2, dim);
	}
	else {
		luaL_error(L, "Video.dim: invalid arguments");
	}
	return 1;
}


template<> const char * Glue<Video>::usr_name() { return "Video"; }
template<> bool Glue<Video>::usr_has_index() { return true; }

template<> void Glue<Video>::usr_push(lua_State * L, Video * u) {
	Glue<Video>::usr_attr_push(L);
	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_VIDEO_ARRAY_FIELD);
	Glue<al::ArrayWrapper>::push(L, u->wrapper());
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

template<> Video * Glue<Video>::usr_new(lua_State * L) {
	Video *cam = new Video();
	return cam;
}

template<> void Glue<Video>::usr_index(lua_State * L, Video * u) {
	Glue<Video>::usr_attr_index(L);
}

template<> void Glue<Video>::usr_newindex(lua_State * L, Video * u) {
	Glue<Video>::usr_attr_newindex(L);
}

template<> int Glue<Video>::usr_tostring(lua_State * L, Video * u) {
	lua_pushfstring(L, "%s: %p", Glue<Video>::usr_name(), u); 
	return 1;
}

template<> void Glue<Video>::usr_gc(lua_State * L, Video * u) { 
	delete u;
}

template<> void Glue<Video>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{"open", lua_video_open},
		{"play", lua_video_play},
		{"pause", lua_video_pause},
		{"stop", lua_video_stop},
		{"array", lua_video_array},
		{ NULL, NULL}
	};
	
	static luaL_reg getters[] = {
		{"pos", lua_video_pos},
		{"speed", lua_video_speed},
		{"frame", lua_video_frame},
		{"nframes", lua_video_nframes},
		{"dur", lua_video_dur},
		{"dim", lua_video_dim},
		{ NULL, NULL}
	};
	
	static luaL_reg setters[] = {
		{"pos", lua_video_pos},
		{"speed", lua_video_speed},
		{"frame", lua_video_frame},
		{ NULL, NULL}
	};
	Glue<Video>::usr_attr_mt(L, methods, getters, setters);
}



/*! Video camera streamer
	
	@module video.VideoCamera
*/

/*! Open a video camera stream
	@param [dim] The dimensions (defaults to 720x480)
	@name M:open
*/
int lua_video_camera_open(lua_State *L) {
	VideoCamera *s = Glue<VideoCamera>::checkto(L, 1);
	if(s) {
		if(lua_type(L, 2) == LUA_TSTRING) {
			const char *uid = lua_tostring(L, 2);
			int dim[] = {720, 480};
			lua::to_vec(L, 3, 2, dim);
			s->open(uid, dim[0], dim[1]);
		}
		else {
			int dim[] = {720, 480};
			lua::to_vec(L, 2, 2, dim);
			s->open(dim[0], dim[1]);
		}
	}
	else {
		luaL_error(L, "VideoCamera.open: invalid arguments");
	}
	return 1;
}

/*! Close a video camera stream
	@name M:close
*/
int lua_video_camera_close(lua_State *L) {
	VideoCamera *s = Glue<VideoCamera>::checkto(L, 1);
	if(s) {
		s->close();
	}
	else {
		luaL_error(L, "VideoCamera.close: invalid arguments");
	}
	return 1;
}

/*! Get the internal array data
	@ret The array
	@name M:array
*/
int lua_video_camera_array(lua_State *L) {
	VideoCamera *s = Glue<VideoCamera>::checkto(L, 1);
	if(s) {
		s->next_frame();

		lua_getfenv(L, 1);
		lua_pushstring(L, LUA_VIDEO_ARRAY_FIELD);
		lua_rawget(L, -2);
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "VideoCamera.array: invalid arguments");
	}
	return 1;
}

int lua_video_list_devices(lua_State *L) {
	vector<video::Device> devices;
	VideoCamera::list_devices(devices);
	lua_newtable(L);
	for(int i=0; i < devices.size(); i++) {
		lua_newtable(L);
		lua_pushstring(L, devices[i].name.c_str());
		lua_setfield(L, -2, "name");
		lua_pushstring(L, devices[i].uid.c_str());
		lua_setfield(L, -2, "uid");
		lua_rawseti(L, -2, i+1);
	}
	return 1;
}

/*! Set the pixel format
	Possible values are video.RGBA and video.RGB.
	@name M.format
	@LuaMethod GETSET
*/
int lua_video_camera_format(lua_State *L) {
	VideoCamera *s = Glue<VideoCamera>::checkto(L, 1);
	if(s) {
		if(lua::is<int>(L, 2)) {
			s->set_pixel_format((video::PixelFormat)lua::to<int>(L, 2));
		}
		else {
			lua::push<int>(L, (int)s->get_pixel_format());
		}
	}
	else {
		luaL_error(L, "VideoCamera.format: invalid arguments");
	}
	return 0;
}

template<> const char * Glue<VideoCamera>::usr_name() { return "VideoCamera"; }
template<> bool Glue<VideoCamera>::usr_has_index() { return true; }

template<> void Glue<VideoCamera>::usr_push(lua_State * L, VideoCamera * u) {
	Glue<VideoCamera>::usr_attr_push(L);
	lua_getfenv(L, -1);
	lua_pushstring(L, LUA_VIDEO_ARRAY_FIELD);
	Glue<al::ArrayWrapper>::push(L, u->wrapper());
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

template<> VideoCamera * Glue<VideoCamera>::usr_new(lua_State * L) {
	VideoCamera *cam = new VideoCamera();
	return cam;
}

template<> void Glue<VideoCamera>::usr_index(lua_State * L, VideoCamera * u) {
	Glue<VideoCamera>::usr_attr_index(L);
}

template<> void Glue<VideoCamera>::usr_newindex(lua_State * L, VideoCamera * u) {
	Glue<VideoCamera>::usr_attr_newindex(L);
}

template<> int Glue<VideoCamera>::usr_tostring(lua_State * L, VideoCamera * u) {
	lua_pushfstring(L, "%s: %p", Glue<VideoCamera>::usr_name(), u); 
	return 1;
}

template<> void Glue<VideoCamera>::usr_gc(lua_State * L, VideoCamera * u) { 
	delete u;
}

template<> void Glue<VideoCamera>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{"open", lua_video_camera_open},
		{"close", lua_video_camera_close},
		{"array", lua_video_camera_array},
		{"list_devices", lua_video_list_devices},
		{ NULL, NULL}
	};
	
	static luaL_reg getters[] = {
		{"format", lua_video_camera_format},
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{"format", lua_video_camera_format},
		{ NULL, NULL}
	};
	Glue<VideoCamera>::usr_attr_mt(L, methods, getters, setters);
}



/*! Video recorder
	
	@module video.VideoRecorder
*/

/*! Open a file for saving a video
	@param filename The filename of the video to save
	@param [dim] The dimensions (defaults to 720x480)
	@name M:open
*/
int lua_video_recorder_open(lua_State *L) {
	VideoRecorder *s = Glue<VideoRecorder>::checkto(L, 1);
	if(s && lua::is<const char *>(L, 2)) {
		const char *filename = lua::to<const char *>(L, 2);
	
		int dim[] = {720, 480};
		lua::to_vec(L, 3, 2, dim);
		s->open(filename, dim[0], dim[1]);
	}
	else {
		luaL_error(L, "VideoRecorder.filename: invalid arguments");
	}
	return 1;
}

/*! Close the video file
	@name M:close
*/
int lua_video_recorder_close(lua_State *L) {
	VideoRecorder *s = Glue<VideoRecorder>::checkto(L, 1);
	if(s) {
		s->close();
	}
	else {
		luaL_error(L, "VideoRecorder.close: invalid arguments");
	}
	return 1;
}

/*! Save a frame of video
	@param mat The video frame to save
	@name M:fromarray
*/
int lua_video_recorder_fromarray(lua_State *L) {
	VideoRecorder *s = Glue<VideoRecorder>::checkto(L, 1);
	AlloArray *lat = lua_array_to(L, 2);
	if(s && lat) {
		s->save_frame(lat);
	}
	else {
		luaL_error(L, "VideoRecorder.fromarray: invalid arguments");
	}
	return 0;
}

template<> const char * Glue<VideoRecorder>::usr_name() { return "VideoRecorder"; }
template<> bool Glue<VideoRecorder>::usr_has_index() { return true; }

template<> void Glue<VideoRecorder>::usr_push(lua_State * L, VideoRecorder * u) {
	Glue<VideoRecorder>::usr_attr_push(L);
}

template<> VideoRecorder * Glue<VideoRecorder>::usr_new(lua_State * L) {
	VideoRecorder *rec = new VideoRecorder();
	return rec;
}

template<> void Glue<VideoRecorder>::usr_index(lua_State * L, VideoRecorder * u) {
	Glue<VideoRecorder>::usr_attr_index(L);
}

template<> void Glue<VideoRecorder>::usr_newindex(lua_State * L, VideoRecorder * u) {
	Glue<VideoRecorder>::usr_attr_newindex(L);
}

template<> int Glue<VideoRecorder>::usr_tostring(lua_State * L, VideoRecorder * u) {
	lua_pushfstring(L, "%s: %p", Glue<VideoRecorder>::usr_name(), u); 
	return 1;
}

template<> void Glue<VideoRecorder>::usr_gc(lua_State * L, VideoRecorder * u) { 
	delete u;
}

template<> void Glue<VideoRecorder>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{"open", lua_video_recorder_open},
		{"close", lua_video_recorder_close},
		{"fromarray", lua_video_recorder_fromarray},
		{ NULL, NULL}
	};
	
	static luaL_reg getters[] = {
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ NULL, NULL}
	};
	Glue<VideoRecorder>::usr_attr_mt(L, methods, getters, setters);
}




#ifdef __cplusplus
extern "C" {
#endif

int luaopen_video(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	
	struct luaL_reg lib[] = {
		{ NULL, NULL }
	};
	luaL_register(L, libname, lib);
	
	Glue<Video>::define(L);
	Glue<Video>::register_ctor(L);
	
	Glue<VideoCamera>::define(L);
	Glue<VideoCamera>::register_ctor(L);
	
	Glue<VideoRecorder>::define(L);
	Glue<VideoRecorder>::register_ctor(L);
	
	lua::pushfield<int>(L, -1, "RGB", (int)video::PIX_FMT_RGB);
	lua::pushfield<int>(L, -1, "RGBA", (int)video::PIX_FMT_RGBA);

	return 1;
}


#ifdef __cplusplus
}
#endif