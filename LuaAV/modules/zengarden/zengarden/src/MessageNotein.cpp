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

#include <stdio.h>
#include "MessageNotein.h"

MessageNotein::MessageNotein(PdMessage *initMessage, PdGraph *graph) :
    RemoteMessageReceiver(0, 3, graph) {
  if (initMessage->isFloat(0) &&
      (initMessage->getFloat(0) >= 1.0f && initMessage->isFloat(0) <= 16.0f)) {
    // channel provided (Pd channels are indexed from 1, while ZG channels are indexed from 0)
    channel = (int) (initMessage->getFloat(0)-1.0f);
    name = (char *) calloc(13, sizeof(char));
    sprintf(name, "zg_notein_%i", channel);
  } else {
    // no channel provided, use omni
    channel = -1;
    name = StaticUtils::copyString((char *) "zg_notein_omni");
  }
}

MessageNotein::~MessageNotein() {
  free(name);
}

const char *MessageNotein::getObjectLabel() {
  return "notein";
}

ObjectType MessageNotein::getObjectType() {
  return MESSAGE_NOTEIN;
}

int MessageNotein::getChannel() {
  return channel;
}

bool MessageNotein::isOmni() {
  return (channel == -1);
}

void MessageNotein::processMessage(int inletIndex, PdMessage *message) {
  if (isOmni()) {
    // send channel
    PdMessage *outgoingMessage = getNextOutgoingMessage(2);
    outgoingMessage->setTimestamp(message->getTimestamp());
    outgoingMessage->setFloat(0, message->getFloat(2));
    sendMessage(2, outgoingMessage);
  }
  
  // send velocity
  PdMessage *outgoingMessage = getNextOutgoingMessage(1);
  outgoingMessage->setTimestamp(message->getTimestamp());
  outgoingMessage->setFloat(0, message->getFloat(1));
  sendMessage(1, outgoingMessage);
  
  // send note
  outgoingMessage = getNextOutgoingMessage(0);
  outgoingMessage->setTimestamp(message->getTimestamp());
  outgoingMessage->setFloat(0, message->getFloat(0));
  sendMessage(0, outgoingMessage);
}
