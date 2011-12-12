/*
 *  Copyright 2010 Reality Jockey, Ltd.
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
#include "DspMinimum.h"
#include "PdGraph.h"

DspMinimum::DspMinimum(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
}

DspMinimum::~DspMinimum() {
  // nothing to do
}

const char *DspMinimum::getObjectLabel() {
  return "min~";
}

void DspMinimum::processMessage(int inletIndex, PdMessage *message) {
  if (inletIndex == 1) {
    if (message->isFloat(0)) {
      processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
      constant = message->getFloat(0);
    }
  }
}

void DspMinimum::processDspWithIndex(int fromIndex, int toIndex) {
  switch (signalPrecedence) {
    case DSP_DSP: {
      if (ArrayArithmetic::hasAccelerate) {
        #if __APPLE__
        vDSP_vmin(dspBufferAtInlet0+fromIndex, 1, dspBufferAtInlet1+fromIndex, 1,
            dspBufferAtOutlet0+fromIndex, 1, toIndex-fromIndex);
        #endif
      } else {
        for (int i = fromIndex; i < toIndex; i++) {
          if (dspBufferAtInlet0[i] <= dspBufferAtInlet1[i]) {
            dspBufferAtOutlet0[i] = dspBufferAtInlet0[i];
          } else {
            dspBufferAtOutlet0[i] = dspBufferAtInlet1[i];
          }
        }
      }
      break;
    }
    case DSP_MESSAGE: {
      if (ArrayArithmetic::hasAccelerate) {
        #if __APPLE__
        int duration = toIndex - fromIndex;
        float vconst[duration];
        vDSP_vfill(&constant, vconst, 1, duration);
        vDSP_vmin(dspBufferAtInlet0 + fromIndex, 1, vconst, 1, dspBufferAtOutlet0 + fromIndex, 1,
            duration);
        #endif
      } else {
        for (int i = fromIndex; i < toIndex; i++) {
          if (dspBufferAtInlet0[i] <= constant) {
            dspBufferAtOutlet0[i] = dspBufferAtInlet0[i];
          } else {
            dspBufferAtOutlet0[i] = constant;
          }
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
