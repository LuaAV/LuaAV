#include "luaav_app_stateview_cocoa.h"
#include "luaav_app_stateview.h"
#include "luaav_app_screens_osx.h"

namespace luaav {

class StateViewImpl{
public:
	typedef StateView		Generic;

	StateViewImpl(Generic *generic)
	:	mGeneric(generic),
		mStateView(0)
	{}
	
	~StateViewImpl() {}
	
	Generic * generic() {return mGeneric;}
public:
	Generic *mGeneric;
	StateViewCocoa *mStateView;
};

}	// luaav::


@implementation StateViewCocoa

@synthesize root, outlineView, codeView;

- (id)initWithContentRect:(NSRect)contentRect screen:(NSScreen *)screen {

	int styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

	if(self = [super initWithContentRect: contentRect 
					styleMask: styleMask
					backing: NSBackingStoreBuffered 
					defer: NO
					screen:screen])
	{
		[self setTitle: @"State View"];
		
		// containing split view
		NSRect splitViewFrame = [NSWindow contentRectForFrameRect:contentRect styleMask:styleMask];
		splitView = [[NSSplitView alloc] initWithFrame:splitViewFrame];
		
		
		/*************************************************/
		// scroll view for outline view
		NSRect outlineScrollViewFrame = splitViewFrame;
		outlineScrollViewFrame.size.height /= 2;
		
		outlineScrollView  = [[NSScrollView alloc]  initWithFrame:outlineScrollViewFrame];
			[outlineScrollView setHasVerticalScroller:YES];
		
		
			NSRect outlineScrollContentRect = NSMakeRect(
													outlineScrollViewFrame.origin.x, 
													outlineScrollViewFrame.origin.y, 
													[outlineScrollView contentSize].width,
													[outlineScrollView contentSize].width
												);
			
			outlineClipView = [[NSClipView alloc] initWithFrame:outlineScrollContentRect];
		
		
		
		/*************************************************/
		// outline view for displaying trees of data
		outlineView = [[NSOutlineView alloc] initWithFrame:outlineScrollContentRect];
			[outlineView setUsesAlternatingRowBackgroundColors:YES];
			[outlineView setDataSource: self];
			[outlineView setDelegate:   self];
			[outlineView setColumnAutoresizingStyle:NSTableViewUniformColumnAutoresizingStyle];
			[outlineView setRowHeight:15.0];
		
			// Name Column
			keyColumn = [[NSTableColumn alloc] initWithIdentifier:@"Name"];
			[[keyColumn headerCell] setStringValue:@"Name"];
			[keyColumn setEditable:NO];
			[[keyColumn dataCell] setFont:[NSFont fontWithName:[[[keyColumn dataCell] font] fontName] size:11]];
			
			[outlineView addTableColumn:keyColumn];
			[outlineView setOutlineTableColumn:keyColumn];
			
			
			/*
			// Type Column
			typeColumn = [[NSTableColumn alloc] initWithIdentifier:@"Type"];
			[[typeColumn headerCell] setStringValue:@"Type"];
			[typeColumn setEditable:NO];
			[outlineView addTableColumn:typeColumn];
			*/
			
			// Value Column
			valueColumn = [[NSTableColumn alloc] initWithIdentifier:@"Value"];
			[[valueColumn headerCell] setStringValue:@"Value"];
			[valueColumn setEditable:NO];
			[[valueColumn dataCell] setFont:[NSFont fontWithName:[[[valueColumn dataCell] font] fontName] size:11]];
			[outlineView addTableColumn:valueColumn];
		
			// root of tree data
			root = [[NSMutableArray alloc] init];
		/*************************************************/
		
		
		// add the outline view to the scroll and clip views
		[outlineClipView setDocumentView:outlineView];
		[outlineScrollView setContentView:outlineClipView];
		
		// add the outline view to the split view
		[splitView addSubview:outlineScrollView];
		
		
		/*************************************************/
		// detail view of outline view items
		NSRect codeScrollViewFrame = splitViewFrame;
		codeScrollViewFrame.size.height /= 2;
		
		codeScrollView  = [[NSScrollView alloc]  initWithFrame:codeScrollViewFrame];
		[codeScrollView setHasVerticalScroller:YES];
		
		NSRect codeContentRect = NSMakeRect(0, 0, [outlineScrollView contentSize].width,[outlineScrollView contentSize].width);
		codeClipView = [[NSClipView alloc] initWithFrame:codeContentRect];
		codeView = [[NSTextView alloc] initWithFrame:codeContentRect];
		
			[codeView setEditable:NO];
			[codeView setFont:[NSFont controlContentFontOfSize:10.0]];

		[codeClipView setDocumentView:codeView];
		[codeScrollView setContentView:codeClipView];		
		[splitView addSubview:codeScrollView];
		
		[splitView adjustSubviews];
		/*************************************************/

		
		self.contentView = splitView;
		
		nameIsAscending = NO; typeIsAscending = NO; valueIsAscending = NO;
		
		_parent = NULL;
	}
	return self;
}

