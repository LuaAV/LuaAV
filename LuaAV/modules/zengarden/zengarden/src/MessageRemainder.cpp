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

#include "MessageRemainder.h"

MessageRemainder::MessageRemainder(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  if (initMessage->getNumElements() > 0 &&
      initMessage->getElement(0)->getType() == FLOAT) {
    init(initMessage->getElement(0)->getFloat());
  } else {
    init(0.0f);
  }
}

MessageRemainder::MessageRemainder(float constant, PdGraph *graph) : MessageObject(2, 1, graph) {
  init(constant);
}

MessageRemainder::~MessageRemainder() {
  // nothing to do
}

void MessageRemainder::init(float constant) {
  this->constant = (int) constant;
}

const char *MessageRemainder::getObjectLabel() {
  return "%";
}

void MessageRemainder::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      MessageElement *messageElement = message->getElement(0);
      if (messageElement->getType() == FLOAT) {
        PdMessage *outgoingMessage = getNextOutgoingMessage(0);
        float remainder = (constant == 0.0f) ? 0.0f : (float) ((int) messageElement-> getFloat() % constant);
        outgoingMessage->getElement(0)->setFloat(remainder);
        outgoingMessage->setTimestamp(message->getTimestamp());
        sendMessage(0, outgoingMessage);
      }
      break;
    }
    case 1: {
      MessageElement *messageElement = message->getElement(0);
      if (messageElement->getType() == FLOAT) {
        constant = (int) messageElement->getFloat();
      }
      break;
    }
    default: {
      break;
    }
  }
}
