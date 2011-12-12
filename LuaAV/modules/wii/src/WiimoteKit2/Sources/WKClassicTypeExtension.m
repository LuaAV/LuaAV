//
//  WiiClassicTypeExtension.m
//  WiimoteKit
//
//  Created by Camille on 23/02/09.
//  Copyright 2009 ZZ prefix. All rights reserved.
//

#import "WKClassicTypeExtension.h"
#import "WiiRemoteInternal.h"

@interface WKClassicTypeExtension (Private)

- (void) parseButtons:(const uint8_t *) data;

@end

@implementation WKClassicTypeExtension

- (WKEventSource) source
{
	[NSException raise:NSGenericException format:@"Abstract method"];
	return kWKExtensionNone;
}

- (void) parseStatus:(const uint8_t *) data range:(NSRange) aRange
{
	NSParameterAssert(aRange.length >= 6);

	// cam: offset the data by the right amount
	data += aRange.location;

	[self parseButtons:data];
	[self parseSpecific:data];
}

#pragma mark Buttons

- (const WKClassicButtonsState) buttonsMask
{
	[NSException raise:NSGenericException format:@"Abstract method"];
	return 0x0;
}

- (void) parseButtons:(const uint8_t *) data
{
	const WKClassicButtonsState mask = [self buttonsMask];
	WKClassicButtonsState buttons = OSReadBigInt16 (data, 4);

	/* stats is negative, so use ~buttons */
	buttons = ~buttons & mask;

	if (buttons != wk_buttons) {
		NSUInteger idx = 0;
		for (idx = 0; idx < 16; idx++) {
			NSUInteger flag = 1 << idx;
			if (flag & mask) {
				/* xor */
				bool before = wk_buttons & flag;
				bool after = buttons & flag;
				if ((before && !after) || (!before && after)) {
					[[self wiiRemote] sendButtonEvent:flag source:[self source] down:before];
				}
			}
		}

		wk_buttons = buttons;
	}
}

#pragma mark Other

- (void) parseSpecific:(const uint8_t *) data
{
	// nothing to do here
}

#pragma mark Calibration

- (size_t) calibrationLength
{
	return 16;
}

- (user_addr_t) calibrationAddress
{
	return EXTENSION_REGISTER_CALIBRATION;
}

@end
