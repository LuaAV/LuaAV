#include "luaav_app_log_linux.h"
#include <string>
#include <fcntl.h>

namespace luaav {

static Log *g_stdoutLog = NULL;
static Log *g_stderrLog = NULL;

static void readData(FILE * stream, std::string & str) {
    int res = 0;
    do {
        char buffer[128];
        res = fread(buffer, 1, 127, stream);
        buffer[res] = '\0';
        str.append(buffer);
    } while (res > 0);
}

void Log :: readpipe() {
    fflush(mStream);

    std::string str;
    readData(mReadStream, str);
    int sz = str.size();
    if (sz > 0) {
        (mConsole->*(mRedirect))(str.c_str());
    }
}


Log :: Log(LuaAVConsole *console, LuaAVConsole::Redirect redirect, FILE * stream)
:   mStream(stream),
    mConsole(console),
    mRedirect(redirect)
{
	// Create a pipe. File descriptors for the two ends of the pipe are placed in fds.
    if (pipe (mPipe) != 0) {
        // error
    }

    // Redirect mPipe[1] to be written with the standard output.
    dup2 (mPipe[1], fileno(stream));

    // for each end of the pipe:
    for (int n=0; n<2; n++) {
        // change the file status/access mode flags to include O_NONBLOCK (non-blocking IO)
        int f = fcntl(mPipe[n], F_GETFL, 0);
        f |= O_NONBLOCK;
        fcntl(mPipe[n], F_SETFL, f);
    }
    // store the input stream:
    mReadStream = fdopen(mPipe[0], "r");
}

Log :: ~Log()
{}

Log * Log :: stdoutRedirect(LuaAVConsole *console, LuaAVConsole::Redirect redirect) {
    if(!g_stdoutLog) g_stdoutLog = new Log(console, redirect, stdout);
    return g_stdoutLog;
}

Log * Log :: stderrRedirect(LuaAVConsole *console, LuaAVConsole::Redirect redirect) {
    if(!g_stderrLog) g_stderrLog = new Log(console, redirect, stderr);
    return g_stderrLog;
}

}   // luaav::
