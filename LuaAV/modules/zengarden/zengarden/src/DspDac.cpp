/*
 *  Copyright 2009 Reality Jockey, Ltd.
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
#include "DspDac.h"
#include "PdGraph.h"

DspDac::DspDac(PdGraph *graph) : DspObject(0, graph->getNumOutputChannels(), 0, 0, graph) {
  // nothing to do
}

DspDac::~DspDac() {
  // nothing to do
}

const char *DspDac::getObjectLabel() {
  return "dac~";
}

void DspDac::processDspWithIndex(int fromIndex, int toIndex) {
  float *globalOutputBuffer;
  switch (numDspInlets) {
    default: {
      /* TODO(mhroth): fit this into the new buffer reference architecture
      for (int i = 2; i < numDspInlets; i++) {
        globalOutputBuffer = graph->getGlobalDspBufferAtOutlet(i);
        ArrayArithmetic::add(globalOutputBuffer, localDspBufferAtInlet[i], globalOutputBuffer, 0, blockSizeInt);
      }
      */
      // allow fallthrough
    }
    case 2: {
      globalOutputBuffer = graph->getGlobalDspBufferAtOutlet(1);
      ArrayArithmetic::add(globalOutputBuffer, dspBufferAtInlet1, globalOutputBuffer, 0, blockSizeInt);
      // allow fallthrough
    }
    case 1: {
      globalOutputBuffer = graph->getGlobalDspBufferAtOutlet(0);
      ArrayArithmetic::add(globalOutputBuffer, dspBufferAtInlet0, globalOutputBuffer, 0, blockSizeInt);
      // allow fallthrough
    }
    case 0: {
      break;
    }
  }
}
