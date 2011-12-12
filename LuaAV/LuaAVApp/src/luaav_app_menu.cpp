#include "lua_utility.h"
#include "luaav_app.h"
#include "luaav_app_menu.h"
#include "lua_glue.h"

#include <string.h>

namespace luaav {

#define LUA_METHOD(name, type) {#name, Self::name, LuaMethod::type}


MenuBar :: MenuBar() {
	implInit();
}

MenuBar :: ~MenuBar() {
	implDestruct();
}

} // luaav::


using luaav::MenuBar;
using luaav::Menu;
using luaav::MenuItem;

/*! An application menu bar
	Represents an application's menu bar (namely the LuaAV menu bar).  Contains 
	functionality for adding menus.
	
	@module app.MenuBar
*/

/*! Appends a menu to the menu bar.
	If a menu with the same name already exists, it will 
	replace that menu.
	
	@param menu Menu to append
	@LuaMethod METHOD
	@name M:append
*/
int menubar_append(lua_State *L) {
	MenuBar *s = Glue<MenuBar>::checkto(L, 1);
	Menu *m = Glue<Menu>::checkto(L, 2);
	if(s && m) {
		int idx = -1;
		if(s->implMenuExists(m, idx)) {
			s->implInsert(m, idx);
		}
		else {
			s->implAppend(m);
		}

		// puts a reference to the Menu in Self's env table
		lua_getfenv(L, 1);

		// store a reference at an array index
		int len = lua_objlen(L, -1);
		lua_pushvalue(L, 2);
		lua_rawseti(L, -2, len+1);

		// pop env table
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "MenuBar.append: invalid object or arguments");
	}
	return 0;
}

/*! Gets a menu by name

	@param name Menu name to find
	@ret named menu if it exists
	@LuaMethod METHOD
	@name M:menu
*/
int menubar_menu(lua_State *L) {
	MenuBar *s = Glue<MenuBar>::checkto(L, 1);
	if(s && lua::is<const char *>(L, 2)) {
		// search for a menu "name" in the env table of MenuBar
		const char *name = lua::to<const char *>(L, 2);
		lua_getfenv(L, 1);
		int len = lua_objlen(L, -1);
		for(int i=1; i<= len; i++) {
			// get the Menu object
			lua_rawgeti(L, -1, i);
			Menu *m = Glue<Menu>::checkto(L, -1);
			if(m) {
				if(strcmp(m->implGetLabel().data(), name) == 0) {
					// on success, put it at the location of the env
					// table so that gets popped instead
					lua_insert(L, 3);
					lua_pop(L, 1);
					return 1;
				}
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "MenuBar.menu: invalid object or arguments");
	}
	return 0;
}


template<> const char * Glue<MenuBar>::usr_name() { return "MenuBar"; }
template<> bool Glue<MenuBar>::usr_has_index() { return true; }


int menubar_proto_append(lua_State *L) {
	int len = lua_objlen(L, 1);
	for(int i=1; i <= len; i++) {
		lua_pushcfunction(L, menubar_append);	// could instead get it from the metatable
		lua_pushvalue(L, 2);
		lua_rawgeti(L, 1, i);
		int err = lua_pcall(L, 2, 0, 0);
		if(err) {
			printf("error calling append in MenuBar prototype: %s\n", lua::to<const char *>(L, -1));
			lua_pop(L, 1);
			return 0;
		}
	}
	return 0;
}

template<> void Glue<MenuBar>::usr_push(lua_State * L, MenuBar * u) {
	Glue<MenuBar>::usr_attr_push(L);
	Glue<MenuBar>::usr_attr_prototype(L);
	
	if(Glue<luaav::MenuBar>::usr_attr_is_prototype(L)) {
		menubar_proto_append(L);
	}
}

template<> MenuBar * Glue<MenuBar>::usr_new(lua_State * L) {
	MenuBar *m = new MenuBar();
	return m;
}

template<> void Glue<MenuBar>::usr_index(lua_State * L, MenuBar * u) {
	Glue<MenuBar>::usr_attr_index(L);
}

template<> void Glue<MenuBar>::usr_newindex(lua_State * L, MenuBar * u) {
	Glue<MenuBar>::usr_attr_newindex(L);
}

template<> int Glue<MenuBar>::usr_tostring(lua_State * L, MenuBar * u) {
	lua_pushfstring(L, "%s: %p", Glue<MenuBar>::usr_name(), u); 
	return 1;
}

template<> void Glue<MenuBar>::usr_gc(lua_State * L, MenuBar * u) { 
	delete u;
}

template<> void Glue<MenuBar>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "append", menubar_append },
		{ "menu", menubar_menu },
		{ NULL, NULL}
	};
	Glue<MenuBar>::usr_attr_mt(L, methods);
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_menubar(lua_State *L) {
	Glue<MenuBar>::define(L);
	Glue<MenuBar>::register_class(L);
	luaL_getmetatable(L, Glue<MenuBar>::mt_name(L));
	return 1;
}


