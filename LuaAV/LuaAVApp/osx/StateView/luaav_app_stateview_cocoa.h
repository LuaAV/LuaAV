#import <Cocoa/Cocoa.h>
#import "LuaAVNSViewCategory.h"

namespace luaav {

class StateViewImpl;

}	// luaav::

@interface StateViewCocoa : NSWindow {
	NSOutlineView			*outlineView;
	NSTextView              *codeView;	
	NSScrollView            *codeScrollView, *outlineScrollView;
	NSClipView              *codeClipView,   *outlineClipView;
	NSSplitView             *splitView;
	NSTableColumn           *keyColumn, *valueColumn, *typeColumn;
	NSMutableArray			*root;
	
	BOOL					nameIsAscending, typeIsAscending, valueIsAscending;

	// implementation class for sending messages back to Lua
	luaav::StateViewImpl	*_parent;
}


- (id)initWithContentRect:(NSRect)contentRect screen:(NSScreen *)screen;
- (NSMutableDictionary *)findNode:(NSString *)name withList:(NSMutableArray *)list;
- (NSMutableArray *)followPath:(NSArray *)path;
- (void)setPath:(NSArray *)path forValue:(NSString *)value forType:(NSString *)type;
- (void)updateDetail;
- (void)setParent:(luaav::StateViewImpl *)parent;

@property (assign) NSMutableArray      *root;
@property (assign) NSOutlineView       *outlineView;
@property (assign) NSTextView          *codeView;
@end