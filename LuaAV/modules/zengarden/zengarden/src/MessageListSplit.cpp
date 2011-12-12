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

#include "MessageListSplit.h"

MessageListSplit::MessageListSplit(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 3, graph) {
  splitIndex = initMessage->isFloat(0) ? (int) initMessage->getFloat(0) : 0;
}

MessageListSplit::~MessageListSplit() {
  // nothing to do
}

const char *MessageListSplit::getObjectLabel() {
  return "list split";
}

void MessageListSplit::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      int numElements = message->getNumElements();
      if (numElements <= splitIndex) {
        // if there aren't enough elements to split on, forward the message on the third outlet
        sendMessage(2, message);
      } else {
        PdMessage *outgoingMessage0 = getNextOutgoingMessage(0);
        outgoingMessage0->setTimestamp(message->getTimestamp());
        outgoingMessage0->clear();
        for (int i = 0; i < splitIndex; i++) {
          outgoingMessage0->addElement(message->getElement(i));
        }
        
        PdMessage *outgoingMessage1 = getNextOutgoingMessage(1);
        outgoingMessage1->setTimestamp(message->getTimestamp());
        outgoingMessage1->clear();
        for (int i = splitIndex; i < numElements; i++) {
          outgoingMessage1->addElement(message->getElement(i));
        }
        
        sendMessage(1, outgoingMessage1);
        sendMessage(0, outgoingMessage0);
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        // split index may not be negative
        splitIndex = (message->getFloat(0) < 0.0f) ? 0 : (int) message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
