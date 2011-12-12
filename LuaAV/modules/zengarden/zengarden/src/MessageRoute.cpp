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

#include "MessageRoute.h"

MessageRoute::MessageRoute(PdMessage *initMessage, PdGraph *graph) : 
    MessageObject(1, initMessage->getNumElements()+1, graph) {
  routeMessage = initMessage->copy();
}

MessageRoute::~MessageRoute() {
  delete routeMessage;
}

const char *MessageRoute::getObjectLabel() {
  return "route";
}

void MessageRoute::processMessage(int inletIndex, PdMessage *message) {
  int numRouteChecks = routeMessage->getNumElements();
  MessageElement *messageElement = message->getElement(0);
  int outletIndex = numRouteChecks; // by default, send the message out of the right outlet
  // find which indicator that message matches
  for (int i = 0; i < numRouteChecks; i++) {
    if (routeMessage->getElement(i)->equals(messageElement)) {
      outletIndex = i;
      break;
    }
  }
  
  if (outletIndex == numRouteChecks) {
    // no match found, forward on right oulet
    sendMessage(outletIndex, message);
  } else {
    // construct a new message to send from the given outlet
    PdMessage *outgoingMessage = getNextOutgoingMessage(outletIndex);
    outgoingMessage->setTimestamp(message->getTimestamp());
    outgoingMessage->clear();
    int numElements = message->getNumElements();
    for (int i = 1; i < numElements; i++) {
      outgoingMessage->addElement(message->getElement(i));
    }
    sendMessage(outletIndex, outgoingMessage);
  }
}
