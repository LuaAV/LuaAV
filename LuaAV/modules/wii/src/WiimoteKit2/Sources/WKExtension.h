//
//  WKExtension.h
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 16/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WKTypes.h>

@class WiiRemote;
@interface WKExtension : NSObject {
@private
	WiiRemote *wk_remote;
}

+ (id)extensionWithType:(WKExtensionType)aType;
- (id)initWithType:(WKExtensionType)aType;

- (WKExtensionType)type;
/* auto-calibrate when attached to a remote */
- (WiiRemote *)wiiRemote;
- (void)setWiiRemote:(WiiRemote *)aRemote;

- (void)parseStatus:(const uint8_t *)data range:(NSRange)aRange;
- (void)parseCalibration:(const uint8_t *)data length:(size_t)length;

#pragma mark Calibration
- (size_t)calibrationLength;
- (user_addr_t)calibrationAddress;

#pragma mark Wiimote Capabilities
- (BOOL) acceptsAccelerometerEvents;
- (BOOL) acceptsIRCameraEvents;

@end
