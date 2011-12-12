#include "luaav_console_qt.h"
#include "lua_utility.h"
#include "luaav_app_log_linux.h"
#include "types/al_pq.hpp"
#include "luaav_app_menu.h"

#include <QHeaderView>
#include <QScrollBar>
#include <QFont>
#include <QPalette>
#include <QColor>
#include <QFrame>
#include <QStatusBar>
#include <QPixmap>
#include <QAction>
#include <QApplication>

#define ICON_COL_WIDTH	(30)
#define MIN_CODE_HEIGHT	(25)

#define LUAAV_VERSION	"3.0"


ScriptModel :: ScriptModel(QObject *parent)
:	QAbstractTableModel(parent)
{}

ScriptModel :: ~ScriptModel()
{}


int ScriptModel :: rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return scriptData.size();
}

int ScriptModel :: columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return 4;
}

QVariant ScriptModel :: data(const QModelIndex &index, int role) const {
	if(! index.isValid()) {
		return QVariant();
	}

	if(index.row() >= scriptData.size() || index.row() < 0) {
		return QVariant();
	}

	if(role == Qt::DisplayRole) {
		QString name = scriptData.at(index.row());
		switch(index.column()) {
			case 3:		return QString("Edit");
			case 2:		return QString("Reload");
			case 1:		return QString("Close");			
			case 0:
			default:	return name; 
		}
	}

	return QVariant();
}

