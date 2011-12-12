#import "lua_utility.h"
#import "luaav_app.h"
#import "luaav_app_screens_osx.h"

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

namespace luaav {
namespace screens {

NSScreen * main_screen() {
	return screen(0);
}

NSScreen * screen(int idx) {
    return [[NSScreen screens] objectAtIndex:idx];
}

int screen_count() {
    return [[NSScreen screens] count];
}

luaav_pt to_luaav_pt(const luaav_pt &pt) {
	NSScreen *main = main_screen();
	NSRect main_frame = [main frame];
	return luaav_pt(pt.x, main_frame.size.height - pt.y);
}

luaav_pt from_luaav_pt(const luaav_pt &pt) {
	NSScreen *main = main_screen();
	NSRect main_frame = [main frame];
	return luaav_pt(pt.x, main_frame.size.height - pt.y);
}

luaav_rect to_luaav_rect(const luaav_rect &r) {
	luaav_pt lp = to_luaav_pt(luaav_pt(r.x, r.y));
	return luaav_rect(lp.x, lp.y - r.height, r.width, r.height);
}

luaav_rect from_luaav_rect(const luaav_rect &r) {
	luaav_pt lp = to_luaav_pt(luaav_pt(r.x, r.y));
	return luaav_rect(lp.x, lp.y - r.height, r.width, r.height);
}

luaav_rect from_luaav_rect_on_screen(const luaav_rect &r) {
	luaav_rect rr = screens::from_luaav_rect(r);
	NSScreen *screen = screens::screen_for_rect(r);
	NSRect screenframe = [screen frame];
	rr.y -= screenframe.origin.y;
	return rr;
}

void screen_rect(NSScreen *screen, luaav_rect &rect) {
	NSRect mainframe = [[NSScreen mainScreen] frame];
	NSRect frame = [screen frame];
	
	luaav_rect mainrect;
	convert_rect(mainframe, mainrect);
	convert_rect(frame, rect);
	
	rect.y = mainrect.height - rect.y - rect.height;
}

void convert_rect(NSRect src, luaav_rect &rect) {
	rect.x = src.origin.x;
	rect.y = src.origin.y;
	rect.width = src.size.width;
	rect.height = src.size.height;
}

NSScreen * screen_for_rect(const luaav_rect &rect) {
	NSArray *screens = [NSScreen screens];
	NSScreen *s = nil;
	
	luaav_pt p;
	p.x = rect.x;
	p.y = rect.y;
	
	int sz = [screens count];
	for(int i=0; i < sz; i++) {
		
		luaav_rect r;
		s = [screens objectAtIndex:i];
		
		screen_rect(s, r);
		
		
		if( luaav_within_rect(p, r)) {
			return s;
		}
	}
	
	s = [screens objectAtIndex:0];
	return s;
}

int push_rect(lua_State *L, luaav_rect &r) {
	lua_newtable(L);
		lua::push(L, r.x);
		lua_rawseti(L, -2, 1);
		
		lua::push(L, r.y);
		lua_rawseti(L, -2, 2);
		
		lua::push(L, r.width);
		lua_rawseti(L, -2, 3);
		
		lua::push(L, r.height);
		lua_rawseti(L, -2, 4);
	return 1;

}

int push_screen(lua_State *L, NSScreen *s) {
	luaav_rect r;
	screen_rect(s, r);
	push_rect(L, r);

	return 1;
}

extern int main_screen(lua_State *L) {
	return push_screen(L, main_screen());
}

int deepest_screen(lua_State *L) {
	return push_screen(L, [NSScreen deepestScreen]);
}

extern int screens(lua_State *L) {
	lua_newtable(L);
	int n = screen_count();
	for(int i=0; i < n; i++) {
		NSScreen *s = screen(i);
		push_screen(L, s);
		lua_rawseti(L, -2, i+1);
	}
	return 1;
}

int hide_menubar(lua_State *L) {
	if(lua::is<bool>(L, 1)) {
		bool v = lua::to<bool>(L, 1);
		if(v) {
			SetSystemUIMode(kUIModeAllSuppressed, 0);
		}
		else {
			SetSystemUIMode(kUIModeNormal, 0);
		}
	}
	else {
		luaL_error(L, "screens.hide_menubar: takes boolean as argument");
	}
	return 0;
}


}	// screens::
}	// luaav::

#define LUA_METHOD_SCREENS(name) {#name, luaav::screens::name}

int luaopen_screens(lua_State *L) {
	const char * libname = lua::to<const char *>(L, 1);
	struct luaL_reg lib[] = {
		LUA_METHOD_SCREENS(main_screen),
		LUA_METHOD_SCREENS(deepest_screen),
		LUA_METHOD_SCREENS(screens),
		LUA_METHOD_SCREENS(hide_menubar),
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);

	return 1;
}

void luaav_app_screens_init() {
	luaav_add_opt_module("app.screens", luaopen_screens);

}