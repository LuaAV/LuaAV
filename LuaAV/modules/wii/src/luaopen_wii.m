#import "luaopen_wii.h"

@implementation WiiHandler
//
- (id)init {
	if (self = [super init]){
		wii = 0;
		roll = pitch = 0.;
		jX = jY = 0.;
		lastbutton = -1;
		
		discovery = [WiiRemoteDiscovery discoveryWithDelegate:self];
		[discovery start];
		
//		//accelerometerReadingType = ACCELEROMETER_IN_ANGLE;
//	
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(expansionPortChanged:)
													 name:@"WiiRemoteExpansionPortChangedNotification"
												   object:nil];
												   
		printf("allocated WiiHandler %d\n", [self retainCount]);
	}
	return self;
}

- (void) close {
	if (wii) {
		[[NSNotificationCenter defaultCenter] removeObserver:self];
		[wii closeConnection];
		[wii release];
	}
	wii = 0;
}

- (void) dealloc {
	printf("dealloc WiiHandler %d\n", [self retainCount]);
	[discovery stop];
	[discovery release];
	[super dealloc];
}

-(void)getData {
	[wii requestUpdates];
}

// ---- wiiremote discovery delegate methods 
- (void) WiiRemoteDiscovered:(WiiRemote *)wiimote {
	wii = wiimote;
	printf("wiimote %p discovered\n", wiimote);
	[discovery stop];
	
	[wii setDelegate:self];
	[wii setMotionSensorEnabled:NO];
	[wii setIRSensorEnabled:YES];
	[wii setLEDEnabled1:NO enabled2:YES enabled3:NO enabled4:NO];
	printf("battery level %f\n", [wii batteryLevel]);


////	//isConnected[wiimoteArraySlot] = 1;
////	
////	switch(wiimoteArraySlot) {
////		case 0: [[wiimotes objectAtIndex:wiimoteArraySlot] setLEDEnabled1:YES enabled2:NO enabled3:NO enabled4:NO]; break;
////		case 1: [[wiimotes objectAtIndex:wiimoteArraySlot] setLEDEnabled1:NO enabled2:YES enabled3:NO enabled4:NO]; break;
////		case 2: [[wiimotes objectAtIndex:wiimoteArraySlot] setLEDEnabled1:NO enabled2:NO enabled3:YES enabled4:NO]; break;
////		case 3: [[wiimotes objectAtIndex:wiimoteArraySlot] setLEDEnabled1:NO enabled2:NO enabled3:NO enabled4:YES]; break;			
////	}
	
	if ([wii isExpansionPortAttached]){
		[wii setExpansionPortEnabled:YES];
	}
}

- (void) WiiRemoteDiscoveryError:(int)code{
	printf("WiiRemoteDiscovery error (%d)\n", code);
	[discovery stop];
	// try again:
	//[discovery start];
}

// ---- wiiremote delegate methods


- (void)expansionPortChanged:(NSNotification *)nc{
	WiiRemote* w = (WiiRemote*)[nc object];
	if ([w isExpansionPortAttached]){
		[w setExpansionPortEnabled:YES];
		printf("expansion port enabled\n");	
	} else {
		[w setExpansionPortEnabled:NO];
		printf("expansion port disabled\n");
	}	
}

- (void) irPointMovedX:(float)px Y:(float)py wiiRemote:(WiiRemote*)wiiRemote {
	//printf("ir %f %f\n", px, py);
	pX = px; pY = py; // gives nan if the point isn't valid.
}
- (void) rawIRData: (IRData[4])irData wiiRemote:(WiiRemote*)wiiRemote {
//	int i;
//	for (i=0; i<4; i++) {
//		printf("%i %i %i\n", irData[i].x, irData[i].y, irData[i].s);
//	}
}
- (void) buttonChanged:(WiiButtonType)type isPressed:(BOOL)isPressed wiiRemote:(WiiRemote*)wiiRemote {
	lastbutton = type;
	buttons[lastbutton] = isPressed;
	//printf("%i %i\n", type, isPressed);	
}
- (void) accelerationChanged:(WiiAccelerationSensorType)type 
			accX:(unsigned char)accX 
			accY:(unsigned char)accY 
			accZ:(unsigned char)accZ 
			wiiRemote:(WiiRemote*)wiiRemote {
			
	WiiAccCalibData data;
	bool isremote = (type == WiiRemoteAccelerationSensor);
	if (true) {
	
		data = isremote ? [wii accCalibData:WiiRemoteAccelerationSensor] : [wii accCalibData:WiiNunchukAccelerationSensor];
		data = [wii accCalibData:WiiRemoteAccelerationSensor];
		
		// this isn't quite right... it doesn't capture wii when it is upside down:
		double ax = (double)(accX - data.accX_zero) / (data.accX_1g - data.accX_zero);
		double ay = (double)(accY - data.accY_zero) / (data.accY_1g - data.accY_zero);
		//double az = (double)(accZ - z0) / (z1 - z0) * (-1.0);
		
		roll = atan(ax) * 4. / 3.14;
		pitch = atan(ay) * 4. / 3.14;

		//printf("%i %f %f\n", isremote, roll, pitch);
	}
}
- (void) joyStickChanged:(WiiJoyStickType)type tiltX:(unsigned char)tiltX tiltY:(unsigned char)tiltY wiiRemote:(WiiRemote*)wiiRemote {

	if (type == WiiNunchukJoyStick) {
		static double max = (double)0xE0;	
		static double center = (double)0x80;
		double scale = 1.0 / (max - center);
		
		jX = (((double)tiltX) - center) * scale;
		jY = (((double)tiltY) - center) * scale;		
	}
}
- (void) analogButtonChanged:(WiiButtonType)type amount:(unsigned)press wiiRemote:(WiiRemote*)wiiRemote {
	printf("analog\n");
}
- (void) wiiRemoteDisconnected:(IOBluetoothDevice*)device {
	printf("Wii disconnected\n");
	wii = 0;
	
	[discovery start];
	printf("Press the 1 and 2 buttons on the Wiimote simultaneously.\n\n");
}

