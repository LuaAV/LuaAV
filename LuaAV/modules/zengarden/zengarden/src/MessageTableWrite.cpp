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

#include "MessageTableWrite.h"
#include "PdGraph.h"

MessageTableWrite::MessageTableWrite(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 0, graph) {
  name = initMessage->isSymbol(0) ? StaticUtils::copyString(initMessage->getSymbol(0)) : NULL;
  table = NULL;
  index = 0;
}

MessageTableWrite::~MessageTableWrite() {
  free(name);
}

const char *MessageTableWrite::getObjectLabel() {
  return "tabwrite";
}

ObjectType MessageTableWrite::getObjectType() {
  return MESSAGE_TABLE_WRITE;
}

char *MessageTableWrite::getName() {
  return name;
}

void MessageTableWrite::setTable(MessageTable *aTable) {
  table = aTable;
}

bool MessageTableWrite::shouldDistributeMessageToInlets() {
  return false;
}

void MessageTableWrite::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case FLOAT: {
          if (table != NULL) {
            int bufferLength = 0;
            float *buffer = table->getBuffer(&bufferLength);
            if (index >=0 && index < bufferLength) {
              buffer[index] = message->getFloat(0);
            }
          }
          break;
        }
        case SYMBOL: {
          printf("%s", message->toString());
          if (message->isSymbol(0, "set") && message->isSymbol(1)) {
            free(name);
            name = StaticUtils::copyString(message->getSymbol(1));
            table = graph->getTable(name);
          }
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
        index = (int) message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