- (void)dealloc {
	[keyColumn release];
	[valueColumn release];
	[codeView release];
	[codeScrollView release];
	[codeClipView release];
	[outlineView release];
	[outlineScrollView release];
	[outlineClipView release];
	
	[splitView release];

	[root release];
	
	[super dealloc];
}

- (NSMutableDictionary *)findItem:(NSString *)name
{
	for(int i=0; i < [root count]; i++) {
		NSMutableDictionary *d = [root objectAtIndex:i];
		NSString *dname = [d objectForKey:@"Name"];
		if([dname compare:name] == NSOrderedSame) {
			return d;
		}
	}
	
	return  nil;
}

- (NSMutableDictionary *)findNode:(NSString *)name withList:(NSMutableArray *)list
{
	for(int i=0; i < [list count]; i++) {
		NSMutableDictionary *d = [list objectAtIndex:i];
		NSString *dname = [d objectForKey:@"Name"];
		if([dname compare:name] == NSOrderedSame) {
			return d;
		}
	}
	
	return  nil;
}

- (NSMutableArray *)followPath:(NSArray *)path
{
	NSMutableArray *parent = root;
	NSMutableDictionary *d = nil;
	
	int i=0;	
	while(parent && (i < [path count]-1)) {
		// get the item at this level
		d = [self findNode:[path objectAtIndex:i] withList:parent];
		if(d) {
			parent = [d objectForKey:@"Children"];
			// if this is going to be the first item at the containing level, create the array
			if(!parent && (i == [path count]-2)) {
				parent = [[NSMutableArray alloc] init];
				[d setObject:parent forKey:@"Children"];
			}
		}
		else {
			parent = nil;
		}
		
		i++;
	}
	
	return parent;
}

- (void)setPath:(NSArray *)path forValue:(NSString *)value forType:(NSString *)type
{
	NSMutableArray *parent = [self followPath:path];
	if(parent) {
		NSString *name = [path objectAtIndex:[path count]-1];
		NSMutableDictionary *d = [self findNode:name withList:parent];
		if(d) {
			[d setObject:value forKey:@"Value"];
			[d setObject:type forKey:@"Type"];
		}
		else {
			d = [NSMutableDictionary dictionaryWithObjectsAndKeys:
								  name, @"Name",
								  type, @"Type",
								  value, @"Value",
								  nil];
								  
			[parent addObject:d];
		}
		
		if([path count] == 1) {
			[outlineView reloadData];
		}
		[self updateDetail];
	}
}

- (void)setParent:(luaav::StateViewImpl *)parent
{
	_parent = parent;
}

- (void)outlineView:(NSOutlineView *)tableView didClickTableColumn:(NSTableColumn *)tableColumn
{
	NSString *key = [tableColumn identifier];
	BOOL ascending = YES;
	NSImage *sortImage;

	if([key isEqualToString: @"Name"]) {
		ascending = nameIsAscending = !nameIsAscending;
		sortImage = (!nameIsAscending) ? [NSImage imageNamed:@"NSAscendingSortIndicator"] : [NSImage imageNamed:@"NSDescendingSortIndicator"];
		[outlineView setIndicatorImage:nil inTableColumn:[outlineView tableColumnWithIdentifier:@"Type"]];
	}
	else if ([key isEqualToString: @"Type"]) {
		ascending = typeIsAscending = !typeIsAscending;
		sortImage = (!typeIsAscending) ? [NSImage imageNamed:@"NSAscendingSortIndicator"] : [NSImage imageNamed:@"NSDescendingSortIndicator"];	
		[outlineView setIndicatorImage:nil inTableColumn:[outlineView tableColumnWithIdentifier:@"Name"]];			
	}
	else if ([key isEqualToString: @"Value"]) {
		return; // can't compare numerical values to strings... //ascending = valueIsAscending = !valueIsAscending;
	}
	
	NSSortDescriptor *sort = [[NSSortDescriptor alloc] initWithKey:key ascending:ascending];
	[root sortUsingDescriptors:[NSArray arrayWithObject:sort]];
	[outlineView reloadData];
	
	[outlineView setIndicatorImage:sortImage inTableColumn:tableColumn];
}

- (void)outlineView:(NSOutlineView *)aOutlineView sortDescriptorsDidChange:(NSArray *)oldDescriptors
{
	NSLog(@"sorting called");
    //[myMutableArray sortUsingDescriptors:[aTableView sortDescriptors]];
    //[aTableView reloadData];
}

int textSort(NSString *string1, NSString *string2, void *context) {
	return [string1 compare:string2];
}

- (NSCell *)outlineView:(NSOutlineView *)outlineView dataCellForTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	if (tableColumn == nil) return nil; // don't return a cell for a nil tableColumn or it will use it to draw the entire row (weird default behavior)

	NSTextFieldCell *dataCell = [[NSTextFieldCell alloc] init];
	[dataCell setFont:[NSFont controlContentFontOfSize:10.0]];
	
	/*if([[tableColumn identifier] isEqualToString:@"Name"]) {
		[dataCell setTextColor:COLOR(1,1,1,1)];
		[dataCell setDrawsBackground:YES];
		
		NSString *type = [item objectForKey:@"Type"];
		if ([type isEqualToString:@"string"]) {
			[dataCell setBackgroundColor:COLOR(.7,0,0,1)];
		}else if ([type isEqualToString:@"number"]) {
			[dataCell setBackgroundColor:COLOR(0,.7,0,1)];
		}else {
			[dataCell setBackgroundColor:COLOR(0,0,.7,1)];
		}
	}*/
	
	return dataCell;
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	[self updateDetail];
}

