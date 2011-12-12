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
#include "DspSubtract.h"
#include "PdGraph.h"

DspSubtract::DspSubtract(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
}

DspSubtract::~DspSubtract() {
  // nothing to do
}

const char *DspSubtract::getObjectLabel() {
  return "-~";
}

void DspSubtract::processMessage(int inletIndex, PdMessage *message) {
  if (inletIndex == 1) {
    if (message->isFloat(0)) {
      processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
      constant = message->getFloat(0);
    }
  }
}

void DspSubtract::processDspWithIndex(int fromIndex, int toIndex) {
  switch (signalPrecedence) {
    case DSP_DSP: {
      ArrayArithmetic::subtract(dspBufferAtInlet0, dspBufferAtInlet1, dspBufferAtOutlet0,
          fromIndex, toIndex);
      break;
    }
    case DSP_MESSAGE: {
      ArrayArithmetic::subtract(dspBufferAtInlet0, constant, dspBufferAtOutlet0, fromIndex, toIndex);
      break;
    }
    case MESSAGE_DSP:
    case MESSAGE_MESSAGE:
    default: {
      break; // nothing to do
    }
  }
}