#ifdef __cplusplus
}
#endif


namespace luaav {

Menu :: Menu() {
	implInit();
}

Menu :: ~Menu() {
	implDestruct();
}

} // luaav::


/*! An application menu
	Represents an application menu and submenus.  Contains functionality for
	adding and removing sumenus and menu items.
	
	@module app.Menu
*/

/*! The menu label (name)
	The menu label (name)
	
	@LuaMethod GETSET
	@name M.label
*/
int menu_label(lua_State *L) {
	Menu *s = Glue<Menu>::checkto(L, 1);
	if(s) {
		if(lua::is<const char *>(L, 2)) {
			s->implSetLabel(lua::to<const char *>(L, 2));
			return 0;
		}
		else {
			lua::push<const char *>(L, s->implGetLabel().data());
			return 1;
		}
	}
	else {
		luaL_error(L, "Menu.label: invalid object");
	}
	return 0;
}

/*! The key accelerator
	The key accelerator
	
	@LuaMethod GETSET
	@name M.key
*/
int menu_key(lua_State *L) {
	Menu *s = Glue<Menu>::checkto(L, 1);
	if(s) {
		if(lua::is<const char *>(L, 2)) {
			s->implSetKey(lua::to<const char *>(L, 2));
			return 0;
		}
		else {
			lua::push<const char *>(L, s->implGetKey().data());
			return 1;
		}
	}
	else {
		luaL_error(L, "Menu.key: invalid object");
	}
	return 0;
}

/*! Inserts a menu or menu item to the menu
	Inserts a menu or menu item to the menu at the specified index.  
	If the index is out of range, it will be clamped.
	
	@param menuitem Menu or menu item to append
	@param index Index of the menu item being inserted
	@LuaMethod METHOD
	@name M:insert
*/
int menu_insert(lua_State *L) {
	Menu *s = Glue<Menu>::checkto(L, 1);
	if(s && lua::is<int>(L, 3)) {
		int idx = lua::to<int>(L, 3);

		MenuItem *mi = NULL;
		Menu *m = NULL;

		if((mi = Glue<MenuItem>::to(L, 2))) {
			int cidx;
			bool exists = s->implItemExists(mi, cidx);
			if(exists)	s->implRemoveItem(mi, cidx);

			idx = (idx > s->implNumItems()) ? (s->implNumItems() - 1) : idx;
			s->implInsertItem(mi, idx);
		}
		else if((m = Glue<Menu>::to(L, 2))) {
			int cidx;
			bool exists = s->implMenuExists(m, cidx);
			if(exists)	s->implRemoveMenu(m, cidx);

			idx = (idx > s->implNumItems()) ? (s->implNumItems() - 1) : idx;
			s->implInsertMenu(m, idx);
		}
		else {
			luaL_error(L, "Menu.insert: invalid object or arguments");
		}
	}
	else {
		luaL_error(L, "Menu.insert: invalid object or arguments");
	}
	return 0;
}

