//
//  WiiRemote+Interrupt.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 13/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import "WiiRemoteInternal.h"

#import <WiimoteKit/WKExtension.h>
#import <WiimoteKit/WKConnection.h>

#define kWiiIRPixelsWidth 1023.0
#define kWiiIRPixelsHeight 767.0
#define WIR_HALFRANGE 512.0
#define WIR_INTERVAL   10.0

@interface WiiRemote (WiiRemoteInputParser)
- (void)parserWriteAck:(const uint8_t *)data length:(size_t)length;
- (void)parseButtons:(const uint8_t *)data length:(size_t)length;
- (void)parseStatus:(const uint8_t *)data length:(size_t)length;
- (void)parseRead:(const uint8_t *)data length:(size_t)length;

/* report */
- (void)parseAccelerometer:(const uint8_t *)data range:(NSRange)range;
- (void)parseIRCamera:(const uint8_t *)data range:(NSRange)range;

@end

@implementation WiiRemote (WiiRemoteInputParser)

- (void)parseButtons:(const uint8_t *)data length:(size_t)length {
	NSParameterAssert(length >= 2);
	WKWiiRemoteButtonsState state = OSReadBigInt16(data, 0) & kWKWiiRemoteButtonsMask;
	if (state != wk_wiiFlags.remoteButtons) {
		NSUInteger idx = 0;
		for (idx = 0; idx < 16; idx++) {
			NSUInteger flag = 1 << idx;
			if (flag & kWKWiiRemoteButtonsMask) {
				/* xor */
				bool before = wk_wiiFlags.remoteButtons & flag;
				bool after = state & flag;
				if ((before && !after) || (!before && after)) {
					[self sendButtonEvent:flag source:kWKEventSourceWiiRemote down:before];
				}
			}
		}
		wk_wiiFlags.remoteButtons = state;
	}
}

- (void)parseStatus:(const uint8_t *)data length:(size_t)length {
	NSParameterAssert(length == 4);
	
	bool plugged = (data[0] & 0x02) != 0;
	if (plugged && !wk_extension) {
		// self initialize extension and notify when we receive it's type.
		[self initializeExtension];
	} else if (!plugged && wk_extension) {
		// unplug extension
		[self setExtensionType:kWKExtensionNone];
	}
	
	bool speaker = (data[0] & 0x04) != 0;
	if (speaker != wk_wiiFlags.speaker) {
		wk_wiiFlags.speaker = speaker;
		[self sendStatusEvent:wk_wiiFlags.speaker source:kWKEventSourceWiiRemote subtype:kWKStatusEventSpeaker];
	}

	bool continuous = (data[0] & 0x08) != 0;
	if (continuous != wk_wiiFlags.continuous) {
		wk_wiiFlags.continuous = continuous;
		WKLog(@"TODO: Continuous status did change");
	}
	
	/* check leds */
	WKLEDState state = (data[0] >> 4) & 0xf;
	if (state != wk_wiiFlags.leds) {
		wk_wiiFlags.leds = state;
		[self sendStatusEvent:wk_wiiFlags.leds source:kWKEventSourceWiiRemote subtype:kWKStatusEventLights];
	}
	
	/* check battery level */
	uint8_t battery = data[3];
	if (battery != wk_wiiFlags.battery) {
		wk_wiiFlags.battery = battery;
		[self sendStatusEvent:wk_wiiFlags.battery source:kWKEventSourceWiiRemote subtype:kWKStatusEventBattery];
	}
}

- (void)didReceiveData:(const uint8_t *)data length:(size_t)length {
	NSAssert(wk_rRequests && CFArrayGetCount(wk_rRequests) > 0, @"inconsistent request queue");
	SEL handler = (SEL)CFArrayGetValueAtIndex(wk_rRequests, 0);
	WKLog (@"did receive data: removing request");
	CFArrayRemoveValueAtIndex(wk_rRequests, 0);
	if (handler)
		(void)objc_msgSend(self, handler, data, length);
	else
		WKLog(@"receive data but does not waiting something");
}

