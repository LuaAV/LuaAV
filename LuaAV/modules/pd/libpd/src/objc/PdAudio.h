/**
 * This software is copyrighted by Reality Jockey Ltd. and Peter Brinkmann. 
 * The following terms (the "Standard Improved BSD License") apply to 
 * all files associated with the software unless explicitly disclaimed 
 * in individual files:
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above  
 * copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 * products derived from this software without specific prior 
 * written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <AudioToolbox/AudioToolbox.h>
#import <AudioUnit/AudioUnit.h>
#import <Foundation/Foundation.h>

#import "PdBase.h"

@interface PdAudio : NSObject {
  AudioUnit audioUnit;
  int numInputChannels;
  int numOutputChannels;
  Float64 sampleRate;
  float microphoneVolume;
  float *floatBuffer;
  int floatBufferLength;
}

@property (nonatomic, readonly) AudioUnit audioUnit;
@property (nonatomic, readonly) int numInputChannels;
@property (nonatomic, readonly) int numOutputChannels;
@property (nonatomic, readonly) Float64 sampleRate;
@property (nonatomic) float microphoneVolume;
@property (nonatomic, readonly) float *floatBuffer;
@property (nonatomic, readonly) int floatBufferLength;


/** Initialising with a sample rate, ticks per buffer, number of input and output channels
 * One Pd-tick is typically 64 samples per channel. Call [PdBase getBlockSize] to get the actual number.
 * [PdBase openAudio] is also called within this method as well as [PdBase computeAudio:YES].
 */
- (id)initWithSampleRate:(float)newSampleRate andTicksPerBuffer:(int)ticks 
    andNumberOfInputChannels:(int)inputChannels andNumberOfOutputChannels:(int)outputChannels;

/** Begin audio/scene playback. To avoid clicks, you have to create a subclass and override this 
 * function and add ramping or any other custom behaviour. Make sure to call the superclass method
 * as well.
 */
- (void)play;

/** Pause audio/scene playback.To avoid clicks, you have to create a subclass and override this 
 * function and add ramping or any other custom behaviour. Make sure to call the superclass method
 * as well.
 */
- (void)pause;

// private
- (void)initializeAudioSession:(int)ticks;
- (void)initializeAudioUnit;

@end