/*! Appends a menu or menu item to the menu
	Appends a menu or menu item to the menu.  Menus will 
	become submenus.
	
	@param menuitem Menu or menu item to append
	@LuaMethod METHOD
	@name M:append
*/
int menu_append(lua_State *L) {
	Menu *s = Glue<Menu>::checkto(L, 1);
	if(s) {
		MenuItem *mi = NULL;
		Menu *m = NULL;
		if((mi = Glue<MenuItem>::to(L, 2))) {
			int idx;
			bool exists = s->implItemExists(mi, idx);
			if(exists)	s->implInsertItem(mi, idx);
			else		s->implAppendItem(mi);
		}
		else if((m = Glue<Menu>::to(L, 2))) {
			int idx;
			bool exists = s->implMenuExists(m, idx);
			if(exists)	s->implInsertMenu(m, idx);
			else		s->implAppendMenu(m);
		}

		// puts a reference to the MenuItem in Self's env table
		lua_getfenv(L, 1);

		// store a reference at an array index
		int len = lua_objlen(L, -1);
		lua_pushvalue(L, 2);
		lua_rawseti(L, -2, len+1);

		// pop env table
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Menu.append: invalid object");
	}
	return 0;
}

/*! Retrieve a menu or menu item by name
	Retieves a menu or menu item by name if it exists.
	
	@param name Menu or menu item name
	@ret menu item found
	@LuaMethod METHOD
	@name M:item
*/
int menu_item(lua_State *L) {
	Menu *s = Glue<Menu>::checkto(L, 1);
	if(s && lua::is<const char *>(L, 2)) {
		const char *name = lua::to<const char *>(L, 2);
		lua_getfenv(L, 1);
		int len = lua_objlen(L, -1);
		for(int i=1; i<= len; i++) {
			// get the MenuItem / Menu object
			lua_rawgeti(L, -1, i);
			MenuItem *mi = NULL;
			Menu *m = NULL;

			if((mi = Glue<MenuItem>::to(L, -1))) {
				//const char *n = mi->implGetLabel().data();
				if(strcmp(mi->implGetLabel().data(), name) == 0) {
					// on success, put it at the location of the env
					// table so that gets popped instead
					lua_insert(L, 3);
					lua_pop(L, 1);
					return 1;
				}
			}
			else if((m = Glue<Menu>::to(L, -1))) {
				//const char *n = m->implGetLabel().data();
				if(strcmp(m->implGetLabel().data(), name) == 0) {
					// on success, put it at the location of the env
					// table so that gets popped instead
					lua_insert(L, 3);
					lua_pop(L, 1);
					return 1;
				}
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Menu.item: invalid object or arguments");
	}
	return 0;
}

/*! Removes a menu or menu item
	Removes a menu or menu item from the menu if 
	it exists
	
	@param menuitem Menu or menu item ro remove
	@LuaMethod METHOD
	@name M:remove
*/
int menu_remove(lua_State *L) {
	Menu *s = Glue<Menu>::checkto(L, 1);
	if(s) {
		MenuItem *mi = NULL;
		Menu *m = NULL;
		if((mi = Glue<MenuItem>::to(L, 2))) {
		//	const char *n = mi->implGetLabel().data();
			int idx;
			bool exists = s->implItemExists(mi, idx);
			if(exists)	s->implRemoveItem(mi, idx);
		}
		else if((m = Glue<Menu>::to(L, 2))) {
			//const char *n = m->implGetLabel().data();
			int idx;
			bool exists = s->implMenuExists(m, idx);
			if(exists)	s->implRemoveMenu(m, idx);
		}

		// puts a reference to the MenuItem in Self's env table
		lua_getfenv(L, 1);

		// store a reference at an array index
		int len = lua_objlen(L, -1);
		lua_pushvalue(L, 2);
		lua_rawseti(L, -2, len+1);

		// pop env table
		lua_pop(L, 1);
	}
	else {
		luaL_error(L, "Menu.remove: invalid object");
	}
	return 0;
}



template<> const char * Glue<Menu>::usr_name() { return "Menu"; }
template<> bool Glue<Menu>::usr_has_index() { return true; }


int menu_proto_append(lua_State *L) {
	int len = lua_objlen(L, 1);
	for(int i=1; i <= len; i++) {
		lua_pushcfunction(L, menu_append);
		lua_pushvalue(L, 2);
		lua_rawgeti(L, 1, i);
		int err = lua_pcall(L, 2, 0, 0);
		if(err) {
			printf("error calling append in Menu prototype: %s\n", lua::to<const char *>(L, -1));
			lua_pop(L, 1);
			return 0;
		}
	}
	return 0;
}

template<> void Glue<Menu>::usr_push(lua_State * L, Menu * u) {
	Glue<Menu>::usr_attr_push(L);
	Glue<Menu>::usr_attr_prototype(L);
	
	if(Glue<Menu>::usr_attr_is_prototype(L)) {
		menu_proto_append(L);
	}
}

template<> Menu * Glue<Menu>::usr_new(lua_State * L) {
	Menu *m = new Menu();
	return m;
}

template<> void Glue<Menu>::usr_index(lua_State * L, Menu * u) {
	Glue<Menu>::usr_attr_index(L);
}

template<> void Glue<Menu>::usr_newindex(lua_State * L, Menu * u) {
	Glue<Menu>::usr_attr_newindex(L);
}

template<> int Glue<Menu>::usr_tostring(lua_State * L, Menu * u) {
	lua_pushfstring(L, "%s: %p", Glue<Menu>::usr_name(), u); 
	return 1;
}

template<> void Glue<Menu>::usr_gc(lua_State * L, Menu * u) { 
	delete u;
}

template<> void Glue<Menu>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "append", menu_append },
		{ "insert", menu_insert },
		{ "item", menu_item },
		{ "remove", menu_remove },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "label", menu_label },
		{ "key", menu_key },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "label", menu_label },
		{ "key", menu_key },
		{ NULL, NULL}
	};
	Glue<Menu>::usr_attr_mt(L, methods, getters, setters);
}


