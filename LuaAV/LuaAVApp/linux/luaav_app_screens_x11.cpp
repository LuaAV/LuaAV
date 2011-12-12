#include "luaav_app.h"
#include "luaav_app_screens_x11.h"
#include "lua_utility.h"
#include <X11/Xutil.h>
#include <X11/Xatom.h>

/* the XF86 Video Mode extension allows us to change the displaymode of the server
 * this allows us to set the display to fullscreen and also read videomodes and
 * other information.
 */
#include <X11/extensions/xf86vmode.h>

/*
#define VisualNoMask		0x0
#define VisualIDMask		0x1
#define VisualScreenMask	0x2
#define VisualDepthMask		0x4
#define VisualClassMask		0x8
#define VisualRedMaskMask	0x10
#define VisualGreenMaskMask	0x20
#define VisualBlueMaskMask	0x40
#define VisualColormapSizeMask	0x80
#define VisualBitsPerRGBMask	0x100
#define VisualAllMask		0x1FF


typedef struct {
	Visual *visual;
	VisualID visualid;
	int screen;
	unsigned int depth;
	int class;
	unsigned long red_mask;
	unsigned long green_mask;
	unsigned long blue_mask;
	int colormap_size;
	int bits_per_rgb;
} XVisualInfo;

m_net_supported = XInternAtom(disp, "_NET_SUPPORTED", False);
00393     m_net_client_list = XInternAtom(disp, "_NET_CLIENT_LIST", False);
00394     m_net_client_list_stacking = XInternAtom(disp, "_NET_CLIENT_LIST_STACKING", False);
00395     m_net_number_of_desktops = XInternAtom(disp, "_NET_NUMBER_OF_DESKTOPS", False);
00396     m_net_desktop_geometry = XInternAtom(disp, "_NET_DESKTOP_GEOMETRY", False);
00397     m_net_desktop_viewport = XInternAtom(disp, "_NET_DESKTOP_VIEWPORT", False);
00398     m_net_current_desktop = XInternAtom(disp, "_NET_CURRENT_DESKTOP", False);
00399     m_net_desktop_names = XInternAtom(disp, "_NET_DESKTOP_NAMES", False);
00400     m_net_active_window = XInternAtom(disp, "_NET_ACTIVE_WINDOW", False);
00401     m_net_workarea = XInternAtom(disp, "_NET_WORKAREA", False);
00402     m_net_supporting_wm_check = XInternAtom(disp, "_NET_SUPPORTING_WM_CHECK", False);
00403     m_net_virtual_roots = XInternAtom(disp, "_NET_VIRTUAL_ROOTS", False);
00404
00405     m_net_close_window = XInternAtom(disp, "_NET_CLOSE_WINDOW", False);
00406     m_net_moveresize_window = XInternAtom(disp, "_NET_MOVERESIZE_WINDOW", False);
00407
00408     // TODO: implement this one
00409     m_net_wm_moveresize = XInternAtom(disp, "_NET_WM_MOVERESIZE", False);
00410
00411     m_net_properties = XInternAtom(disp, "_NET_PROPERTIES", False);
00412     m_net_wm_name = XInternAtom(disp, "_NET_WM_NAME", False);
00413     m_net_wm_desktop = XInternAtom(disp, "_NET_WM_DESKTOP", False);
00414     m_net_wm_window_type = XInternAtom(disp, "_NET_WM_WINDOW_TYPE", False);
00415
00416     // state atom and the supported state atoms
00417     m_net_wm_state = XInternAtom(disp, "_NET_WM_STATE", False);
00418     m_net_wm_state_sticky = XInternAtom(disp, "_NET_WM_STATE_STICKY", False);
00419     m_net_wm_state_shaded = XInternAtom(disp, "_NET_WM_STATE_SHADED", False);
00420     m_net_wm_state_maximized_horz = XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
00421     m_net_wm_state_maximized_vert = XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_VERT", False);
00422     m_net_wm_state_fullscreen = XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", False);
00423
00424     m_net_wm_strut = XInternAtom(disp, "_NET_WM_STRUT", False);
00425     m_net_wm_icon_geometry = XInternAtom(disp, "_NET_WM_ICON_GEOMETRY", False);
00426     m_net_wm_icon = XInternAtom(disp, "_NET_WM_ICON", False);
00427     m_net_wm_pid = XInternAtom(disp, "_NET_WM_PID", False);
00428     m_net_wm_handled_icons = XInternAtom(disp, "_NET_WM_HANDLED_ICONS", False);
00429
00430     m_net_wm_ping = XInternAtom(disp, "_NET_WM_PING", False);

 */

