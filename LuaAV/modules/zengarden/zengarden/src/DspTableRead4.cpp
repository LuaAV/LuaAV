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
#include "DspTableRead4.h"
#include "PdGraph.h"

DspTableRead4::DspTableRead4(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 1, 0, 1, graph) {
  name = initMessage->isSymbol(0) ? StaticUtils::copyString(initMessage->getSymbol(0)) : NULL;
  table = NULL;
  offset = 0.0f;
}

DspTableRead4::~DspTableRead4() {
  free(name);
}

const char *DspTableRead4::getObjectLabel() {
  return "tabread4~";
}

ObjectType DspTableRead4::getObjectType() {
  return DSP_TABLE_READ4;
}

char *DspTableRead4::getName() {
  return name;
}

void DspTableRead4::setTable(MessageTable *aTable) {
  table = aTable;
}

void DspTableRead4::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isSymbol(0, "set") && message->isSymbol(1)) {
        // change the table from which this object reads
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        free(name);
        name = StaticUtils::copyString(message->getSymbol(1));
        table = graph->getTable(name);
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        offset = message->getFloat(0); // set onset into table
      }
      break;
    }
    default: {
      break;
    }
  }
}

void DspTableRead4::processDspWithIndex(int fromIndex, int toIndex) {
  if (table != NULL) { // ensure that there is a table to read from!
    int bufferLength;
    float *buffer = table->getBuffer(&bufferLength);
    if (ArrayArithmetic::hasAccelerate) {
      #if __APPLE__
      //float zero = 0.0f;
      //float bufferLengthFloat = (float) (bufferLength-2);
      //vDSP_vclip(inputBuffer+startSampleIndex, 1, &zero, &bufferLengthFloat,
      //    inputBuffer+startSampleIndex, 1, endSampleIndex-startSampleIndex);
      // NOTE(mhroth): is isn't clear what the clipping behaviour of vDSP_vlint is, but I
      // *think* that it is doing the right thing (i.e., clipping OOB indicies)
      vDSP_vsadd(dspBufferAtInlet0+fromIndex, 1, &offset, dspBufferAtOutlet0+fromIndex, 1,
          toIndex-fromIndex);
      vDSP_vlint(buffer, dspBufferAtInlet0+fromIndex, 1, dspBufferAtOutlet0+fromIndex, 1,
          toIndex-fromIndex, bufferLength);
      #endif
    } else {
      int maxIndex = bufferLength-1;
      for (int i = fromIndex; i < toIndex; i++) {
        int x = (int) (dspBufferAtInlet0[i] + offset);
        if (x <= 0) {
          dspBufferAtOutlet0[i] = buffer[0];
        } else if (x >= maxIndex) {
          dspBufferAtOutlet0[i] = buffer[maxIndex];
        } else {
          // 2-point linear interpolation (basic and fast)
          float dx = dspBufferAtInlet0[i] - ((float) x);
          float y0 = buffer[x];
          float y1 = buffer[x+1];
          float slope = (y1 - y0);
          dspBufferAtOutlet0[i] = (slope * dx) + y0;
        }
      }
    }
  }
}
