//
//  WKDiscovery.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WKDiscovery.h>

#import "WKBase.h"
#import <WiimoteKit/WKTypes.h>

#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import <IOBluetooth/objc/IOBluetoothSDPDataElement.h>
#import <IOBluetooth/objc/IOBluetoothSDPServiceRecord.h>

NSString * const WKDeviceRegistryWillSearchDevicesNotification = @"WKDeviceRegistryWillSearchDevices";

NSString * const WKDeviceRegistryDidSearchDevicesNotification = @"WKDeviceRegistryDidSearchDevices";
NSString * const WKDeviceRegistryErrorKey = @"WKDeviceRegistryErrorKey";

NSString * const WKDeviceRegistryDidFoundDeviceNotification = @"WKDeviceRegistryDidFoundDevice";
NSString * const WKDeviceRegistryFoundDeviceKey = @"WKDeviceRegistryFoundDeviceKey";

@implementation WKDeviceRegistry

+ (WKDeviceRegistry *)sharedRegistry {
	static WKDeviceRegistry *sShared = nil;
	if (!sShared) {
		sShared = [[WKDeviceRegistry alloc] init];
	}
	return sShared;
}

- (id)init {
	if (self = [super init]) {
		wk_devices = [[NSMutableArray alloc] init];
		[IOBluetoothDevice registerForConnectNotifications:self selector:@selector(bluetoothDidConnect:device:)];
	}
	return self;
}

- (void)dealloc {
	[wk_inquiry setDelegate:nil];
	[wk_inquiry release];
	[wk_devices release];
	[super dealloc];
}

- (BOOL)isSearching {
	return wk_inquiry != nil;
}

- (IOReturn)search {
	if ([self isSearching]) return kIOReturnSuccess;
	
	if (!IOBluetoothLocalDeviceAvailable())
		return kIOReturnNotAttached;
	
	wk_inquiry = [IOBluetoothDeviceInquiry inquiryWithDelegate:self];
	[wk_inquiry setInquiryLength:20];
	[wk_inquiry setSearchCriteria:kBluetoothServiceClassMajorAny 
							 majorDeviceClass:kBluetoothDeviceClassMajorPeripheral
							 minorDeviceClass:kBluetoothDeviceClassMinorPeripheral2Joystick];
	[wk_inquiry setUpdateNewDeviceNames:NO];
	
	IOReturn status = [wk_inquiry start];
	if (status == kIOReturnSuccess) {
		[wk_inquiry retain];
	} else {
		NSLog(@"Error: Inquiry did not start, error %s", mach_error_string(status));
		[wk_inquiry setDelegate:nil];
		wk_inquiry = nil;
	}
	
	return status;	
}

- (void)stop {
	[wk_inquiry stop];
}

- (NSArray *)devices {
	return wk_devices;
}

- (void)addDevice:(IOBluetoothDevice *)device {
	if (![wk_devices containsObject:device]) {
		// test if this is a wiimote
		bool wiimote = false;
		if ([device getLastServicesUpdate]) {
			NSArray *services = [device getServices];
			NSUInteger idx = 0;
			for (idx = 0; idx < [services count] && !wiimote; idx++) {
				IOBluetoothSDPServiceRecord *record = [services objectAtIndex:idx];
				NSDictionary *attrs = [record getAttributes];
				IOBluetoothSDPDataElement *vendor = [attrs objectForKey:[NSNumber numberWithLong:kBluetoothSDPAttributeDeviceIdentifierVendorID]];
				IOBluetoothSDPDataElement *product = [attrs objectForKey:[NSNumber numberWithLong:kBluetoothSDPAttributeDeviceIdentifierProductID]];
				if (vendor && product) {
					wiimote = [vendor containsValue:[NSNumber numberWithLong:0x057e]] && 
					[product containsValue:[NSNumber numberWithLong:0x0306]];
				}
			}
		}  else {
			NSLog(@"require data to identify device");
			/* initiate a name request and wait async reply. */
			[device performSDPQuery:self];
			return;
		}
		if (wiimote) {
			[wk_devices addObject:device];
			[[NSNotificationCenter defaultCenter] postNotificationName:WKDeviceRegistryDidFoundDeviceNotification 
																													object:self
																												userInfo:[NSDictionary dictionaryWithObject:device forKey:WKDeviceRegistryFoundDeviceKey]];
		} else {
			NSLog(@"Find a device but does not look like it is a wiimote: %@", device);
		}
	}
}
	
#pragma mark -
- (void)deviceInquiryStarted:(IOBluetoothDeviceInquiry*)sender {
	[[NSNotificationCenter defaultCenter] postNotificationName:WKDeviceRegistryWillSearchDevicesNotification
														object:self];
}


- (void)deviceInquiryDeviceFound:(IOBluetoothDeviceInquiry*)sender	device:(IOBluetoothDevice *)device {
	[self addDevice:device];
}

- (void)deviceInquiryComplete:(IOBluetoothDeviceInquiry*)sender	error:(IOReturn)error	aborted:(BOOL)aborted {
	[[NSNotificationCenter defaultCenter] postNotificationName:WKDeviceRegistryDidSearchDevicesNotification
														object:self
													  userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:error] forKey:WKDeviceRegistryErrorKey]];
	[wk_inquiry setDelegate:nil];
	[wk_inquiry release];
	wk_inquiry = nil;
}

#pragma mark -
- (void)remoteNameRequestComplete:(IOBluetoothDevice *)device status:(IOReturn)status {
	if (kIOReturnSuccess == status) {
		[self addDevice:device];
	} else {
		NSLog(@"name query fail (%d) for device: %@", status, device);
	}
}

- (void)bluetoothDidConnect:(IOBluetoothUserNotification *)aNotification device:(IOBluetoothDevice *)device {
	[self addDevice:device];
}

@end
