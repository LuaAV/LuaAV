#include <unistd.h>
#include <stdlib.h>

#include "av.h"

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"

	int luaopen_builtin(lua_State * L);
}

#if defined(WIN32) || defined(__WINDOWS_MM__) || defined(WIN64)
	#define AV_WINDOWS 1
	// just placeholder really; Windows requires a bit more work yet.
	#include <windows.h>

#else
	// Unixen:
	#include <sys/time.h>
	#include <time.h>
	#include <libgen.h>
	#if defined( __APPLE__ ) && defined( __MACH__ )
		#define AV_OSX 1
		#include <OpenGL/OpenGL.h>
		#include <GLUT/glut.h>

	#else
		#define AV_LINUX 1
		#include <GL/gl.h>
		#include <GL/glut.h>
		
	#endif
#endif

// the path from where it was invoked:
char launchpath[PATH_MAX];
// the path where the binary actually resides, e.g. with modules:
char apppath[PATH_MAX];
// the path of the start file, e.g. user script / workspace:
char workpath[PATH_MAX];
// filename of the main script:
char mainfile[PATH_MAX];

void getpaths(int argc, char ** argv) {
	char wd[PATH_MAX];
	if (getcwd(wd, PATH_MAX) == 0) {
		printf("could not derive working path\n");
		exit(0);
	}
	snprintf(launchpath, PATH_MAX, "%s/", wd);
	
	
	// get binary path:
	char tmppath[PATH_MAX];
	#ifdef AV_OSX
		if (argc > 0) {
			realpath(argv[0], tmppath);
		}
		snprintf(apppath, PATH_MAX, "%s/", dirname(tmppath));
	#elif defined(AV_WINDOWS)
		// Windows only:
		// GetModuleFileName(NULL, apppath, PATH_MAX)
	#else
		// Linux only?
		int count = readlink("/proc/self/exe", tmppath, PATH_MAX);
		if (count > 0) {
			tmppath[count] = '\0';
		} else if (argc > 0) {
			realpath(argv[0], tmppath);
		}
		snprintf(apppath, PATH_MAX, "%s/", dirname(tmppath));
	#endif
	
	char apath[PATH_MAX];
	#if defined(AV_WINDOWS)
	#else
	if (argc > 1) {
		realpath(argv[1], apath);
		
		snprintf(mainfile, PATH_MAX, "%s", basename(apath));
		snprintf(workpath, PATH_MAX, "%s/", dirname(apath));
	} else {
		// just copy the current path:
		snprintf(workpath, PATH_MAX, "%s", launchpath);
		snprintf(mainfile, PATH_MAX, "%s", "main.lua");
	}
	#endif
	
//	printf("launchpath %s\n", launchpath);
//	printf("apppath %s\n", apppath);
//	printf("workpath %s\n", workpath);
//	printf("mainfile %s\n", mainfile);
}

// implement av_Window using GLUT:
struct av_Window_GLUT : public av_Window {
	
	// GLUT specific:
	int id;
	int non_fullscreen_width, non_fullscreen_height;
	bool reload;
	
	av_Window_GLUT() {
		width = 720;
		height = 480;
		is_fullscreen = 0;
		shift = alt = ctrl = 0;
		fps = 60;
		oncreate = 0;
		onresize = 0;
		onvisible = 0;
		ondraw = 0;
		onkey = 0;
		onmouse = 0;
		id = 0;
		non_fullscreen_width = width;
		non_fullscreen_height = height;
		reload = true;
	}
};

// the window
av_Window_GLUT win;

void timerfunc(int id) {

	// TODO: trigger scheduled events... 
	//av_tick();
	
	// update window:
	if (win.reload && win.oncreate) {
		(win.oncreate)(&win);
		win.reload = false;
	}
	if (win.ondraw) {
		(win.ondraw)(&win);
	}	
	glutSwapBuffers();
	
	// reschedule:
	glutTimerFunc((unsigned int)(1000.0/win.fps), timerfunc, 0);
}

void av_window_settitle(av_Window * self, const char * name) {
	glutSetWindowTitle(name);
}

void av_window_setfullscreen(av_Window * self, int b) {
	win.reload = true;
	win.is_fullscreen = b;
	if (b) {
		glutFullScreen();
	} else {
		glutReshapeWindow(win.non_fullscreen_width, win.non_fullscreen_height);
	}
}

void av_window_setdim(av_Window * self, int x, int y) {
	glutReshapeWindow(x, y);
}

av_Window * av_window_create() {
	return &win;
}

void getmodifiers() {
	int mod = glutGetModifiers();
	win.shift = mod & GLUT_ACTIVE_SHIFT;
	win.alt = mod & GLUT_ACTIVE_ALT;
	win.ctrl = mod & GLUT_ACTIVE_CTRL;
}

void onkeydown(unsigned char k, int x, int y) {
	getmodifiers();
	if (win.onkey) {
		(win.onkey)(&win, 1, k);
	}
}

void onkeyup(unsigned char k, int x, int y) {
	getmodifiers();
	if (win.onkey) {
		(win.onkey)(&win, 2, k);
	}
}

void onspecialkeydown(int key, int x, int y) {
	getmodifiers();
	
	#define CS(k) case GLUT_KEY_##k: key = AV_KEY_##k; break;
	switch(key){
		CS(LEFT) CS(UP) CS(RIGHT) CS(DOWN)
		CS(PAGE_UP) CS(PAGE_DOWN)
		CS(HOME) CS(END) CS(INSERT)

		CS(F1) CS(F2) CS(F3) CS(F4)
		CS(F5) CS(F6) CS(F7) CS(F8)
		CS(F9) CS(F10)	CS(F11) CS(F12)
	}
	#undef CS
	
	if (win.onkey) {
		(win.onkey)(&win, 1, key);
	}
}

