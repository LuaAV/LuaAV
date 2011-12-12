/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
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

#include "MessageSymbol.h"

MessageSymbol::MessageSymbol(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  symbol = new MessageElement();
  symbol->setSymbol(initMessage->isSymbol(0) ? initMessage->getSymbol(0) : (char *) "");
}

MessageSymbol::MessageSymbol(char *initSymbol, PdGraph *graph) : MessageObject(2, 1, graph) {
  symbol = new MessageElement();
  symbol->setSymbol((initSymbol != NULL) ? initSymbol : (char *) "");
}

MessageSymbol::~MessageSymbol() {
  delete symbol;
}

const char *MessageSymbol::getObjectLabel() {
  return "symbol";
}

void MessageSymbol::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case SYMBOL: {
          symbol->setSymbol(message->getSymbol(0));
          // allow fallthrough
        }
        case BANG: {
          PdMessage *outgoingMessage = getNextOutgoingMessage(0);
          outgoingMessage->setTimestamp(message->getTimestamp());
          outgoingMessage->setSymbol(0, symbol->getSymbol());
          sendMessage(0, outgoingMessage);
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case 1: {
      if (message->isSymbol(0)) {
        symbol->setSymbol(message->getSymbol(1));
      }
      break;
    }
    default: {
      break;
    }
  }
}
