//
//  WiiRemote.h
//  DarwiinRemote
//
//  Created by KIMURA Hiroaki on 06/12/04.
//  Copyright 2006 KIMURA Hiroaki. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothL2CAPChannel.h>


typedef unsigned char WiiIRModeType;
enum {
	kWiiIRModeBasic			= 0x01,
	kWiiIRModeExtended		= 0x03,
	kWiiIRModeFull			= 0x05
};

typedef struct {
	int x, y, s;
} IRData;

typedef struct {
	unsigned char accX_zero, accY_zero, accZ_zero, accX_1g, accY_1g, accZ_1g; 
} WiiAccCalibData;

typedef struct {
	unsigned char x_min, x_max, x_center, y_min, y_max, y_center; 
} WiiJoyStickCalibData;


typedef UInt16 WiiButtonType;
enum {
	WiiRemoteAButton,
	WiiRemoteBButton,
	WiiRemoteOneButton,
	WiiRemoteTwoButton,
	WiiRemoteMinusButton,
	WiiRemoteHomeButton,
	WiiRemotePlusButton,
	WiiRemoteUpButton,
	WiiRemoteDownButton,
	WiiRemoteLeftButton,
	WiiRemoteRightButton,
	
	WiiNunchukZButton,
	WiiNunchukCButton,
	
	WiiClassicControllerXButton,
	WiiClassicControllerYButton,
	WiiClassicControllerAButton,
	WiiClassicControllerBButton,
	WiiClassicControllerLButton,
	WiiClassicControllerRButton,
	WiiClassicControllerZLButton,
	WiiClassicControllerZRButton,
	WiiClassicControllerUpButton,
	WiiClassicControllerDownButton,
	WiiClassicControllerLeftButton,
	WiiClassicControllerRightButton,
	WiiClassicControllerMinusButton,
	WiiClassicControllerHomeButton,
	WiiClassicControllerPlusButton
};


typedef UInt16 WiiExpansionPortType;
enum{
	WiiExpNotAttached,
	WiiNunchuk,
	WiiClassicController
};

typedef UInt16 WiiAccelerationSensorType;
enum{
	WiiRemoteAccelerationSensor,
	WiiNunchukAccelerationSensor
};


typedef UInt16 WiiJoyStickType;
enum{
	WiiNunchukJoyStick,
	WiiClassicControllerLeftJoyStick,	//not available
	WiiClassicControllerRightJoyStick	//not available
};

@interface WiiRemote : NSObject {
	
	IOBluetoothDevice* wiiDevice;
	IOBluetoothL2CAPChannel *ichan;
	IOBluetoothL2CAPChannel *cchan;
	
	id _delegate;
	
	
	unsigned char accX;
	unsigned char accY;
	unsigned char accZ;
	unsigned short buttonData;
	
	float lowZ, lowX;
	int orientation;
	int leftPoint; // is point 0 or 1 on the left. -1 when not tracking.
	
	
	WiiExpansionPortType expType;
	WiiAccCalibData wiiCalibData, nunchukCalibData;
	WiiJoyStickCalibData nunchukJoyStickCalibData;
	WiiIRModeType wiiIRMode;
	IRData	irData[4];
	double batteryLevel;
	double warningBatteryLevel;
	
	BOOL readingRegister;
	BOOL isMotionSensorEnabled, isIRSensorEnabled, isVibrationEnabled, isExpansionPortEnabled;
	BOOL isExpansionPortAttached, initExpPort;
	BOOL isLED1Illuminated, isLED2Illuminated, isLED3Illuminated, isLED4Illuminated;
	NSTimer* statusTimer;
	IOBluetoothUserNotification *disconnectNotification;
	BOOL buttonState[28];
	
	
	
	//nunchuk
	unsigned char nStickX;
	unsigned char nStickY;
	unsigned char nAccX;
	unsigned char nAccY;
	unsigned char nAccZ;
	unsigned char nButtonData;
	
	//classic controller
	unsigned char cButtonData;
	unsigned char cStickX1;
	unsigned char cStickY1;
	unsigned char cStickX2;
	unsigned char cStickY2;
	unsigned char cAnalogL;
	unsigned char cAnalogR;
	
} 
- (NSString*) address;
- (void)setDelegate:(id)delegate;
- (double)batteryLevel;

- (WiiExpansionPortType)expansionPortType;
- (BOOL)isExpansionPortAttached;
- (BOOL)available;
- (BOOL)isButtonPressed:(WiiButtonType)type;
- (WiiJoyStickCalibData)joyStickCalibData:(WiiJoyStickType)type;
- (WiiAccCalibData)accCalibData:(WiiAccelerationSensorType)type;

- (IOReturn)connectTo:(IOBluetoothDevice*)device;
- (IOReturn)closeConnection;
- (IOReturn)requestUpdates;
- (IOReturn)setIRSensorEnabled:(BOOL)enabled;
- (IOReturn)setForceFeedbackEnabled:(BOOL)enabled;
- (IOReturn)setMotionSensorEnabled:(BOOL)enabled;
- (IOReturn)setExpansionPortEnabled:(BOOL)enabled;
- (IOReturn)setLEDEnabled1:(BOOL)enabled1 enabled2:(BOOL)enabled2 enabled3:(BOOL)enabled3 enabled4:(BOOL)enabled4;

@end

@interface NSObject( WiiRemoteDelegate )

- (void) irPointMovedX:(float)px Y:(float)py wiiRemote:(WiiRemote*)wiiRemote;
- (void) rawIRData: (IRData[4])irData wiiRemote:(WiiRemote*)wiiRemote;
- (void) buttonChanged:(WiiButtonType)type isPressed:(BOOL)isPressed wiiRemote:(WiiRemote*)wiiRemote;
- (void) accelerationChanged:(WiiAccelerationSensorType)type accX:(unsigned char)accX accY:(unsigned char)accY accZ:(unsigned char)accZ wiiRemote:(WiiRemote*)wiiRemote;
- (void) joyStickChanged:(WiiJoyStickType)type tiltX:(unsigned char)tiltX tiltY:(unsigned char)tiltY wiiRemote:(WiiRemote*)wiiRemote;
- (void) analogButtonChanged:(WiiButtonType)type amount:(unsigned)press wiiRemote:(WiiRemote*)wiiRemote;
- (void) wiiRemoteDisconnected:(IOBluetoothDevice*)device;


//- (void) dataChanged:(unsigned short)buttonData accX:(unsigned char)accX accY:(unsigned char)accY accZ:(unsigned char)accZ mouseX:(float)mx mouseY:(float)my;


@end
