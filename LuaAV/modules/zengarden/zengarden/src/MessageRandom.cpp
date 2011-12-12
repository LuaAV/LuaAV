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

#include "MessageRandom.h"

MessageRandom::MessageRandom(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  if (initMessage->getNumElements() > 0 &&
      initMessage->getElement(0)->getType() == FLOAT) {
    max_inc = ((int) initMessage->getElement(0)->getFloat()) - 1;
  } else {
    max_inc = 1;
  }
  twister = new MTRand();
}

MessageRandom::MessageRandom(int N, PdGraph *graph) : MessageObject(2, 1, graph) {
  this->max_inc = N-1;
  twister = new MTRand();
}

MessageRandom::~MessageRandom() {
  delete twister;
}

const char *MessageRandom::getObjectLabel() {
  return "random";
}

bool MessageRandom::shouldDistributeMessageToInlets() {
  return false;
}

void MessageRandom::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      MessageElement *messageElement = message->getElement(0);
      switch (messageElement->getType()) {
        case SYMBOL: {
          if (strcmp(messageElement->getSymbol(), "seed") == 0 &&
              message->getNumElements() > 1 &&
              message->getElement(1)->getType() == FLOAT) {
            twister->seed((int) message->getElement(1)->getFloat()); // reset the seed
          }
          break;
        }
        case BANG: {
          PdMessage *outgoingMessage = getNextOutgoingMessage(0);
          outgoingMessage->setTimestamp(message->getTimestamp());
          outgoingMessage->getElement(0)->setFloat((float) twister->randInt(max_inc));
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
      if (message->getElement(0)->getType() == FLOAT) {
        max_inc = ((int) message->getElement(0)->getFloat()) - 1;
      }
      break;
    }
    default: {
      break; // ERROR!
    }
  }
}
