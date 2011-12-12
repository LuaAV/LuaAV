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

#include "MessageListAppend.h"

MessageListAppend::MessageListAppend(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  appendMessage = initMessage->copy();
}

MessageListAppend::~MessageListAppend() {
  delete appendMessage;
}

const char *MessageListAppend::getObjectLabel() {
  return "list append";
}

bool MessageListAppend::shouldDistributeMessageToInlets() {
  return false;
}

void MessageListAppend::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      PdMessage *outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->setTimestamp(message->getTimestamp());
      outgoingMessage->clear();
      if (!message->isBang(0)) {
        // if the incoming message is a bang, then it is considered to be a list of length zero
        int numElements = message->getNumElements();
        for (int i = 0; i < numElements; i++) {
          outgoingMessage->addElement(message->getElement(i));
        }
      }
      int numElements = appendMessage->getNumElements();
      for (int i = 0; i < numElements; i++) {
        outgoingMessage->addElement(appendMessage->getElement(i));
      }
      if (outgoingMessage->getNumElements() == 0) {
        // this is how Pd works... if both the pre and append messages are bang (zero-length)
        // then the output must have at least a bang
        outgoingMessage->addElement();
      }
      sendMessage(0, outgoingMessage);
      break;
    }
    case 1: {
      if (message->isBang(0)) {
        // bangs are considered a list of size zero
        appendMessage->clear();
      } else {
        // NOTE(mhroth): would be faster to copy in place rather than destroying and creating memory
        // can change if it becomes a problem
        delete appendMessage;
        appendMessage = message->copy();
      }
      break;
    }
    default: {
      break;
    }
  }
}
