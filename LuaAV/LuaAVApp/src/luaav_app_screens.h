#ifndef _LUAAV_SCREENS_H_
#define _LUAAV_SCREENS_H_ 1

#include "luaav_utility.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void luaav_app_screens_init();

/*! Screen querying module
	screens provides platform-specific functionality for querying
	attached screens of a system.  It can be used, for example, to 
	automatically position and size windows based on attached monitors.

	@module app.screens
*/
int luaopen_screens(lua_State *L);

/*! Get a list of all available screens
	Returns a list of all available screens as tables 
	where each screen is represented as an x, y, w, h 
	array of 4 numbers.
	
	@ret screens as an array
*/
extern int screens(lua_State *L);

/*! Get the main screen
	Returns the main screen as an x, y, w, h 
	array of 4 numbers.
	
	@ret main screen
*/
extern int main_screen(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif