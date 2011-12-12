//
//  WiiClassic.h
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 16/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WKExtension.h>
#import <WiimoteKit/WKClassicTypeExtension.h>

/*!
 @struct
 @abstract Calibration information stored on the Classic Controller
 @field xl Left joystick X-axis 
 @field yl Left joystick Y-axis
 @field xr Right joystick X-axis 
 @field xr Right joystick Y-axis
 
 @field tl Left analog trigger
 @field tr Right analog trigger
 */
typedef struct _WKClassicControllerCalibration {
	WKAxisCalibration xl, yl;
	WKAxisCalibration xr, yr;
	WKButtonCalibration tl, tr;
} WKClassicControllerCalibration;

enum {
	kWKClassicButtonUp    = 1 << 0,
	kWKClassicButtonLeft  = 1 << 1,
	kWKClassicButtonZR    = 1 << 2,
	kWKClassicButtonX     = 1 << 3,
	kWKClassicButtonA     = 1 << 4,
	kWKClassicButtonY     = 1 << 5,
	kWKClassicButtonB     = 1 << 6,
	kWKClassicButtonZL    = 1 << 7,
	
	kWKClassicButtonR     = 1 << 9,
	kWKClassicButtonPlus  = 1 << 10,
	kWKClassicButtonHome  = 1 << 11,
	kWKClassicButtonMinus = 1 << 12,
	kWKClassicButtonL     = 1 << 13,
	kWKClassicButtonDown  = 1 << 14,
	kWKClassicButtonRight = 1 << 15,
	
	kWKClassicButtonsMask  = 0xfeff,
};

@interface WiiClassic : WKClassicTypeExtension {
@private
	// Calibration data for Classic Controller extension
	WKClassicControllerCalibration wk_calib;
	
	CGFloat wk_xl, wk_yl; // Normalized value of left joystick.  Values range between -0.5 - 0.5
	uint8_t wk_rawXL, wk_rawYL; // Raw value of left joystick.  Values range between 0 - 255.

	CGFloat wk_xr, wk_yr; // Normalized value of right joystick.  Values range between -0.5 - 0.5
	uint8_t wk_rawXR, wk_rawYR; // Raw value of right joystick.  Values range between 0 - 255.
	
	CGFloat wk_tl, wk_tr; // Raw value of analog trigger.  Values range between 0 - 255.	
	uint8_t wk_rawTL, wk_rawTR; // Normalized value of analog trigger.  Values range between 0.0 - 1.0
}

@end
