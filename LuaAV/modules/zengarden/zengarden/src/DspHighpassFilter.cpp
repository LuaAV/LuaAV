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

#include <float.h>
#include "ArrayArithmetic.h"
#include "DspHighpassFilter.h"
#include "PdGraph.h"

DspHighpassFilter::DspHighpassFilter(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 1, 0, 1, graph) {
  sampleRate = graph->getSampleRate();
  tapIn = 0.0f;
  tapOut = 0.0f;
  coefficients = (float *) calloc(5, sizeof(float));
  // by default, the filter is initialised completely open
  calculateFilterCoefficients(initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f);
}

DspHighpassFilter::~DspHighpassFilter() {
  free(coefficients);
}

const char *DspHighpassFilter::getObjectLabel() {
  return "hip~";
}

void DspHighpassFilter::calculateFilterCoefficients(float cutoffFrequency) {
  if (cutoffFrequency <= 0.0f) {
    cutoffFrequency = FLT_MIN;
  } else if (cutoffFrequency > sampleRate/2.0f) {
    cutoffFrequency = sampleRate/2.0f;
  }
  float rc = 1.0f / (2.0f * M_PI * cutoffFrequency);
  alpha = rc / (rc + (1.0f/sampleRate));
  coefficients[0] = alpha;
  coefficients[1] = -1.0f * alpha;
  coefficients[3] = -1.0f * alpha;
}

void DspHighpassFilter::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isSymbol(0) && strcmp(message->getSymbol(0), "clear") == 0) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        tapIn = 0.0f;
        tapOut = 0.0f;
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        calculateFilterCoefficients(message->getFloat(0));
      }
      break;
    }
    default: {
      break;
    }
  }
}

// http://en.wikipedia.org/wiki/High-pass_filter
void DspHighpassFilter::processDspWithIndex(int fromIndex, int toIndex) {
  if (ArrayArithmetic::hasAccelerate) {
    #if __APPLE__
    const int duration = toIndex - fromIndex;
    const int durationBytes = duration * sizeof(float);
    float filterInputBuffer[duration+2];
    memcpy(filterInputBuffer+2, dspBufferAtInlet0+fromIndex, durationBytes);
    filterInputBuffer[1] = tapIn;
    float filterOutputBuffer[duration+2];
    filterOutputBuffer[1] = tapOut;
    vDSP_deq22(filterInputBuffer, 1, coefficients, filterOutputBuffer, 1, duration);
    memcpy(dspBufferAtOutlet0+fromIndex, filterOutputBuffer+2, durationBytes);
    tapIn = dspBufferAtInlet0[toIndex-1];
    tapOut = dspBufferAtOutlet0[toIndex-1];
    #endif
  } else {
    dspBufferAtOutlet0[fromIndex] = alpha * (tapOut + dspBufferAtInlet0[fromIndex] - tapIn);
    for (int i = fromIndex+1; i < toIndex; i++) {
      dspBufferAtOutlet0[i] = alpha * (dspBufferAtOutlet0[i-1] + dspBufferAtInlet0[i] - dspBufferAtInlet0[i-1]);
    }
    tapIn = dspBufferAtInlet0[toIndex-1];
    tapOut = dspBufferAtOutlet0[toIndex-1];
  }
}
