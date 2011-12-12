//
//  WiiRemoteDiscovery.m
//  DarwiinRemote
//
//  Created by Ian Rickard on 12/9/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import "WiiRemoteDiscovery.h"


@implementation WiiRemoteDiscovery
- (id) init{
	inquiry = nil;
	_delegate = nil;
	printf("WiiRemoteDiscovery init\n");
	return self;
}

+ (WiiRemoteDiscovery*) discoveryWithDelegate:(id)delegate {
	WiiRemoteDiscovery *out = [[WiiRemoteDiscovery alloc] init];
	[out setDelegate: delegate];
	printf("WiiRemoteDiscovery discoveryWithDelegate\n");
	return out;
}

- (void)setDelegate:(id)delegate{
	_delegate = delegate;
}

- (IOReturn)start {
	if (nil != inquiry) {
		NSLog(@"Warning: Attempted to start already-started WiiRemoteDiscovery");
		return kIOReturnSuccess;
	}
	
	if (nil == _delegate) {
		NSLog(@"Warning: starting WiiRemoteDiscovery without delegate set");
	}
	
	inquiry = [IOBluetoothDeviceInquiry inquiryWithDelegate: self];
	
	if (nil == inquiry) {
		NSLog(@"Error: Failed to alloc IOBluetoothDeviceInquiry");
		return kIOReturnNotAttached;
	}
	
	// calling initWithDelegate after this isn't correct.  see:
	// http://lists.apple.com/archives/Bluetooth-dev/2005/Aug/msg00006.html
	
	
	// retain ourselves while there's an outstanding inquiry, don't want to go disappearing
	// before we get called as a delegate
	[self retain];
	
	IOReturn ret = [inquiry start];
	
	if (ret == kIOReturnSuccess){
		// copied from old code, is this necesary?  Shouldn't it already be retained once when it was initted?
		[inquiry retain];
	} else {
		NSLog(@"Error: Inquiry did not start, error %d", ret);
		[inquiry setDelegate:nil];
		[inquiry release];
		inquiry = nil;
	}
	return ret;
}

- (IOReturn)stop {
	if (nil == inquiry) {
		NSLog(@"Warning: Attempted to stop already-stopped WiiRemoteDiscovery");
		return kIOReturnSuccess;
	}
	IOReturn ret = [inquiry stop];
	
	if (ret != kIOReturnSuccess && ret != kIOReturnNotPermitted) {
		// kIOReturnNotPermitted is if it's already stopped
		NSLog(@"Error: Inquiry did not stop, error %d", ret);
	}
	
	[inquiry setDelegate:nil];
	// and release the hold on ourselves
	[self release];
	
	[inquiry release];
	inquiry = nil;
	
	
	printf("WiiRemoteDiscovery stop %d\n", [self retainCount]);
	
	return ret;
}

- (void)dealloc {
	
	printf("deallocating WiiRemoteDiscovery\n");
	if (nil != inquiry) {
		printf("need to stop\n");
		[self stop];
	}
	
	[super dealloc];
	printf("deallocated WiiRemoteDiscovery\n");
	
}

/////// IOBluetoothDeviceInquiry delegates //////

- (void) deviceInquiryComplete:(IOBluetoothDeviceInquiry*)sender 
        error:(IOReturn)error aborted:(BOOL)aborted {
	
	if (aborted) return; // called by stop ;)
	
	if (kIOReturnSuccess != error) {
		[_delegate WiiRemoteDiscoveryError:error];
		[self stop];
		return;
	}
	
	//[inquiry clearFoundDevices];
	IOReturn ret = [inquiry start];
	
	if (ret != kIOReturnSuccess) {
		NSLog(@"Error: Restarting Inquiry failed: %d", ret);
		[_delegate WiiRemoteDiscoveryError: ret];
		[inquiry stop];
	}
}

- (void)checkDevice:(IOBluetoothDevice*)device {
	if ([[device getName] isEqualToString:@"Nintendo RVL-CNT-01"]){
	
		WiiRemote *wii = [[WiiRemote alloc] init];
		IOReturn ret = [wii connectTo:device];
		
		if (ret == kIOReturnSuccess) {
			[_delegate WiiRemoteDiscovered: wii];
		} else {
			[wii release];
			// initWithDevice generated error message
			[_delegate WiiRemoteDiscoveryError: ret];
		}
	}
}

- (void)deviceInquiryDeviceNameUpdated:(IOBluetoothDeviceInquiry*)sender
		device:(IOBluetoothDevice*)device devicesRemaining:(int)devicesRemaining {
	[self checkDevice:device];
}

- (void) deviceInquiryDeviceFound:(IOBluetoothDeviceInquiry*)sender 
        device:(IOBluetoothDevice*)device {
	[self checkDevice:device];
}

@end
