#include "al_mainloop.h"
#include "stdlib.h"
#include "assert.h"

/*
	Platform specific implementations
		(implemented in separate file)
*/
void al_main_platform_attach(al_sec interval);
int al_main_platform_enter(al_sec interval);

/*
	If there is anything that is a singleton, it's the main loop!
*/	
static al_main_t * g_main;

/*!
	Main initialization and termination
*/
static int al_main_quit();

#pragma mark C API

al_main_t * al_main_get() {
	if (g_main == 0) {
		g_main = (al_main_t *)malloc(sizeof(al_main_t));
		assert(g_main != 0); /* if this fails, then your OS is probably going down */
		g_main->t0 = al_time_nsec();
		g_main->isRunning = 0;
		g_main->userdata = NULL;
		g_main->interval = 1;
		g_main->tickhandler = NULL;
		g_main->quithandler = NULL;
	}
	return g_main;
}

int al_main_quit() {
	if (g_main != 0) {
		free(g_main);
		g_main = 0;
	}
	return 0;
}

int al_main_enter(al_sec interval, main_tick_handler tickhandler, void * userdata, main_quit_handler quithandler) {
	al_main_get();	
	if (!g_main->isRunning) {
		g_main->interval = interval;
		g_main->isRunning = 1;
		g_main->tickhandler = tickhandler;
		g_main->quithandler = quithandler;
		g_main->userdata = userdata;
		return al_main_platform_enter(interval);
	}
	return 0;
}

void al_main_exit() {
	if (g_main->isRunning) {
		g_main->isRunning = 0;
		if (g_main->quithandler)
			g_main->quithandler(g_main->userdata);
	}
}

void al_main_attach(al_sec interval, main_tick_handler tickhandler, void * userdata, main_quit_handler quithandler) {
	al_main_get();	
	if (!g_main->isRunning) {
		g_main->interval = interval;
		g_main->isRunning = 1;
		g_main->tickhandler = tickhandler;
		g_main->quithandler = quithandler;
		g_main->userdata = userdata;
		al_main_platform_attach(interval);
	}
}

void al_main_register(main_tick_handler tickhandler, void * userdata, main_quit_handler quithandler) {
	al_main_get();
	if (!g_main->isRunning) {
		g_main->isRunning = 1;
		g_main->tickhandler = tickhandler;
		
		g_main->quithandler = quithandler;
		g_main->userdata = userdata;
	}
}

void al_main_tick() {
	g_main->logicaltime = al_time_nsec() - g_main->t0;
	// pass control to user-specified mainloop:
	(g_main->tickhandler)(g_main->logicaltime, g_main->userdata);
}

al_nsec al_main_time_nsec() {
	return g_main->logicaltime;
}