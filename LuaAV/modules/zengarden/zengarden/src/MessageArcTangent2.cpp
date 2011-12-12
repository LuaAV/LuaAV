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

#include "MessageArcTangent2.h"

MessageArcTangent2::MessageArcTangent2(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
}

MessageArcTangent2::~MessageArcTangent2() {
  // nothing to do
}

const char *MessageArcTangent2::getObjectLabel() {
  return "atan2";
}

void MessageArcTangent2::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isFloat(0)) {
        PdMessage *outgoingMessage = getNextOutgoingMessage(0);
        outgoingMessage->setFloat(0, atan2f(message->getFloat(0), constant));
        outgoingMessage->setTimestamp(message->getTimestamp());
        sendMessage(0, outgoingMessage); // send a message from outlet 0
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        constant = message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