- (void)parseRead:(const uint8_t *)data length:(size_t)length {
	NSParameterAssert(length == 19);
	// user_addr_t addr = OSReadBigInt16(data, 1);
	// const uint8_t *memory = data + 3;
	if(data[0] & 0x08) {
		WKLog(@"Error reading data from Wiimote: Bytes do not exist.");
	} else if(data[0] & 0x07) {
		WKLog(@"Error reading data from Wiimote: Attempt to read from write-only registers.");
	} else if (data[0] & 0x0f) {
		WKLog(@"Error reading data from Wiimote: undefined error %#x", (int)data[0]);
	} else {
		size_t size = (data[0] >> 4) + 1;
//		NSAssert3(wk_wiiFlags.expected <= (size + [wk_buffer length]),
//				 @"receive more data than requested (expected: %i, received:%i, buffer size:%i)",
//				 wk_wiiFlags.expected, size, [wk_buffer length]);
		if (wk_wiiFlags.expected > 16) {
			/* multipart request */
			if (!wk_buffer) wk_buffer = [[NSMutableData alloc] init];
			[wk_buffer appendBytes:data + 3 length:size];
			/* if download completed */
			if ([wk_buffer length] == wk_wiiFlags.expected) {
				[self didReceiveData:[wk_buffer bytes] length:[wk_buffer length]];
				[wk_buffer setLength:0];
				wk_wiiFlags.expected = 0;
			}
		} else {
			[self didReceiveData:data + 3 length:size];
		}
	}
}

- (void)parseAccelerometer:(const uint8_t *)data range:(NSRange)range {
	NSParameterAssert(range.length == 3);
	

	WKAccelerometerEventData event;
	bzero(&event, sizeof(event));
#if defined(NINE_BITS_ACCELEROMETER)
	// cam: added 9th bit of resolution to the wii acceleration
	// see http://www.wiili.org/index.php/Talk:Wiimote#Remaining_button_state_bits
	uint16_t adjust = OSReadBigInt16(data, 0);
	
	data += range.location;
	event.rawx = (data[0] << 2) | (adjust & 0x0060) >> 5;
	event.rawy = (data[1] << 2) | (adjust & 0x2000) >> 12;
	event.rawz = (data[2] << 2) | (adjust & 0x4000) >> 13;
#else
	event.rawx = data[0];
	event.rawy = data[1];
	event.rawz = data[2];	
#endif
	
	if (event.rawx != wk_accState.rawX || event.rawy != wk_accState.rawY || event.rawz != wk_accState.rawZ) {
		/* delta */
		event.rawdx = event.rawx - wk_accState.rawX;
		event.rawdy = event.rawy - wk_accState.rawY;
		event.rawdz = event.rawz - wk_accState.rawZ;

		/* compute calibrated values */
		if (wk_accCalib.x0) {
			event.x = ((CGFloat)event.rawx - wk_accCalib.x0) / SCALE_MAX_GRAVITY(wk_accCalib.xG - wk_accCalib.x0);
			event.dx = event.x - wk_accState.x;
		}
		if (wk_accCalib.y0) {
			event.y = ((CGFloat)event.rawy - wk_accCalib.y0) / SCALE_MAX_GRAVITY(wk_accCalib.yG - wk_accCalib.y0);
			event.dy = event.y - wk_accState.y;
		}
		if (wk_accCalib.z0) {
			event.z = ((CGFloat)event.rawz - wk_accCalib.z0) / SCALE_MAX_GRAVITY(wk_accCalib.zG - wk_accCalib.z0);
			event.dz = event.z - wk_accState.z;
		}
		
		wk_accState.x = event.x;
		wk_accState.y = event.y;
		wk_accState.z = event.z;
		
		wk_accState.rawX = event.rawx;
		wk_accState.rawY = event.rawy;
		wk_accState.rawZ = event.rawz;
		
		/* compute orientation for IR */
		wk_accState.lowZ = wk_accState.lowZ * 0.9 + wk_accState.rawZ * 0.1;
		wk_accState.lowX = wk_accState.lowX * 0.9 + wk_accState.rawX * 0.1;
		
		float absz = fabsf (wk_accState.lowZ - WIR_HALFRANGE);
		float absx = fabsf (wk_accState.lowX - WIR_HALFRANGE);
		
		if (wk_wiiFlags.orientation == 0 || wk_wiiFlags.orientation == 2) absx -= WIR_INTERVAL;
		if (wk_wiiFlags.orientation == 1 || wk_wiiFlags.orientation == 3) absz -= WIR_INTERVAL;
		
		if (absz >= absx) {
			if (absz > WIR_INTERVAL)
				wk_wiiFlags.orientation = (wk_accState.lowZ > WIR_HALFRANGE) ? 0 : 2;
		} else {
			if (absx > WIR_INTERVAL)
				wk_wiiFlags.orientation = (wk_accState.lowX > WIR_HALFRANGE) ? 3 : 1;
		}

		//WKLog (@"orientation = %i (absx: %0.2f - absz: %0.2f - lx: %0.2f - lz: %0.2f)", wk_wiiFlags.orientation, absx, absz, wk_accState.lowX, wk_accState.lowZ);

		// only send event if calibrated values have been received.
		if (wk_accCalib.x0 && wk_accCalib.y0 && wk_accCalib.z0)
			[self sendAccelerometerEvent:&event source:kWKEventSourceWiiRemote];
	}
}

