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

#include "MessageListPrepend.h"

MessageListPrepend::MessageListPrepend(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  prependMessage = initMessage->copy();
}

MessageListPrepend::~MessageListPrepend() {
  delete prependMessage;
}

const char *MessageListPrepend::getObjectLabel() {
  return "list prepend";
}

void MessageListPrepend::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      PdMessage *outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->setTimestamp(message->getTimestamp());
      outgoingMessage->clear();
      int numElements = message->getNumElements();
      for (int i = 0; i < numElements; i++) {
        outgoingMessage->addElement(prependMessage->getElement(i));
      }
      numElements = prependMessage->getNumElements();
      for (int i = 0; i < numElements; i++) {
        outgoingMessage->addElement(message->getElement(i));
      }
      sendMessage(0, outgoingMessage);
      break;
    }
    case 1: {
      // NOTE(mhroth): would be faster to copy in place rather than destroying and creating memory
      // can change if it becomes a problem
      delete prependMessage;
      prependMessage = message->copy();
      break;
    }
    default: {
      break;
    }
  }
}
