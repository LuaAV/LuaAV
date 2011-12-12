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

#include "MessageTrigger.h"
#include "PdGraph.h"

MessageTrigger::MessageTrigger(PdMessage *initMessage, PdGraph *graph) :
    MessageObject(1, initMessage->getNumElements(), graph) {
  castMessage = initMessage->copy();
  castMessage->resolveSymbolsToType();
}

MessageTrigger::~MessageTrigger() {
  delete castMessage;
}

const char *MessageTrigger::getObjectLabel() {
  return "trigger";
}

void MessageTrigger::processMessage(int inletIndex, PdMessage *message) {
  for (int i = numMessageOutlets-1; i >= 0; i--) { // send messages from outlets right-to-left
    PdMessage *outgoingMessage = getNextOutgoingMessage(i);
    
    // TODO(mhroth): There is currently no support for converting to a LIST type
    switch (message->getType(0)) { // converting from...
      case FLOAT: {
        switch (castMessage->getType(i)) { // converting to...
          case ANYTHING:
          case FLOAT: {
            outgoingMessage->setFloat(0, message->getFloat(0));
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          case SYMBOL: {
            outgoingMessage->setSymbol(0, "float");
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          case BANG: {
            outgoingMessage->getElement(0)->setBang();
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          } default: {
            // send bang
            outgoingMessage->getElement(0)->setBang();
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
        }
        break;
      }
      case SYMBOL: {
        switch (castMessage->getType(i)) {
          case FLOAT: {
            graph->printErr("error : trigger: can only convert 's' to 'b' or 'a'");
            break;
          }
          case ANYTHING: {
            outgoingMessage->setSymbol(0, message->getSymbol(0));
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          case SYMBOL: {
            graph->printErr("error : trigger: can only convert 's' to 'b' or 'a'");
            break;
          }
          case BANG: {
            outgoingMessage->getElement(0)->setBang();
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          default: {
            // send bang
            outgoingMessage->getElement(0)->setBang();
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
        }
        break;
      }
      case BANG: {
        switch (castMessage->getType(i)) {
          case FLOAT: {
            outgoingMessage->setFloat(0, 0.0f);
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          case SYMBOL: {
            outgoingMessage->setSymbol(0, "symbol");
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          case ANYTHING:
          case BANG: {
            outgoingMessage->getElement(0)->setBang();
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
          default: {
            // send bang, error
            outgoingMessage->getElement(0)->setBang();
            outgoingMessage->setTimestamp(message->getTimestamp());
            sendMessage(i, outgoingMessage);
            break;
          }
        }
        break;
      }
      default: {
        // produce a bang if the input type is unknown (error)
        outgoingMessage->getElement(0)->setBang();
        outgoingMessage->setTimestamp(message->getTimestamp());
        sendMessage(i, outgoingMessage);
        break;
      }
    }
  }
}
