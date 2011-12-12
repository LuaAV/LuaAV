//
//  WiiRemoteInternal.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 16/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import "WiiRemoteInternal.h"

#import <WiimoteKit/WKExtension.h>
#import <WiimoteKit/WKConnection.h>

/* 
 address format: ffaaaaaa.
 ff: flags. 0x4 mean "access register" and is vital if you do not want to read or write anywhere in the wiimote eeprom.
 aaaaaa: virtual address.
 */
static __inline__
user_addr_t __WiiRemoteTranslateAddress(user_addr_t address, WKAddressSpace space) {
	switch (space) {
		case kWKMemorySpaceExtension:
			return address + 0x04a40000;
			break;
		case kWKMemorySpaceSpeaker:
			return address + 0x04a20000;
			break;
		case kWKMemorySpaceIRCamera:
			return address + 0x04b00000;
			break;
		case kWKMemorySpaceWiiRemote:
			return address;
		default:
			@throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"invalid address space" userInfo:nil];
	}
}

@implementation WiiRemote (WiiRemoteInternal)

- (IOReturn)sendCommand:(const uint8_t *)cmd length:(size_t)length {
	NSParameterAssert(length <= 22);
	// cam: it is not safe to send data to the wiimote as long as the control channel
	// is not opened.  This happens sometimes when a wiimote is connected with an
	// extension already plugged-in.
	// don't forbid the execution of this method, the command will be queued anyway.
//	NSAssert (wk_wiiFlags.connected, @"cannot send data with a disconnected wiimote");
	
	uint8_t buffer[23];
	bzero(buffer, 23);
	
	buffer[0] = 0x52; /* magic number (HID spec) */
	memcpy(buffer + 1, cmd, length);
	if ([self isRumbleEnabled]) buffer[2] |= 1; // rumble bit must be set in all control report.

	WKLog(@"=> send command %#x (cmd[1]: %#x,  cmd[2]: %#x)", cmd[0], length > 1 ? cmd[1] : 0, length > 2 ? cmd[2] : 0);
	return [[self connection] sendData:buffer length:(length + 1) context:nil];
}

- (IOReturn)readDataAtAddress:(user_addr_t)address space:(WKAddressSpace)space length:(size_t)length handler:(SEL)handler {
	uint8_t buffer[7];
	bzero(buffer, 7);
	
	buffer[0] = WKOutputReportReadMemory;
	/* write address and size in big endian */
	OSWriteBigInt32(buffer, 1, __WiiRemoteTranslateAddress(address, space));
	OSWriteBigInt16(buffer, 5, length);
	
	IOReturn err = [self sendCommand:buffer length:sizeof(buffer)];
	if (kIOReturnSuccess == err) {
		// set the length of expect data to be received
		wk_wiiFlags.expected = length;

		// null array callback is what we need for integer array
		if (!wk_rRequests) wk_rRequests = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
		CFArrayAppendValue(wk_rRequests, handler);
		WKLog (@"read request queued");
	}
	return err;
}

- (IOReturn)writeData:(const uint8_t *)data length:(size_t)length atAddress:(user_addr_t)address space:(WKAddressSpace)space next:(SEL)next {
	NSParameterAssert(length <= 16);
	
	uint8_t buffer[22];
	bzero(buffer, 22);
	buffer[0] = WKOutputReportWriteMemory;
	
	user_addr_t vmaddr = __WiiRemoteTranslateAddress(address, space);
	
#if !defined(WII_ALLOW_UNSAFE_WRITE)
	if (space == kWKMemorySpaceWiiRemote && (address < 0x0fca || address > 0x15a9))
		@throw [NSException exceptionWithName:NSInvalidArgumentException
																	 reason:@"Trying to write data in an unsafe place" userInfo:nil];
#endif
	
	OSWriteBigInt32(buffer, 1, vmaddr);
	buffer[5] = length;
	/* append data */
	memcpy(buffer + 6, data, length);
	
	IOReturn err = [self sendCommand:buffer length:22];
	if (kIOReturnSuccess == err) {
		// null array callback is what we need for integer array
		if (!wk_wRequests) wk_wRequests = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
		CFArrayAppendValue(wk_wRequests, next);
	}
	return err;
}

#pragma mark Extension

- (IOReturn)initializeExtension {
	if (!wk_wiiFlags.extInit) {
		wk_wiiFlags.extInit = 1;
		return [self writeData:(const uint8_t []){ 0x55 } length:1 atAddress:EXTENSION_REGISTER_STATUS
	space:kWKMemorySpaceExtension next:@selector(didInitializeExtension0:)];
	}
	return kIOReturnSuccess;
}

