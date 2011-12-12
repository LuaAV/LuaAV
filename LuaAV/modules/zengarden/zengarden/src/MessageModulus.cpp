/*
 *  Copyright 2009, 2010 Reality Jockey, Ltd.
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

#include "MessageModulus.h"

MessageModulus::MessageModulus(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  constant = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
  lastOutput = 0.0f;
}

MessageModulus::MessageModulus(float constant, PdGraph *graph) : MessageObject(2, 1, graph) {
  init(constant);
}

MessageModulus::~MessageModulus() {
  // nothing to do
}

void MessageModulus::init(float constant) {
  this->constant = constant;
}

const char *MessageModulus::getObjectLabel() {
  return "mod";
}

void MessageModulus::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case FLOAT: {
          float remainder = (constant == 0.0f) ? 0.0f : (int) message-> getFloat(0) % (int) constant;
          lastOutput = (remainder < 0) ? remainder + abs(constant) : remainder;
          // allow fallthrough
        }
        case BANG: {
          PdMessage *outgoingMessage = getNextOutgoingMessage(0);
          outgoingMessage->setFloat(0, lastOutput);
          outgoingMessage->setTimestamp(message->getTimestamp());
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
        constant = message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
