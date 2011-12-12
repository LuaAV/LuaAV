#include "luaav_app_codepad_window_qt.h"
#include "luaav_linux.h"
#include <QPushButton>
#include <stdio.h>
#include <QEvent>
#include <QCoreApplication>
#include <QCloseEvent>


std::list<CodePadWindow *> CodePadWindow::cWindows;


CodePadWindow :: CodePadWindow()
:	QTabWidget(0)
{
	setWindowTitle(tr("Code Pad"));
	setWindowIcon(QIcon(QPixmap(LUAAV_ICON_PATH)));

	setTabsClosable(true);

	// use this to create a "New Tab" button
//	QPushButton *btn = new QPushButton();
//	setCornerWidget(btn);

	setAttribute(Qt::WA_DeleteOnClose, true);

	connect(this, SIGNAL(tabCloseRequested(int)),
			this, SLOT(closeTab(int)));
}

CodePadWindow :: ~CodePadWindow() {
}

CodePadWindow * CodePadWindow :: existingActiveWindow() {
	if(cWindows.size() <= 0) {
		return NULL;
	}
	else {
		return cWindows.front();
	}
}

CodePadWindow * CodePadWindow :: activeWindow() {
	if(cWindows.size() <= 0) {
		CodePadWindow *w = new CodePadWindow();
		w->setGeometry(10, 100, 350, 500);
		w->show();
		cWindows.push_back(w);
	}
	return cWindows.front();
}

CodePadWindow * CodePadWindow :: holder(QWidget *w) {
	std::list<CodePadWindow *>::iterator it = cWindows.begin();
	for(; it != cWindows.end(); ++it) {
		if((*it)->indexOf(w) >= 0) {
			return *it;
		}
	}
	return 0;
}

void CodePadWindow :: add(QWidget *w, const char *name) {
	setUpdatesEnabled(false);
	int idx = addTab(w, name);
	setCurrentIndex(idx);

	

	setUpdatesEnabled(true);
}

void CodePadWindow :: closeIfEmpty() {
	if(count() <= 0) {
		std::list<CodePadWindow *>::iterator it = cWindows.begin();
		for(; it != cWindows.end(); ++it) {
			if((*it) == this) {
				cWindows.erase(it);
				break;
			}
		}
		
		close();
	}
}

void CodePadWindow :: remove(CodePadWindow *window, QWidget *w) {
	int idx = window->indexOf(w);
	if(w >= 0) {
		window->removeTab(idx);
	}

	window->closeIfEmpty();
}

void CodePadWindow :: closeTab(int index) {
	QWidget *w = widget(index);
	removeTab(index);
	delete w;

	closeIfEmpty();
}

