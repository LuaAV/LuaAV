#import "navigator.h"

// this should be a userdata, not a global!
static Navigator * gNav = nil;

@implementation Navigator

- (id)initWithLuaState:(lua_State *)L
{
	int i;
	if (self = [super init]) {
		gNav = self;
		
		OSErr	error;
	
		// Make sure the framework is installed
		if(InstallConnexionHandlers != NULL)
		{
			// Install message handler and register our client
			if (InstallConnexionHandlers(TestMessageHandler, 0L, 0L)) {	
				printf("could not install 3dconnexion handler\n");
			} else {
				// This takes over in our application only
				//fConnexionClientID = RegisterConnexionClient('MCTt', "\p3DxClientTest", kConnexionClientModeTakeOver, kConnexionMaskAll);
				
				// This takes over system-wide
				fConnexionClientID = RegisterConnexionClient(kConnexionClientWildcard, 0L, kConnexionClientModeTakeOver, kConnexionMaskAll);
				
				printf("3dconnxexion id: %i\n", fConnexionClientID);
			}
		}
	}
	return self;
}

void TestMessageHandler(io_connect_t connection, natural_t messageType, void *messageArgument)
{
	static ConnexionDeviceState	lastState;
	ConnexionDeviceState		*state;
	Navigator * nav = gNav;
	//printf("msg\n");
	
	switch(messageType)
	{
		case kConnexionMsgDeviceState:
			state = (ConnexionDeviceState*)messageArgument;
			if(state->client == nav->fConnexionClientID)
			{
               BlockMoveData(state, &nav->cachedState, (long)sizeof(ConnexionDeviceState));
			}
			break;

		default:
			// other messageTypes can happen and should be ignored
			break;
	}
}

@end

int poll(lua_State * L) {
	ConnexionDeviceState * state = &gNav->cachedState;
	lua_pushnumber(L, (double)state->axis[0]);
	lua_pushnumber(L, (double)state->axis[1]);
	lua_pushnumber(L, (double)state->axis[2]);
	lua_pushnumber(L, (double)state->axis[3]);
	lua_pushnumber(L, (double)state->axis[4]);
	lua_pushnumber(L, (double)state->axis[5]);
	lua_pushboolean(L, (state->buttons & 1));
	lua_pushboolean(L, (state->buttons & 2));
	return 8;
}

int luaopen_navigator(lua_State *L)
{	
	static const luaL_reg lib[] = 
	{
		{"poll", poll },
		{NULL, NULL}
	};
	
	luaL_register(L, lua_tostring(L, -1), lib);
	gNav = [[Navigator alloc] initWithLuaState: L];
	
	return 1;
}