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

#include "MessageDelay.h"
#include "PdGraph.h"

MessageDelay::MessageDelay(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  delayMs = initMessage->isFloat(0) ? (double) initMessage->getFloat(0) : 0.0;
  scheduledMessage = NULL;
}

MessageDelay::~MessageDelay() {
  // nothing to do
}

const char *MessageDelay::getObjectLabel() {
  return "delay";
}

void MessageDelay::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case SYMBOL: {
          if (message->isSymbol(0, "stop")) {
            if (scheduledMessage != NULL) {
              graph->cancelMessage(this, 0, scheduledMessage);
            }
            break;
          }
          // allow fall-through
        }
        case FLOAT:
        case BANG: {
          if (scheduledMessage != NULL) {
            graph->cancelMessage(this, 0, scheduledMessage);
            scheduledMessage = NULL;
          }
          scheduledMessage = getNextOutgoingMessage(0);
          scheduledMessage->setTimestamp(message->getTimestamp() + delayMs);
          graph->scheduleMessage(this, 0, scheduledMessage);
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
        if (scheduledMessage != NULL) {
          // if an outstanding message exists when the delay is reset, the message is cancelled
          graph->cancelMessage(this, 0, scheduledMessage);
          scheduledMessage = NULL;
        }
        delayMs = (double) message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
