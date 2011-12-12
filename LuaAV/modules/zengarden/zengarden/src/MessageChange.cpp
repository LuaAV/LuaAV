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

#include "MessageChange.h"

MessageChange::MessageChange(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
   prevValue = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
}

MessageChange::~MessageChange() {
  // nothing to do
}

const char *MessageChange::getObjectLabel() {
  return "change";
}

void MessageChange::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case FLOAT: {
          // output only if input is different than what is already there
            if (message->getFloat(0) != prevValue) {
              PdMessage *outgoingMessage = getNextOutgoingMessage(0);
              outgoingMessage->getElement(0)->setFloat(message->getFloat(0));
              outgoingMessage->setTimestamp(message->getTimestamp());
              prevValue = message->getFloat(0);
              sendMessage(0, outgoingMessage);
            }
            break;
        }
        case BANG: {
          // force output
          PdMessage *outgoingMessage = getNextOutgoingMessage(0);
          outgoingMessage->getElement(0)->setFloat(prevValue);
          outgoingMessage->setTimestamp(message->getTimestamp());
          sendMessage(0, outgoingMessage);
          break;
        }
        case SYMBOL: {
          if (message->isSymbol(0, "set") && message->isFloat(1)) {
            prevValue = message->getFloat(1);
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
}
