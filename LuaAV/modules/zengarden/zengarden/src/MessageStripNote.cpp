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

#include "MessageStripNote.h"

MessageStripNote::MessageStripNote(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 2, graph) {
  // nothing to do
}

MessageStripNote::~MessageStripNote() {
  // nothing to do
}

const char *MessageStripNote::getObjectLabel() {
  return "stripnote";
}

void MessageStripNote::processMessage(int inletIndex, PdMessage *message) {
  if (inletIndex == 0) {
    if (message->getNumElements() == 2 && message->isFloat(0) && message->isFloat(1) &&
        message->getFloat(1) > 0.0f) {
      PdMessage *outgoingMessage = getNextOutgoingMessage(1);
      outgoingMessage->setTimestamp(message->getTimestamp());
      outgoingMessage->setFloat(0, message->getFloat(1));
      sendMessage(1, outgoingMessage);
      
      outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->setTimestamp(message->getTimestamp());
      outgoingMessage->setFloat(0, message->getFloat(0));
      sendMessage(0, outgoingMessage);
    }
  }
  // NOTE(mhroth): no idea what right inlet is for
}
