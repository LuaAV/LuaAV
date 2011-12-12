#include "luaav_app_menu_qt.h"
#include "lua_utility.h"

using std::string;

namespace luaav {

#define mQtMenuBar	(mImpl->mMbar)
#define mQtMenu		(mImpl->mMenu)
#define mQtItem		(mImpl->mItem)

#define qtMenu		mImpl->mMenu
#define qtMenuItem	mImpl->mItem

void MenuBar :: implInit() {
	mImpl = new Implementation(this);
	mQtMenuBar = LuaAVConsole::console()->menuBar();
}

void MenuBar :: implDestruct() {
	delete mImpl;
}

bool MenuBar :: implMenuExists(Menu *m, int &idx) {
	QList<QAction *> menus = mQtMenuBar->actions();
	QString title = m->qtMenu->menuAction()->text();

	int i = 0;	
	QAction *existing_item = NULL;
	for(QList<QAction *>::iterator it = menus.begin(); it != menus.end(); ++it) {
		if((*it)->text().compare(title) == 0) {
			existing_item = *it;			
			idx = i;
		}
		i++;
	}

	return existing_item != NULL;
}

void MenuBar :: implInsert(Menu *m, int idx) {
	QList<QAction *> items = mQtMenuBar->actions();
	int i = 0;
	QList<QAction *>::iterator it = items.begin();
	for(; it != items.end(); ++it) {
		if(i == idx) {
			break;
		}
		i++;
	}
	if(it == items.end()) {
		mQtMenuBar->addAction(m->qtMenuItem);
	}
	else {
		mQtMenuBar->insertAction(*it, m->qtMenuItem);
	}
}

void MenuBar :: implAppend(Menu *m) {
	mQtMenuBar->addAction(m->qtMenuItem);
}

int MenuBar :: implNumItems() {
	return mQtMenuBar->actions().size();
}


MenuImpl::KeyMasks MenuImpl::cKeymasks[] = {
	{"Shift", Qt::ShiftModifier},
	{"Alt", Qt::AltModifier},
	{"Cmd", Qt::MetaModifier},
	{"Ctrl", Qt::ControlModifier},
	{0, 0},
};

const char * stringForKeymask(int mask) {
	switch(mask) {
		case Qt::ShiftModifier:		return "Shift";
		case Qt::AltModifier:		return "Alt";
		case Qt::MetaModifier:		return "Ctrl";
		case Qt::ControlModifier:	return "Ctrl";
		default: return "";
	}
}

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
	
	mQtMenu = new QMenu();
	mQtItem = mQtMenu->menuAction();
}

void Menu :: implDestruct() {
	delete mImpl;
}

void Menu :: implSetLabel(const char *v) {
	mQtMenu->setTitle(QString(v));
}

string Menu :: implGetLabel() {
	QByteArray bytes = mQtMenu->title().toAscii();
	return string(bytes.data());
}

void Menu :: implSetKey(const char *v) {
//	NSString *nsv = [NSString stringWithCString:v encoding:NSASCIIStringEncoding];
//	[mCocoaMenuItem setTitle:nsv];
}

string Menu :: implGetKey() {
	return string();

//	NSString *v = [mCocoaMenuItem title];
//	return string([v cStringUsingEncoding: NSASCIIStringEncoding]);
}

bool Menu :: implItemExists(MenuItem *mi, int &idx) {
	if(mi->qtMenuItem) {
		QList<QAction *> items = mQtMenu->actions();
		QString title = mi->qtMenuItem->text();

		int i = 0;	
		QAction *existing_item = NULL;
		for(QList<QAction *>::iterator it = items.begin(); it != items.end(); ++it) {
			if((*it)->text().compare(title) == 0) {
				existing_item = *it;			
				idx = i;
			}
			i++;
		}

		return existing_item != NULL;
	}
	else {
		return false;
	}
}

void Menu :: implInsertItem(MenuItem *mi, int idx) {
	QList<QAction *> items = mQtMenu->actions();
	int i = 0;
	QList<QAction *>::iterator it = items.begin();
	for(; it != items.end(); ++it) {
		if(i == idx) {
			break;
		}
		i++;
	}
	if(it == items.end()) {
		if(mi->qtMenuItem) {
			mQtMenu->addAction(mi->qtMenuItem);
		}
		else {
			mi->qtMenuItem = mQtMenu->addAction(QString(mi->mImpl->label.data()));
			mi->qtMenuItem->connect(mi->qtMenuItem, SIGNAL(triggered()), mi->mImpl, SLOT(dispatch()));
			mi->mImpl->makeShortcut();
		}
	}
	else {
		if(mi->qtMenuItem) {
			mQtMenu->insertAction(*it, mi->qtMenuItem);
		}
		else {
			mi->qtMenuItem = new QAction(mQtMenu);
			mi->qtMenuItem->connect(mi->qtMenuItem, SIGNAL(triggered()), mi->mImpl, SLOT(dispatch()));
			mi->qtMenuItem->setText(QObject::tr(mi->mImpl->label.data()));
			mQtMenu->insertAction(*it, mi->qtMenuItem);
			mi->mImpl->makeShortcut();
		}
	}
}

