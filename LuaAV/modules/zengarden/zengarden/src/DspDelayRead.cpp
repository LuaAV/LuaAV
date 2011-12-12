/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
 *                 info@rjdj.me
 *                 http://rjdj.me/
 * 
 *  This file is part of ZenGarden.
 *
 *  ZenGarden is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ZenGarden is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ZenGarden.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ArrayArithmetic.h"
#include "DspDelayRead.h"
#include "DspDelayWrite.h"
#include "PdGraph.h"

DspDelayRead::DspDelayRead(PdMessage *initMessage, PdGraph *graph) : DelayReceiver(1, 0, 0, 1, graph) {
  if (initMessage->isSymbol(0) && initMessage->isFloat(1)) {
    name = StaticUtils::copyString(initMessage->getSymbol(0));
    delayInSamples = StaticUtils::millisecondsToSamples(initMessage->getFloat(1), graph->getSampleRate());
    delayInSamplesInt = (int) delayInSamples;
  } else {
    graph->printErr("delread~ must be initialised in the format [delread~ name delay].");
    delayInSamples = 0.0f;
  }
}

DspDelayRead::~DspDelayRead() {
  // nothing to do
}

const char *DspDelayRead::getObjectLabel() {
  return "delread~";
}

ObjectType DspDelayRead::getObjectType() {
  return DSP_DELAY_READ;
}

void DspDelayRead::processMessage(int inletIndex, PdMessage *message) {
  if (message->isFloat(0)) {
    // update the delay time
    processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
    delayInSamples = StaticUtils::millisecondsToSamples(message->getFloat(0), graph->getSampleRate());
    delayInSamplesInt = (int) delayInSamples;
  }
}

void DspDelayRead::processDspWithIndex(int fromIndex, int toIndex) {
  int headIndex;
  int bufferLength;
  float *buffer = delayline->getBuffer(&headIndex, &bufferLength);
  if (fromIndex == 0 && toIndex == blockSizeInt) {
    // this handles the most common case. Messages are rarely sent to delread~.
    int delayIndex = headIndex - blockSizeInt - delayInSamplesInt;
    if (delayIndex < 0) {
      delayIndex += bufferLength;
    }
    if (delayIndex > bufferLength - blockSizeInt) {
      int samplesInBuffer = bufferLength - delayIndex; // samples remaining in the buffer that belong in this block
      dspBufferAtOutlet0 = localDspOutletBuffers;
      memcpy(dspBufferAtOutlet0, buffer + delayIndex, samplesInBuffer * sizeof(float));
      memcpy(dspBufferAtOutlet0 + samplesInBuffer, buffer, (blockSizeInt - samplesInBuffer) * sizeof(float));
    } else {
      dspBufferAtOutlet0 = buffer + delayIndex;
    }
  } else {
    /*
    //float delayIndex = (float) headIndex - delayInSamples - ((float) graph->getBlockSize() - blockIndexOfLastMessage);
    int delayIndex = (headIndex-blockSizeInt) - (int) (delayInSamples+blockIndexOfLastMessage);
    if (delayIndex < 0) {
      delayIndex += bufferLength;
    }
    /*
     * TODO(mhroth): finish this logic
    if (delayIndex > bufferLength - blockSizeInt) {
      int samplesInBuffer = bufferLength - delayIndex;
      memcpy(originalOutputBuffer, buffer + delayIndex, samplesInBuffer * sizeof(float));
      memcpy(originalOutputBuffer + samplesInBuffer, buffer, (blockSizeInt - samplesInBuffer) * sizeof(float));
      localDspBufferAtOutlet[0] = originalOutputBuffer;
    } else {
      memcpy(originalOutputBuffer + getStartSampleIndex(), 
             buffer + delayIndex, 
             (int) (newBlockIndex - blockIndexOfLastMessage) * sizeof(float));
    }
    */
    //localDspBufferAtOutlet[0] = originalOutputBuffer;
  }
}
