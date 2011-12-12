#ifndef LUAAV_APP_CODEPAD_H
#define LUAAV_APP_CODEPAD_H

#include "luaav.h"
#include "luaav_app.h"
#include "luaav_udata.hpp"
#include "luaav_utility.hpp"
#include "lua_utility.h"

#include <map>
#include <string>

namespace luaav {

class CodePadImpl;

class CodePad : public Udata<CodePad> {
protected:
	typedef CodePad			Self;
	typedef Udata<CodePad>	CodePadUdata;
	typedef CodePadImpl		Implementation;
	
public:
	CodePad(const char *name, int x=0, int y=0, int w=350, int h=450);
	~CodePad();
	
	// callbacks
	void eval(const char *code);
	void highlight(int line, const char *text);
	void insert(int line, int n);
	void remove(int line, int n);

	static int current_name(lua_State *L);

	static int __new(lua_State *L);
	static int name_method(lua_State *L);
	static int text(lua_State *L);
	static int create_format(lua_State *L);
	static int format(lua_State *L);
	static int selection(lua_State *L);
	static int highlight_error(lua_State *L);
	static int paste(lua_State *L);
	static int cursor_pos(lua_State *L);

	static const LuaMethod * getLuaMethods() {return lua_methods;}
	static const char ** getSuperclassTable() {return superclass_table;}
	static const char *name;
	
protected:
	void implInit();
	void implDestruct();
	void implCreate(const char *name, int x, int y, int w, int h);
	void implDestroy();
	static const char * implGetCurrentName();
	void implSetName(const char *name);
	const char * implGetName();
	void implSetText(const char *text);
	std::string implGetText();
	void implCreateFormat(const char *name, float *color, std::map<std::string, int> &properties);
	void implFormat(const char *name, int start, int end);
	void implGetSelection(int &line1, int &pos1, int &line2, int &pos2);
	void implErrorHighlight(int line);
	void implClearErrorHighlight();
	void implPaste(int pos, const char *text);
	int implCursorPos();

public:
	Implementation *mImpl;
	

protected:
	static const LuaMethod lua_methods[];
	static const char *superclass_table[];
};

}	// luaav::

#endif	// LUAAV_APP_CODEPAD_H
