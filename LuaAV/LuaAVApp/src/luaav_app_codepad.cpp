#include "luaav_app_codepad.h"

namespace luaav {

const char * CodePad::name = "CodePad";
const char * CodePad::superclass_table[] = {NULL};


#define LUA_METHOD(name, type) {#name, Self::name, LuaMethod::type}

const LuaMethod CodePad::lua_methods[] = {
	LUA_METHOD(current_name, METHOD),	
	{"new", Self::__new, LuaMethod::METHOD},
	{"name", Self::name_method, LuaMethod::METHOD},	
	LUA_METHOD(text, METHOD),
	LUA_METHOD(create_format, METHOD),
	LUA_METHOD(format, METHOD),
	LUA_METHOD(selection, METHOD),
	LUA_METHOD(highlight_error, METHOD),
	LUA_METHOD(paste, METHOD),
	LUA_METHOD(cursor_pos, METHOD),
	{0, 0, (LuaMethod::MethodType)0}
};

#undef LUA_METHOD

CodePad :: CodePad(const char *name, int x, int y, int w, int h)
:	mImpl(0)
{
	implInit();
	implCreate(name, x, y, w, h);
}

CodePad :: ~CodePad() {
	implDestroy();
	implDestruct();
}

void CodePad :: eval(const char *code) {
	lua_State *L = getState();
	instance_to_userdata(L, this);
	lua_getfenv(L, -1);
	lua_getfield(L, -1, "eval");
	
	if(lua_isfunction(L, -1)) {
		lua_pushvalue(L, -3);
		lua_pushstring(L, code);
		lua::pcall(L, 2);
	}
	else {
		lua_pop(L, 1);
	}
	
	lua_pop(L, 2);	// env table and udata
}

void CodePad :: highlight(int line, const char *text) {
	lua_State *L = getState();
	instance_to_userdata(L, this);
	lua_getfenv(L, -1);
	lua_getfield(L, -1, "highlight");
	
	if(lua_isfunction(L, -1)) {
		lua_pushvalue(L, -3);
		lua_pushinteger(L, line);
		lua_pushstring(L, text);
		lua::pcall(L, 3);
	}
	else {
		lua_pop(L, 1);
	}
	
	lua_pop(L, 2);	// env table and udata
}

void CodePad :: insert(int line, int n) {
	lua_State *L = getState();
	instance_to_userdata(L, this);
	lua_getfenv(L, -1);
	lua_getfield(L, -1, "insert");
	
	if(lua_isfunction(L, -1)) {
		lua_pushvalue(L, -3);
		lua_pushinteger(L, line);
		lua_pushinteger(L, n);
		lua::pcall(L, 3);
	}
	else {
		lua_pop(L, 1);
	}
}

void CodePad :: remove(int line, int n) {
	lua_State *L = getState();
	instance_to_userdata(L, this);
	lua_getfenv(L, -1);
	lua_getfield(L, -1, "remove");
	
	if(lua_isfunction(L, -1)) {
		lua_pushvalue(L, -3);
		lua_pushinteger(L, line);
		lua_pushinteger(L, n);
		lua::pcall(L, 3);
	}
	else {
		lua_pop(L, 1);
	}
}

int CodePad :: current_name(lua_State *L) {
	const char *name = implGetCurrentName();
	if(name) {
		lua_pushstring(L, name);
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

int CodePad :: __new(lua_State *L) {
	Self *s = NULL;
	const char *name = lua_type(L, 1) == LUA_TSTRING ? lua_tostring(L, 1) : "<no title>";
	if(lua_isnumber(L, 2) && lua_isnumber(L, 3) && 
		lua_isnumber(L, 4) && lua_isnumber(L, 5))
	{
		int x = (int)lua_tonumber(L, 2);
		int y = (int)lua_tonumber(L, 3);
		int w = (int)lua_tonumber(L, 4);
		int h = (int)lua_tonumber(L, 5);
		s = new Self(name, x, y, w, h);
	}
	else {
		s = new Self(name);
	}
	Self::udata_push(L, s);
	return 1;
}

int CodePad :: name_method(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s) {
		if(lua_type(L, 2) == LUA_TSTRING) {
			const char *name = lua_tostring(L, 2);
			s->implSetName(name);
		}
		else {
			lua_pushstring(L, s->implGetName());
			return 1;;
		}
	}
	else {
		luaL_error(L, "CodePad.name: invalid object");
	}
	return 0;
}

int CodePad :: text(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s) {
		if(lua_type(L, 2) == LUA_TSTRING) {
			const char *t = lua_tostring(L, 2);
			s->implSetText(t);
		}
		else {
			lua_pushstring(L, s->implGetText().data());
		}
	}
	else {
		luaL_error(L, "CodePad.text: invalid object");
	}
	return 1;
}

int CodePad :: create_format(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s && lua_istable(L, 2)) {
		float color[] = {0, 0, 0};
		std::map<std::string, int> properties;

		lua_getfield(L, 2, "name");
		if(lua_type(L, -1) != LUA_TSTRING) {
			luaL_error(L, "CodePad.create_format: must have a name field");
		}

		lua_getfield(L, 2, "color");
		if(lua_istable(L, -1)) {
			for(int i=0; i < 3; i++) {
				lua_rawgeti(L, -1, i+1);
				color[i] = lua_tonumber(L, -1);
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);

	
		const char *name = lua_tostring(L, -1);
		lua_pop(L, 1);

		// should use lua_next, but will hard code some vals for now
		lua_getfield(L, 2, "weight");
		if(lua_isnumber(L, -1)) {
			int v = (int)lua_tonumber(L, -1);
			properties.insert(std::pair<std::string, int>(std::string("font_weight"), v));
		}
		lua_pop(L, 1);

		lua_getfield(L, 2, "italic");
		if(lua_isnumber(L, -1)) {
			int v = (int)lua_tonumber(L, -1);
			properties.insert(std::pair<std::string, int>(std::string("italic"), v));
		}
		lua_pop(L, 1);

		lua_getfield(L, 2, "underline");
		if(lua_isnumber(L, -1)) {
			int v = (int)lua_tonumber(L, -1);
			properties.insert(std::pair<std::string, int>(std::string("underline"), v));
		}
		lua_pop(L, 1);

		s->implCreateFormat(name, color, properties);
	}
	else {
		luaL_error(L, "CodePad.create_format: invalid arguments");
	}
	return 0;
}

int CodePad :: format(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s && lua_type(L, 2) == LUA_TSTRING && lua_isnumber(L, 3)
		&& lua_isnumber(L, 4))
	{
		const char *name = lua_tostring(L, 2);
		int start = lua_tonumber(L, 3);
		int end = lua_tonumber(L, 4);
		s->implFormat(name, start, end);
	}
	else {
		luaL_error(L, "CodePad.format: invalid arguments");
	}
	return 0;
}

