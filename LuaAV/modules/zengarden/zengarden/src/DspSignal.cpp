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
#include "DspSignal.h"
#include "PdGraph.h"

DspSignal::DspSignal(PdMessage *initMessage, PdGraph *graph) : DspObject(1, 0, 0, 1, graph) {
  constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
}

DspSignal::~DspSignal() {
  // nothing to do
}

const char *DspSignal::getObjectLabel() {
  return "sig~";
}

void DspSignal::processMessage(int inletIndex, PdMessage *message) {
  if (message->isFloat(0)) {
    processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
    constant = message->getFloat(0);
  }
}

void DspSignal::processDspWithIndex(int fromIndex, int toIndex) {
  if (ArrayArithmetic::hasAccelerate) {
    #if __APPLE__
    vDSP_vfill(&constant, dspBufferAtOutlet0+fromIndex, 1, toIndex-fromIndex);
    #endif
  } else {
    memset_pattern4(dspBufferAtOutlet0+fromIndex, &constant, (toIndex-fromIndex) * sizeof(float));
  }
}