- (void)parseIRCamera:(const uint8_t *)data range:(NSRange)range {
	WKIREventData event;
	bzero(&event, sizeof(event));
	
	const uint8_t *irdata = data + range.location;
	
	switch(range.length) {
		case 10:
			event.points[0].rawx = irdata[0] | ((irdata[2] >> 4) & 0x03) << 8;
			event.points[0].rawy = irdata[1] | ((irdata[2] >> 6) & 0x03) << 8;
			
			event.points[1].rawx = irdata[3] | ((irdata[2] >> 0) & 0x03) << 8;
			event.points[1].rawy = irdata[4] | ((irdata[2] >> 2) & 0x03) << 8;
			
			event.points[0].size = 0x07;
			event.points[1].size = 0x07;
			
			/* to improve */
			event.points[0].exists = !(irdata[0] == 0xff && irdata[1] == 0xff);
			event.points[1].exists = !(irdata[3] == 0xff && irdata[4] == 0xff);
			break;
		case 12:
		{
			NSUInteger idx = 0;
			for (idx = 0; idx < 4; idx++) {
				const uint8_t *pdata = irdata + (idx * 3);
				if (pdata[0] != 0xff || pdata[1] != 0xff || pdata[2] != 0xff) {
					event.points[idx].exists = true;
					event.points[idx].size = pdata[2] & 0x0f;
					event.points[idx].rawx = pdata[0] | ((pdata[2] >> 4) & 0x03) << 8;
					event.points[idx].rawy = pdata[1] | ((pdata[2] >> 6) & 0x03) << 8;
				} else {
					event.points[idx].exists = false;
				}
			}
		}
			break;
		default:
			WKLog(@"Unsupported IR Camera mode: %u bytes report", range.length);
			return;
	}
	
	/* save IR sate in Wiimote */
	NSUInteger idx = 0;
	for (idx = 0; idx < 4; idx++) {
		wk_irPoints[idx].exists = event.points[idx].exists ? 1 : 0;
		wk_irPoints[idx].size = event.points[idx].size;
		wk_irPoints[idx].rawX = event.points[idx].rawx;
		wk_irPoints[idx].rawY = event.points[idx].rawy;
	}

	/* compute wiimote's left point index */
	int p1 = -1;
	int p2 = -1;
	// we should modify this loop to take the points with the lowest s (the brightest ones)
	for (idx=0 ; idx<4 ; idx++) {
		if (p1 == -1) {
			if (wk_irPoints[idx].exists)
				p1 = idx;
		} else {
			if (wk_irPoints[idx].exists) {
				p2 = idx;
				break;
			}
		}
	}
	
	//	NSLogDebug (@"p1=%i ; p2=%i", p1, p2);

	double ox, oy;
	if ((p1 > -1) && (p2 > -1)) {
		int l = wk_wiiFlags.leftidx;
		if (wk_wiiFlags.leftidx == -1) {
			switch (wk_wiiFlags.orientation) {
				case 0: l = (wk_irPoints[p1].rawX < wk_irPoints[p2].rawX) ? p1 : p2; break;
				case 1: l = (wk_irPoints[p1].rawY > wk_irPoints[p2].rawY) ? p1 : p2; break;
				case 2: l = (wk_irPoints[p1].rawX > wk_irPoints[p2].rawX) ? p1 : p2; break;
				case 3: l = (wk_irPoints[p1].rawY < wk_irPoints[p2].rawY) ? p1 : p2; break;
			}
			
			wk_wiiFlags.leftidx = l;
		}
		
		int r = (l == p1) ? p2 : p1;

		double dx = wk_irPoints[r].rawX - wk_irPoints[l].rawX;
		double dy = wk_irPoints[r].rawY - wk_irPoints[l].rawY;
		double d  = hypot (dx, dy);
		
		dx /= d;
		dy /= d;
		
		double cx = (wk_irPoints[l].rawX + wk_irPoints[r].rawX)/kWiiIRPixelsWidth  - 1.0;
		double cy = (wk_irPoints[l].rawY + wk_irPoints[r].rawY)/kWiiIRPixelsHeight - 1.0;
		
		ox = -dy*cy - dx*cx;
		oy = -dx*cy + dy*cx;

		// cam:
		// Compensate for distance. There must be fewer than 0.75*768 pixels between the spots for this to work.
		// In other words, you have to be far enough away from the sensor bar for the two spots to have enough
		// space on the image sensor to travel without one of the points going off the image.
		double gain = 4;
		if (d < (0.75 * kWiiIRPixelsHeight)) 
			gain = 1 / (1 - d/kWiiIRPixelsHeight);
		
		ox *= gain;
		oy *= gain;		
		//		NSLog(@"x:%5.2f;  y: %5.2f;  angle: %5.1f\n", ox, oy, angle*180/M_PI);
	} else {
		ox = oy = -100;
		if (wk_wiiFlags.leftidx != -1) {
			//	printf("Not tracking.\n");
			wk_wiiFlags.leftidx = -1;
		}
	}
	
	event.x = ox;
	event.y = oy;
//	WKLog (@"left idx  = %i", wk_wiiFlags.leftidx);
//	WKLog (@"x: %0.2f - y: %0.2f", ox, oy);
	
	/* send event */
	[self sendIREvent:&event source:kWKEventSourceWiiRemote];
}

