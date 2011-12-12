//
//  WKDiscovery.h
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

extern NSString * const WKDeviceRegistryWillSearchDevicesNotification;

extern NSString * const WKDeviceRegistryDidSearchDevicesNotification;
extern NSString * const WKDeviceRegistryErrorKey;

extern NSString * const WKDeviceRegistryDidFoundDeviceNotification;
extern NSString * const WKDeviceRegistryFoundDeviceKey;

@class IOBluetoothDeviceInquiry;
@interface WKDeviceRegistry : NSObject {
@private
	NSMutableArray *wk_devices;
	IOBluetoothDeviceInquiry *wk_inquiry;
}

+ (WKDeviceRegistry *)sharedRegistry;

- (BOOL)isSearching;

- (IOReturn)search;
- (void)stop;

- (NSArray *)devices;

@end
