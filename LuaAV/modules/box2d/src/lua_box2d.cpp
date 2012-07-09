#include "lua_box2d.h"
#include "lua_utility.h"
#include "World2d.h"

/*! Box2D - 2D physics engine
	Box2D is a 2D rigid body simulation library for games. It can be used to make objects move in 
	believable ways and make the virtual worlds more interactive. From the world's point of view a 
	physics engine is just a system for procedural animation.
	
	@module box2d
*/

#include <Box2D/Box2D.h>

namespace lua {

template<> inline void push<b2BodyType>(lua_State * L, b2BodyType v) {
	lua_pushinteger(L, (lua_Integer)v);
}

template<> inline void push<ContactListener::Events>(lua_State * L, ContactListener::Events v) {
	lua_pushinteger(L, (lua_Integer)v);
}

}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_box2d_Contact(lua_State *L);
extern int luaopen_box2d_Body(lua_State *L);
extern int luaopen_box2d_Joint(lua_State *L);
extern int luaopen_box2d_World(lua_State *L);

extern int luaopen_box2d(lua_State *L) {
	const char *libname = lua_tostring(L, -1);
	static const luaL_reg space_lib[] = {
		{NULL, NULL}
	};
	luaL_register(L, libname, space_lib);
	
	// set Box2D enums
	lua::pushfield<>(L, -1, "staticBody", b2_staticBody);
	lua::pushfield<>(L, -1, "kinematicBody", b2_kinematicBody);
	lua::pushfield<>(L, -1, "dynamicBody", b2_dynamicBody);
	
	lua::pushfield<>(L, -1, "BeginContact", ContactListener::eBeginContact);
	lua::pushfield<>(L, -1, "EndContact", ContactListener::eEndContact);
	lua::pushfield<>(L, -1, "PreSolve", ContactListener::ePreSolve);
	lua::pushfield<>(L, -1, "PostSolve", ContactListener::ePostSolve);
	
	lua::preloadlib(L, "box2d.Contact", luaopen_box2d_Contact);
	lua::preloadlib(L, "box2d.Body", luaopen_box2d_Body);
	lua::preloadlib(L, "box2d.Joint", luaopen_box2d_Joint);
	lua::preloadlib(L, "box2d.World", luaopen_box2d_World);
	
	return 1;
}

#ifdef __cplusplus
}
#endif