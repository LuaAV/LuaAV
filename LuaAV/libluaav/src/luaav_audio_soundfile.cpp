
#include "lua_array.h"
#include "sndfile.h"

/*! Reading soundfiles from disk.

	@module audio.soundfile
*/

/*!	Read in a soundfile.
	The Array returned is one dimensional. Multi-channel soundfiles return multi-component arrays.
	
	<luacode>
	local sf = audio.soundfile(LuaAV.findfile("gong.wav"))
	print(sf)							--> Array
	print(sf.components)				--> 2 (stereo)
	print(sf.dim[1])					--> 265776 (sample frames)
	print(sf.dim[1]/audio.samplerate)	--> 6.03 (seconds)
	</luacode>
		
	@param soundfile path (string)
	@ret Array containing the soundfile data
	@name M.read
*/
int lua_soundfile_read(lua_State * L) {
	const char * path = luaL_checkstring(L, 1);
	
	SNDFILE *sf;
	SF_INFO sfinfo;
	
	sf = sf_open(path, SFM_READ, &sfinfo);
	if (sf == NULL) luaL_error(L, "failed to open soundfile %s", path);
	
	// call Array constructor with appropriate format:
	lua_pushcfunction(L, Glue<al::ArrayWrapper>::create);
	lua_pushinteger(L, sfinfo.channels);	
	lua_pushinteger(L, AlloFloat32Ty);
	lua_createtable(L, 1, 0);
	lua_pushinteger(L, sfinfo.frames);
	lua_rawseti(L, -2, 1);
	lua_call(L, 3, 1);
	
	al::ArrayWrapper * a = Glue<al::ArrayWrapper>::checkto(L, -1);

	// copy data in:
	sf_readf_float(sf, (float *)a->data.ptr, sfinfo.frames);
	
	sf_close(sf);
	
	return 1;
}

extern "C" int luaopen_audio_soundfile(lua_State * L) {

	// ensure array already is loaded:
	luaL_dostring(L, "require'Array'");
	lua_pop(L, 1);

	struct luaL_reg lib[] = {
		{ "read", lua_soundfile_read },
		{ NULL, NULL },
	};
	luaL_register(L, "audio.soundfile", lib);
	return 1;
}