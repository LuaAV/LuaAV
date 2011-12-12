#ifndef _LUAAV_RESPONDER_H_
#define _LUAAV_RESPONDER_H_ 1

/*!
	\class LuaAVResponder

	Transfers handlers from Cocoa application to methods in LuaAV.h / LuaAV.lua
	
	Application lifetime:
		luaav::init()
		luaav::quit()
	
	File (script) operations:
		luaav::fileopen()
		luaav::fileclose()
		
	App menu callbacks
		luaav::dispatch()
*/

enum {
	LOCAL_TABLE, REMOTE_TABLE, MODULES_TABLE
};

#import <Cocoa/Cocoa.h>
#import <AppKit/NSNibLoading.h>

@class ModulesTableView;

@interface LuaAVResponder : NSResponder <NSTableViewDelegate> {
	NSMenu * menu;
	NSWindow *window;
	NSMutableArray *localRepos;
	NSMutableArray *remoteRepos;
	NSMutableArray *modules;
	IBOutlet NSArrayController *localController;
	IBOutlet NSArrayController *remoteController;
	IBOutlet NSArrayController *modulesController;
	IBOutlet NSView *repoListView;
	IBOutlet NSWindow *rocksWindow;
	IBOutlet ModulesTableView *modulesTableView;
	IBOutlet NSTableView *remoteTableView;
}
@property (retain) NSMutableArray *localRepos, *remoteRepos, *modules;

+ (id)responder;
- (void)addModuleDictionary:(NSDictionary *)d;
- (void)clearModuleList;
- (IBAction)installModule:(id)item;

- (IBAction)addModule:(id)sender;

/*!
	Open a file using the OpenPanel dialog.
*/
- (IBAction)openFile: (id)sender;

/*!
	Create (and open) a new file.
*/
- (IBAction)newFile: (id)sender;

/*!
	Create (and open) a new file.
*/
- (IBAction)showRocksWindow: (id)sender;

/*!
	Open a file from the File > Recent menu
*/
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename;

/*!
	Actually load the file
*/
- (void)loadFile:(NSString *)file;

/*!
	Called when the app is quitting
*/
- (void)applicationWillTerminate:(NSNotification *)aNotification;

/*!
	Resolves menu actions stored as lua coroutines in the menu items
*/
- (void)dispatch:(NSMenuItem *)item;

/*!
	Menu handler
*/
- (void)setMenu:(NSMenu *)aMenu;
- (NSMenu *)menu;

@end

#endif	//_LUAAV_RESPONDER_H_