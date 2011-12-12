/*
	A scheduled callback using NSTimer, which runs in the NSApplication main loop
*/

#import <Cocoa/Cocoa.h>

#include "system/al_mainloop.h"


/*
	A class to receive event callbacks from the OS
*/
@interface OSClock : NSObject {
	NSTimer * timer;
	al_main_t * main;
}

- (id)initWithInterval:(al_sec)ti;
- (void)dealloc;
- (void)tick;

@end

@implementation OSClock

-(void)tick
{
	al_main_tick();
}

- (id)initWithInterval:(al_sec)interval;
{
	if (self = [super init])
	{
		main = al_main_get();
		timer = [NSTimer timerWithTimeInterval:(NSTimeInterval)interval
									target:self 
									selector:@selector(tick) 
									userInfo:nil 
									repeats:YES];		
		
		// attach to the runloop it was created from
		// keep on going even during modal dialogs, window scrolling etc.
		[[NSRunLoop currentRunLoop] addTimer:timer forMode:NSEventTrackingRunLoopMode]; 
		[[NSRunLoop currentRunLoop] addTimer:timer forMode:NSModalPanelRunLoopMode]; 
		[[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode]; 
	}
	return self;
}

- (void)dealloc
{
	if(timer) {
		[timer invalidate];
		timer = nil;
	}
    [super dealloc];
}

@end

static OSClock * gClock;

/*
	Main impl
*/
#pragma mark Main impl

void al_main_platform_attach(al_sec interval) {
	if (!gClock) {
		gClock = [[OSClock alloc] initWithInterval:interval];	
	} else {
		// what to do if loop already exists?
	}
}

int al_main_platform_enter(al_sec interval) {
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	// create a new runloop if it doesn't exist:
	NSRunLoop * rl = [NSRunLoop currentRunLoop];
	
	al_main_t * main = al_main_get();
	al_main_platform_attach(interval);
	
	// main loop:
    while (main->isRunning && [rl runMode: NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:main->interval]]) { /*printf(".");*/ }
	
	// done
	[pool release];
	[gClock dealloc];
	return 0;
}

