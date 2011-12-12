/*
 *  WiiRemoteInterbal.h
 *  WiimoteKit
 *
 *  Created by Jean-Daniel Dupas on 16/01/08.
 *  Copyright 2008 Shadow Lab. All rights reserved.
 */

#import "WKBase.h"

#import <WiimoteKit/WKEvent.h>
#import <WiimoteKit/WiiRemote.h>

enum {
	kWKMemorySpaceWiiRemote = 1,
	kWKMemorySpaceExtension,
	kWKMemorySpaceIRCamera,
	kWKMemorySpaceSpeaker,
} ;
typedef NSUInteger WKAddressSpace;

// Maximum acceleration tolerated by the Wiimote accerelometers
#if defined(SCALED_ACCELERATIONS)
#	define SCALE_MAX_GRAVITY(a) (5.0 * (a))
#else
#	define SCALE_MAX_GRAVITY(a) (a)
#endif

// Wiimote registers (relative to respective address range)
#define IR_REGISTER_STATUS              0x30
#define IR_REGISTER_SENSITIVITY_1       0x00
#define IR_REGISTER_SENSITIVITY_2       0x1a
#define IR_REGISTER_MODE                0x33

#define EXTENSION_REGISTER_STATUS   	0xf0
#define EXTENSION_REGISTER_STATUS2		0xfb
#define EXTENSION_REGISTER_TYPE         0xfa
#define EXTENSION_REGISTER_CALIBRATION  0x20

#define EXTENSION_IDENTIFIER_TYPE0		0x0000
#define EXTENSION_IDENTIFIER_PARTIAL	0xFFFF
#define EXTENSION_IDENTIFIER_NUNCHUK	0xa4200000
#define EXTENSION_IDENTIFIER_CLASSIC	0xa4200101

// Wiimote output commands
enum WKOutputReport {
	WKOutputReportNone          = 0x00,
	//WKOutputReport???         = 0x10,
	WKOutputReportLEDs          = 0x11, // 1 byte
	WKOutputReportMode          = 0x12, // 2 bytes
	WKOutputReportIRCamera      = 0x13, // 1 byte
	WKOutputReportSpeakerStatus = 0x14, // 1 byte
	WKOutputReportStatus        = 0x15, // 1 byte
	WKOutputReportWriteMemory   = 0x16, // 21 byte
	WKOutputReportReadMemory    = 0x17, // 6 byte
	WKOutputReportSpeakerData   = 0x18, // 21 byte
	WKOutputReportSpeakerMute   = 0x19, // 1
	WKOutputReportIRCamera2     = 0x1a, // 1 byte
};

// The report format in which the Wiimote should return data
enum WKInputReport {
	// Status report
	kWKInputReportStatus    = 0x20, // 6 bytes
	// Read data from memory location
	kWKInputReportReadData  = 0x21, // 21 bytes
	// Write status to memory location
	kWKInputReportWriteData = 0x22, // 4 bytes
	
	kWKInputReportDefault                = 0x30, // 2 bytes. buttons only*
	kWKInputReportAccelerometer          = 0x31, // 3 bytes*
	kWKInputReportShortExtension         = 0x32, // 9 bytes
	kWKInputReportAccelerometerIR        = 0x33, // 3 - 12 bytes*
	kWKInputReportExtension              = 0x34, // 19 bytes*
	kWKInputReportAccelerometerExtension = 0x35, // 3 - 16 bytes*
	kWKInputReportIRExtension            = 0x36, // 10 - 9 bytes*
	kWKInputReportAll                    = 0x37, // 3 - 10 - 6 bytes*
	kWKInputReportLongExtension          = 0x3d, // 21 bytes. WARNING, this report does not include buttons.
	/* interleaved */
	kWKInputReportAllInterleavedLow      = 0x3e, // 1 - 18 bytes
	kWKInputReportAllInterleavedHigh     = 0x3f, // 1 - 18 bytes
};

@interface WiiRemote (WiiRemoteInternal)

/* Commands */
- (IOReturn)sendCommand:(const uint8_t *)cmd length:(size_t)length;

/* I/O */
- (IOReturn)readDataAtAddress:(user_addr_t)address space:(WKAddressSpace)space length:(size_t)length handler:(SEL)handler;
- (IOReturn)writeData:(const uint8_t *)data length:(size_t)length atAddress:(user_addr_t)address space:(WKAddressSpace)space next:(SEL)next;

/* IRCamera */
- (WKIRMode)irMode;
- (IOReturn)setIrMode:(WKIRMode)aMode;

/* Extensions */
- (IOReturn)initializeExtension;
- (void)setExtension:(WKExtension *)anExtension;
- (void)setExtensionType:(WKExtensionType)extensionType;

#pragma mark Query Status
- (IOReturn)refreshStatus;
- (IOReturn)refreshReportMode;
- (IOReturn)refreshCalibration;
- (IOReturn)refreshExtensionCalibration;

#pragma mark Callbacks
- (void)didReceiveAck:(const uint8_t *)ack;

@end

typedef struct {
	struct {
		bool exists;
		uint8_t size;
		uint16_t rawx, rawy;
	} points[4];
	CGFloat x, y;
} WKIREventData;

typedef struct {
	CGFloat x, y, z;
	CGFloat dx, dy, dz;
	NSUInteger rawx, rawy, rawz;
	NSUInteger rawdx, rawdy, rawdz;
} WKAccelerometerEventData;

typedef struct {
	CGFloat x, y;
	CGFloat dx, dy;
	NSUInteger rawx, rawy;
	NSUInteger rawdx, rawdy;
} WKJoystickEventData;

typedef struct {
	CGFloat x;
	CGFloat dx;
	NSUInteger rawx;
	NSUInteger rawdx;
} WKAnalogEventData;

@interface WiiRemote (WiiRemoteEvent)

- (void)sendEvent:(WKEvent *)theEvent;

- (void)sendIREvent:(WKIREventData *)data source:(WKEventSource)source;

- (void)sendAccelerometerEvent:(WKAccelerometerEventData *)data source:(WKEventSource)source;
- (void)sendButtonEvent:(NSUInteger)button source:(WKEventSource)source down:(BOOL)isButtonDown;

- (void)sendStatusEvent:(NSUInteger)value source:(WKEventSource)source subtype:(WKEventSubtype)subtype;

- (void)sendAnalogEvent:(WKAnalogEventData *)data source:(WKEventSource)source subtype:(WKEventSubtype)subtype;
- (void)sendJoystickEvent:(WKJoystickEventData *)data source:(WKEventSource)source subtype:(WKEventSubtype)subtype;

@end
