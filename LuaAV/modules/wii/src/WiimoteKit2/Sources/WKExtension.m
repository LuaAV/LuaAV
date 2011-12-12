//
//  WKExtension.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 16/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WKExtension.h>

#import <WiimoteKit/WiiRemote.h>
#import <WiimoteKit/WiiNunchuk.h>
#import <WiimoteKit/WiiClassic.h>

#import "WiiRemoteInternal.h"

@implementation WKExtension

+ (id)extensionWithType:(WKExtensionType)aType {
	Class cls = Nil;
	switch (aType) {
		case kWKExtensionNunchuk:
			cls = [WiiNunchuk class];
			break;
		case kWKExtensionClassicController:
			cls = [WiiClassic class];
			break;
	}
	return [[[cls alloc] initWithType:aType] autorelease];
}

- (id)initWithType:(WKExtensionType)aType {
	NSParameterAssert([self class] != [WKExtension class]);
	if (self = [super init]) {
		
	}
	return self;
}
- (WiiRemote *)wiiRemote { return wk_remote; }
- (void)setWiiRemote:(WiiRemote *)aRemote {
	if (aRemote != wk_remote) {
		wk_remote = aRemote;
		[wk_remote refreshExtensionCalibration];
	}
}

- (WKExtensionType)type { return 0; }
- (size_t)calibrationLength { return 0; }
- (user_addr_t)calibrationAddress { return 0; }

#pragma mark Wiimote Capabilities
- (BOOL) acceptsAccelerometerEvents
{
	return YES;
}

- (BOOL) acceptsIRCameraEvents
{
	return YES;
}

- (void)parseStatus:(const uint8_t *)data range:(NSRange)aRange {}
- (void)parseCalibration:(const uint8_t *)data length:(size_t)length {}

@end
