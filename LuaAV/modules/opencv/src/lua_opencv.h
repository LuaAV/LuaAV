#ifndef LUA_OPENCV_H

#include "lua_array.h"
#include "cv.h"
#include "cv.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

extern int opencv_type(int components, AlloTy ty);


#endif // LUA_OPENCV_H
