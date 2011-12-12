//
//  WiiRemote+Control.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 13/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import "WiiRemoteInternal.h"

#import <WiimoteKit/WKExtension.h>
#import <WiimoteKit/WKConnection.h>

#pragma mark -
@implementation WiiRemote (WKControlLink)

- (CGFloat)battery {
	/* maximum level is 0xc */
	return wk_wiiFlags.battery / (CGFloat)0xc;
}

- (WKExtension *)extension {
	return wk_extension;
}

- (WKLEDState)leds {
	return wk_wiiFlags.leds;
}
- (void)setLeds:(WKLEDState)leds {
	NSLog(@"setting");
	IOReturn err = kIOReturnSuccess;
	leds &= 0xf;
	if (wk_wiiFlags.leds != leds) {
		uint8_t cmd[] = { WKOutputReportLEDs, leds << 4 };
		err = [self sendCommand:cmd length:2];
		if (kIOReturnSuccess == err)
			err = [self refreshStatus]; // will send a led did change notification
	}
	WKPrintIOReturn(err, "setLeds");
}

- (BOOL)isRumbleEnabled {
	return wk_wiiFlags.rumble;
}
- (void)setRumbleEnabled:(BOOL)rumble {
	IOReturn err = kIOReturnSuccess;
	/* canonize bool */
	NSUInteger flag = rumble ? 1 : 0;
	if (flag != wk_wiiFlags.rumble) {
		wk_wiiFlags.rumble = flag;
		/* send a message to apply change */
		err = [self refreshStatus];
	}
	WKPrintIOReturn(err, "setRumbleEnabled");
}


@end
