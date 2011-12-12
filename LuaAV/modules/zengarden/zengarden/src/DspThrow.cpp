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

#include "DspThrow.h"
#include "PdGraph.h"

DspThrow::DspThrow(PdMessage *initMessage, PdGraph *graph) : DspObject(0, 1, 0, 0, graph) {
  if (initMessage->isSymbol(0)) {
    name = StaticUtils::copyString(initMessage->getSymbol(0));
    buffer = (float *) calloc(blockSizeInt, sizeof(float));
    // dspBufferAtOutlet0 is otherwise not used as throw~ has no outlets. It is used to hold a
    // reference to the local buffer
    dspBufferAtOutlet0 = buffer;
  } else {
    name = NULL;
    graph->printErr("throw~ may not be initialised without a name. \"set\" message not supported.\n");
  }
}

DspThrow::~DspThrow() {
  free(name);
  free(dspBufferAtOutlet0);
}

const char *DspThrow::getObjectLabel() {
  return "throw~";
}

char *DspThrow::getName() {
  return name;
}

void DspThrow::processMessage(int inletIndex, PdMessage *message) {
  if (inletIndex == 0 && message->isSymbol(0) && message->isSymbol(1)) {
    graph->printErr("throw~ does not support the \"set\" message.\n");
  }
}

float *DspThrow::getBuffer() {
  return buffer;
}

void DspThrow::processDspWithIndex(int fromIndex, int toIndex) {
  if (numConnectionsToInlet0 > 1) {
    // inlet has been resolved, and thus the input exists in a temporary array
    buffer = dspBufferAtOutlet0;
    memcpy(buffer, dspBufferAtInlet0, numBytesInBlock);
  } else {
    // inlet does not need to be resolved, and is thus a pointer to a permanent array
    buffer = dspBufferAtInlet0;
  }
}
