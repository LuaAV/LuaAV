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

#include "MessageLogicalAnd.h"

MessageLogicalAnd::MessageLogicalAnd(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  left = 0.0f;
  right = 0.0f;
}

MessageLogicalAnd::~MessageLogicalAnd() {
  // nothing to do
}

const char *MessageLogicalAnd::getObjectLabel() {
  return "&&";
}

void MessageLogicalAnd::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case FLOAT: {
          left = message->getFloat(0);
          // allow fallthrough
        }
        case BANG: {
          PdMessage *outgoingMessage = getNextOutgoingMessage(0);
          outgoingMessage->setTimestamp(message->getTimestamp());
          outgoingMessage->setFloat(0, (left == 0.0f || right == 0.0f) ? 0.0f : 1.0f);
          sendMessage(0, outgoingMessage);
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        right = message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
