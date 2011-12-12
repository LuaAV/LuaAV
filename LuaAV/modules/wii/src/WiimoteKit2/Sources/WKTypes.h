/*
 *  WiimoteTypes.h
 *  WiimoteKit
 *
 *  Created by Jean-Daniel Dupas on 12/01/08.
 *  Copyright 2008 Shadow Lab. All rights reserved.
 *
 */

#ifndef MAC_OS_X_VERSION_10_5
  #define MAC_OS_X_VERSION_10_5 1050
#endif

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5
	#if !__LP64__
		typedef int NSInteger;
		typedef unsigned int NSUInteger;

		typedef float CGFloat;
		#define CGFLOAT_MIN FLT_MIN
		#define CGFLOAT_MAX FLT_MAX
	#else
		#error 64 bits no supported for deployement version < MAC_OS_X_VERSION_10_5.
	#endif

	#define NSIntegerMax    LONG_MAX
	#define NSIntegerMin    LONG_MIN
	#define NSUIntegerMax   ULONG_MAX
#endif

/*!
 @enum 
 @abstract The extension plugged into the Wiimote
 @constant kWKExtensionNone No extension
 @constant kWKExtensionNunchuk Nunchuk extension
 @constant kWKExtensionClassicController Classic Controller extension
 */
enum {
	kWKExtensionNone				= 0,
	kWKExtensionNunchuk				= 1,
	kWKExtensionClassicController	= 2,
};
typedef NSUInteger WKExtensionType;

#pragma mark Base types
typedef struct _WKAxisCalibration {
	uint8_t max;
	uint8_t min;
	uint8_t center;
} WKAxisCalibration;

typedef struct _WKButtonCalibration {
	uint8_t min;
	uint8_t max;
} WKButtonCalibration;

#pragma mark Calibrations
/*!
 @struct 
 @abstract Current state of the accelerometers
 @field rawX Raw accelerometer data (Values range between 0 - 255).
 @field rawY Raw accelerometer data (Values range between 0 - 255).
 @field rawZ Raw accelerometer data (Values range between 0 - 255).
 @field x Normalized accelerometer data.  Values range between 0 - ? 
 @field y Normalized accelerometer data.  Values range between 0 - ? 
 @field z Normalized accelerometer data.  Values range between 0 - ? 
 */
typedef struct _WKAccelerationState {
	uint16_t rawX, rawY, rawZ;
	CGFloat lowX, lowZ;
	CGFloat x, y, z;
} WKAccelerationState;

/*!
 @struct
 @abstract Accelerometer calibration information
 @field x0 Zero point of accelerometer
 @field y0 Zero point of accelerometer
 @field z0 Zero point of accelerometer
 @field xG Gravity at rest of accelerometer
 @field yG Gravity at rest of accelerometer
 @field zG Gravity at rest of accelerometer
 */
typedef struct _WKAccelerationCalibration {
	// cam: changed from uint8_t to uint16_t to allow for 9 bits acceleration resolution
	uint16_t x0, y0, z0;
	uint16_t xG, yG, zG;
} WKAccelerationCalibration;
