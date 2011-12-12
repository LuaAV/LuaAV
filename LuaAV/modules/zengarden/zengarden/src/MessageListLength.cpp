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

#include "MessageListLength.h"

MessageListLength::MessageListLength(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
  // nothing to do
}

MessageListLength::~MessageListLength() {
  // nothing to do
}

const char *MessageListLength::getObjectLabel() {
  return "list length";
}

void MessageListLength::processMessage(int inletIndex, PdMessage *message) {
  PdMessage *outgoingMessage = getNextOutgoingMessage(0);
  outgoingMessage->setTimestamp(message->getTimestamp());
  if (message->isBang(0)) {
    // bangs are not considered to add length to lists
    outgoingMessage->getElement(0)->setFloat(0.0f);
  } else {
    outgoingMessage->getElement(0)->setFloat((float) message->getNumElements());
  }
  sendMessage(0, outgoingMessage);
}