- (void)parserWriteAck:(const uint8_t *)data length:(size_t)length {
	NSParameterAssert(length == 2);
	[self didReceiveAck:data];
}

- (void)parseInterleavedLow:(const uint8_t *)data range:(NSRange)range {
	NSParameterAssert(range.length == 19);
	
	wk_interleaved[0] = data[range.location];
	
	/* extract Z bytes from button unused bits */
	wk_interleaved[2] = (data[0] & 0x60) >> 1;
	wk_interleaved[2] |= (data[1] & 0x60) << 1;
	
	/* copy low bytes of ir sensor */
	memcpy(wk_interleaved + 3, data + 1, 18);
}

- (void)parseInterleavedHigh:(const uint8_t *)data range:(NSRange)range {
	NSParameterAssert(range.length == 19);
	
	wk_interleaved[1] = data[range.location];
	
	/* extract Z bytes from button unused bits */
	wk_interleaved[2] |= (data[0] & 0x60) >> 5;
	wk_interleaved[2] |= (data[1] & 0x60) >> 3;
	
	/* copy high bytes of ir sensor */
	memcpy(wk_interleaved + 3 + 18, data + 1, 18);
	
	/* data completed */
	[self parseAccelerometer:wk_interleaved range:NSMakeRange(0, 3)];
	[self parseIRCamera:wk_interleaved + 3 range:NSMakeRange(0, 36)];
}

