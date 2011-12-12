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

#include "MessageMoses.h"

MessageMoses::MessageMoses(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 2, graph) {
  if (initMessage->getNumElements() > 0 &&
      initMessage->getElement(0)->getType() == FLOAT) {
    init(initMessage->getElement(0)->getFloat());
  } else {
    init(0.0f);
  }
}

MessageMoses::MessageMoses(float constant, PdGraph *graph) : MessageObject(2, 2, graph) {
  init(constant);
}

MessageMoses::~MessageMoses() {
  // nothing to do
}

void MessageMoses::init(float constant) {
  this->constant = constant;
}

const char *MessageMoses::getObjectLabel() {
  return "moses";
}

void MessageMoses::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      MessageElement *messageElement = message->getElement(0);
      if (messageElement->getType() == FLOAT) {
        if (messageElement->getFloat() < constant) {
          PdMessage *outgoingMessage = getNextOutgoingMessage(0);
          outgoingMessage->getElement(0)->setFloat(messageElement-> getFloat());
          outgoingMessage->setTimestamp(message->getTimestamp());
          sendMessage(0, outgoingMessage); // send a message from outlet 0
        } else {
          PdMessage *outgoingMessage = getNextOutgoingMessage(1);
          outgoingMessage->getElement(0)->setFloat(messageElement-> getFloat());
          outgoingMessage->setTimestamp(message->getTimestamp());
          sendMessage(1, outgoingMessage); // send a message from outlet 1
        }
      }
      break;
    }
    case 1: {
      MessageElement *messageElement = message->getElement(0);
      if (messageElement->getType() == FLOAT) {
        constant = messageElement->getFloat();
      }
      break;
    }
    default: {
      break;
    }
  }
}
