#include "luaav_app.h"
#include "luaav_app_audiodriver.h"
#include "luaav_app_menu.h"
#include "luaav_app_screens.h"
#include "luaav_app_window.h"
#include "luaav_filewatcher.h"

#include "luaav_console_qt.h"
#include <QtGui>

#include <stdlib.h>
#include <string>

#include <sys/wait.h>

void luaav_app_paths_init(
	std::string &app_path,
	std::string &app_resource_path,
	std::string &app_home_path,
	std::string &app_config_path,
	std::string &app_log_path)
{
	//user's home directory:
    char *p = getenv("HOME");
    if(!p) {
        printf("Error: unable to get home directory from getenv(\"HOME\")\n");
    }
    else {
        app_home_path.assign(p);
    }

	// path to application bundle:
    p = getenv("PWD");
	if(!p) {
        printf("Error: unable to get app directory from getenv(\"PWD\")\n");
    }
    else if(app_path.size() > 0 && app_path.data()[0] == '.') {
		const char *first_slash = strchr(app_path.data(), '/');
		
		app_path.assign(p);
		if(first_slash) {
			char subpath[512];
			strcpy(subpath, first_slash);
			app_path.assign(p);
			app_path.append(subpath);
		}

		char resolved[1024];
		std::string sym(app_path);		
		sym.append("/LuaAV");
		int n = readlink(sym.data(), resolved, 1024);
		if(n != -1) {
			resolved[n] = '\0';

			const char *slash = strrchr(resolved, '/');
			resolved[n-strlen(slash)] = '\0';
			app_path.assign(resolved);
		}
    }

	app_config_path.assign(app_home_path);
    app_config_path.append("/.LuaAV");

	app_log_path.assign(app_config_path);
	app_log_path.append("/LuaAV.log");

	app_resource_path.assign(app_path);
    app_resource_path.append("/resources");
}




int quit(lua_State * L) {
	QCoreApplication::quit();
	return 0;
}	

int app_openpanel(lua_State *L) {
	QString dir = QString();
	if(lua_type(L, 1) == LUA_TSTRING) {
		dir = QString(lua_tostring(L, 1));
	}

	QStringList files = QFileDialog::getOpenFileNames(0, QObject::tr("Open"), dir);
	if(files.size() >= 1) {
		if(files.size() == 1) {
			std::string str = files.at(0).toStdString();
			lua_pushstring(L, str.data());
		}
		else {
			lua_newtable(L);
			int i=1;
			for(QStringList::iterator it = files.begin(); it != files.end(); ++it) {
				const char *filename = (*it).toAscii().data();
				lua_pushstring(L, filename);
				lua_rawseti(L, -2, i);
				i++;
			}
		}
	}
	else {
		lua_pushnil(L);
	}

	return 1;
}

int app_savepanel(lua_State *L) {
	QString dir = QString();
	std::string title("Save");
	if(lua_type(L, 1) == LUA_TSTRING) {
		dir = QString(lua_tostring(L, 1));
	}
	if(lua_type(L, 2) == LUA_TSTRING) {
		title.assign(lua_tostring(L, 2));
	}

	// TODO: check if valid
	QString res = QFileDialog::getSaveFileName(0, QObject::tr(title.c_str()), dir);
	const char *filename = res.toAscii().data();
	lua_pushstring(L, filename);
	return 1;
}

int app_openurl(lua_State *L) {
	if(lua_type(L, 1) == LUA_TSTRING) {
		pid_t child = fork();

		if(child == 0) {
			char cmd[1024];
			sprintf(cmd, "xdg-open %s", lua_tostring(L, 1));
			execl("/bin/bash", "/bin/bash", "-c", cmd, NULL);
			_exit(0);
		}
		else if(child == -1) {
			// ERROR
		}
		else {
			int status;
			waitpid(child, &status, 0);
		}	
	}
	else {
		luaL_error(L, "app.openurl: invalid url");
	}
	return 0;
}

int app_fork(lua_State *L) {
	if(lua_type(L, 1) == LUA_TSTRING) {
		pid_t child = fork();

		if(child == 0) {
			const char *cmd = lua_tostring(L, 1);
			execl("/bin/bash", "/bin/bash", "-c", cmd, NULL);
			_exit(0);
		}
		else if(child == -1) {
			// ERROR
		}
		else {
			int status;
			waitpid(child, &status, 0);
		}	
	}
	else {
		luaL_error(L, "app.fork: invalid command");
	}
	return 0;
}

int app_clipboard(lua_State *L) {
	if(lua_isstring(L, 1)) {
		QApplication::clipboard()->setText(QString(lua_tostring(L, 1)));
	}
	else {
		QString data = QApplication::clipboard()->text();
		lua_pushstring(L, data.toAscii().data());
	}
	return 0;
}

int app_window_event(lua_State *L) {
	if(lua_isstring(L, 1)) {
		// NOT NEEDED with Qt
		/*
		char qt_event[64];

		const char *event = lua_tostring(L, 1);
		sprintf(qt_event, "%s()", event);

		QWidget *win = QApplication::activeWindow();
		const QMetaObject *meta = win->metaObject();
		int slot_index = meta->indexOfSlot(qt_event);
		printf("slot_index %s: %d\n", qt_event, slot_index);
		*/
	}
	else {
		luaL_error(L, "app.window_event: invalid command");
	}
	return 0;
}


