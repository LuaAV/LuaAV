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

#include "MessageValue.h"
#include "PdGraph.h"

MessageValue::MessageValue(PdMessage *initMessage, PdGraph *graph) : MessageObject(1, 1, graph) {
  if (initMessage->isSymbol(0)) {
    name = StaticUtils::copyString(initMessage->getSymbol(0));
  } else {
    name = NULL;
    graph->printErr("Object \"value\" MUST be initialised with a name.");
  }
}

MessageValue::~MessageValue() {
  free(name);
}

const char *MessageValue::getObjectLabel() {
  return "value";
}

void MessageValue::processMessage(int inletIndex, PdMessage *message) {
  switch (message->getType(0)) {
    case FLOAT: {
      graph->setValueForName(name, message->getFloat(0));
      break;
    }
    case BANG: {
      PdMessage *outgoingMessage = getNextOutgoingMessage(0);
      outgoingMessage->setTimestamp(message->getTimestamp());
      outgoingMessage->setFloat(0, graph->getValueForName(name));
      sendMessage(0, outgoingMessage);
      break;
    }
    default: {
      break;
    }
  }
}
