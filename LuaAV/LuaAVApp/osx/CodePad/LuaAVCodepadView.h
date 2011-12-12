#include "luaav_app_codepad.h"
#include "luaav.h"
#include "lua_utility.h"
#include "luaav_app.h"

#import <Cocoa/Cocoa.h>
#import "MarkerLineNumberView.h"
#import "LuaAVTextView.h"

@interface LuaAVCodepadView : NSView {
	// name and label
	NSString				*_name;
	NSString				*_label;
	
	// text highlighting
	NSMutableDictionary		*formats;
	
	// layout, text and line numbers
	NSScrollView			*scrollView;
	LuaAVTextView			*_textView;
	NSTextStorage			*textStorage;
	MarkerLineNumberView	*lineNumberView;
	
	// implementation class for sending messages back to Lua
	luaav::CodePadImpl		*_parent;
}

@property (assign) NSMutableDictionary *formats;

// create
- (id)initWithFrame:(NSRect)frame name:(NSString *)aName;

// properties
- (NSString *)label;
- (NSString *)name;
- (void)setName:(NSString *)aName;
- (LuaAVTextView *)textView;
- (luaav::CodePadImpl *)parent;
- (void)setParent:(luaav::CodePadImpl *)p;
- (void)setText:(const char *)text;
- (NSString *)getText;

// functionality
- (void)format:(const char *)aName fromStart:(int)start toEnd:(int)end;
- (void)insertText:(const char *)text atPos:(int)pos;
- (int)cursorPos;
- (void)eval;
- (void)getSelectionWithLine:(int *)line1 pos:(int *)pos1 line:(int *)line2 pos:(int *)pos2;
- (void)detach;

@end

