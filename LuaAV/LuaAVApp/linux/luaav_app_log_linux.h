#ifndef INCLUDE_LUAAV_APP_LOG_LINUX_H
#define INCLUDE_LUAAV_APP_LOG_LINUX_H

#ifdef LUAAV_APP_USE_QT
	#include "luaav_console_qt.h"
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

namespace luaav {

class Log {
public:
    Log(LuaAVConsole *console, LuaAVConsole::Redirect redirect, FILE * stream);
    ~Log();

    static Log * stdoutRedirect(LuaAVConsole *console, LuaAVConsole::Redirect redirect);
    static Log * stderrRedirect(LuaAVConsole *console, LuaAVConsole::Redirect redirect);

    void readpipe();

protected:
 	/* File descriptors for pipe. */
    int mPipe[2];
    FILE * mStream, * mReadStream;
    LuaAVConsole *mConsole;
    LuaAVConsole::Redirect mRedirect;

};

}   // luaav::

#endif	// INCLUDE_LUAAV_APP_LOG_LINUX_H
