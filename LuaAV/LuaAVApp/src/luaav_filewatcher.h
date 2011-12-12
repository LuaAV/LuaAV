#ifndef INCLUDE_LUAAV_FILEWATCHER_H
#define INCLUDE_LUAAV_FILEWATCHER_H

#include "luaav.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

AL_API int luaopen_app_file(lua_State * L);

#ifdef __cplusplus
}
#endif

namespace luaav {


class FileWatcher {
public:
	typedef void (*watcher_cb)(const char *, void*);

	static void watchfile(const char *filename, watcher_cb cb=0, void *udata=0);
	static bool watchingfile(const char *filename);
	static void unwatchfile(const char *filename);
	
	
	FileWatcher(lua_State *L, const char *f)
	: L(L), filename(f)
	{}
	
	~FileWatcher() {}
	
	std::string filename;
	lua_State *L;	
};

}	// luaav::

#endif	// INCLUDE_LUAAV_FILEWATCHER_H

