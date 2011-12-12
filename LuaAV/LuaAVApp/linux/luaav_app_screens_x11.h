#ifndef INCLUDE_LUAAV_APP_SCREENS_X11_H
#define INCLUDE_LUAAV_APP_SCREENS_X11_H

#include "luaav.h"
#include "luaav_utility.hpp"
#include <X11/Xutil.h>

namespace luaav {
namespace screens {

extern Display *g_display;

extern Screen * main_screen();
extern Screen * screen(int idx);
extern int screen_count();
extern void screen_rect(Screen *screen, luaav_rect *rect);

}   //screens::
}   //luaav::

#endif	// INCLUDE_LUAAV_APP_SCREENS_X11_H
