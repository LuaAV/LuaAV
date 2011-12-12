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

#include "DspReceive.h"
#include "PdGraph.h"

DspReceive::DspReceive(PdMessage *initMessage, PdGraph *graph) : DspObject(0, 0, 0, 1, graph) {
  if (initMessage->isSymbol(0)) {
    name = StaticUtils::copyString(initMessage->getSymbol(0));
  } else {
    name = NULL;
    graph->printErr("receive~ not initialised with a name.\n");
  }
  sendBuffer = NULL;
  memset(localDspOutletBuffers, 0, blockSizeInt * sizeof(float));
}

DspReceive::~DspReceive() {
  free(name);
}

const char *DspReceive::getObjectLabel() {
  return "receive~";
}

ObjectType DspReceive::getObjectType() {
  return DSP_RECEIVE;
}

char *DspReceive::getName() {
  return name;
}

void DspReceive::setBuffer(float **buffer) {
  // maintain a double-pointer to the buffer of the associated send~
  sendBuffer = buffer;
}

void DspReceive::processDsp() {
  // replace the local outlet buffer with a pointer to the input buffer of the associated send~
  // sendBuffer may be null if there is no related send~
  if (sendBuffer == NULL) {
    // TODO(mhroth): if sendBuffer is null, point to a zero buffer
  } else {
    dspBufferAtOutlet0 = *sendBuffer;
  }
}
