#import "ModulesTableView.h"


@implementation ModulesTableView

- (NSMenu *)menuForEvent:(NSEvent *)theEvent {
	NSPoint menuPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	int row = [self rowAtPoint:menuPoint];

	if (![[self selectedRowIndexes] containsIndex:row]) {
		[self selectRowIndexes: [NSIndexSet indexSetWithIndex:row]
			byExtendingSelection:NO];
	}

	if ([self numberOfSelectedRows] <=0) {
		return nil;
	}
	else {
		return [self menu];
	}
}

@end
