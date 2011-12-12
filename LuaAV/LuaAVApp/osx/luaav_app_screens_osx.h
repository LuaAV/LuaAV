#ifndef _LUAAV_SCREENS_OSX_H_
#define _LUAAV_SCREENS_OSX_H_ 1

#include "luaav_app_screens.h"
#import <Cocoa/Cocoa.h>

namespace luaav {
namespace screens {

extern NSScreen * main_screen();
extern NSScreen * screen(int idx);
extern int screen_count();
extern luaav_pt to_luaav_pt(const luaav_pt &pt);
extern luaav_pt from_luaav_pt(const luaav_pt &pt);
extern luaav_rect to_luaav_rect(const luaav_rect &r);
extern luaav_rect from_luaav_rect(const luaav_rect &r);
extern luaav_rect from_luaav_rect_on_screen(const luaav_rect &r);
extern void screen_rect(NSScreen *screen, luaav_rect &rect);
extern void convert_rect(NSRect src, luaav_rect &rect);
extern NSScreen * screen_for_rect(const luaav_rect &rect);

}   // screens::
}   // luaav::

#endif