-(int) button:(lua_State *)L {
	if (lua_isnumber(L, -1)) {
		// polling current state:
		int which = (lua_tointeger(L, -1) - 1) % 16;
		lua_pushboolean(L, buttons[which]);
		return 1;
	} else if (lastbutton >= 0) {
		// consume event:
		lua_pushinteger(L, lastbutton + 1);
		lua_pushboolean(L, buttons[lastbutton]);
		lastbutton = -1;
		return 2;
	} 
	// no event to consume:
	return 0;
}

-(int) joystick:(lua_State *)L {
	lua_pushnumber(L, jX);
	lua_pushnumber(L, jY);
	return 2;
}

-(int) tilt:(lua_State *)L {
	lua_pushnumber(L, pitch);
	lua_pushnumber(L, roll);
	return 2;
}

@end

static int wii_create(lua_State * L) {
	WiiHandler ** u = (WiiHandler **)lua_newuserdata(L, sizeof(WiiHandler **));
	WiiHandler * anwii = [[WiiHandler alloc] init];
	*u = anwii;
	luaL_getmetatable(L, "Wii");
	lua_setmetatable(L, -2);
	return 1;
}

static WiiHandler * wii_get(lua_State * L) {
	WiiHandler ** u = luaL_checkudata(L, 1, "Wii");
	return u ? *u : 0;
}

static int wii_gc(lua_State * L) {
	WiiHandler * x = wii_get(L);
	[x close];
	[x release];
	return 0;
}

static int wii_tostring(lua_State * L) {
	WiiHandler * x = wii_get(L);
	lua_pushfstring(L, "Wiimote %p\n", x);
	return 1;
}

static int wii_button(lua_State * L) {
	WiiHandler * x = wii_get(L);
	if (x == 0) return 0;
	return [x button:L];
}

static int wii_joystick(lua_State * L) {
	WiiHandler * x = wii_get(L);
	if (x == 0) return 0;
	return [x joystick:L];
}

static int wii_tilt(lua_State * L) {
	WiiHandler * x = wii_get(L);
	if (x == 0) return 0;
	return [x tilt:L];
}

static int wii_battery(lua_State * L) {
	WiiHandler * x = wii_get(L);
	if (x == 0 || x->wii == 0) return 0;
	lua_pushnumber(L, [x->wii batteryLevel]);
	return 1;
}

static void gc_sentinel(lua_State * L, int idx, lua_CFunction callback) {
	lua_pushvalue(L, idx); // value @idx
	lua_newuserdata(L, sizeof(void *)); // sentinel userdata
		lua_newtable(L);	// userdata metatable with __gc = callback
		lua_pushcfunction(L, callback);
		lua_setfield(L, -2, "__gc");
		lua_setmetatable(L, -2);

	/* check for (weak-valued) sentinel table; create if needed */
	lua_getfield(L, LUA_REGISTRYINDEX, "gc_sentinels");
	if (lua_isnoneornil(L, -1)) {
		lua_pop(L, 1);
		lua_newtable(L);

		// make weak-keyed
		lua_pushstring(L, "v");
		lua_setfield(L, -2, "__mode");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_pushvalue(L, -1);
		lua_setmetatable(L, -2);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, "gc_sentinels");
	}

	lua_insert(L, -3);
	lua_insert(L, -2);
	lua_settable(L, -3); // lua::sentinel[value @idx] = sentinel userdata
	lua_pop(L, 1); // lua::sentinel
}

int luaclose_wii(lua_State * L) {
	
	return 0;
}

int luaopen_wii(lua_State * L)
{
	const char * libname = lua_tostring(L, 1);	
	static const luaL_reg lib[] = 
	{
		{"create", wii_create },
		{NULL, NULL}
	};
	luaL_register(L, libname, lib);
	
	gc_sentinel(L, -1, luaclose_wii);
	
	luaL_newmetatable(L, "Wii");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, wii_gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, wii_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pushcfunction(L, wii_button);
	lua_setfield(L, -2, "button");
	lua_pushcfunction(L, wii_joystick);
	lua_setfield(L, -2, "joystick");
	lua_pushcfunction(L, wii_tilt);
	lua_setfield(L, -2, "tilt");
	lua_pushcfunction(L, wii_battery);
	lua_setfield(L, -2, "battery");
	lua_pop(L, 1);
	
	return 1;
}
