#ifndef LUAAV_APP_CODEPAD_WINDOW_QT_H
#define LUAAV_APP_CODEPAD_WINDOW_QT_H

#include <QTabWidget>
#include <list>

class CodePadWindow : public QTabWidget {
	Q_OBJECT

public:
	CodePadWindow();
	virtual ~CodePadWindow();

	// creates a window if one doesn't exist
	static CodePadWindow * existingActiveWindow();
	static CodePadWindow * activeWindow();
	static CodePadWindow * holder(QWidget *w);

	void add(QWidget *w, const char *name);
	static void remove(CodePadWindow *window, QWidget *w);

private slots:
	void closeTab(int index);

protected:
	void closeIfEmpty();

	static std::list<CodePadWindow *>	cWindows;
};

#endif	// LUAAV_APP_CODEPAD_WINDOW_QT_H