#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_menu_impl(lua_State *L);

int luaopen_menu(lua_State *L) {
	Glue<Menu>::define(L);
	Glue<Menu>::register_class(L);
	luaL_getmetatable(L, Glue<Menu>::mt_name(L));
	luaopen_menu_impl(L);
	return 1;
}


#ifdef __cplusplus
}
#endif




namespace luaav {

MenuItem :: MenuItem(lua_State *state, bool separator)
:	mImpl(NULL)
{
	mState = luaav_getrootstate(state);
	implInit(separator);
	implRegister();
}

MenuItem :: ~MenuItem() {
	implUnregister();
	implDestruct();
}

// this could be moved to a platform-specific header for including in LuaAVResponder
MenuItem * MenuItem :: itemForPtr(void *ptr) {
	return implGetMenuItem(ptr);
}

void MenuItem :: do_dispatch(lua_State *L) {
	if(lua_isthread(L, -1)) {
		lua_State *C = lua_tothread(L, -1);
		if(lua_isfunction(C, 1)) {

			// duplicate stack so it's around for the next call
			int top = lua_gettop(C);
			for(int i=1; i <= top; i++) {
				lua_pushvalue(C, i);
			}

			// run the dispatcher in the coroutine
			int err = lua_pcall(C, top-1, 0, 0);
			if(err) {
				luaav_error_msg("error dispatching menu action: %s\n", lua_tostring(C, -1));
				lua_pop(C, 1);
			}
		}
	}
}

void MenuItem :: dispatch() {
	lua_State *L = mState;
	if(L) {
		// find the coroutine to finish the dispatch
		if(Glue<MenuItem>::usr_get_reference_map(L, this)) {
		
			lua_getfenv(L, -1);
			lua_getfield(L, -1, MENU_ACTION_FIELD);

			// call thread
			do_dispatch(L);
			
			lua_pop(L, 1);
		}
	}
}

} // luaav::