int CodePad :: selection(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s){
		int line1, pos1, line2, pos2;
		s->implGetSelection(line1, pos1, line2, pos2);
		lua_newtable(L);
			lua_pushnumber(L, line1);
			lua_rawseti(L, -2, 1);
			lua_pushnumber(L, pos1);
			lua_rawseti(L, -2, 2);

		lua_newtable(L);
			lua_pushnumber(L, line2);
			lua_rawseti(L, -2, 1);
			lua_pushnumber(L, pos2);
			lua_rawseti(L, -2, 2);
	}
	else {
		luaL_error(L, "CodePad.selection: invalid arguments");
	}
	return 2;
}

int CodePad :: highlight_error(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s) {
		if(lua_isnumber(L, 2)) {
			int line = (int)lua_tonumber(L, 2);
			s->implErrorHighlight(line);
		}
		else {
			s->implClearErrorHighlight();
		}
	}
	else {
		luaL_error(L, "CodePad.highlight_error: invalid arguments");
	}
	return 0;
}

int CodePad :: paste(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s && lua_isnumber(L, 2) && lua_type(L, 3) == LUA_TSTRING) {
		int pos = (int)lua_tonumber(L, 2);
		const char *text = lua_tostring(L, 3);
		s->implPaste(pos, text);
	}
	else {
		luaL_error(L, "CodePad.paste: invalid arguments");
	}
	return 0;
}

int CodePad :: cursor_pos(lua_State *L) {
	Self *s = Self::to_udata(L, 1);
	if(s) {
		lua_pushinteger(L, s->implCursorPos());
	}
	else {
		luaL_error(L, "CodePad.cursor_pos: invalid arguments");
	}
	return 1;
}

}	// luaav::