namespace luaav {
namespace screens {

Display *g_display = NULL;


Screen * main_screen() {
    return XDefaultScreenOfDisplay(g_display);
}

Screen * screen(int idx) {
    return XScreenOfDisplay(g_display, idx);
}

int screen_count() {
    return XScreenCount(g_display);
}

/*
    Display *display;
int screen_num;
unsigned int display_width, display_height;
   .
   .
// Open display
screen_num = DefaultScreen(display);
   .
   .
// Display size is a member of display structure
display_width = DisplayWidth(display, screen_num);
display_height = DisplayHeight(display, screen_num);

*/

void screen_rect(Screen *screen, luaav_rect *rect) {
    int nmodes = 0;
    XF86VidModeModeInfo **modes;
    XF86VidModeGetAllModeLines(screens::g_display, 0, &nmodes, &modes);

    rect->width = modes[0]->hdisplay;
    rect->height = modes[0]->vdisplay;
//    printf("nmodes: %d   mode[0]: %d %d %d %d\n", nmodes, modes[0]->hdisplay, modes[0]->vdisplay);

 //   for(int i=nmodes-1; i >= 0; i--) {
  //      XF86VidModeDeleteModeLine(screens::g_display, 0, modes[i]);
 //   }
  //  printf("modes\n");

    Atom hints = XInternAtom(g_display, "_NET_WORKAREA", True);
    if(hints != None) {
        Window root = RootWindowOfScreen(screen);

        unsigned long nitems, bytesLeft;
        Atom actualType;
        int actualFormat;
        unsigned char* data = 0;

       if (XGetWindowProperty(g_display, root, hints, 0, 4, False,
                                XA_CARDINAL, &actualType, &actualFormat, &nitems, &bytesLeft,
                                &data) == Success)
        {
            const long* const position = (const long*)data;
            if(actualType == XA_CARDINAL && actualFormat == 32 && nitems == 4) {
             //   rect->x = position[0];
            //    rect->y = position[1];
                rect->width = (rect->width < position[2]) ? position[2] : rect->width;
                rect->height = (rect->height < position[3]) ? position[3] : rect->height;
 //               rect->height = position[3];
              //  printf("cardinal dims: %d %d\n", position[2], position[3]);
            }
            XFree(data);
        }
/*        printf("geom\n");
        Atom ageom = XInternAtom(g_display, "_NET_DESKTOP_GEOMETRY", True);
        if (XGetWindowProperty(g_display, root, ageom, 0, 2, False,
                                XA_CARDINAL, &actualType, &actualFormat, &nitems, &bytesLeft,
                                &data) == Success)
        {
            const long* const position = (const long*)data;
            if(actualType == XA_CARDINAL && actualFormat == 32 && nitems == 2) {
              //  rect->x = position[0];
             //  rect->y = position[1];
                rect->width = position[0];
                rect->height = position[1];
            }
            XFree(data);
        }
*/
//        printf("view\n");
        Atom aview = XInternAtom(g_display, "_NET_DESKTOP_VIEWPORT", True);
        if (XGetWindowProperty(g_display, root, aview, 0, 2, False,
                                XA_CARDINAL, &actualType, &actualFormat, &nitems, &bytesLeft,
                                &data) == Success)
        {
            const long* const position = (const long*)data;
            if(actualType == XA_CARDINAL && actualFormat == 32 && nitems == 2) {
                rect->x = position[0];
                rect->y = position[1];
            }
            XFree(data);
        }
  //      printf("done\n");
    }
}


int push_rect(lua_State *L, luaav_rect &rect) {
    lua_newtable(L);
        lua_pushinteger(L, rect.x);
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, rect.y);
        lua_rawseti(L, -2, 2);
        lua_pushinteger(L, rect.width);
        lua_rawseti(L, -2, 3);
        lua_pushinteger(L, rect.height);
        lua_rawseti(L, -2, 4);

    return 1;
}


int push_screen(lua_State *L, Screen *screen) {
    luaav_rect rect;
    screen_rect(screen, &rect);
    push_rect(L, rect);

	return 1;
}

int lua_main_screen(lua_State *L) {
    Screen *mainScreen = main_screen();
    return push_screen(L, mainScreen);
}

int lua_deepest_screen(lua_State *L) {
    luaL_error(L, "deepest_screen not implemented yet");
    return 1;
}

int lua_screens(lua_State *L) {
	lua_newtable(L);

	int screen_count = screens::screen_count();
	for(int i=0; i < screen_count; i++) {
        Screen *screen = screens::screen(i);
        push_screen(L, screen);
        lua_rawseti(L, -2, i+1);
	}

	return 1;
}


}	//screens::
}	//luaav::

#define LUA_METHOD_SCREENS(name) {#name, luaav::screens::lua_##name}

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_screens(lua_State *L) {
	const char *libname = lua_tostring(L, -1);

	struct luaL_reg lib[] = {
		LUA_METHOD_SCREENS(main_screen),
		LUA_METHOD_SCREENS(deepest_screen),
		LUA_METHOD_SCREENS(screens),
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);

	luaav::screens::g_display = XOpenDisplay(NULL); //falls back to DISPLAY env variable for name

	return 1;
}

void luaav_app_screens_init() {
	luaav_add_opt_module("app.screens", luaopen_screens);
}

#ifdef __cplusplus
}
#endif