- (void)didInitializeExtension0:(NSUInteger)status {
	[self writeData:(const uint8_t []){ 0x00 } length:1 atAddress:EXTENSION_REGISTER_STATUS2 
space:kWKMemorySpaceExtension next:@selector(didInitializeExtension:)];
}

- (void)didInitializeExtension:(NSUInteger)status {
	// don't know what the param status contains (see 0x22 report for details) ?
	
	// read expansion device type
	[self readDataAtAddress:EXTENSION_REGISTER_TYPE space:kWKMemorySpaceExtension length:8 handler:@selector(handleExtensionType:length:)];
}

- (void)handleExtensionType:(const uint8_t *)data length:(size_t)length {
	NSParameterAssert(length == 8);
	uint16_t wiitype0 =  OSReadBigInt(data, 0) >> 16;
	uint32_t wiitype1 =  OSReadBigInt32(data, 2);
	WKExtensionType type = kWKExtensionNone;
	/* set extension type */
	switch (wiitype0) {
		case EXTENSION_IDENTIFIER_TYPE0:
			switch (wiitype1) {
				case EXTENSION_IDENTIFIER_PARTIAL:
					WKLog(@"TODO: report extension error: unplug and try again");
					// maybe we should retry
					break;
				case EXTENSION_IDENTIFIER_NUNCHUK:
					type = kWKExtensionNunchuk;
					break;
				case EXTENSION_IDENTIFIER_CLASSIC:
					type = kWKExtensionClassicController;
					break;
				default:
					WKLog(@"TODO: report unsupported extension");
					break;
			}
			break;
	}
	[self setExtensionType:type];
	wk_wiiFlags.extInit = 0;
}

- (void)setExtension:(WKExtension *)anExtension {
	if (anExtension != wk_extension) {
		if (wk_extension) {
			[wk_extension setWiiRemote:nil];
			[wk_extension release];
		}
		wk_extension = [anExtension retain];
		[wk_extension setWiiRemote:self];
	}
}

- (void)setExtensionType:(WKExtensionType)type {
	if ([wk_extension type] != type) {
		if (type != kWKExtensionNone) {
			[self setExtension:[WKExtension extensionWithType:type]];
		} else
			[self setExtension:nil];
		
		if ([self irMode] != kWKIRModeOff) {
			[self setIrMode:exp != kWKExtensionNone ? kWKIRModeBasic : kWKIRModeExtended];
		}
		
		[self refreshReportMode];
		[self sendStatusEvent:type source:kWKEventSourceWiiRemote subtype:kWKStatusEventExtention];
	}
}

- (void)didReceiveAck:(const uint8_t *)ack {
	switch (ack[0]) {
		case WKOutputReportMode:
			WKLog(@"did change report mode: %#hhx", ack[1]);
			break;		
		case WKOutputReportWriteMemory: {
			NSAssert(wk_wRequests && CFArrayGetCount(wk_wRequests) > 0, @"Inconsistent write requests queue");
			SEL action = (SEL)CFArrayGetValueAtIndex(wk_wRequests, 0);
			CFArrayRemoveValueAtIndex(wk_wRequests, 0);
			// do not want to bother with NSInvocation, so just call the message directly.
			if (action) 
				(void)objc_msgSend(self, action, (NSUInteger)ack[1]);
		}
			break;
		default:
			WKLog(@"did do something but don't know what: %#hx", ack);
			break;
	}
}


#pragma mark Refresh
- (IOReturn) refreshStatus {
	if (wk_wiiFlags.inTransaction) {
		wk_wiiFlags.status = 1;
		return kIOReturnSuccess;
	} else {
		uint8_t cmd[] = {WKOutputReportStatus, 0x00};
		return [self sendCommand:cmd length:2];
	}
}

- (IOReturn)refreshReportMode {
	IOReturn err = kIOReturnSuccess;
	if (wk_wiiFlags.inTransaction) {
		wk_wiiFlags.reportMode = 1;
	} else {
		/* update report mode */
		uint8_t cmd[] = {WKOutputReportMode, 0x02, kWKInputReportDefault}; // Just buttons.
		
		if (wk_wiiFlags.continuous) cmd[1] = 0x04;
		
		/* wiimote accelerometer */
		if (wk_wiiFlags.accelerometer) {
			if (wk_extension && wk_wiiFlags.extension) {
				if (wk_irMode != kWKIRModeOff && [wk_extension acceptsIRCameraEvents]) {
					cmd[2] = kWKInputReportAll;
				} else { // extension but no ir
					cmd[2] = kWKInputReportAccelerometerExtension;
				}
			} else { // no extension
				if (wk_irMode != kWKIRModeOff) { // accel + ir
					cmd[2] = kWKInputReportAccelerometerIR;
				} else { // only accelerometers
					cmd[2] = kWKInputReportAccelerometer;
				}				
			}
		} else if (wk_irMode != kWKIRModeOff) {
			if (wk_extension && wk_wiiFlags.extension) {
				if ([wk_extension acceptsIRCameraEvents])
					cmd[2] = kWKInputReportIRExtension;
				else
					cmd[2] = kWKInputReportExtension;
			} else {
				/* IR only does not exists (and does not have sense) */
				cmd[2] = kWKInputReportAccelerometerIR;
			}
		} else if (wk_extension && wk_wiiFlags.extension) {
			cmd[2] = kWKInputReportExtension;
		}
		err = [self sendCommand:cmd length:3];
	}
	return err;
}

