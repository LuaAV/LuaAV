/*
	A scheduled callback using QTimer
*/

#include "al_mainloop_qt.h"


OSClock :: OSClock(al_sec interval)
:	timer(0)
{
	timer = new QTimer();
	timer->setInterval(interval*1000);
	timer->setSingleShot(false);
	connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
}

OSClock :: ~OSClock() {
	delete timer;
}

void OSClock :: start() {
	timer->start();
}

void OSClock :: tick() {
	al_main_tick();
}


static OSClock * gClock = 0;

/*
	Main impl
*/
#pragma mark Main impl

void al_main_platform_attach(al_sec interval) {
	if (!gClock) {
		gClock = new OSClock(interval);
		gClock->start();
	} else {
		// what to do if loop already exists?
	}
}

int al_main_platform_enter(al_sec interval) {
/*	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	// create a new runloop if it doesn't exist:
	NSRunLoop * rl = [NSRunLoop currentRunLoop];
	
	al_main_t * main = al_main_get();
	al_main_platform_attach(interval);
	
	// main loop:
    while (main->isRunning && [rl runMode: NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:main->interval]]) {  }
	
	// done
	[pool release];
	[gClock dealloc];*/
	return 0;
}

