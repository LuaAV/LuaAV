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

#include <math.h>
#include "DspLog.h"
#include "PdGraph.h"

DspLog::DspLog(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  // by default assume ln
  log2_base = initMessage->isFloat(0) ? log2f(initMessage->getFloat(0)) : M_LOG2E;
}

DspLog::~DspLog() {
  // nothing to do
}

const char *DspLog::getObjectLabel() {
  return "log~";
}

void DspLog::processMessage(int inletIndex, PdMessage *message) {
  if (inletIndex == 1) {
    if (message->isFloat(0)) {
      processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
      if (message->getFloat(0) <= 0.0f) {
        graph->printErr("log~ base cannot be set to a non-positive number: %d\n", message->getFloat(0));
      } else {
        log2_base = log2f(message->getFloat(0));
      }
    }
  }
}

void DspLog::processDspWithIndex(int fromIndex, int toIndex) {
  switch (signalPrecedence) {
    case DSP_DSP: {
      for (int i = fromIndex; i < toIndex; i++) {
        if (dspBufferAtInlet0[i] <= 0.0f || dspBufferAtInlet1[i] <= 0.0f) {
          dspBufferAtOutlet0[i] = -1000.0f; // Pd's "error" float value
        } else {
          dspBufferAtOutlet0[i] = log2Approx(dspBufferAtInlet0[i]) / log2Approx(dspBufferAtInlet1[i]);
        }
      }
      break;
    }
    case DSP_MESSAGE: {
      for (int i = fromIndex; i < toIndex; i++) {
        if (dspBufferAtInlet0[i] <= 0.0f) {
          dspBufferAtOutlet0[i] = -1000.0f;
        } else {
          dspBufferAtOutlet0[i] = log2Approx(dspBufferAtInlet0[i]) / log2_base;
        }
      }
      break;
    }
    case MESSAGE_DSP:
    case MESSAGE_MESSAGE:
    default: {
      break; // nothing to do
    }
  }
}

// this implementation is reproduced from http://www.musicdsp.org/showone.php?id=91
float DspLog::log2Approx(float x) {
  // input is assumed to be positive
  int y = (*(int *)&x);
  return (((y & 0x7f800000)>>23)-0x7f)+(y & 0x007fffff)/(float)0x800000;
}
