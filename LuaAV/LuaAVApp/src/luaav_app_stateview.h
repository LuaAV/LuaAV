#ifndef LUAAV_APP_STATEVIEW_H
#define LUAAV_APP_STATEVIEW_H

#include "luaav.h"
#include "luaav_app.h"
#include "luaav_udata.hpp"
#include "luaav_utility.hpp"
#include "lua_utility.h"

#include <map>
#include <string>

namespace luaav {

class StateViewImpl;

class StateView : public Udata<StateView> {
protected:
	typedef StateView			Self;
	typedef Udata<StateView>	CodePadUdata;
	typedef StateViewImpl		Implementation;
	
public:
	StateView(int x=0, int y=0, int w=350, int h=450);
	~StateView();

	void children(const char *parent_name);

	static int __new(lua_State *L);
	static int add_item(lua_State *L);

	static const LuaMethod * getLuaMethods() {return lua_methods;}
	static const char ** getSuperclassTable() {return superclass_table;}
	static const char *name;
	
protected:
	void implInit();
	void implDestruct();
	void implCreate(int x, int y, int w, int h);
	void implDestroy();

	void implAddItem(const char *name, const char *type, const char *v);

public:
	Implementation *mImpl;
	

protected:
	static const LuaMethod lua_methods[];
	static const char *superclass_table[];
};

}	// luaav::

#endif	// LUAAV_APP_STATEVIEW_H
