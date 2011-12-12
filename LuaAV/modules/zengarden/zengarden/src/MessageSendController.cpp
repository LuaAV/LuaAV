/*
 *  Copyright 2009 Reality Jockey, Ltd.
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

#include "MessageSendController.h"
#include "PdContext.h"

// it might nice if this class were implemented using a hashtable with receiver name as the key
// and Lists as the value.
MessageSendController::MessageSendController(PdContext *aContext) : MessageObject(0, 0, NULL) {
  context = aContext;
  nameList = new ZGLinkedList();
  receiverLists = new ZGLinkedList();
}

MessageSendController::~MessageSendController() {
  nameList->resetIterator();
  char *name = NULL;
  while ((name = (char *) nameList->getNext()) != NULL) {
    free(name);
  }
  delete nameList;
  
  receiverLists->resetIterator();
  ZGLinkedList *receiverList = NULL;
  while ((receiverList = (ZGLinkedList *) receiverLists->getNext()) != NULL) {
    delete receiverList;
  }
  delete receiverLists;
}

const char *MessageSendController::getObjectLabel() {
  return "sendcontroller";
}

int MessageSendController::getNameIndex(char *receiverName) {
  if (strcmp("pd", receiverName) == 0) {
    return SYSTEM_NAME_INDEX; // a special case for sending messages to the system
  }
  
  nameList->resetIterator();
  char *name = NULL;
  int i = 0;
  while ((name = (char *) nameList->getNext()) != NULL) {
    if (strcmp(name, receiverName) == 0) {
      return i;
    }
    i++;
  }
  
  return -1;
}

void MessageSendController::receiveMessage(char *name, PdMessage *message) {
  processMessage(getNameIndex(name), message);
}

void MessageSendController::processMessage(int inletIndex, PdMessage *message) {
  sendMessage(inletIndex, message);
}

void MessageSendController::sendMessage(int outletIndex, PdMessage *message) {
  if (outletIndex == SYSTEM_NAME_INDEX) {
    context->receiveSystemMessage(message);
  } else {
    ZGLinkedList *receiverList = (ZGLinkedList *) receiverLists->get(outletIndex);
    receiverList->resetIterator();
    RemoteMessageReceiver *receiver = NULL;
    while ((receiver = (RemoteMessageReceiver *) receiverList->getNext()) != NULL) {
      receiver->receiveMessage(0, message);
    }
  }
}

void MessageSendController::addReceiver(RemoteMessageReceiver *receiver) {
  int nameIndex = getNameIndex(receiver->getName());
  if (nameIndex == -1) {
    nameList->add(StaticUtils::copyString(receiver->getName()));
    receiverLists->add((void *) new ZGLinkedList());
    nameIndex = nameList->size() - 1;
  }
  
  ZGLinkedList *receiverList = (ZGLinkedList *) receiverLists->get(nameIndex);
  receiverList->add(receiver);
}
