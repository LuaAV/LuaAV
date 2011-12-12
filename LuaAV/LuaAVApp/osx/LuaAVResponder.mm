#import "LuaAVResponder.h"
#import "ModulesTableView.h"

#include "luaav.h"
#include "lua_utility.h"
#include "luaav_app.h"
#include "luaav_app_menu.h"
#include "LuaAVConsole.h"

@implementation LuaAVResponder
@synthesize localRepos, remoteRepos, modules;
static LuaAVResponder * singleton = nil;

+ (id)responder {
	return singleton;
}

/*!
	Called right when the application is launched and is minimally loaded into memory
*/
- (void)awakeFromNib {
	if(!singleton) {
		localRepos = [[NSMutableArray alloc] init];
		remoteRepos = [[NSMutableArray alloc] init];
		modules = [[NSMutableArray alloc] init];
		[NSApp setDelegate:self];
		singleton = self;

		[[LuaAVConsole console] initScriptview];

		luaav_app_init(NULL);
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification {
	NSTableView *tv = [aNotification object];
	int selectedRow = [tv selectedRow];
	NSDictionary *repo = nil;
	lua_State * L = luaav_getstate(luaav_app_state());
	
	switch([tv tag]) {
		case LOCAL_TABLE:
			//		
			break;
			
		case REMOTE_TABLE:
			repo = [remoteRepos objectAtIndex:selectedRow];
			lua_getglobal(L, "rocksmanager_search");
			if (lua_type(L, -1) == LUA_TFUNCTION) {
				NSString *name = [repo valueForKey:@"name"];
				const char *cname = [name cStringUsingEncoding: NSASCIIStringEncoding];
				lua::push<const char *>(L, cname);
				lua_pcall(L, 1, 0, 0);
			}
			else {
				lua_pop(L, 1);
			}
			break;
		case MODULES_TABLE:
			//
			break;
		default:
			break;
	}
}

- (IBAction)addModule:(id)sender {
	NSDictionary *d = [NSDictionary dictionaryWithObjectsAndKeys:@"Test", @"name", @"GPL", @"license", @"this is a module", @"description", @"LuaAV", @"repository", @"1.0", @"version", nil];
    [modulesController addObject:d];
}

- (void)addModuleDictionary:(NSDictionary *)d {
	[modulesController addObject:d];
}

- (void)clearModuleList {
	[modules removeAllObjects];
}

- (IBAction)installModule:(id)item {
	lua_State * L = luaav_getstate(luaav_app_state());
	lua_getglobal(L, "rocksmanager_install");
	if (lua_type(L, -1) == LUA_TFUNCTION) {
		NSArray *selection = [modulesController selectedObjects];
		NSDictionary *repo = [remoteRepos objectAtIndex:[remoteTableView selectedRow]];
		NSDictionary *module = [selection objectAtIndex:0];
		NSString *repoName = [repo valueForKey:@"name"];
		NSString *name = [module valueForKey:@"name"];
		NSString *version = [module valueForKey:@"version"];
		
		const char *cRepoName = [repoName cStringUsingEncoding: NSASCIIStringEncoding];
		const char *cName = [name cStringUsingEncoding: NSASCIIStringEncoding];
		const char *cVersion = [version cStringUsingEncoding: NSASCIIStringEncoding];
		lua::push<const char *>(L, cRepoName);
		lua::push<const char *>(L, cName);
		lua::push<const char *>(L, cVersion);
		lua_pcall(L, 3, 0, 0);
	}
	else {
		lua_pop(L, 1);
	}
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	luaav_app_quit();
}

// from open dialog
- (IBAction)openFile:(id)sender {
	luaav_app_menuitem_action("(File).(Open...)");
}

// from new dialog
- (IBAction)newFile:(id)sender {
	luaav_app_menuitem_action("(File).(New...)");
}


- (IBAction)showRocksWindow: (id)sender {
	[NSBundle loadNibNamed:@"RocksWindow" owner:self];
	[rocksWindow makeKeyAndOrderFront:self];

	// setup repositories
	NSMutableDictionary *d1 = [
		NSMutableDictionary dictionaryWithObjectsAndKeys:
		@"LuaAV", @"name", 
	nil];
	NSMutableDictionary *d2 = [
		NSMutableDictionary dictionaryWithObjectsAndKeys:
		@"LuaRocks", @"name", 
	nil];
	[remoteController addObject:d1];
	[remoteController addObject:d2];
	
	NSMutableDictionary *ld1 = [
		NSMutableDictionary dictionaryWithObjectsAndKeys:
		@"LuaAV App", @"name", 
	nil];
	[localController addObject:ld1];
}

// from menus
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename {
	[self loadFile:filename];
	return YES;
}

- (void)loadFile:(NSString *)file {
	const char *filename = [file cStringUsingEncoding: NSASCIIStringEncoding];
	
	lua_State * L = luaav_getstate(luaav_app_state());
	lua_getglobal(L, "fileopen");
	if (lua_type(L, -1) == LUA_TFUNCTION) {
		lua::push<const char *>(L, filename);
		lua_pcall(L, 1, 0, 0);
	}
	else {
		lua_pop(L, 1);
	}
}

- (void)dispatch:(NSMenuItem *)item {
	luaav::MenuItem *mi = luaav::MenuItem::itemForPtr((void *)item);
	if(!mi) return;
	mi->dispatch();
}

- (void)keyDown:(NSEvent *)theEvent {
	printf("KeyDown\n");
}

- (void)setMenu:(NSMenu *)aMenu  {
	menu = aMenu;
}

- (NSMenu *)menu {
	return menu;
}

@end