- (IOReturn)refreshCalibration {
	return [self readDataAtAddress:0x16 space:kWKMemorySpaceWiiRemote length:8 handler:@selector(handleCalibration:length:)];
}
- (void)handleCalibration:(const uint8_t *)data length:(size_t)length {
#if defined(NINE_BITS_ACCELEROMETER)
	wk_accCalib.x0 = (data[0] << 2);
	wk_accCalib.y0 = (data[1] << 2);
	wk_accCalib.z0 = (data[2] << 2);
	wk_accCalib.xG = (data[4] << 2);
	wk_accCalib.yG = (data[5] << 2);
	wk_accCalib.zG = (data[6] << 2);
#else
	wk_accCalib.x0 = data[0];
	wk_accCalib.y0 = data[1];
	wk_accCalib.z0 = data[2];
	wk_accCalib.xG = data[4];
	wk_accCalib.yG = data[5];
	wk_accCalib.zG = data[6];
#endif	
}

- (IOReturn)refreshExtensionCalibration {
	size_t length = [[self extension] calibrationLength];
	user_addr_t addr = [[self extension] calibrationAddress];
	if (addr == 0 || length == 0)
		return kIOReturnSuccess;
	
	return [self readDataAtAddress:addr space:kWKMemorySpaceExtension length:length handler:@selector(handleExtensionCalibration:length:)];
}

- (void)handleExtensionCalibration:(const uint8_t *)data length:(size_t)length {
	return [[self extension] parseCalibration:data length:length];
}

#pragma mark IR Camera
- (void)__stopIRCamera {
	wk_wiiFlags.irInit = 0;
	wk_wiiFlags.irAbort = 0;
	[self sendCommand:(const uint8_t[]){ WKOutputReportIRCamera, 0x00 } length:2];
	[self sendCommand:(const uint8_t[]){ WKOutputReportIRCamera2, 0x00 } length:2];
	[self refreshReportMode];
}

- (WKIRMode)irMode {
	return wk_irMode;
}
- (IOReturn)setIrMode:(WKIRMode)aMode {
	if (aMode != wk_irMode) {
		if (aMode == kWKIRModeOff) {
			if (wk_wiiFlags.irInit) {
				wk_wiiFlags.irAbort = 1;
			} else {
				wk_irMode = aMode;
				[self __stopIRCamera];
			}
		} else {
			/* if was previously off, we have to start it */
			if (wk_irMode == kWKIRModeOff) {
				if (!wk_wiiFlags.irInit) {
					wk_wiiFlags.irInit = 1;
					/* start ir sensor */
					[self sendCommand:(const uint8_t[]){ WKOutputReportIRCamera, 0x04 } length:2];
					[self sendCommand:(const uint8_t[]){ WKOutputReportIRCamera2, 0x04 } length:2];
					[self writeData:(const uint8_t[]){ 0x01 } length:1 atAddress:IR_REGISTER_STATUS 
										space:kWKMemorySpaceIRCamera next:@selector(didStartIRCamera:)];
					/* following operation are done in the data send callback */
				}
				wk_irMode = aMode;
			}  else {
				/* set mode */
				[self writeData:(const uint8_t[]){ aMode } length:1 atAddress:IR_REGISTER_MODE space:kWKMemorySpaceIRCamera next:nil];
				wk_irMode = aMode;
				[self refreshReportMode];
			}
		}
	}
	return kIOReturnSuccess;
}