- (void)updateDetail
{
	NSString *displayString;
	id item = [[outlineView itemAtRow:[outlineView selectedRow]] objectForKey:@"Value"];
	
	if ([item isKindOfClass:[NSNumber class]]) {
		displayString = [NSString stringWithFormat:@"%lf", [item doubleValue]];
	}
	else if ([item isKindOfClass:[NSString class]]) {
		displayString = item;
	}
	else if (item == nil ) {
		displayString = @"";
	}
	else {
		displayString = @"don't know how to display this yet";
	}
		
	[codeView setString:displayString];
}

- (int)outlineView:(NSOutlineView *)ov numberOfChildrenOfItem:(id)item
{
//	printf("numberOfChildrenOfItem\n");

	if(item == nil) {
		return [root count];
	}

//	printf("\tNOT ROOT\n");
	
	NSString *type = [item objectForKey:@"Type"];
	if([type compare:@"table"] == NSOrderedSame) {
		NSString *name = [item objectForKey:@"Name"];
		
		id parent = [ov parentForItem:item];
		while(parent) {
			NSString *pname = [parent objectForKey:@"Name"];
			name = [NSString stringWithFormat:@"%@/%@", pname, name];
			parent = [ov parentForItem:parent];
		}
		
		_parent->generic()->children([name UTF8String]);
	
		id children = [item objectForKey:@"Children"];
		if(children) {
			return [children count];
		}
	}
	
    return 0; 
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
//	printf("isItemExpandable\n");
    NSString *type = [item objectForKey:@"Type"];
	if([type compare:@"table"] == NSOrderedSame) {
//		printf("EXPANDABLE\n");
//		NSString *name = [item objectForKey:@"Name"];
//		_parent->generic()->children([name UTF8String]);
		return YES;
	}

    return NO;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(int)index ofItem:(id)item
{
//	printf("child: ofItem:\n");
	
	if(item == nil) {
		return [root objectAtIndex:index];
	}

	id children = [item objectForKey:@"Children"];    
	if(children) {
//		printf("\tNOT ROOT\n");
		return [children objectAtIndex:index];
	}
    
    return nil;
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
    // Note that you can find the parent of a given item using [outlineView parentForItem:item];
	if([[tableColumn identifier] compare:@"Name"] == NSOrderedSame) {
		return [item objectForKey:@"Name"];
	} 
    else if([[tableColumn identifier] compare:@"Value"] == NSOrderedSame) 
    {
		id itemValue = [item objectForKey:@"Value"];
        if([itemValue isKindOfClass:[NSString class]]) {
            return itemValue;
        } 
		else if([itemValue isKindOfClass:[NSNumber class]]) {
            return [NSString stringWithFormat:@"%lf", [itemValue doubleValue]];
        }
        else if([itemValue isKindOfClass:[NSDictionary class]]) {
            return [NSString stringWithFormat:@"%d items", [itemValue count]];
        }
        else if([itemValue isKindOfClass:[NSArray class]]) {
            return [NSString stringWithFormat:@"%d items", [itemValue count]];
        }
    }
	else if([[tableColumn identifier] compare:@"Type"] == NSOrderedSame) {
		return [item objectForKey:@"Type"];
    }
    return nil;
}

@end


namespace luaav {

#define mCocoaStateView (mImpl->mStateView)

void StateView :: implInit() {
	mImpl = new Implementation(this);
}

void StateView :: implDestruct() {
	delete mImpl;
	mImpl = 0;
}

void StateView :: implCreate(int x, int y, int w, int h) {
	if(! mCocoaStateView) {
		luaav_rect frame = luaav_rect(x, y, w, h);
		NSScreen *screen = screens::screen_for_rect(frame);
		luaav_rect r = screens::from_luaav_rect_on_screen(frame);
		
		mCocoaStateView = [[StateViewCocoa alloc] initWithContentRect:NSMakeRect(r.x, r.y, r.width, r.height) screen:screen];
		[mCocoaStateView setParent:this->mImpl];
		[mCocoaStateView makeKeyAndOrderFront:nil];
	}
}

void StateView :: implDestroy() {
	if(mCocoaStateView) {
		[mCocoaStateView dealloc];
		mCocoaStateView = 0;
	}
}

void StateView :: implAddItem(const char *name, const char *type, const char *v) {
	if(mCocoaStateView) {
		NSString *nsname = [NSString stringWithCString:name];
		NSArray *path = [nsname pathComponents];
		
		[mCocoaStateView setPath:path forValue:[NSString stringWithCString:v] forType:[NSString stringWithCString:type]];
	}
}

}	// luaav::