void onspecialkeyup(int key, int x, int y) {
	getmodifiers();
	
	#define CS(k) case GLUT_KEY_##k: key = AV_KEY_##k; break;
	switch(key){
		CS(F1) CS(F2) CS(F3) CS(F4)
		CS(F5) CS(F6) CS(F7) CS(F8)
		CS(F9) CS(F10)	CS(F11) CS(F12)
		
		CS(LEFT) CS(UP) CS(RIGHT) CS(DOWN)
		CS(PAGE_UP) CS(PAGE_DOWN)
		CS(HOME) CS(END) CS(INSERT)
	}
	#undef CS
	
	if (win.onkey) {
		(win.onkey)(&win, 2, key);
	}
}

void onmouse(int button, int state, int x, int y) {
	getmodifiers();
	win.button = button;
	if (win.onmouse) {
		(win.onmouse)(&win, state, win.button, x, y);
	}
}

void onmotion(int x, int y) {
	if (win.onmouse) {
		(win.onmouse)(&win, 2, win.button, x, y);
	}
}

void onpassivemotion(int x, int y) {
	if (win.onmouse) {
		(win.onmouse)(&win, 3, win.button, x, y);
	}
}

void onvisibility(int state) {
	if (win.onvisible) (win.onvisible)(&win, state);
}

void ondisplay() {}
void onreshape(int w, int h) {
	win.width = w;
	win.height = h;
	if (!win.is_fullscreen) {
		win.non_fullscreen_width = win.width;
		win.non_fullscreen_height = win.height;
	}
	if (win.onresize) {
		(win.onresize)(&win, w, h);
	}
}

void terminate() {
	printf("exit -- bye\n");
}

bool initializedGLUT = 0;
void initGLUT() {
	if (initializedGLUT) return;
	int argc = 0;
	char * argv[] = { NULL };
	glutInit(&argc, argv);
	
//	screen_width = glutGet(GLUT_SCREEN_WIDTH);
//	screen_height = glutGet(GLUT_SCREEN_HEIGHT);	
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(win.width, win.height);
	glutInitWindowPosition(0, 0);
	
	win.id = glutCreateWindow("");	// << FAIL?
	//printf("initializing window\n");
	glutSetWindow(win.id);
	
//	glutIgnoreKeyRepeat(1);
//	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(onkeydown);
	glutKeyboardUpFunc(onkeyup);
	glutMouseFunc(onmouse);
	glutMotionFunc(onmotion);
	glutPassiveMotionFunc(onpassivemotion);
	glutSpecialFunc(onspecialkeydown);
	glutSpecialUpFunc(onspecialkeyup);
	glutVisibilityFunc(onvisibility);
	glutReshapeFunc(onreshape);
	glutDisplayFunc(ondisplay);
	
	glutTimerFunc((unsigned int)(1000.0/win.fps), timerfunc, 0);
	
	atexit(terminate);
	
	initializedGLUT = true;
}

void av_sleep(double seconds) {
	#ifdef AV_WINDOWS
		Sleep((DWORD)(seconds * 1.0e3));
	#else
		time_t sec = (time_t)seconds;
		long long int nsec = 1.0e9 * (seconds - (double)sec);
		timespec tspec = { sec, nsec };
		while (nanosleep(&tspec, &tspec) == -1) {
			continue;
		}
	#endif
}

double av_time() {
	timeval t;
	gettimeofday(&t, NULL);
	return (double)t.tv_sec + (((double)t.tv_usec) * 1.0e-6);
}	

#include "av_ffi_header.cpp"

int luaopen_builtin(lua_State * L) {

	initGLUT();

	static struct luaL_reg lib[] = {
		// { "name", func },
		{ NULL, NULL },
	};
	luaL_register(L, "builtin", lib);
	
	luaL_loadstring(L, av_ffi_header);
	lua_pcall(L, 0, 1, 0);
	lua_setfield(L, -2, "header");
	
	return 1;
}


extern "C" int main(int argc, char * argv[]) {
	
	// initialize paths:
	getpaths(argc, argv);
	
	// execute in the context of wherever this is run from:
	chdir(workpath);
	
	//chdir("./");

	lua_State * L = lua_open();
	luaL_openlibs(L);
	luaL_loadstring(L, "package.path = './modules/?.lua;./modules/?/init.lua;'..package.path; print(jit.version)");
	lua_call(L, 0, 0);
	
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
		lua_pushcfunction(L, luaopen_builtin);
		lua_setfield(L, -2, "builtin");
	lua_pop(L, 2);
	
	
	lua_getglobal(L, "debug");
	lua_pushliteral(L, "traceback");
	lua_gettable(L, -2);
	lua_setfield(L, LUA_REGISTRYINDEX, "debug.traceback");
	
	lua_settop(L, 0); // clean stack
		
	const char * startfile = argc > 1 ? argv[1] : "./start.lua";
	lua_getfield(L, LUA_REGISTRYINDEX, "debug.traceback");
	int debugtraceback = lua_gettop(L);
	int err = luaL_loadfile(L, startfile);
	if (err == 0) {
		for (int i=0; i<argc; i++) {
			lua_pushstring(L, argv[i]);
		}
		err = lua_pcall(L, argc, LUA_MULTRET, debugtraceback);
	}
	if (err) {
		printf("error (%s): %s\n", startfile, lua_tostring(L, -1));
		return 0;
	}
	
	glutMainLoop();
	
	printf("bye\n");
	return 0;
}
