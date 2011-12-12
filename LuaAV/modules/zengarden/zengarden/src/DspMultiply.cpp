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
#include "DspMultiply.h"
#include "PdGraph.h"

DspMultiply::DspMultiply(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
  inputConstant = 0.0f;
}

DspMultiply::~DspMultiply() {
  // nothing to do
}

const char *DspMultiply::getObjectLabel() {
  return "*~";
}

void DspMultiply::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isFloat(0)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        inputConstant = message->getFloat(0);
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        constant = message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}

void DspMultiply::processDspWithIndex(int fromIndex, int toIndex) {
  switch (signalPrecedence) {
    case MESSAGE_DSP: {
      memset_pattern4(dspBufferAtInlet0+fromIndex, &inputConstant, (toIndex-fromIndex)*sizeof(float));
      // allow fallthrough
    }
    case DSP_DSP: {
      ArrayArithmetic::multiply(dspBufferAtInlet0, dspBufferAtInlet1, dspBufferAtOutlet0,
          fromIndex, toIndex);
      break;
    }
    case DSP_MESSAGE: {
      ArrayArithmetic::multiply(dspBufferAtInlet0, constant, dspBufferAtOutlet0, fromIndex, toIndex);
      break;
    }
    case MESSAGE_MESSAGE:
    default: {
      break; // nothing to do
    }
  }
}
