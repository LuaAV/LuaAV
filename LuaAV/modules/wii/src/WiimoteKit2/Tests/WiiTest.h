//
//  WiiTest.h
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 12/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WiimoteKit.h>

//@class WiiRemote;
@interface WiiTest : NSObject {
	IBOutlet NSLevelIndicator *ibBattery;
	IBOutlet NSProgressIndicator *ibSearch;
	
	IBOutlet NSSlider *ibSliderX, *ibSliderY, *ibSliderZ;
	
	WiiRemote *wk_wiimote;
	
	NSTimer *wk_funny;
	bool wk_reverse;
}

- (IBAction)rumble:(id)sender;
- (IBAction)infrared:(id)sender;
- (IBAction)accelerometer:(id)sender;


- (IBAction)funny:(id)sender;
- (IBAction)continous:(id)sender;

@end
