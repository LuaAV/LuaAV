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

#include "DspSnapshot.h"
#include "PdGraph.h"

DspSnapshot::DspSnapshot(PdMessage *initMessage, PdGraph *graph) : DspObject(1, 1, 1, 0, graph) {
  // nothing to do
}

DspSnapshot::~DspSnapshot() {
  // nothing to do
}

const char *DspSnapshot::getObjectLabel() {
  return "snapshot~";
}

ConnectionType DspSnapshot::getConnectionType(int outletIndex) {
  return MESSAGE;
}

void DspSnapshot::processMessage(int inletIndex, PdMessage *message) {
  switch (message->getType(0)) {
    case SYMBOL: {
      graph->printErr("snapshot~ does not support the \"set\" message.");
      break;
    }
    case BANG: {
      PdMessage *outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->setTimestamp(message->getTimestamp());
      float blockIndex = graph->getBlockIndex(message);
      outgoingMessage->setFloat(0, dspBufferAtInlet0[(int) blockIndex]);
      sendMessage(0, outgoingMessage);
      break;
    }
    default: {
      break;
    }
  }
}