- (void)didStartIRCamera:(NSUInteger)status {
	if (wk_wiiFlags.irAbort) {
		[self __stopIRCamera];
	} else {
		[self writeData:(const uint8_t[]){ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0xc0 } length:9 
					atAddress:IR_REGISTER_SENSITIVITY_1  space:kWKMemorySpaceIRCamera next:@selector(didSetIRCameraSensibilityOne:)];	
	}
}
- (void)didSetIRCameraSensibilityOne:(NSUInteger)status {
	if (wk_wiiFlags.irAbort) {
		[self __stopIRCamera];
	} else {
		[self writeData:(const uint8_t[]){ 0x40, 0x00 } length:2 atAddress:IR_REGISTER_SENSITIVITY_2  
							space:kWKMemorySpaceIRCamera next:@selector(didSetIRCameraSensibilityTwo:)];
	}
}
- (void)didSetIRCameraSensibilityTwo:(NSUInteger)status {
	if (wk_wiiFlags.irAbort) {
		[self __stopIRCamera];
	} else {
		[self writeData:(const uint8_t[]){ 0x08 } length:1 atAddress:IR_REGISTER_STATUS
							space:kWKMemorySpaceIRCamera next:@selector(didSetIRCameraStatus:)];
	}
}
- (void)didSetIRCameraStatus:(NSUInteger)status {
	if (wk_wiiFlags.irAbort) {
		[self __stopIRCamera];
	} else {
		[self writeData:(const uint8_t[]){ wk_irMode } length:1 atAddress:IR_REGISTER_MODE
							space:kWKMemorySpaceIRCamera next:nil];
		[self refreshReportMode];
		wk_wiiFlags.irInit = 0;
	}
}


@end

@implementation WiiRemote (WiiRemoteEvent)

- (void)sendEvent:(WKEvent *)theEvent {
	if ([wk_delegate respondsToSelector:@selector(wiimote:sendEvent:)])
		[wk_delegate wiimote:self sendEvent:theEvent];
}

- (void)sendStatusEvent:(NSUInteger)value source:(WKEventSource)source subtype:(WKEventSubtype)subtype {
	WKEvent *event = [WKEvent eventWithType:kWKEventStatusChange wiimote:self source:source];
	[event setSubtype:subtype];
	[event setStatus:value];
	[self sendEvent:event];
}

- (void)sendIREvent:(WKIREventData *)data source:(WKEventSource)source {
	WKEvent *event = [WKEvent eventWithType:kWKEventIRCamera wiimote:self source:source];
	WKIRPoint *points[4];
	bzero(points, sizeof(points));
	NSUInteger idx = 0;
	for (idx = 0; idx < 4; idx++) {
		if (data->points[idx].exists) {
			points[idx] = [[WKIRPoint alloc] initWithSize:data->points[idx].size 
												absoluteX:data->points[idx].rawx
												absoluteY:data->points[idx].rawy];
			[points[idx] autorelease];
		}
	}
	[event setPoints:points count:4];
	[event setX:data->x];
	[event setY:data->y];
	[self sendEvent:event];
}

- (void)sendAnalogEvent:(WKAnalogEventData *)data source:(WKEventSource)source subtype:(WKEventSubtype)subtype {
	WKEvent *event = [WKEvent eventWithType:kWKEventAnalogButtonChange wiimote:self source:source];
	if (subtype) [event setSubtype:subtype];
	
	[event setX:data->x];
	[event setDeltaX:data->dx];
	
	[event setAbsoluteX:data->rawx];
	[event setAbsoluteDeltaX:data->rawdx];
	
	[self sendEvent:event];	
}

- (void)sendJoystickEvent:(WKJoystickEventData *)data source:(WKEventSource)source subtype:(WKEventSubtype)subtype {
	WKEvent *event = [WKEvent eventWithType:kWKEventJoystickMove wiimote:self source:source];
	if (subtype) [event setSubtype:subtype];

	[event setX:data->x];
	[event setY:data->y];
	
	[event setDeltaX:data->dx];
	[event setDeltaY:data->dy];

	[event setAbsoluteX:data->rawx];
	[event setAbsoluteY:data->rawy];
	
	[event setAbsoluteDeltaX:data->rawdx];
	[event setAbsoluteDeltaY:data->rawdy];

	[self sendEvent:event];	
}

- (void)sendAccelerometerEvent:(WKAccelerometerEventData *)data source:(WKEventSource)source {
	WKEvent *event = [WKEvent eventWithType:kWKEventAccelerometer wiimote:self source:source];

	[event setX:data->x];
	[event setY:data->y];
	[event setZ:data->z];
	
	[event setDeltaX:data->dx];
	[event setDeltaY:data->dy];
	[event setDeltaZ:data->dz];	
	
	[event setAbsoluteX:data->rawx];
	[event setAbsoluteY:data->rawy];
	[event setAbsoluteZ:data->rawz];
	
	[event setAbsoluteDeltaX:data->rawdx];
	[event setAbsoluteDeltaY:data->rawdy];
	[event setAbsoluteDeltaZ:data->rawdz];	
	
	[self sendEvent:event];	
}

- (void)sendButtonEvent:(NSUInteger)button source:(WKEventSource)source down:(BOOL)isButtonDown {
	WKEvent *event = [WKEvent eventWithType:isButtonDown ? kWKEventButtonDown : kWKEventButtonUp wiimote:self source:source];
	[event setButton:button];
	[self sendEvent:event];
}

@end

