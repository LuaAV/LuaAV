#import <Cocoa/Cocoa.h>


@interface LuaAVTextView : NSTextView {
	NSRect highlightedArea;
	NSRectPointer pointer; 
	NSUndoManager *undoManager;
}

- (void) highlightRange:(NSRange)range;
- (void) removeHighlight;

@end
