#ifndef _LUAAV_APP_MENU_H_
#define _LUAAV_APP_MENU_H_ 1

#include "luaav.h"
#include <string>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

AL_API int luaav_app_menus_init();
AL_API void luaav_app_menuitem_action(const char *path);

#ifdef __cplusplus
}
#endif

/*

MenuBar{
    File = Menu{
        Open = MenuItem{
            key = "O",
            connections = {"activate", "file.open"},
        },

        Quit = MenuItem{
            key = "Q",
            connections = {"activate", "file.quit"},
        },

		key = F
    },
}

create_menu
create_menuitem
menubar_append
menu_append
key_binding
connection

TODO: Add separator

*/

using std::string;
using std::map;

namespace luaav {

#define MENU_ACTION_FIELD	"__menu_action_field"

class Menu;
class MenuItem;

class MenuBarImpl;
class MenuImpl;
class MenuItemImpl;


class LUAAV_API MenuBar {
private:
	friend class MenuBarImpl;

protected:
	typedef MenuBar			Self;
	typedef MenuBarImpl		Implementation;

public:

	MenuBar();
	~MenuBar();
	
	// implementation-specific functions
	void implInit();
	void implDestruct();

	// attribute handling
	bool implMenuExists(Menu *m, int &idx);
	void implInsert(Menu *m, int idx);
	void implAppend(Menu *m);

	int implNumItems();
	
	Implementation *mImpl;
};


class LUAAV_API Menu {
private:
	friend class MenuImpl;

protected:
	typedef Menu		Self;
	typedef MenuImpl	Implementation;

public:

	Menu();
	~Menu();

	// implementation-specific functions
	//static int impl_register_udata(lua_State *L);
	void implInit();
	void implDestruct();

	// attribute handling
	void implSetLabel(const char *v);
	string implGetLabel();
	void implSetKey(const char *v);
	string implGetKey();

	// MenuItem/Submenu handling
	bool implItemExists(MenuItem *mi, int &idx);
	void implInsertItem(MenuItem *mi, int idx);
	void implAppendItem(MenuItem *mi);
	void implRemoveItem(MenuItem *mi, int idx);

	bool implMenuExists(Menu *m, int &idx);
	void implInsertMenu(Menu *m, int idx);
	void implAppendMenu(Menu *m);
	void implRemoveMenu(Menu *m, int idx);

	int implNumItems();

	// pointer to the binding-specific implementation
	Implementation *mImpl;

};


class LUAAV_API MenuItem {
private:
	friend class MenuItemImpl;

protected:
	typedef MenuItem		Self;
	typedef MenuItemImpl	Implementation;

public:

	MenuItem(lua_State *state, bool separator = false);
	~MenuItem();
	
	void dispatch();
	static void do_dispatch(lua_State *L);

	// implementation-specific functions
	void implInit(bool separator = false);
	void implDestruct();
	void implRegister();
	void implUnregister();
	static MenuItem * implGetMenuItem(void *ptr);

	// attribute handling
	void implSetLabel(const char *v);
	string implGetLabel();
	void implSetKey(const char *v);
	string implGetKey();
	void implSetKeyMask(unsigned int v);
	unsigned int implGetKeyMask();
	bool implIsSeparator();

	// callbaack handling
	void implEnableAction();
	void implDisableAction();


	static MenuItem * itemForPtr(void *ptr);

	// pointer to the binding-specific implementation
	Implementation *mImpl;
	lua_State *mState;
};

}	// luaav:

#endif	// _LUAAV_MENU_H_
