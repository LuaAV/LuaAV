//
//  Wiimote.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WiiRemote.h>

#import "WiiRemoteInternal.h"
#import <WiimoteKit/WKExtension.h>
#import <WiimoteKit/WKConnection.h>

#import <IOBluetooth/objc/IOBluetoothDevice.h>

@implementation WiiRemote

- (id)initWithDevice:(IOBluetoothDevice *)aDevice {
	if (self = [super init]) {
		wk_wiiFlags.leftidx = -1;
		wk_connection = [[WKConnection alloc] initWithDevice:aDevice];
		if (!wk_connection) {
			while (kIOReturnBusy == [aDevice closeConnection]) {
				NSLog(@"device is busy");
				[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:1]];
			}
			
			[self release];
			self = nil;
		} else {
			/* default value */
			wk_wiiFlags.leds = 0x1f;
			wk_wiiFlags.extension = 1; // listen extension by default.
			[wk_connection setDelegate:self];
		}
	}
	return self;
}

- (void)dealloc {
	[wk_extension setWiiRemote:nil];
	[wk_extension release];
	
	[wk_connection setDelegate:nil];
	[wk_connection close];
	[wk_connection release];
	
	if (wk_rRequests) CFRelease(wk_rRequests);
	if (wk_wRequests) CFRelease(wk_wRequests);
	[wk_buffer release];
	[super dealloc];
}

- (NSString *)description {
	return [NSString stringWithFormat:@"<%@ %p> { %@: %@ }", 
					[self class], self, [[wk_connection device] getName], [self address]];
}

#pragma mark -
- (id)delegate {
	return wk_delegate;
}
- (void)setDelegate:(id)aDelegate {
	wk_delegate = aDelegate;
}

- (NSString *)address {
	return [[wk_connection device] getAddressString];
}

- (IOReturn)connect {
	return [wk_connection connect];
}

- (BOOL)isConnected {
	return wk_connection != nil && wk_wiiFlags.connected;
}
- (WKConnection *)connection {
	return wk_connection;
}

- (void)connectionDidOpen:(WKConnection *)aConnection {
	wk_wiiFlags.connected = 1;
	/* turn off all leds */
	//[self setLeds:0];

	/* get status */
	// status is already asked when setting the leds.
	[self refreshStatus];
	
	/* request wiimote calibration */
	[self refreshCalibration];
	
	if ([wk_delegate respondsToSelector:@selector(wiimoteDidConnect:)])
		[wk_delegate wiimoteDidConnect:self];
}

-(void)connectionDidClose:(WKConnection *)aConnection {
	wk_wiiFlags.connected = 0;
	if ([wk_delegate respondsToSelector:@selector(wiimoteDidDisconnect:)])
		[wk_delegate wiimoteDidDisconnect:self];
}

- (BOOL)isContinuous {
	return wk_wiiFlags.continuous;
}
- (void)setContinuous:(BOOL)value {
	bool flag = value ? 1 : 0;
	if (wk_wiiFlags.continuous != flag) {
		wk_wiiFlags.continuous = flag;
		[self refreshReportMode];
	}
}

- (BOOL)acceptsIRCameraEvents {
	return [self irMode] != kWKIRModeOff;
}

- (void)setAcceptsIRCameraEvents:(BOOL)flag {
	IOReturn err = kIOReturnSuccess;

	// Don't set the IR mode if the extension doesn't accepts it
	if ([self extension] && ![[self extension] acceptsIRCameraEvents]) {
		// notify the inconsistency (IR mode is already set)
		if ([self irMode] != kWKIRModeOff) {
			WKLog (@"*** warning: ir mode already set, but extension forbids use of IR sensor");
			flag = kWKIRModeOff;
		} else
			return;
	}

	if (!flag && wk_irMode != kWKIRModeOff) {
		err = [self setIrMode:kWKIRModeOff];
	} else if (flag && wk_irMode == kWKIRModeOff) {
		if (!wk_extension)
			err = [self setIrMode:kWKIRModeExtended];
		else
			err = [self setIrMode:kWKIRModeBasic];
	}
	WKPrintIOReturn(err, "setAcceptsIRCameraEvents");
}

- (BOOL)acceptsExtensionEvents {
	return wk_wiiFlags.extension;
}
- (void)setAcceptsExtensionEvents:(BOOL)accept {
	bool flag = accept ? 1 : 0;
	if (wk_wiiFlags.extension != flag) {
		wk_wiiFlags.extension = flag;
		[self refreshReportMode];
	}
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

- (BOOL)acceptsAccelerometerEvents {
	return wk_wiiFlags.accelerometer;
}
- (void)setAcceptsAccelerometerEvents:(BOOL)accept {
	// Don't turn on the accelerometer if the extension doesn't accepts it
	if ([self extension] && ![[self extension] acceptsAccelerometerEvents]) {
		// notify the inconsistency (IR mode is already set)
		if (wk_wiiFlags.accelerometer) {
			WKLog (@"*** warning: accelerometer already turned on, but extension forbids use of accelerometer");
			wk_wiiFlags.accelerometer = NO;
		} else
			return;
	}
	
	bool flag = accept ? 1 : 0;
	if (wk_wiiFlags.accelerometer != flag) {
		wk_wiiFlags.accelerometer = flag;
		[self refreshReportMode];
	}
}

@end
