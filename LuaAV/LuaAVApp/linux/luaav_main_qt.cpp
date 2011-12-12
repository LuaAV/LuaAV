#include <QApplication>
#include "luaav_console_qt.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	LuaAVConsole *console = LuaAVConsole::console();
	console->init(argv[0]);
	console->show();
	return app.exec();
}
