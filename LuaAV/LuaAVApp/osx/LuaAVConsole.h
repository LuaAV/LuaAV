#ifndef _LUAAV_CONSOLE_H_
#define _LUAAV_CONSOLE_H_ 1


/*
	The main LuaAV console window
*/

#import <Cocoa/Cocoa.h>

#include "luaav.h"


AL_API int luaopen_app_console(lua_State * L);

/*
	Utility object to store script data for the scriptsTable NSTableView
*/
@interface ScriptData : NSObject {
    NSString        * name;
    NSString        * path;
	luaav_state		* state;
}
- initWithName:(NSString *)newName withPath:(NSString *)newPath withObject:(luaav_state*)ptr;
- (void)setName:(NSString *)newName;
- (void)setPath:(NSString *)newPath;
- (NSString *)name;
- (NSString *)path;
- (luaav_state *)state;
@end

///*
//	Extended the scripts table view to accept drag-n-drop
//*/
//@interface ScriptsTableView : NSTableView
//{
//
//}
//- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender;
//- (void)draggingEnded:(id <NSDraggingInfo>)sender;
//- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender;
//- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender;
//- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender;
//- (id)initWithFrame:(NSRect)frame;
//- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)flag;
//@end


@interface LuaAVConsole : NSObject {
	
	IBOutlet id luaOutputTextView;
	bool isLogging;
	bool isLuaOutputTextViewScrolling;
	
	NSString * outputLogFile;
	
	IBOutlet id luaInputTextView;
	IBOutlet id luaStatusTextField;
	IBOutlet id LuaAVWindow;
	
	IBOutlet NSTableView * scriptsTable;
    NSMutableArray       * scripts;  // the array the scriptsTable will use
	int					   selectedScript;
}

// interface actions:
- (IBAction)clearLoggingWindow: (id)sender;
- (IBAction)toggleScrollingLoggingWindow: (id)sender;
- (IBAction)help: (id)sender;


- (void)scriptCall:(id)sender withName:(const char *)name;
- (IBAction)closeScript:(id)sender;
- (IBAction)reloadScript:(id)sender;
- (IBAction)editScript:(id)sender;
- (void)scriptsTableDoubleClick;
- (IBAction)tableViewSelected:(id)sender;
- (IBAction)executeText:(id)sender;

- (void)postError:(NSString *)text;
- (void)stdoutRedirect:(NSString *)text;
- (void)stderrRedirect:(NSString *)text;

// called on awakeFromNIB
- (void)initScriptview;

// get static ptr to singleton console
+ (id)console;

// public methods:
- (id)window;
- (void)copyText;
- (void)selectAllText;
- (void)startLogging:(const char *)path;
- (void)stopLogging;
- (void)statusMessage: (const char *)message;
- (void)clearLog;
- (BOOL)isLogScrolling;
- (void)setLogScrolling:(BOOL)scrolling;

- (void)addScript:(NSString *)name withPath:(NSString *)path withObject:(luaav_state *)ptr;


@end

#endif	//_LUAAV_CONSOLE_H_