//
//  WiiTest.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import "WiiTest.h"

#import <mach/mach_error.h>

#import <IOBluetooth/objc/IOBluetoothDevice.h>

int main(int argc, const char **argv) {
	return NSApplicationMain(argc, argv);
}

@implementation WiiTest

- (id)init {
	if (self = [super init]) {
		[[NSNotificationCenter defaultCenter] addObserver:self
											     selector:@selector(didFoundDevice:) 
													 name:WKDeviceRegistryDidFoundDeviceNotification
												   object:nil];		
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(willSearchDevice:) 
													 name:WKDeviceRegistryWillSearchDevicesNotification
												   object:nil];
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(didSearchDevice:) 
													 name:WKDeviceRegistryDidSearchDevicesNotification
												   object:nil];
		
	}
	return self;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

- (void)awakeFromNib {	
	[[WKDeviceRegistry sharedRegistry] search];
}

- (IBAction)rumble:(id)sender {
	[wk_wiimote setRumbleEnabled:![wk_wiimote isRumbleEnabled]];
}
- (IBAction)infrared:(id)sender {
	[wk_wiimote setAcceptsIRCameraEvents:![wk_wiimote acceptsIRCameraEvents]];
//	[wk_wiimote setSpeakerEnabled:YES];
//	[wk_wiimote setSpeakerMuted:YES];
}
- (IBAction)accelerometer:(id)sender {
	[wk_wiimote setAcceptsAccelerometerEvents:![wk_wiimote acceptsAccelerometerEvents]];
}

- (IBAction)funny:(id)sender {
	if (wk_funny) {
		[wk_wiimote setLeds:0x9];
		[wk_funny invalidate];
		[wk_funny release];
		wk_funny = nil;
	} else {
		wk_funny = [[NSTimer scheduledTimerWithTimeInterval:.075 target:self selector:@selector(funnyTimer:) userInfo:nil repeats:YES] retain];
	}
}

- (IBAction)continous:(id)sender {
	[wk_wiimote setContinuous:![wk_wiimote isContinuous]];
}

- (void)funnyTimer:(NSTimer *)aTimer {
	WKLEDState leds = [wk_wiimote leds];
	
	if (!leds) {
		leds = 1;
	} else {
		if (leds == 1 || leds == 8) 
			wk_reverse = !wk_reverse;
		if (wk_reverse) {
			leds = (leds << 1) & 0xf;
		} else {
			leds = (leds >> 1) & 0xf;
		}
	}
	
	[wk_wiimote setLeds:leds];
}

- (void)willSearchDevice:(NSNotification *)aNotification {
	[ibSearch startAnimation:nil];
}
- (void)didSearchDevice:(NSNotification *)aNotification {
	[ibSearch stopAnimation:nil];
}

- (void)didFoundDevice:(NSNotification *)aNotification {
	IOBluetoothDevice *device = [[aNotification userInfo] objectForKey:WKDeviceRegistryFoundDeviceKey];
	if (!wk_wiimote) {
		wk_wiimote = [[WiiRemote alloc] initWithDevice:device];
		if (!wk_wiimote) {
			while (kIOReturnBusy == [device closeConnection]) {
				NSLog(@"device is busy");
				[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:1]];
			}
		} else {
			[wk_wiimote setDelegate:self];
			[wk_wiimote connect];
		}
	} else {
		NSLog(@"does not currently support many devices");
	}
}

- (void)wiimoteDidConnect:(WiiRemote *)aRemote {
	[wk_wiimote setAcceptsExtensionEvents:YES];
	[wk_wiimote setLeds:0x1];
}

- (void)wiimoteDidDisconnect:(WiiRemote *)aRemote {
	[wk_wiimote release];
	wk_wiimote = nil;
}

- (void)handleWiimoteIREvent:(WKEvent *)anEvent {
	fprintf(stderr, "--\n");
	WKIRPoint **points = [anEvent points];
	for (NSUInteger idx = 0; idx < [anEvent numberOfPoints]; idx++) {
		if (points[idx])
			NSLog(@"ir point %lu: %@", idx, points[idx]);
	}
}

- (void)handleWiimoteButton:(WKEvent *)anEvent down:(BOOL)pressed {
	NSString *action = pressed ? @"down" : @"up";
	switch ([anEvent button]) {
		case kWKWiiRemoteButtonA:
			NSLog(@"Wiimote A %@", action);
			break;
		case kWKWiiRemoteButtonB:
			NSLog(@"Wiimote A %@", action);
			break;
			// ... other buttons
	}
}

- (void)handleWiimoteStatusEvent:(WKEvent *)anEvent {
	switch ([anEvent subtype]) {
		case kWKStatusEventBattery:
			[ibBattery setDoubleValue:[anEvent status] * 100. / 0xc0];
			break;
	}
}

- (void)handleWiimoteAccelerometerEvent:(WKEvent *)anEvent {
	[ibSliderX setDoubleValue:[anEvent absoluteX]];
	[ibSliderY setDoubleValue:[anEvent absoluteY]];
	[ibSliderZ setDoubleValue:[anEvent absoluteZ]];
}

- (void)handleWiimoteEvent:(WKEvent *)anEvent {
	switch ([anEvent type]) {
		case kWKEventIRCamera:
			[self handleWiimoteIREvent:anEvent];
			break;
		case kWKEventButtonUp:
			[self handleWiimoteButton:anEvent down:NO];
			break;
		case kWKEventButtonDown:
			[self handleWiimoteButton:anEvent down:YES];
			break;
		case kWKEventStatusChange:
			[self handleWiimoteStatusEvent:anEvent];
			break;
		case kWKEventAccelerometer:
			[self handleWiimoteAccelerometerEvent:anEvent];
			break;
	}
}

- (void)handleNunchukEvent:(WKEvent *)anEvent {
	NSLog(@"EVENT");
	switch ([anEvent type]) {
		case kWKEventButtonUp:
			//[self handleWiimoteButton:anEvent down:NO];
			NSLog(@"button up");
			break;
		case kWKEventButtonDown:
			[self handleWiimoteButton:anEvent down:YES];
			break;
	}
}

- (void)wiimote:(WiiRemote *)aRemote sendEvent:(WKEvent *)anEvent {
	switch ([anEvent source]) {
		case kWKEventSourceWiiRemote:
			[self handleWiimoteEvent:anEvent];
			break;
		case kWKEventSourceNunchuk:
			[self handleNunchukEvent:anEvent];
			break;
	}
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	[wk_wiimote release];
	wk_wiimote = nil;
}

@end