/*! An application menu item
	Represents an application menu item.  Contains functionality for
	setting the label and shortcut/accelerator keys as well as the 
	actiion to be performed when the menu item is selected.
	
	@module app.MenuItem
*/

/*! The menu item label (name)
	The menu item label (name)
	
	@LuaMethod GETSET
	@name M.label
*/
int menuitem_label(lua_State *L) {
	MenuItem *s = Glue<MenuItem>::checkto(L, 1);
	if(s) {
		if(lua::is<const char *>(L, 2)) {
			s->implSetLabel(lua::to<const char *>(L, 2));
			return 0;
		}
		else {
			lua::push<const char *>(L, s->implGetLabel().data());
			return 1;
		}
	}
	else {
		luaL_error(L, "MenuItem.label: invalid object");
	}
	return 0;
}


/*! Separator flag
	Returns a flag of whether or not 
	the MenuItem is a separator
	
	@LuaMethod GET
	@name M.separator
*/
int menuitem_separator(lua_State *L) {
	MenuItem *s = Glue<MenuItem>::checkto(L, 1);
	if(s) {
		lua::push(L, s->implIsSeparator());
	}
	else {
		luaL_error(L, "MenuItem.separator: invalid object");
	}
	return 1;
}

/*! The menu item key (for shortcuts)
	The menu item key (for shortcuts)
	
	@LuaMethod GETSET
	@name M.key
*/
int menuitem_key(lua_State *L) {
	MenuItem *s = Glue<MenuItem>::checkto(L, 1);
	if(s) {
		if(lua::is<const char *>(L, 2)) {
			s->implSetKey(lua::to<const char *>(L, 2));
			return 0;
		}
		else {
			lua::push<const char *>(L, s->implGetKey().data());
			return 1;
		}
	}
	else {
		luaL_error(L, "MenuItem.key: invalid object");
	}
	return 0;
}


/*! The menu item key mask (for shortcuts)
	The menu item key mask (for shortcuts).  Possible values are 
	Menu.Shift, Menu.Alt, Menu.Cmd, and Menu.Ctrl.
	
	@LuaMethod GETSET
	@name M.keymask
*/
int menuitem_keymask(lua_State *L) {
	MenuItem *s = Glue<MenuItem>::checkto(L, 1);
	if(s) {
		if(lua::is<unsigned int>(L, 2)) {
			s->implSetKeyMask(lua::to<unsigned int>(L, 2));
			return 0;
		}
		else {
			lua::push(L, s->implGetKeyMask());
			return 1;
		}
	}
	else {
		luaL_error(L, "MenuItem.keymask: invalid object");
	}
	return 0;
}


/*! Dispatch the menu item action.
	Dispatch the menu item action as if it were selected.
	
	@LuaMethod METHOD
	@name M:dispatch
*/
int menuitem_dispatch(lua_State *L) {
	MenuItem *s = Glue<MenuItem>::checkto(L, 1);
	if(s) {
		s->dispatch();
	}
	else {
		luaL_error(L, "MenuItem.dispatch: invalid object");
	}
	return 0;
}