QVariant ScriptModel :: headerData(int section, Qt::Orientation orientation, int role) const {
	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	if(orientation == Qt::Horizontal) {
		switch(section) {
			case 0:	return tr("Name");
			case 1:	return tr("Close");
			case 2:	return tr("Reload");
			case 3:	return tr("Edit");
			default: return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags ScriptModel :: flags(const QModelIndex &index) const {
	if(!index.isValid()) {
		return Qt::ItemIsEnabled;
	}

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}
	
bool ScriptModel :: setData(const QModelIndex &index, const QVariant &value, int role) {
	if(index.isValid() && role == Qt::EditRole) {
		int row = index.row();
		scriptData.replace(row, value.toString());
		emit(dataChanged(index, index));
		return true;
	}

	return false;
}

bool ScriptModel :: insertRows(int position, int rows, const QModelIndex &index) {
	Q_UNUSED(index);
	beginInsertRows(QModelIndex(), position, position+rows-1);

	for(int row=0; row < rows; row++) {
		scriptData.insert(position, QString(" "));
	}

	endInsertRows();
	return true;
}

bool ScriptModel :: removeRows(int position, int rows, const QModelIndex &index) {
	Q_UNUSED(index);
	beginRemoveRows(QModelIndex(), position, position+rows-1);

	for(int row=0; row < rows; row++) {
		scriptData.removeAt(position);
	}

	endRemoveRows();
	return true;
}




LuaAVConsole * LuaAVConsole::cConsole = NULL;

LuaAVConsole :: LuaAVConsole()
:	mScriptsLoaded(0),
	mScrolling(true),
	mLogging(false),
	mScriptModel(0),
	mSplitter(0),
	mScriptSplitter(0),
	mTableView(0),
	mCodeEdit(0),
	mTextEdit(0),
	mOpenButton(0),
	mNewButton(0),
	mStatusLabel(0),
	mClearButton(0),
	mScrollButton(0),
	mSelectedScript(-1),
	mStdoutLog(0),
	mStderrLog(0)
{
	// main window properties
	setWindowTitle(QString("LuaAV"));
	setGeometry(300, 600, 650, 375);

	setWindowIcon(QIcon(QPixmap(LUAAV_ICON_PATH)));

	// data model
	mScriptModel = new ScriptModel(this);


	// split scripts and text
	mSplitter = new QSplitter(this);
	mSplitter->setFrameShadow(QFrame::Plain);
	mSplitter->setLineWidth(0);

	setCentralWidget(mSplitter);


	// script splitter
	mScriptSplitter = new QSplitter(this);
	mScriptSplitter->setOrientation(Qt::Vertical);

		// script view
		mTableView = new QTableView(this);
		mTableView->setModel(mScriptModel);
		mTableView->setSortingEnabled(false);
		mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		mTableView->horizontalHeader()->hide();
		mTableView->verticalHeader()->hide();
		mTableView->setSelectionMode(QAbstractItemView::SingleSelection);
		mTableView->setWordWrap(true);
		mTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		QPalette pp(mTableView->palette());
		pp.setColor(QPalette::Highlight, QColor(130, 130, 130));
		mTableView->setPalette(pp);
		QFont font(mTableView->font());
		font.setPointSize(8);
		mTableView->setFont(font);
		mScriptSplitter->addWidget(mTableView);

		// code input
		mCodeEdit = new QLineEdit(this);
		QPalette codePalette(mCodeEdit->palette());
		codePalette.setColor(QPalette::Base, QColor(200, 200, 200));
		mCodeEdit->setPalette(codePalette);
	//	mCodeEdit->setFrameShadow(QFrame::Plain);
	//	mCodeEdit->setFrameStyle(QFrame::NoFrame);
	//	mCodeEdit->setLineWidth(0);
		mCodeEdit->insert(QString("print('hello world')"));
		mCodeEdit->setMinimumHeight(MIN_CODE_HEIGHT);
	//	mCodeEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		mScriptSplitter->addWidget(mCodeEdit);

	mSplitter->addWidget(mScriptSplitter);


	// text window
	mTextEdit = new QTextEdit(this);
	QPalette palette(mTextEdit->palette());
	palette.setColor(QPalette::Base, QColor(140, 140, 140));
	mTextEdit->setPalette(palette);
	mTextEdit->setFrameShadow(QFrame::Plain);
	mTextEdit->setFrameStyle(QFrame::NoFrame);
	mTextEdit->setReadOnly(true);
	mTextEdit->setLineWidth(0);
	mTextEdit->setTabStopWidth(45);
	mSplitter->addWidget(mTextEdit);

	// status buttons and label
	mOpenButton = new QPushButton(this);
	mOpenButton->setText(QString("Open"));

	mNewButton = new QPushButton(this);
	mNewButton->setText(QString("New"));

	mStatusLabel = new QLabel(this);
	mStatusLabel->setAlignment(Qt::AlignCenter);

	mClearButton = new QPushButton(this);
	mClearButton->setText(QString("Clear"));

	QAction *clearAction = new QAction(this);
	clearAction->setShortcut(QKeySequence(tr("Ctrl+x")));
	addAction(clearAction);
	connect(clearAction, SIGNAL(triggered()), mClearButton, SLOT(click()));

	mScrollButton = new QPushButton(this);
	mScrollButton->setText(QString("Scroll"));
	mScrollButton->setCheckable(true);


	QStatusBar *status = statusBar();
	status->addWidget(mOpenButton);
	status->addWidget(mNewButton);
	status->addWidget(mStatusLabel, 20);
	status->addWidget(mClearButton);
	status->addWidget(mScrollButton);

	// signal/slot connections
	connect(mTextEdit->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(scroll()));
	connect(mSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(resizeTable(int,int)));
	connect(mTableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableViewClicked(const QModelIndex&)));

	connect(mOpenButton, SIGNAL(clicked()), this, SLOT(openFile()));
	connect(mNewButton, SIGNAL(clicked()), this, SLOT(newFile()));
	connect(mClearButton, SIGNAL(clicked()), mTextEdit, SLOT(clear()));
	connect(mScrollButton, SIGNAL(clicked(bool)), this, SLOT(setScrolling(bool)));
	connect(this, SIGNAL(clearConsole()), mTextEdit, SLOT(clear()));
	connect(mCodeEdit, SIGNAL(returnPressed()), this, SLOT(evalCode()));
}

LuaAVConsole :: ~LuaAVConsole() {
	delete mScriptModel;
	delete mSplitter;
	delete mScriptSplitter;
	delete mTableView;
	delete mCodeEdit;
	delete mTextEdit;
	delete mOpenButton;
	delete mNewButton;
	delete mStatusLabel;
	delete mClearButton;	
	delete mScrollButton;
}

void LuaAVConsole :: showEvent(QShowEvent *event) {
	QList<int> sizes;	
	int h = mScriptSplitter->height() - mScriptSplitter->handleWidth();
	sizes.append(h-MIN_CODE_HEIGHT);
	sizes.append(MIN_CODE_HEIGHT);
	mScriptSplitter->setSizes(sizes);

	sizes.replace(0, 233);
	sizes.replace(1, 367);
	mSplitter->setSizes(sizes);

	resizeTable(mSplitter->sizes().at(0), 0);
}

void LuaAVConsole :: resizeEvent(QResizeEvent *event) {
	resizeTable(mSplitter->sizes().at(0), 0);
}

void LuaAVConsole :: init(const char *path) {
	luaav_app_init(path);
}

LuaAVConsole * LuaAVConsole :: console() {
	if(!cConsole) {
		cConsole = new LuaAVConsole();
	}
	return cConsole;
}

void LuaAVConsole :: closeEvent(QCloseEvent *event) {
	luaav_app_quit();
}


#define LUAAV_LOG_POLL_PERIOD (1./30.)

static int readpipes(al_sec now, LuaAVConsole *console, luaav_state *S) {
	console->readPipes();
	
	al::PQHelper queue(luaav_state_mainpq(S));
	queue.send(LUAAV_LOG_POLL_PERIOD, LUAAV_LOG_POLL_PERIOD, readpipes, console, S);
	return 0;
}

void LuaAVConsole :: startLogging(const char *path) {
	if(!mLogging) {
		mLogging = true;
		mLogPath.assign(path);


		time_t tt;
		char logstr[512];

		time(&tt);

		sprintf(logstr, "LuaAV %s %s\n", LUAAV_VERSION, ctime(&tt));
		mTextEdit->textCursor().insertText(QString(logstr));

		mStdoutLog = luaav::Log::stdoutRedirect(this, &LuaAVConsole::stdoutRedirect);
		mStderrLog = luaav::Log::stderrRedirect(this, &LuaAVConsole::stderrRedirect);

		luaav_state *S = luaav_app_state();
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(LUAAV_LOG_POLL_PERIOD, LUAAV_LOG_POLL_PERIOD, readpipes, this, S);	
	}
}

void LuaAVConsole :: readPipes() {
	mStdoutLog->readpipe();
	mStderrLog->readpipe();
}

void LuaAVConsole :: stdoutRedirect(const char *text) {
	mTextEdit->moveCursor(QTextCursor::End);
	mTextEdit->textCursor().insertText(QString(text));
}

void LuaAVConsole :: stderrRedirect(const char *text) {
	mTextEdit->moveCursor(QTextCursor::End);
	mTextEdit->textCursor().insertText(QString(text));
}

void LuaAVConsole :: stopLogging() {
	if(mLogging) {
		
		mLogging = true;
	}
}

void LuaAVConsole :: addScript(const char *name, const char *filename, luaav_state *S) {
	int idx = mScriptModel->getList().size();
	mScriptModel->insertRow(idx, QModelIndex());

	QModelIndex index = mScriptModel->index(idx, 0, QModelIndex());
	mScriptModel->setData(index, QString(name), Qt::EditRole);

	resizeTable(mSplitter->sizes().at(0), 0);

	// table buttons for script actions
	char bname[16];
	QPushButton *closeButton = new QPushButton();
	closeButton->setIcon(QIcon(QPixmap("/usr/share/luaav/x.png")));
	closeButton->setFlat(true);
	closeButton->setAutoFillBackground(true);
	sprintf(bname, "close:%d", mScriptsLoaded);
	closeButton->setObjectName(QString(bname));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(fileAction()));
	mTableView->setIndexWidget(mScriptModel->index(idx, 1), closeButton);

	QPushButton *reloadButton = new QPushButton();
	reloadButton->setIcon(QIcon(QPixmap("/usr/share/luaav/reload.png")));
	reloadButton->setFlat(true);
	reloadButton->setAutoFillBackground(true);
	sprintf(bname, "reload:%d", mScriptsLoaded);	
	reloadButton->setObjectName(QString(bname));
	connect(reloadButton, SIGNAL(clicked()), this, SLOT(fileAction()));
	mTableView->setIndexWidget(mScriptModel->index(idx, 2), reloadButton);

	QPushButton *editButton = new QPushButton();
	editButton->setIcon(QIcon(QPixmap("/usr/share/luaav/eye.png")));
	editButton->setFlat(true);
	editButton->setAutoFillBackground(true);
	sprintf(bname, "edit:%d", mScriptsLoaded);
	editButton->setObjectName(QString(bname));
	connect(editButton, SIGNAL(clicked()), this, SLOT(fileAction()));
	mTableView->setIndexWidget(mScriptModel->index(idx, 3), editButton);

	mScripts.push_back(new ScriptData(mScriptsLoaded, name, filename, S));
	mScriptsLoaded++;
}

void LuaAVConsole :: removeScript(const char *name, const char *filename) {
	QList<QString> data = mScriptModel->getList();
	QString qfilename(filename);
	
	int i=0;
	for(QList<QString>::iterator it = data.begin(); it != data.end(); ++it) {
		if((*it).compare(qfilename) == 0) {
			mScriptModel->removeRows(i, 1, QModelIndex());
			if(i == mSelectedScript) {
				mSelectedScript = data.size()-1;
			}

			int j=0;
			for(std::vector<ScriptData *>::iterator sit = mScripts.begin(); 
							sit != mScripts.end(); 
							++sit)
			{
				
				if(i == j) {
					ScriptData *sd = *sit;
					mScripts.erase(sit);
					delete sd;
					break;
				}
				j++;
			}
		}
		i++;
	}
}

const char * LuaAVConsole :: getFilename(int idx) {
	QList<QString> data = mScriptModel->getList();
	if(idx >= 0 && idx < data.size()) {
		return data.at(idx).toStdString().data();
	}
	else {
		return "< INVALID INDEX >";
	}
}

const char * LuaAVConsole :: getFilenameWithScriptIndex(int scriptIdx) {
	QList<QString> data = mScriptModel->getList();
	for(std::vector<ScriptData *>::iterator sit = mScripts.begin(); 
		sit != mScripts.end(); 
		++sit)
	{
		if((*sit)->idx == scriptIdx) {
			return (*sit)->path.data();
		}
	}

	return "< INVALID INDEX >";
}

const char * LuaAVConsole :: selectedScriptPath() {
	if(mSelectedScript >= 0 && mScripts.size() > mSelectedScript) {
		return getFilename(mSelectedScript);
	}
	else if(mScripts.size() > 0) {
		return getFilename(mScripts.size()-1);
	}
	else {
		return NULL;
	}
}

void LuaAVConsole :: scroll() {
	if(mScrolling) {
		QScrollBar *scrollbar = mTextEdit->verticalScrollBar();
		scrollbar->setSliderPosition(scrollbar->maximum());
	}
}

void LuaAVConsole :: resizeTable(int pos, int index) {
	int w = pos - 3*ICON_COL_WIDTH;
	w = (w < ICON_COL_WIDTH) ? ICON_COL_WIDTH : w;
	mTableView->setColumnWidth(0, w);
	mTableView->setColumnWidth(1, ICON_COL_WIDTH);
	mTableView->setColumnWidth(2, ICON_COL_WIDTH);
	mTableView->setColumnWidth(3, ICON_COL_WIDTH);
}

void LuaAVConsole :: tableViewClicked(const QModelIndex &index) {
//	printf("Clicked: %d %d\n", index.column(), index.row());
	const char *filename = getFilename(index.row());
	switch(index.column()) {
		case 0:	mSelectedScript = index.row(); break;
		case 1: lua::call(luaav_getstate(luaav_app_state()), "fileclose", filename); break;
		case 2:	lua::call(luaav_getstate(luaav_app_state()), "fileopen", filename);  break;
		case 3: lua::call(luaav_getstate(luaav_app_state()), "fileedit", filename);  break;
		default: break;
	}
}

void LuaAVConsole :: evalCode() {
	QString code = mCodeEdit->text();
	const char *src = code.toStdString().data();

	if(strlen(src) <= 0) {
		return;
	}

	lua_State *L = NULL;
	
	
	if(mScriptModel->getList().size() > 0) {
		QModelIndex idx = mTableView->currentIndex();
		int row = idx.row();
		if(row >= 0) {
			ScriptData *data = mScripts.at(row);
			if (data) {
				L = luaav_getstate(data->S);
			}
		}
		else {
			L = luaav_getstate(luaav_app_state());
		}
	}
	else {
		L = luaav_getstate(luaav_app_state());
	}
	
	if(L) {
		if (lua::dostring(L, src)) {
			printf("error: %s\n", lua_tostring(L, -1));
		}
	}
}

void LuaAVConsole :: setScrolling(bool v) {
	scrolling(!v);
}

void LuaAVConsole :: openFile() {
	luaav_app_menuitem_action("(File).(Open...)");
}

void LuaAVConsole :: newFile() {
	luaav_app_menuitem_action("(File).(New...)");
}

void LuaAVConsole :: fileAction() {
	QObject *s = sender();
	const char *name = s->objectName().toStdString().data();	
	const char *idx = strchr(name, ':')+1;
	int i = 0;
	sscanf(idx, "%d", &i);

	const char *filename = getFilenameWithScriptIndex(i);

	if(strncmp(name, "close", strlen("close")) == 0) {
		lua::call(luaav_getstate(luaav_app_state()), "fileclose", filename);
	}
	else if(strncmp(name, "reload", strlen("reload")) == 0) {
		lua::call(luaav_getstate(luaav_app_state()), "fileopen", filename);
	}
	else if(strncmp(name, "edit", strlen("edit")) == 0) {
		lua::call(luaav_getstate(luaav_app_state()), "fileedit", filename);
	}
}

void LuaAVConsole :: clearText() {
	emit clearConsole();
}

void LuaAVConsole :: setStatus(const char *label) {
	mStatusLabel->setText(QString(label));
}

void LuaAVConsole :: setScrollButton(bool v) {
	mScrollButton->setDown(!v);
	scrolling(v);
}


#pragma mark Lua API

int app_console_startlogging(lua_State * L) {
	const char * path = luaL_checkstring(L, 1);
	LuaAVConsole::console()->startLogging(path);
	return 0;
}

int app_console_stoplogging(lua_State * L) {
	LuaAVConsole::console()->stopLogging();
	return 0;
}

int app_console_clearlog(lua_State * L) {
	LuaAVConsole::console()->clearText();
	return 0;
}

int app_console_scrolling(lua_State * L) {
	if (lua_isboolean(L, 1)) {
		LuaAVConsole::console()->setScrollButton(lua_toboolean(L, 1) ? true : false);
	}
	lua::push(L, LuaAVConsole::console()->scrolling());
	return 1;
}

int app_console_statusbar(lua_State * L) {
	LuaAVConsole::console()->setStatus(luaL_checkstring(L, 1));
	return 0;
}

int app_console_addscript(lua_State * L) {
	const char *filename = lua_tostring(L, 1);
	LuaAVConsole::console()->addScript(filename, filename, (luaav_state *)lua_touserdata(L, 2));
	return 0;
}

int app_console_removescript(lua_State * L) {
	const char *filename = lua_tostring(L, 1);
	LuaAVConsole::console()->removeScript(filename, filename);
	return 0;
}

int app_console_selectedscript(lua_State * L) {
	const char *filename = LuaAVConsole::console()->selectedScriptPath();
	if(filename) {
		lua::push(L, filename);
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

int app_console_isfocused(lua_State * L) {
	bool focused = (LuaAVConsole::console() == QApplication::activeWindow());
	lua_pushboolean(L, focused? 1 : 0);
	return 1;
}

int app_console_copy(lua_State * L) {
	LuaAVConsole::console()->getTextEdit()->copy();
	return 0;
}

int app_console_selectall(lua_State * L) {
	LuaAVConsole::console()->getTextEdit()->selectAll();
	return 1;
}

int app_console_rect(lua_State *L) {
	if(lua_istable(L, 1)) {
		lua_rawgeti(L, 1, 1);
		int x = (int)lua_tonumber(L, -1);
		lua_rawgeti(L, 1, 2);
		int y = (int)lua_tonumber(L, -1);
		lua_rawgeti(L, 1, 3);
		int w = (int)lua_tonumber(L, -1);
		lua_rawgeti(L, 1, 4);
		int h = (int)lua_tonumber(L, -1);
		lua_pop(L, 4);
	
		LuaAVConsole::console()->setGeometry(QRect(x, y, w, h));
	}
	else {
		lua_newtable(L);
		QRect r = LuaAVConsole::console()->geometry();
		lua_pushinteger(L, r.x());
		lua_rawseti(L, -2, 1);
		lua_pushinteger(L, r.y());
		lua_rawseti(L, -2, 2);
		lua_pushinteger(L, r.width());
		lua_rawseti(L, -2, 3);
		lua_pushinteger(L, r.height());
		lua_rawseti(L, -2, 4);
		return 1;
	}
	return 0;
}


int luaopen_app_console(lua_State * L) {
	const char * libname = lua_tostring(L, 1);
	struct luaL_reg lib[] = {
		{ "startlogging", app_console_startlogging },
		{ "stoplogging", app_console_stoplogging },
		{ "clearlog", app_console_clearlog },
		{ "scrolling", app_console_scrolling },
		{ "statusbar", app_console_statusbar },
		{ "addscript", app_console_addscript },
		{ "removescript", app_console_removescript },
		{ "selectedscript", app_console_selectedscript },
		{ "isfocused", app_console_isfocused },
		{ "copy", app_console_copy },
		{ "selectall", app_console_selectall },
		{ "rect", app_console_rect },
		{ NULL, NULL },
	};
	luaL_register(L, libname, lib);
	return 1;
}
