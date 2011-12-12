#ifndef INC_LUAOPEN_VAMP_H
#define INC_LUAOPEN_VAMP_H


#ifdef _WIN32

#include <windows.h>

#define DLOPEN(a,b)  LoadLibrary((a).c_str())
#define DLSYM(a,b)   GetProcAddress((HINSTANCE)(a),(b))
#define DLCLOSE(a)   FreeLibrary((HINSTANCE)(a))
#define DLERROR()    ""

#define PLUGIN_SUFFIX "dll"

#else

#include <dlfcn.h>

#define DLOPEN(a,b)  dlopen((a).c_str(),(b))
#define DLSYM(a,b)   dlsym((a),(b))
#define DLCLOSE(a)   dlclose((a))
#define DLERROR()    dlerror()

#ifdef __APPLE__

#define PLUGIN_SUFFIX  "dylib"
#define HAVE_OPENDIR 1

#else 

#define PLUGIN_SUFFIX  "so"
#define HAVE_OPENDIR 1

#endif /* __APPLE__ */

#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#include "luaav.h"

extern int luaopen_vamp(lua_State * L);

#ifdef __cplusplus
}

#include "vamp-hostsdk/PluginHostAdapter.h"
#include "vamp-hostsdk/PluginInputDomainAdapter.h"
#include "vamp-hostsdk/PluginLoader.h"

#endif

#endif
