//
//  WiiRemoteDiscovery.h
//  DarwiinRemote
//
//  Created by Ian Rickard on 12/9/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h> 
#import "WiiRemote.h"


@interface WiiRemoteDiscovery : NSObject {
	IOBluetoothDeviceInquiry* inquiry;
	id _delegate;
}

+ (WiiRemoteDiscovery*) discoveryWithDelegate:(id)delegate;
- (void) setDelegate:(id)delegate;
- (IOReturn)start;
- (IOReturn)stop;

@end


@interface NSObject( WiiRemoteDiscoveryDelegate )

- (void) WiiRemoteDiscovered:(WiiRemote*)wiimote;
- (void) WiiRemoteDiscoveryError:(int)code;

@end;