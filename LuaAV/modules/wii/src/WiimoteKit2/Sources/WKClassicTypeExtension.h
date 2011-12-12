//
//  WiiClassicTypeExtension.h
//  WiimoteKit
//
//  Created by Camille on 23/02/09.
//  Copyright 2009 ZZ prefix. All rights reserved.
//

#import <WiimoteKit/WKExtension.h>
#import <WiimoteKit/WKEvent.h>

typedef NSUInteger WKClassicButtonsState;

@interface WKClassicTypeExtension : WKExtension {
@private
	// Current button state
	WKClassicButtonsState wk_buttons;
}

// methods that must be subclassed
- (WKEventSource) source;
- (const WKClassicButtonsState) buttonsMask;
- (void) parseSpecific:(const uint8_t *) data;

@end
