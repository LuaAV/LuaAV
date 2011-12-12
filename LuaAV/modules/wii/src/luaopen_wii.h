#ifndef INC_LUAOPEN_WII_H
#define INC_LUAOPEN_WII_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

extern int luaopen_wii(lua_State * L);

#ifdef __cplusplus
}
#endif

#import <Cocoa/Cocoa.h>
#import <WiiRemote/WiiRemote.h>
#import <WiiRemote/WiiRemoteDiscovery.h>

// this will be a Lua userdata
@interface WiiHandler : NSObject {
	WiiRemoteDiscovery * discovery;
@public
	WiiRemote * wii;
	// all the buttons:
	/*
		0: A
		1: trigger
		2: button 1
		3: button 2
		4: minus
		5: home
		6: plus
		7: up
		8: down
		9: left
		10: right
		11: nunchuck Z (lower trigger)
		12: nunchuck c (upper trigger)
	*/
	bool buttons[16];
	int lastbutton;
	// nunchuck joystick:
	double jX, jY;
	// accelerometer:
	double roll, pitch;
	// ir point (also raw ir data is available, but difficult to identify?)
	double pX, pY;
}

- (void) getData;
- (void) WiiRemoteDiscovered:(WiiRemote*)wiimote;
- (void) WiiRemoteDiscoveryError:(int)code;
- (void) irPointMovedX:(float)px Y:(float)py wiiRemote:(WiiRemote*)wiiRemote;
- (void) rawIRData: (IRData[4])irData wiiRemote:(WiiRemote*)wiiRemote;
- (void) buttonChanged:(WiiButtonType)type isPressed:(BOOL)isPressed wiiRemote:(WiiRemote*)wiiRemote;
- (void) accelerationChanged:(WiiAccelerationSensorType)type accX:(unsigned char)accX accY:(unsigned char)accY accZ:(unsigned char)accZ wiiRemote:(WiiRemote*)wiiRemote;
- (void) joyStickChanged:(WiiJoyStickType)type tiltX:(unsigned char)tiltX tiltY:(unsigned char)tiltY wiiRemote:(WiiRemote*)wiiRemote;
- (void) analogButtonChanged:(WiiButtonType)type amount:(unsigned)press wiiRemote:(WiiRemote*)wiiRemote;
- (void) wiiRemoteDisconnected:(IOBluetoothDevice*)device;

@end


#endif