void Menu :: implAppendItem(MenuItem *mi) {
	if(mi->qtMenuItem) {
		mQtMenu->addAction(mi->qtMenuItem);
	}
	else {
		mi->qtMenuItem = mQtMenu->addAction(QString(mi->mImpl->label.data()));
		mi->qtMenuItem->connect(mi->qtMenuItem, SIGNAL(triggered()), mi->mImpl, SLOT(dispatch()));
		mi->mImpl->makeShortcut();
	}
}

void Menu :: implRemoveItem(MenuItem *mi, int idx) {
	mQtMenu->removeAction(mi->qtMenuItem);
}

bool Menu :: implMenuExists(Menu *m, int &idx) {
	QList<QAction *> items = mQtMenu->actions();
	QString title = m->qtMenuItem->text();

	int i = 0;	
	QAction *existing_item = NULL;
	for(QList<QAction *>::iterator it = items.begin(); it != items.end(); ++it) {
		if((*it)->text().compare(title) == 0) {
			existing_item = *it;			
			idx = i;
		}
		i++;
	}

	return existing_item != NULL;
}

void Menu :: implInsertMenu(Menu *m, int idx) {
	QList<QAction *> items = mQtMenu->actions();
	int i = 0;
	QList<QAction *>::iterator it = items.begin();
	for(; it != items.end(); ++it) {
		if(i == idx) {
			break;
		}
		i++;
	}
	if(it == items.end()) {
		mQtMenu->addAction(m->qtMenuItem);
	}
	else {
		mQtMenu->insertAction(*it, m->qtMenuItem);
	}
}

void Menu :: implAppendMenu(Menu *m) {
	mQtMenu->addAction(m->qtMenuItem);
}

void Menu :: implRemoveMenu(Menu *m, int idx) {
	mQtMenu->removeAction(m->qtMenuItem);
}


int Menu :: implNumItems() {
	return mQtMenu->actions().size();
}


void MenuItem :: implInit(bool separator) {
	mImpl = new Implementation(this);
	if(separator) {
		qtMenuItem = new QAction(NULL);
		qtMenuItem->setSeparator(true);
	}
}

void MenuItem :: implDestruct() {
	delete mImpl;
}

void MenuItem :: implRegister() {
	// not needed with Qt
}

void MenuItem :: implUnregister() {
	// not needed with Qt
}

MenuItem * MenuItem :: implGetMenuItem(void *ptr) {
	fprintf(stderr, "MenuItem::implGetMenuItem(void *ptr): not implemented with Qt.  Do not call this function!\n");
	return NULL;
}

void MenuItem :: implSetLabel(const char *v) {
	if(mQtItem) {
		mQtItem->setText(QString(v));
	}
	else {
		mImpl->label.assign(v);
	}
}

string MenuItem :: implGetLabel() {
	if(mQtItem) {
		QByteArray bytes = mQtItem->text().toAscii();
		return string(bytes.data());
	}
	else {
		return mImpl->label;	
	}
}

void MenuItem :: implSetKey(const char *v) {
	mImpl->key.assign(v);
}

string MenuItem :: implGetKey() {
	return mImpl->key;
}

void MenuItem :: implSetKeyMask(unsigned int v) {
	mImpl->keymask = (int)v;
}

unsigned int MenuItem :: implGetKeyMask() {
	return mImpl->keymask;
}

bool MenuItem :: implIsSeparator() {
	if(mQtItem) {
		return mQtItem->isSeparator();
	}
	return false;
}

void MenuItem :: implEnableAction() {
	if(mQtItem) {
		mQtItem->setEnabled(true);
	}
}

void MenuItem :: implDisableAction() {
	if(mQtItem) {
		mQtItem->setEnabled(false);
	}
}

void MenuItemImpl :: makeShortcut() {
	if(mItem && key.length() > 0) {
		char sc[16];
		if(strcmp(key.data(), "enter") == 0) {
			//sprintf(sc, "%s+%s", stringForKeymask(keymask), Qt::Key_Return);
			if(keymask == Qt::MetaModifier) {
				mItem->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Return));
			}
			else {
				mItem->setShortcut(QKeySequence(keymask + Qt::Key_Return));
			}
		}
		else {
			sprintf(sc, "%s+%s", stringForKeymask(keymask), key.data());
			mItem->setShortcut(tr(sc));
		}
		
	}	
}

void MenuItemImpl :: dispatch() {
	this->generic()->dispatch();
}

}	// luaav::
