//
//  WiiNunchuk.h
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 15/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WKExtension.h>

/*!
 @struct
 @abstract Calibration information stored on the Nunchuk
 @field x Joystick X-axis calibration
 @field y Joystick Y-axis calibration
 @field acc Accelerometer calibration data
 */
typedef struct _WKNunchukCalibration {
	WKAxisCalibration x, y; // joystick calibration
	WKAccelerationCalibration acc;
} WKNunchukCalibration;

enum {
	kWKNunchukButtonC = 1 << 0,
	kWKNunchukButtonZ = 1 << 1,
};
@interface WiiNunchuk : WKExtension {
@private
	WKAccelerationState wk_acc; // State of accelerometers
	WKNunchukCalibration wk_calib; // Calibration data for Nunchuk extension
	
	struct _wk_wnFlags {
		/* buttons */
		unsigned int c:1, z:1; // Digital button on Nunchuk extension
	} wk_wnFlags;
	
	/* joystick */
	CGFloat wk_x, wk_y; // Normalized joystick position. Values range between -0.5 and 0.5.
	uint8_t wk_rawX, wk_rawY; // joystick position before normalization. Values range between 0 and 255.
}

@end
