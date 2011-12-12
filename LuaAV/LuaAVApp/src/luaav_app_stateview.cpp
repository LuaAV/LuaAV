#include "luaav_app_stateview.h"
#include <cstring>

namespace luaav {

const char * StateView::name = "StateView";
const char * StateView::superclass_table[] = {NULL};


#define LUA_METHOD(name, type) {#name, Self::name, LuaMethod::type}

const LuaMethod StateView::lua_methods[] = {
	{"new", Self::__new, LuaMethod::METHOD},
	LUA_METHOD(add_item, METHOD),
	{0, 0, (LuaMethod::MethodType)0}
};

#undef LUA_METHOD

StateView :: StateView(int x, int y, int w, int h)
:	mImpl(0)
{
	implInit();
	implCreate(x, y, w, h);
}

StateView :: ~StateView() {
	implDestroy();
	implDestruct();
}

void StateView :: children(const char *parent_name) {
	lua_State *L = getState();
	instance_to_userdata(L, this);
	lua_getfenv(L, -1);
	lua_getfield(L, -1, "children");
	
	if(lua_isfunction(L, -1)) {
		lua_pushvalue(L, -3);
		lua_pushstring(L, parent_name);
		lua::pcall(L, 2);
	}
	else {
		lua_pop(L, 1);
	}
}

int StateView :: __new(lua_State *L) {
	Self *s = NULL;
	if(lua_isnumber(L, 1) && lua_isnumber(L, 2) && 
		lua_isnumber(L, 3) && lua_isnumber(L, 4))
	{
		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);
		int w = (int)lua_tonumber(L, 3);
		int h = (int)lua_tonumber(L, 4);
		s = new Self(x, y, w, h);
	}
	else {
		s = new Self();
	}
	Self::udata_push(L, s);
	return 1;
}

int StateView :: add_item(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s && lua_type(L, 2) == LUA_TSTRING && 
		lua_type(L, 3) == LUA_TSTRING)
	{
		const char *name = lua_tostring(L, 2);
		const char *type = lua_tostring(L, 3);
		const char *v = lua_tostring(L, 4);
		
		s->implAddItem(name, type, v);
	}
	else {
		luaL_error(L, "StateView.add_item: invalid object or arguments");
	}
	return 0;
}

}	// luaav::
