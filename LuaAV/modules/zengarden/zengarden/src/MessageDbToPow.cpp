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

#include "MessageDbToPow.h"

MessageDbToPow::MessageDbToPow(PdGraph *graph) : MessageObject(1, 1, graph) {
  // nothing to do
}

MessageDbToPow::~MessageDbToPow() {
  // nothing to do
}

const char *MessageDbToPow::getObjectLabel() {
  return "dbtopow";
}

void MessageDbToPow::processMessage(int inletIndex, PdMessage *message) {
  if (message->getElement(0)->getType() == FLOAT) {
    if (message->getElement(0)->getFloat() <= 0) {
      PdMessage *outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->getElement(0)->setFloat(0);
      outgoingMessage->setTimestamp(message->getTimestamp());
      sendMessage(0, outgoingMessage);
    }
    else if (message->getElement(0)->getFloat() > 0) {
      PdMessage *outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->getElement(0)->setFloat(powf(0.00001f * powf(10.0f,(message->getElement(0)->getFloat())/20.0f),2.0f));
      outgoingMessage->setTimestamp(message->getTimestamp());
      sendMessage(0, outgoingMessage); // send a message from outlet 0
    }
  }
}