- (void)connection:(WKConnection *)aConnection didReceiveData:(uint8_t *)data length:(size_t)length {
	//NSLog(@"%@: %@", NSStringFromSelector(_cmd), [NSData dataWithBytesNoCopy:data length:length freeWhenDone:NO]);
	// printf("-\n");
	
	wk_wiiFlags.inTransaction = 1;
	NSUInteger type = data[0];
	
	length -= 1;
	data = data + 1;
	
	NSUInteger offset = 0;
	if (type != kWKInputReportLongExtension) {
		/* button state seems to be include in almost all reports */
		[self parseButtons:data length:length];
		offset = 2;
	}
	
	/* end of packet reached */
	if (length == 0) return;
	
	@try {
		switch (type) {
			case kWKInputReportStatus:
				[self parseStatus:data + offset length:length - offset];
				break;
			case kWKInputReportReadData:
				[self parseRead:data + offset length:length - offset];
				break;
			case kWKInputReportWriteData:
				[self parserWriteAck:data + offset length:length - offset];
				break;
				
				/* reports */
			case kWKInputReportDefault:
				// should never append
				break;
			case kWKInputReportAccelerometer:
				[self parseAccelerometer:data range:NSMakeRange(offset, 3)];
				break;
			case kWKInputReportShortExtension:
				[[self extension] parseStatus:data range:NSMakeRange(offset, 9)];
				break;
			case kWKInputReportAccelerometerIR:
				[self parseAccelerometer:data range:NSMakeRange(offset, 3)];
				[self parseIRCamera:data range:NSMakeRange(offset + 3, 12)];
				break;
			case kWKInputReportExtension:
				[[self extension] parseStatus:data range:NSMakeRange(offset, 19)];
				break;
			case kWKInputReportAccelerometerExtension:
				[self parseAccelerometer:data range:NSMakeRange(offset, 3)];
				[[self extension] parseStatus:data range:NSMakeRange(offset + 3, 16)];
				break;
			case kWKInputReportIRExtension:
				[self parseIRCamera:data range:NSMakeRange(offset, 10)];
				[[self extension] parseStatus:data range:NSMakeRange(offset + 10, 9)];
				break;
			case kWKInputReportAll:
				[self parseAccelerometer:data range:NSMakeRange(offset, 3)];
				[self parseIRCamera:data range:NSMakeRange(offset + 3, 10)];
				[[self extension] parseStatus:data range:NSMakeRange(offset + 13, 6)];
				break;
			case kWKInputReportLongExtension:
				[[self extension] parseStatus:data range:NSMakeRange(offset, 21)];
				break;
				/* interleaved */
			case kWKInputReportAllInterleavedLow:
				[self parseInterleavedLow:data range:NSMakeRange(offset, 19)];
				break;
			case kWKInputReportAllInterleavedHigh:
				[self parseInterleavedHigh:data range:NSMakeRange(offset, 19)];
				break;
			default:
				WKLog(@"Unsupported input report: %#lx", (long)type);
		}
	} @catch (NSException *exception) {
		WKLog(@"IO Exception: %@", exception);
	}
	
	wk_wiiFlags.inTransaction = 0;

	if ([wk_delegate respondsToSelector:@selector(wiimoteDidInterpretReport:)])
		[wk_delegate wiimoteDidInterpretReport:self];
	
	/* process pending request */
	if (wk_wiiFlags.status) {
		wk_wiiFlags.status = 0;
		[self refreshStatus];
	}
	if (wk_wiiFlags.reportMode) {
		wk_wiiFlags.reportMode = 0;
		[self refreshReportMode];
	}
}

@end
