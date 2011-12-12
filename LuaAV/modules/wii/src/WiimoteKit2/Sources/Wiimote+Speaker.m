//
//  Wiimote+Speaker.m
//  WiimoteKit
//
//  Created by Jean-Daniel Dupas on 13/01/08.
//  Copyright 2008 Shadow Lab. All rights reserved.
//

#import <WiimoteKit/WiiRemote.h>

#import "WiiRemoteInternal.h"

@implementation WiiRemote (WiiRemoteSpeaker)

- (BOOL)isSpeakerEnabled {
	return wk_wiiFlags.speaker;
}

- (void)__stopSpeaker {
	wk_wiiFlags.speakerInit = 0;
	wk_wiiFlags.speakerAbort = 0;
	[self sendCommand:(const uint8_t[]){ WKOutputReportSpeakerStatus, 0x00 } length:2];
	[self refreshStatus];
}

- (void)setSpeakerEnabled:(BOOL)flag {
	flag = flag ? 1 : 0; // canoniz
	if (wk_wiiFlags.speaker != flag) {
		if (!flag) {
			if (wk_wiiFlags.speakerInit) {
				wk_wiiFlags.speakerAbort = 1;
			} else {
				[self __stopSpeaker];
			}
		} else {
			if (!wk_wiiFlags.speakerInit) {
				wk_wiiFlags.speakerInit = 1;
				/* start speaker */
				[self sendCommand:(const uint8_t[]){ WKOutputReportSpeakerStatus, 0x04 } length:2];
				[self sendCommand:(const uint8_t[]){ WKOutputReportSpeakerMute, 0x04 } length:2];
				
				[self writeData:(const uint8_t[]){ 0x01 } length:1 atAddress:0x09 
									space:kWKMemorySpaceSpeaker next:@selector(didStartSpeaker:)];
				/* following operation are done in the data send callback */
			}
		}
	}
}

- (BOOL)isSpeakerMuted {
	return wk_wiiFlags.muted;
}
- (void)setSpeakerMuted:(BOOL)flag {
	IOReturn err = kIOReturnSuccess;
	flag = flag ? 1 : 0; // canoniz
	if (wk_wiiFlags.muted != flag) {
		err = [self sendCommand:(const uint8_t[]){ WKOutputReportSpeakerMute, flag ? 0x04 : 0 } length:2];
//		if (kIOReturnSuccess == err)
//			err = [self refreshStatus]; // will send a speaker state change notification
	}
	WKPrintIOReturn(err, "setSpeakerMute");	
}

- (void)setSpeakerDataSource:(id<WiiRemoteSpakerDataSource>)source {
	
}

- (OSStatus)getAudioFormat:(AudioStreamBasicDescription *)format {
	bzero(format, sizeof(*format));
	format->mSampleRate = 3920;
	format->mBitsPerChannel = 4;
	
	format->mFormatID = kAudioFormatLinearPCM;
	format->mFormatFlags = kLinearPCMFormatFlagIsBigEndian;
	format->mChannelsPerFrame = 1; // mono
	return noErr;
}

#pragma mark Initialization
- (void)didStartSpeaker:(NSUInteger)status {
	if (wk_wiiFlags.speakerAbort) {
		[self __stopSpeaker];
	} else {
		[self writeData:(const uint8_t[]){ 0x08 } length:1 atAddress:0x01 
							space:kWKMemorySpaceSpeaker next:@selector(didConfigureSpeakerOne:)];
	}
}
- (void)didConfigureSpeakerOne:(NSUInteger)status {
	if (wk_wiiFlags.speakerAbort) {
		[self __stopSpeaker];
	} else {
		/*
		 byte 3: sample rate
		   - 0x0B: 4000/4364Hz (~4200Hz)
		   - 0x0C: 3692/4000Hz (~3920Hz)
		   - 0x0D: 3429/3692Hz (~3640Hz)
		   - 0x0E: 3200/3429Hz (~3360Hz)
		   - 0x0F: 3000/3200Hz (~3080Hz)
		 byte 4: volume (default: 64)
		 */
		[self writeData:(const uint8_t[]){ 0x00, 0x00, 0x00, 0x0C, 0x40, 0x00, 0x00 } length:7 atAddress:0x01
							space:kWKMemorySpaceSpeaker next:@selector(didConfigureSpeakerTwo:)];
	}
}
- (void)didConfigureSpeakerTwo:(NSUInteger)status {
	if (wk_wiiFlags.speakerAbort) {
		[self __stopSpeaker];
	} else {
		[self writeData:(const uint8_t[]){ 0x01 } length:1 atAddress:0x08
							space:kWKMemorySpaceSpeaker next:@selector(didConfigureSpeakerThree:)];
	}
}
- (void)didConfigureSpeakerThree:(NSUInteger)status {
	if (wk_wiiFlags.speakerAbort) {
		[self __stopSpeaker];
	} else {
		// unmute
		[self sendCommand:(const uint8_t[]){ WKOutputReportSpeakerMute, 0 } length:2];
		wk_wiiFlags.speakerInit = 0;
	}
}

@end
