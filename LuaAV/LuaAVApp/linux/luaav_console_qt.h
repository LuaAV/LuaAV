#ifndef INCLUDE_LUAAV_CONSOLE_LINUX_H
#define INCLUDE_LUAAV_CONSOLE_LINUX_H

#include "luaav_app.h"
#include "luaav_linux.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTableView>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include <vector>
#include <string>
using std::string;

namespace luaav {
	class Log;
}

class ScriptModel : public QAbstractTableModel {
	Q_OBJECT

public:
	ScriptModel(QObject *parent=0);
	~ScriptModel();

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;	
	bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

	QList<QString> getList() {return scriptData;}

private:
	QList<QString>		scriptData;
};


class LuaAVConsole : public QMainWindow {
	Q_OBJECT

protected:
	LuaAVConsole();
	~LuaAVConsole();

public:

	class ScriptData{
	public:
		ScriptData(int idx, const char *name, const char *path, luaav_state *S)
		:	idx(idx),
			name(name),
			path(path),
			S(S)
		{}

		~ScriptData() {}

		int idx;
		std::string name;
		std::string path;
		luaav_state *S;
	};
	
	typedef void (LuaAVConsole::*Redirect)(const char *);

	void init(const char *path);
	static LuaAVConsole * console();

	void startLogging(const char *path);
	void readPipes();
	void stdoutRedirect(const char *text);
	void stderrRedirect(const char *text);	
	void stopLogging();
	void scrolling(bool v) {mScrolling = v;}
	bool scrolling() {return mScrolling;}

	void addScript(const char *name, const char *filename, luaav_state *S);
	void removeScript(const char *name, const char *filename);
	const char * selectedScriptPath();
	const char * getFilename(int idx);
	const char * getFilenameWithScriptIndex(int scriptIdx);
	void clearText();
	void setStatus(const char *label);
	void setScrollButton(bool v);

	QTextEdit * getTextEdit() {return mTextEdit;}

private slots:
	void scroll();
	void resizeTable(int pos, int index);
	void tableViewClicked(const QModelIndex &index);
	void evalCode();
	void setScrolling(bool v);
	void openFile();
	void newFile();
	void fileAction();

signals:
	void clearConsole();

protected:
	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);

	static LuaAVConsole *cConsole;

	int			mScriptsLoaded;	// used to give each script a unique ID
	bool		mScrolling;
	bool		mLogging;
	string		mLogPath;
	ScriptModel	*mScriptModel;
	QSplitter	*mSplitter;
	QSplitter	*mScriptSplitter;
	QTableView	*mTableView;
	QLineEdit	*mCodeEdit;
	QTextEdit	*mTextEdit;
	QPushButton	*mOpenButton;
	QPushButton	*mNewButton;
	QLabel		*mStatusLabel;
	QPushButton	*mClearButton;	
	QPushButton	*mScrollButton;
	int			mSelectedScript;

	std::vector<ScriptData *>	mScripts;

	luaav::Log *mStdoutLog;
	luaav::Log *mStderrLog;
};

#endif	// INCLUDE_LUAAV_CONSOLE_LINUX_H
