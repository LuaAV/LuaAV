#import "lua_utility.h"
#import "luaav_app_menu.h"
//#import "LuaAVPlatform.h"
#import <Cocoa/Cocoa.h>
#import <string>

#if __LP64__ || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
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
	NSMenu *mMbar;
};

// Menu Cocoa Imlementation
class MenuImpl {
protected:
	typedef Menu	Generic;

public:
	struct KeyMasks {
	public:
		const char *name;
		NSUInteger mask;
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
	NSMenu *mMenu;
	NSMenuItem *mItem;

	static KeyMasks cKeymasks[];
};

// MenuItem Cocoa Imlementation
class MenuItemImpl {
protected:
	typedef MenuItem	Generic;

public:
	MenuItemImpl(MenuItem *item)
	:	mGeneric(item),
		mItem(0)
	{}

    Generic * generic() {
		return mGeneric;
	}
	
    Generic *mGeneric;
	NSMenuItem *mItem;
	
	static map<NSMenuItem *, MenuItem *> cUdataMap;
};


#define mCocoaMenuBar	(mImpl->mMbar)
#define mCocoaMenu		(mImpl->mMenu)
#define mCocoaMenuItem	(mImpl->mItem)

#define cocoaMenu		mImpl->mMenu
#define cocoaMenuItem	mImpl->mItem

void MenuBar :: implInit() {
	mImpl = new Implementation(this);
	mCocoaMenuBar = [NSApp mainMenu];
}

void MenuBar :: implDestruct() {
	delete mImpl;
}

bool MenuBar :: implMenuExists(Menu *m, int &idx) {
	NSMenuItem *existing_item = [mCocoaMenuBar itemWithTitle: [m->cocoaMenuItem title]];
	if(existing_item) {
		idx = (int)[mCocoaMenuBar indexOfItem: existing_item];
	}
	return existing_item != nil;
}

void MenuBar :: implInsert(Menu *m, int idx) {
	[mCocoaMenuBar removeItemAtIndex:idx];
	[mCocoaMenuBar insertItem:m->cocoaMenuItem atIndex:idx];
	[mCocoaMenuBar setSubmenu:m->cocoaMenu forItem:m->cocoaMenuItem];
}

void MenuBar :: implAppend(Menu *m) {
	[mCocoaMenuBar addItem: m->cocoaMenuItem];
	[mCocoaMenuBar setSubmenu:m->cocoaMenu forItem:m->cocoaMenuItem];
}

int MenuBar :: implNumItems() {
	return (int)[mCocoaMenuBar numberOfItems];
}


MenuImpl::KeyMasks MenuImpl::cKeymasks[] = {
	{"Shift", NSShiftKeyMask},
	{"Alt", NSAlternateKeyMask},
	{"Cmd", NSCommandKeyMask},
	{"Ctrl", NSControlKeyMask},
	{0, 0},
};

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_menu_impl(lua_State *L) {
	for(int i=0; MenuImpl::cKeymasks[i].name; i++) {
		lua::push<const char *>(L, MenuImpl::cKeymasks[i].name);
		lua_pushinteger(L, MenuImpl::cKeymasks[i].mask);
		lua_rawset(L, -3);
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

void Menu :: implInit() {
	mImpl = new Implementation(this);
	mCocoaMenu = [[NSMenu alloc] initWithTitle:@""];
	mCocoaMenuItem = [[NSMenuItem alloc] initWithTitle:@"" 
									action:(SEL)nil
									keyEquivalent:@""];
}

void Menu :: implDestruct() {
	[mCocoaMenu release];
	[mCocoaMenuItem release];
	delete mImpl;
}

void Menu :: implSetLabel(const char *v) {
	NSString *nsv = [NSString stringWithCString:v encoding:NSASCIIStringEncoding];

	[mCocoaMenuItem setTitle:nsv];
	[mCocoaMenu setTitle:nsv];
}

string Menu :: implGetLabel() {
	NSString *v = [mCocoaMenuItem title];
	return string([v cStringUsingEncoding: NSASCIIStringEncoding]);
}

void Menu :: implSetKey(const char *v) {
	NSString *nsv = [NSString stringWithCString:v encoding:NSASCIIStringEncoding];
	[mCocoaMenuItem setTitle:nsv];
}

string Menu :: implGetKey() {
	NSString *v = [mCocoaMenuItem title];
	return string([v cStringUsingEncoding: NSASCIIStringEncoding]);
}

bool Menu :: implItemExists(MenuItem *mi, int &idx) {
	NSMenuItem *existing_item = [mCocoaMenu itemWithTitle: [mi->cocoaMenuItem title]];
	if(existing_item) {
		idx = (int)[mCocoaMenu indexOfItem: existing_item];
	}
	return existing_item != nil;
}

void Menu :: implInsertItem(MenuItem *mi, int idx) {
	NSMenuItem *existing_item = [mCocoaMenu itemWithTitle: [mi->cocoaMenuItem title]];
	if(existing_item) {
		[mCocoaMenu removeItemAtIndex:idx];
	}
	[mCocoaMenu insertItem:mi->cocoaMenuItem atIndex:idx];
}

void Menu :: implAppendItem(MenuItem *mi) {
	[mCocoaMenu addItem:mi->cocoaMenuItem];
}

void Menu :: implRemoveItem(MenuItem *mi, int idx) {
	[mCocoaMenu removeItem:mi->cocoaMenuItem];
}

bool Menu :: implMenuExists(Menu *m, int &idx) {
	NSMenuItem *existing_item = [mCocoaMenu itemWithTitle: [m->cocoaMenuItem title]];
	if(existing_item) {
		idx = (int)[mCocoaMenu indexOfItem: existing_item];
	}
	return existing_item != nil;
}

void Menu :: implInsertMenu(Menu *m, int idx) {
	NSMenuItem *existing_item = [mCocoaMenu itemWithTitle: [m->cocoaMenuItem title]];
	if(existing_item) {
		[mCocoaMenu removeItemAtIndex:idx];
	}
	[mCocoaMenu insertItem:m->cocoaMenuItem atIndex:idx];
	[mCocoaMenu setSubmenu:m->cocoaMenu forItem:m->cocoaMenuItem];
}

void Menu :: implAppendMenu(Menu *m) {
	[mCocoaMenu addItem: m->cocoaMenuItem];
	[mCocoaMenu setSubmenu:m->cocoaMenu forItem:m->cocoaMenuItem];
}

void Menu :: implRemoveMenu(Menu *m, int idx) {
	[mCocoaMenu removeItem:m->cocoaMenuItem];
}


int Menu :: implNumItems() {
	return (int)[mCocoaMenu numberOfItems];
}


map<NSMenuItem *, MenuItem *> MenuItemImpl::cUdataMap;

void MenuItem :: implInit(bool separator) {
	mImpl = new Implementation(this);
	if(separator) {
		mCocoaMenuItem = [NSMenuItem separatorItem];
		[mCocoaMenuItem retain];
	}
	else {
		mCocoaMenuItem = [[NSMenuItem alloc] initWithTitle:@"" 
										action:(SEL)nil
										keyEquivalent:@""];
		[mCocoaMenuItem setKeyEquivalentModifierMask:0];
	}
}

void MenuItem :: implDestruct() {
	[mCocoaMenuItem release];
	delete mImpl;
}

void MenuItem :: implRegister() {
	Implementation::cUdataMap[mCocoaMenuItem] = this;
}

void MenuItem :: implUnregister() {
	map<NSMenuItem *, MenuItem *>::iterator it = Implementation::cUdataMap.find(mCocoaMenuItem);
	if(it != Implementation::cUdataMap.end()) {
		Implementation::cUdataMap.erase(it);
	}
}

MenuItem * MenuItem :: implGetMenuItem(void *ptr) {
	MenuItem *mi = NULL;
	map<NSMenuItem *, MenuItem *>::iterator it = Implementation::cUdataMap.find((NSMenuItem *)ptr);
	if(it != Implementation::cUdataMap.end()) {
		mi = it->second;
	}
	return mi;
}

void MenuItem :: implSetLabel(const char *v) {
	NSString *nsv = [NSString stringWithCString: v
						encoding:NSASCIIStringEncoding];
			
	[mCocoaMenuItem setTitle:nsv];
}

string MenuItem :: implGetLabel() {
	NSString *v = [mCocoaMenuItem title];
	return string([v cStringUsingEncoding: NSASCIIStringEncoding]);
}

void MenuItem :: implSetKey(const char *v) {
	if(strcmp(v, "enter") == 0) {
		[mCocoaMenuItem setKeyEquivalent:[NSString stringWithFormat:@"%c", NSCarriageReturnCharacter]];
	}
	else {
		NSString *nsv = [NSString stringWithCString: v
						encoding:NSASCIIStringEncoding];
		[mCocoaMenuItem setKeyEquivalent:nsv];
	}
}

string MenuItem :: implGetKey() {
	NSString *v = [mCocoaMenuItem keyEquivalent];
	return string([v cStringUsingEncoding: NSASCIIStringEncoding]);
}

void MenuItem :: implSetKeyMask(unsigned int v) {
	[mCocoaMenuItem setKeyEquivalentModifierMask:(NSUInteger)v];
}

unsigned int MenuItem :: implGetKeyMask() {
	return (unsigned int)[mCocoaMenuItem keyEquivalentModifierMask];
}

bool MenuItem :: implIsSeparator() {
	return [mCocoaMenuItem isSeparatorItem] ? true : false;
}

void MenuItem :: implEnableAction() {
	[mCocoaMenuItem setAction: @selector(dispatch:)];
}

void MenuItem :: implDisableAction() {
	[mCocoaMenuItem setAction: nil];
}

}	// luaav::