/*! The menu item action function
	The menu item action function.  Called when the menu item is 
	selected.  
	
	@LuaMethod GETSET
	@name M.action
*/
int menuitem_action(lua_State *L) {
	MenuItem *s = Glue<MenuItem>::checkto(L, 1);
	if(s) {
		if(lua_isfunction(L, 2)) {
			int nargs = lua_gettop(L) - 1;

			// push a coroutine
			lua_State * C = lua_newthread(L);
			lua_insert(L, 2);

			lua_xmove(L, C, nargs);
			
			lua_getfenv(L, 1);
			lua_insert(L, 2);
			lua_setfield(L, 2, MENU_ACTION_FIELD);
			lua_pop(L, 1);

			s->implEnableAction();
		}
		else {
			/*
			// nil menu action
			lua_getfenv(L, 1);
			lua_pushnil(L);
			lua_setfield(L, -2, MENU_ACTION_FIELD);
			lua_pop(L, 1);

			s->implDisableAction();

			// allow collection of menu item udata
			lua_pushvalue(L, 1);
			lua_getmetatable(L, 1);
			dec_ref(L);
			lua_pop(L, 2);
			*/
			lua_getfenv(L, 1);
			lua_getfield(L, -1, MENU_ACTION_FIELD);
			lua_insert(L, -2);
			lua_pop(L, 1);
			return 1;
		}
	}
	else {
		luaL_error(L, "MenuItem.action: invalid object");
	}
	return 0;
}


template<> const char * Glue<MenuItem>::usr_name() { return "MenuItem"; }
template<> bool Glue<MenuItem>::usr_has_index() { return true; }


template<> void Glue<MenuItem>::usr_push(lua_State * L, MenuItem * u) {
	Glue<MenuItem>::usr_attr_push(L);
	Glue<MenuItem>::usr_attr_prototype(L);
	Glue<MenuItem>::usr_set_reference_map(L, u);
}

template<> MenuItem * Glue<MenuItem>::usr_new(lua_State * L) {
	bool separator = false;
	if(Glue<MenuItem>::usr_attr_is_prototype(L)) {
		separator = lua::optfield(L, 1, false, "separator");
		lua_pushnil(L);
		lua_setfield(L, 1, "separator");
	}
	
	MenuItem *m = new MenuItem(luaav_getrootstate(L), separator);
	return m;
}

template<> void Glue<MenuItem>::usr_index(lua_State * L, MenuItem * u) {
	Glue<MenuItem>::usr_attr_index(L);
}

template<> void Glue<MenuItem>::usr_newindex(lua_State * L, MenuItem * u) {
	Glue<MenuItem>::usr_attr_newindex(L);
}

template<> int Glue<MenuItem>::usr_tostring(lua_State * L, MenuItem * u) {
	lua_pushfstring(L, "%s: %p", Glue<MenuItem>::usr_name(), u); 
	return 1;
}

template<> void Glue<MenuItem>::usr_gc(lua_State * L, MenuItem * u) { 
	delete u;
}

template<> void Glue<MenuItem>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "dispatch", menuitem_dispatch },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "label", menuitem_label },
		{ "key", menuitem_key },
		{ "keymask", menuitem_keymask },
		{ "action", menuitem_action },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "label", menuitem_label },
		{ "key", menuitem_key },
		{ "keymask", menuitem_keymask },
		{ "action", menuitem_action },
		{ NULL, NULL}
	};
	Glue<MenuItem>::usr_attr_mt(L, methods, getters, setters);
}


#ifdef __cplusplus
extern "C" {
#endif

int luaopen_menuitem(lua_State *L) {
	Glue<MenuItem>::define(L);
	Glue<MenuItem>::register_class(L);
	luaL_getmetatable(L, Glue<MenuItem>::mt_name(L));
	return 1;
}


#ifdef __cplusplus
}
#endif


#pragma mark C API

int luaav_app_menus_init() {

	luaav_add_opt_module("app.Menu", luaopen_menu);
	luaav_add_opt_module("app.MenuBar", luaopen_menubar);
	luaav_add_opt_module("app.MenuItem", luaopen_menuitem);
	return 0;
}


void luaav_app_menuitem_action(const char *path) {
	luaav_state * S = luaav_app_state();
	if (S) {
		lua_State * L = luaav_getstate(S);
		int top = lua_gettop(L);
		
		lua::call(L, "menuitem_action", path);
		
		if(lua_gettop(L) > 0 && lua_isthread(L, -1)) {
			luaav::MenuItem::do_dispatch(L);
		}
		lua_pop(L, top-lua_gettop(L));
	}
}

