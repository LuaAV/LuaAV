//
//  WiiNunchuk.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 15/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import "WiiNunchuk.h"
#import "WiiRemoteInternal.h"

@implementation WiiNunchuk

- (WKExtensionType)type {
	return kWKExtensionNunchuk;
}

- (void)parseStatus:(const uint8_t *)data range:(NSRange)aRange {
	NSParameterAssert(aRange.length >= 6);
	
	const uint8_t *memory = data + aRange.location;
	
	/* C and Z */
	uint8_t buttons = memory[5];
	bool button = (buttons & 0x02) == 0;
	if (wk_wnFlags.c != button) {
		wk_wnFlags.c = button;
		[[self wiiRemote] sendButtonEvent:kWKNunchukButtonC source:kWKEventSourceNunchuk down:button];
	}
	
	button = (buttons & 0x01) == 0;
	if (wk_wnFlags.z != button) {
		wk_wnFlags.z = button;
		[[self wiiRemote] sendButtonEvent:kWKNunchukButtonZ source:kWKEventSourceNunchuk down:button];
	}
	
	/* Joystick */
	uint8_t rawx, rawy;
	rawx = memory[0];
	rawy = memory[1];
	if (rawx != wk_rawX || rawy != wk_rawY) {
		WKJoystickEventData event;
		bzero(&event, sizeof(event));
		event.rawx = rawx;
		event.rawy = rawy;
		
		event.rawdx = event.rawx - wk_rawX;
		event.rawdy = event.rawy - wk_rawY;
		
		if(wk_calib.x.max) {
			event.x = ((CGFloat)event.rawx - wk_calib.x.center) / (wk_calib.x.max - wk_calib.x.min);
			event.dx = event.x - wk_x;
		}
		if(wk_calib.y.max) {
			event.y = ((CGFloat)event.rawy - wk_calib.y.center) / (wk_calib.y.max - wk_calib.y.min);
			event.dy = event.y - wk_y;
		}
		
		wk_x = event.x;
		wk_y = event.y;
		
		wk_rawX = event.rawx;
		wk_rawY = event.rawy;
		
		[[self wiiRemote] sendJoystickEvent:&event source:kWKEventSourceNunchuk subtype:0];
	}
	
	/* Accelerometer */
	WKAccelerometerEventData event;
	bzero(&event, sizeof(event));
	/* position */
	event.rawx = memory[2];
	event.rawy = memory[3];
	event.rawz = memory[4];
	if (event.rawx != wk_acc.rawX || event.rawy != wk_acc.rawY || event.rawz != wk_acc.rawZ) {
		
		/* delta */
		event.rawdx = event.rawx - wk_acc.rawX;
		event.rawdy = event.rawy - wk_acc.rawY;
		event.rawdz = event.rawz - wk_acc.rawZ;		

		/* compute calibrated values */
		if (wk_calib.acc.x0) {
			event.x = ((CGFloat)event.rawx - wk_calib.acc.x0) / SCALE_MAX_GRAVITY (wk_calib.acc.xG - wk_calib.acc.x0);
			event.dx = event.x - wk_acc.x;
		}
		if (wk_calib.acc.y0) {
			event.y = ((CGFloat)event.rawy - wk_calib.acc.y0) / SCALE_MAX_GRAVITY (wk_calib.acc.yG - wk_calib.acc.y0);
			event.dy = event.y - wk_acc.y;
		}
		if (wk_calib.acc.z0) {
			event.z = ((CGFloat)event.rawz - wk_calib.acc.z0) / SCALE_MAX_GRAVITY (wk_calib.acc.zG - wk_calib.acc.z0);
			event.dz = event.z - wk_acc.z;
		}
		
		wk_acc.x = event.x;
		wk_acc.y = event.y;
		wk_acc.z = event.z;
		
		wk_acc.rawX = event.rawx;
		wk_acc.rawY = event.rawy;
		wk_acc.rawZ = event.rawz;
		
		[[self wiiRemote] sendAccelerometerEvent:&event source:kWKEventSourceNunchuk];
	}
}

- (void)parseCalibration:(const uint8_t *)memory length:(size_t)length {
	wk_calib.acc.x0 = memory[0];
	wk_calib.acc.y0 = memory[1];
	wk_calib.acc.z0 = memory[2];
	wk_calib.acc.xG = memory[4];
	wk_calib.acc.yG = memory[5];
	wk_calib.acc.zG = memory[6];
	
	wk_calib.x.max    = memory[8];
	wk_calib.x.min    = memory[9];
	wk_calib.x.center = memory[10];
	wk_calib.y.max    = memory[11];
	wk_calib.y.min    = memory[12];
	wk_calib.y.center = memory[13];
}

#pragma mark Calibration
- (size_t)calibrationLength {
	return 16;
}

- (user_addr_t)calibrationAddress {
	return EXTENSION_REGISTER_CALIBRATION;
}

@end
