#ifndef INCLUDE_LUAAV_APP_MENU_LINUX_H
#define INCLUDE_LUAAV_APP_MENU_LINUX_H

#include "luaav_app_menu.h"

#ifdef LUAAV_APP_USE_QT
	#include "luaav_console_qt.h"
	#include <QtGui>
	#include <string>
#endif


using std::string;

namespace luaav {

// MenuBar Cocoa Imlementation
class MenuBarImpl {
protected:
	typedef MenuBar	Generic;

public:
	MenuBarImpl(MenuBar *mbar)
	:	mGeneric(mbar),
		mMbar(0)
	{}

    Generic * generic() {
		return mGeneric;
	}
	
    Generic *mGeneric;
	QMenuBar *mMbar;
};

// Menu Cocoa Imlementation
class MenuImpl {
protected:
	typedef Menu	Generic;

public:
	struct KeyMasks {
	public:
		const char *name;
		int mask;
	};

public:
	MenuImpl(Menu *menu)
	:	mGeneric(menu),
		mMenu(0),
		mItem(0)
	{}

    Generic * generic() {
		return mGeneric;
	}
	
    Generic *mGeneric;
	QMenu *mMenu;
	QAction *mItem;

	static KeyMasks cKeymasks[];
};

// MenuItem Cocoa Imlementation
class MenuItemImpl : public QObject {
	Q_OBJECT

protected:
	typedef MenuItem	Generic;

public:
	MenuItemImpl(MenuItem *item)
	:	mGeneric(item),
		mItem(0),
		keymask(Qt::ControlModifier)
	{}

    Generic * generic() {
		return mGeneric;
	}
	
    Generic *mGeneric;
	QAction *mItem;

	string label;
	string key;
	int keymask;

	void makeShortcut();

private slots:
	void dispatch();
};

} // luaav::

#endif	// INCLUDE_LUAAV_APP_MENU_LINUX_H
