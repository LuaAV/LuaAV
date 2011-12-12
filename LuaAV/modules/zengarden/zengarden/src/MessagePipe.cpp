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

#include "MessagePipe.h"
#include "PdGraph.h"
#include "ZGLinkedList.h"

MessagePipe::MessagePipe(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  delayMs = initMessage->isFloat(0) ? (double) initMessage->getFloat(0) : 0.0;
  scheduledMessagesList = new ZGLinkedList();
}

MessagePipe::~MessagePipe() {
  delete scheduledMessagesList;
}

const char *MessagePipe::getObjectLabel() {
  return "pipe";
}

bool MessagePipe::shouldDistributeMessageToInlets() {
  return false;
}

void MessagePipe::sendMessage(int outletIndex, PdMessage *message) {
  // remove the scheduled message from the list before it is sent
  scheduledMessagesList->remove(message);
  MessageObject::sendMessage(outletIndex, message);
}

void MessagePipe::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getType(0)) {
        case SYMBOL: {
          if (message->isSymbol(0, "flush")) {
            // cancel all scheduled messages and send them immediately
            PdMessage *scheduledMessage = NULL;
            scheduledMessagesList->resetIterator();
            while ((scheduledMessage = (PdMessage *) scheduledMessagesList->getNext()) != NULL) {
              graph->cancelMessage(this, 0, scheduledMessage);
              scheduledMessage->setTimestamp(message->getTimestamp());
              sendMessage(0, scheduledMessage);
            }
            break;
          } else if (message->isSymbol(0, "clear")) {
            // cancel all scheduled messages
            PdMessage *scheduledMessage = NULL;
            scheduledMessagesList->resetIterator();
            while ((scheduledMessage = (PdMessage *) scheduledMessagesList->getNext()) != NULL) {
              graph->cancelMessage(this, 0, scheduledMessage);
            }
            break;
          }
          // allow fall-through
        }
        case FLOAT:
        case BANG: {
          message->setTimestamp(message->getTimestamp() + delayMs);
          scheduledMessagesList->add(message);
          graph->scheduleMessage(this, 0, message);
          break;
        }
        default: {
          break;
        }
      }
    }
    case 1: {
      if (message->isFloat(0)) {
        delayMs = (double) message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
