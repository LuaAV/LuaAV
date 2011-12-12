//
//  navigator.h
//  navigator
//
//  Created by GrahamWakefield on 10/22/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <3DConnexionClient/ConnexionClientAPI.h>
#include "lua_navigator.h"

@interface Navigator : NSObject 
{
	UInt16			fConnexionClientID;
	lua_State *		L;
	
	ConnexionDeviceState cachedState;
}

- (id)initWithLuaState: (lua_State *)L;

void TestMessageHandler(io_connect_t connection, natural_t messageType, void *messageArgument);

@end
