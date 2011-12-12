#ifndef INC_LUAOPEN_CSOUND_H
#define INC_LUAOPEN_CSOUND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "csound.h"

extern int luaopen_csound(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif
