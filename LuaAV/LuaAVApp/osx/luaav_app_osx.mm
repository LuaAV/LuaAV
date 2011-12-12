#include "luaav_app.h"
#include "luaav_app_audiodriver.h"
#include "luaav_app_menu.h"
#include "luaav_app_screens.h"
#include "luaav_app_window.h"
#include "luaav_filewatcher.h"

#import <Cocoa/Cocoa.h>
#import "LuaAVConsole.h"
#import "LuaAVResponder.h"

#include <string>

void luaav_app_paths_init(
	std::string &app_path,
	std::string &app_resource_path,
	std::string &app_home_path,
	std::string &app_config_path,
	std::string &app_log_path)
{
	// path to application bundle:
	NSString *NSappPath = [[NSBundle mainBundle] executablePath];
	NSappPath = [NSappPath stringByResolvingSymlinksInPath];
	NSArray *bundlePathComponents = [NSappPath componentsSeparatedByString:@"/Contents"];
	NSappPath = [bundlePathComponents objectAtIndex:0];
	bundlePathComponents = [NSappPath pathComponents];
	NSappPath = [NSappPath substringToIndex: ([NSappPath length] - [[bundlePathComponents lastObject] length])];
	if([NSappPath length] > 1) {
		NSappPath = [NSappPath substringToIndex: [NSappPath length]-1];
	}
	
	// config path
	NSArray *configpaths = NSSearchPathForDirectoriesInDomains(	NSApplicationSupportDirectory, 
															NSUserDomainMask, 
															YES);
	NSString *configpath = [configpaths objectAtIndex:0];
	if(configpath) {
		configpath = [configpath stringByAppendingPathComponent: [[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString *)kCFBundleExecutableKey]];
	}
	else {
		configpath = @"~/Library/Application Support/LuaAV/";
		configpath = [configpath stringByExpandingTildeInPath];
	}
	// create a log file:
	NSFileManager * fileManager = [NSFileManager defaultManager];
	// make sure the Application Support folder exists
	if ([fileManager fileExistsAtPath: configpath] == NO) {
		[fileManager createDirectoryAtPath: configpath attributes: nil];
	}
	// get or create the log file
	NSString * outputLogFile = [[configpath stringByAppendingPathComponent:@"LuaAV.log"] stringByExpandingTildeInPath];
	
	app_path.assign( [NSappPath cStringUsingEncoding: NSASCIIStringEncoding] );
	app_resource_path.assign([[[NSBundle mainBundle] resourcePath] cStringUsingEncoding: NSASCIIStringEncoding]);
	app_home_path.assign([NSHomeDirectory() cStringUsingEncoding: NSASCIIStringEncoding]); 
	app_config_path.assign([configpath cStringUsingEncoding: NSASCIIStringEncoding]);
	app_log_path.assign([outputLogFile cStringUsingEncoding: NSASCIIStringEncoding]);
}

int quit(lua_State * L) {
	[NSApp terminate:0];
	return 0;
}	

int app_openpanel(lua_State *L) {
	NSString *dir = nil;
	if(lua_type(L, 1) == LUA_TSTRING) {
		dir = [NSString stringWithCString:lua_tostring(L, 1) encoding:NSASCIIStringEncoding];
	}
	
	NSOpenPanel *opanel = [NSOpenPanel openPanel];
	[opanel setAllowsMultipleSelection:YES];
	int returnCode = [opanel runModalForDirectory:dir file:nil types:nil];

	if(returnCode == NSOKButton) {
		NSArray *filenames = [opanel filenames];
		
		lua_newtable(L);
		for(int i=0; i < [filenames count]; i++) {
			NSString *file = [filenames objectAtIndex:i];
			const char *filepath = [file cStringUsingEncoding: NSASCIIStringEncoding];
			lua::push<const char *>(L, filepath);
			lua_rawseti(L, -2, i+1);
		}
	}
	else {
		lua_pushnil(L);
	}
	
	return 1;
}

int app_savepanel(lua_State *L) {
	NSString *dir = nil;
	NSString *title = [NSString stringWithCString:"Save" encoding:NSASCIIStringEncoding];
	if(lua::is<const char *>(L, 1)) {
		dir = [NSString 
				stringWithCString:lua::to<const char *>(L, 1) 
				encoding:NSASCIIStringEncoding
			];
	}
	if(lua::is<const char *>(L, 2)) {
		title = [NSString 
				stringWithCString:lua::to<const char *>(L, 2) 
				encoding:NSASCIIStringEncoding
			];
	}
	
	
	NSSavePanel *spanel = [NSSavePanel savePanel];
	[spanel setTitle: title];
	[spanel setCanCreateDirectories:YES];
	[spanel setCanSelectHiddenExtension:YES];
	NSArray * types = [NSArray arrayWithObject: @"lua"];
	[types retain];
	[spanel setAllowedFileTypes: types];
	[spanel setAllowsOtherFileTypes:YES];
	
	int result = [spanel runModal];
	if (result == NSOKButton) {
		NSString * filename = [[spanel URL] path];
		const char *filepath = [filename cStringUsingEncoding: NSASCIIStringEncoding];
		lua::push<const char *>(L, filepath);
	} else {
		lua_pushnil(L);
	}
	[types release];
	return 1;
}

int app_openurl(lua_State *L) {
	if(lua::is<const char *>(L, 1)) {
		NSURL *url = [NSURL
						URLWithString: [NSString 
											stringWithCString:lua::to<const char *>(L, 1) 
											encoding:NSASCIIStringEncoding
										]
									];
		NSArray *urls = [NSArray arrayWithObject: url];
		[[NSWorkspace sharedWorkspace] 
				openURLs:urls 
				withAppBundleIdentifier:nil 
				options:NSWorkspaceLaunchDefault 
				additionalEventParamDescriptor:[NSAppleEventDescriptor nullDescriptor]
				launchIdentifiers:nil];
	}
	else {
		luaL_error(L, "app.openurl: invalid url");
	}
	return 0;
}

int app_clipboard(lua_State *L) {
	if(lua::is<const char *>(L, 1)) {
		NSString *val = [NSString 
							stringWithCString:lua::to<const char *>(L, 1) 
							encoding:NSASCIIStringEncoding
						];
		NSPasteboard *pb = [NSPasteboard generalPasteboard];
		[pb declareTypes:[NSArray arrayWithObject: NSStringPboardType] owner:nil];
		BOOL res = [pb setString:val forType:NSStringPboardType];
		if(!res) {
			fprintf(stderr, "Failed to copy data to the clipboard\n");
		}
	}
	else {
		NSString *val = [[NSPasteboard generalPasteboard] stringForType:NSStringPboardType];
		if(val) {
			lua::push<const char *>(L, [val cStringUsingEncoding: NSASCIIStringEncoding]);
			return 1;
		}
	}
	return 0;
}

int app_window_event(lua_State *L) {
	if(lua::is<const char *>(L, 1)) {
		NSString *name = [NSString 
							stringWithCString:lua::to<const char *>(L, 1) 
							encoding:NSASCIIStringEncoding
						];
		SEL method = NSSelectorFromString(name);
		id win = [[NSApplication sharedApplication] keyWindow];
		
		if([win respondsToSelector:method]) {
			[win performSelector:method];
		}
	}
	else {
		luaL_error(L, "app.window_event: invalid event name");
	}
	return 0;
}

NSString * makeString(const char *s) {
	return [NSString 
		stringWithCString:s 
		encoding:NSASCIIStringEncoding
	];
}

int app_add_module(lua_State *L) {
	[[LuaAVResponder responder]
		addModuleDictionary:
		[NSDictionary dictionaryWithObjectsAndKeys:
			makeString(lua::optfield<const char *>(L, 1, "<No Name>", "name")), @"name", 
			makeString(lua::optfield<const char *>(L, 1, "<No License>", "license")), @"license", 
			makeString(lua::optfield<const char *>(L, 1, "<No Description>", "description")), @"description", 
			makeString(lua::optfield<const char *>(L, 1, "<No Repsoitory>", "repository")), @"repository", 
			makeString(lua::optfield<const char *>(L, 1, "<No Version>", "version")), @"version", 
		nil]
	];
	return 0;
}

int app_clear_modules(lua_State *L) {
	[[LuaAVResponder responder] clearModuleList];
	return 0;
}