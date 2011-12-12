//
//  WKConnection.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WKConnection.h>

#import "WKBase.h"
#import <WiimoteKit/WKTypes.h>

#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothL2CAPChannel.h>

enum {
	kWKConnectionStatusClosed,
	kWKConnectionStatusOpening,
	kWKConnectionStatusOpened,
	kWKConnectionStatusError,
};

@interface _WKRequest : NSObject {
@private
	void *wk_ctxt;
	NSData *wk_data;
	NSUInteger wk_retry;
}

+ (_WKRequest *)requestWithBytes:(const uint8_t *)bytes length:(size_t)length context:(void *)ctxt;

- (const uint8_t *)bytes;
- (size_t)length;
- (void *)context;

- (NSUInteger)retry;
- (void)incrementRetry;

@end

@interface WKConnection (WKConnectionInternal)
- (IOReturn)cleanup;
@end

@implementation WKConnection

//static NSMutableArray *sWKConnections = nil;
//+ (void)initialize {
//	if ([WKConnection class] == self) {
//		sWKConnections = [[NSMutableArray alloc] init];
//	}
//}

- (id)initWithDevice:(IOBluetoothDevice *)aDevice {
	if ([aDevice isConnected]) {
		WKLog(@"already connected");
		[self release];
		return nil;
	}
	if (self = [super init]) {
		wk_device = [aDevice retain];
		wk_backup = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc {
	[self cleanup];
	[wk_device release];
	[wk_backup release];
	[wk_pending release];
	[super dealloc];
}

#pragma mark -
- (IOReturn)cleanup {
	if (wk_control) {
		[wk_control setDelegate:nil];
		[wk_control closeChannel];
		[wk_control release];
		wk_control = nil;
	}
	
	if (wk_interupt) {
		[wk_interupt setDelegate:nil];
		[wk_interupt closeChannel];
		[wk_interupt release];
		wk_interupt = nil;
	}
	
	if ([wk_device isConnected]) {
		[wk_device closeConnection];
	}
	return kIOReturnSuccess;
}

- (IOReturn)connectControl {
	if (wk_status != kWKConnectionStatusClosed)
		return kIOReturnSuccess;
	
	IOReturn err = kIOReturnSuccess;
	err = [wk_device openL2CAPChannelAsync:&wk_control withPSM:kBluetoothL2CAPPSMHIDControl delegate:self];
	if (kIOReturnSuccess == err) {
		wk_status = kWKConnectionStatusOpening;
		[wk_control retain];	
	} else {
		wk_control = nil;
		WKPrintIOReturn(err, "control connection");
	}
	return err;
}

- (IOReturn)connect {
	WKLog(@"connecting '%@'", [wk_device getName]);
	/* open interrupt link */
	IOReturn err = [wk_device openL2CAPChannelAsync:&wk_interupt withPSM:kBluetoothL2CAPPSMHIDInterrupt delegate:self];
	if (kIOReturnSuccess == err) {
		[wk_interupt retain];
		err = [self connectControl];
	} else {
		wk_interupt = nil;
	}
	
	if (err != kIOReturnSuccess) {
		WKPrintIOReturn(err, "connect");
		[self cleanup];
	}
	return err;
}

- (IOReturn)close {
	return [self cleanup];
}

- (id)delegate {
	return wk_delegate;
}
- (void)setDelegate:(id)aDelegate {
	wk_delegate = aDelegate;
}

- (IOBluetoothDevice *)device {
	return wk_device;
}

- (IOReturn)sendData:(const uint8_t *)data length:(size_t)length context:(void *)ctxt {
	IOReturn err = kIOReturnSuccess;
	if (wk_status == kWKConnectionStatusOpened) {
		// provide very simplistic locking for this class
		@synchronized (self) {
			err = [wk_control writeAsync:(uint8_t *)data length:length refcon:ctxt];
			if (kIOReturnSuccess == err && !wk_resend) {
				//WKLog(@"------------- Backup request");
				[wk_backup addObject:[_WKRequest requestWithBytes:data length:length context:ctxt]];
			}
		}
	} else {
		WKLog(@"add pending request");
		if (!wk_pending) wk_pending = [[NSMutableArray alloc] init];
		[wk_pending addObject:[_WKRequest requestWithBytes:data length:length context:ctxt]];
		/* if closed, send an open request */
		if (wk_status == kWKConnectionStatusClosed)
			err = [self connectControl];
	}
	WKPrintIOReturn(err, "send raw");
	return err;
}

#pragma mark -
#pragma mark Bluetooth Aync Callbacks
//- (void)l2capChannelReconfigured:(IOBluetoothL2CAPChannel*)l2capChannel {
//	NSLog(@"[%@ %@]", [self class], NSStringFromSelector(_cmd));
//}
//- (void)l2capChannelQueueSpaceAvailable:(IOBluetoothL2CAPChannel*)l2capChannel {
//	NSLog(@"[%@ %@]", [self class], NSStringFromSelector(_cmd));
//}
- (void)l2capChannelOpenComplete:(IOBluetoothL2CAPChannel*)l2capChannel status:(IOReturn)error {
	switch ([l2capChannel getPSM]) {
		case kBluetoothL2CAPPSMHIDControl:
			wk_status = kWKConnectionStatusOpened;
			WKLog(@"Control opened: %s", mach_error_string(error));
			
			if ([wk_backup count] > 0) {
				wk_resend = YES;
				WKLog(@"sending losed requests");
				NSUInteger idx = 0;
				for (idx = 0; idx < [/*cam: is this really what we want? : wk_backup*/wk_pending count]; idx++) {
					_WKRequest *request = [wk_pending objectAtIndex:idx];
					[self sendData:[request bytes] length:[request length] context:[request context]];
				}
				
				wk_resend = NO;
			}
			
			if ([wk_pending count] > 0) {
				WKLog(@"sending pending requests");
				wk_resend = YES;
				NSUInteger idx = 0;
				for (idx = 0; idx < [wk_pending count]; idx++) {
					_WKRequest *request = [wk_pending objectAtIndex:idx];
					if (kIOReturnSuccess == [self sendData:[request bytes] length:[request length] context:[request context]]) {
						//WKLog(@"------------- Backup request");
						[wk_backup addObject:request];
					}
				}				
				[wk_pending removeAllObjects];
				wk_resend = NO;
				wk_retry = 0;
			}
			// cam: warning, if we have pending request the delegate may eventually
			// never be notified of the success of the opening.
			// for example, in the case an extension is plugged-in when the wiimote
			// is being connected.
//			else {
				if ([wk_delegate respondsToSelector:@selector(connectionDidOpen:)]) {
					[wk_delegate connectionDidOpen:self];
				}
//			}
			
			break;
		case kBluetoothL2CAPPSMHIDInterrupt:
			WKLog(@"Interrupt opened: %s", mach_error_string(error));
			break;
	}
}

- (void)l2capChannelData:(IOBluetoothL2CAPChannel*)l2capChannel data:(uint8_t *)dataPointer length:(size_t)dataLength {
	switch ([l2capChannel getPSM]) {
		case kBluetoothL2CAPPSMHIDControl:
			@synchronized (self) {
				NSAssert([wk_backup count] > 0, @"invalid request backup state");
				WKLog(@"ack: %d", (int)*dataPointer);
				if (*dataPointer) {
					/* error, resend the request */
					_WKRequest *request = [wk_backup objectAtIndex:0];
					if ([request retry] >= 3) {
						WKLog(@"fail 3 times. Remove the request.");
					} else {
						wk_resend = YES;
						if (kIOReturnSuccess == [self sendData:[request bytes] length:[request length] context:[request context]]) {
							WKLog(@"Request resend (%u).", [request retry]);
							[wk_backup addObject:request];
							[request incrementRetry];
						} else {
							WKLog(@"error while resending request. Discard Request.");
						}
						wk_resend = NO;
					}
				}
				//WKLog(@"------------- Remove request");
				[wk_backup removeObjectAtIndex:0];
			}
			break;
		case kBluetoothL2CAPPSMHIDInterrupt:
			/* trunk the first byte */
			if (dataLength > 1 && [wk_delegate respondsToSelector:@selector(connection:didReceiveData:length:)])
				[wk_delegate connection:self didReceiveData:dataPointer + 1 length:dataLength - 1];
			break;
	}
}

- (void)l2capChannelWriteComplete:(IOBluetoothL2CAPChannel*)l2capChannel refcon:(void*)refcon status:(IOReturn)error {
	switch ([l2capChannel getPSM]) {
		case kBluetoothL2CAPPSMHIDControl:
			WKLog(@"did send (%p)", refcon);
			if ([wk_delegate respondsToSelector:@selector(connection:didSendData:error:)]) 
				[wk_delegate connection:self didSendData:refcon error:error];
			break;
//		case kBluetoothL2CAPPSMHIDInterrupt:
//			NSLog(@"Interrupt write done: %s", mach_error_string(error));
//			break;
	}
}

- (void)l2capChannelClosed:(IOBluetoothL2CAPChannel *)l2capChannel {
	switch ([l2capChannel getPSM]) {
		case kBluetoothL2CAPPSMHIDControl:
			WKLog(@"Control closed!");
			bool opening = wk_status == kWKConnectionStatusOpening;
			wk_status = kWKConnectionStatusClosed;
			[wk_control setDelegate:nil];
			[wk_control release];
			wk_control = nil;
			
			if (opening || [wk_pending count]) {
				if (opening) {
					WKLog(@"------------ connection closed but not properly opened => retry.");
				} else {
					WKLog(@"------------ connection closed but remaining pending requests => retry.");
				}
				if (wk_retry < 3) {
					wk_retry++;
					/* retry */
					[self connectControl];
				} else {
					WKLog(@"------------ failed 3 times.");
					[self close];
				}
			}
			if ([wk_backup count] > 0) { WKLog(@"connection closed but waiting %u acks.", [wk_backup count]); }
			break;
		case kBluetoothL2CAPPSMHIDInterrupt:
			[self retain]; // delegate may release the last reference
			wk_status = kWKConnectionStatusError; // prevent control reconnection
			if ([wk_delegate respondsToSelector:@selector(connectionDidClose:)]) {
				[wk_delegate connectionDidClose:self];
			}
			WKLog(@"Interrupt closed!");
			[self cleanup];
			[self release];
			break;
	}
}

@end

#pragma mark Requests
@implementation _WKRequest

+ (_WKRequest *)requestWithBytes:(const uint8_t *)bytes length:(size_t)length context:(void *)ctxt {
	_WKRequest *request = [[_WKRequest alloc] init];
	request->wk_ctxt = ctxt;
	request->wk_data = [[NSData alloc] initWithBytes:bytes length:length];
	return [request autorelease];
}

- (void)dealloc {
	[wk_data release];
	[super dealloc];
}

- (const uint8_t *)bytes {
	return [wk_data bytes];
}

- (size_t)length {
	return [wk_data length];
}
- (void *)context {
	return wk_ctxt;
}

- (NSUInteger)retry {
	return wk_retry;
}

- (void)incrementRetry {
	wk_retry++;
}

@end

