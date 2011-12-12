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

#include "OrderedMessageQueue.h"

OrderedMessageQueue::OrderedMessageQueue() : ZGLinkedList() {
  // nothing to do
}

OrderedMessageQueue::~OrderedMessageQueue() {
  LinkedListNode* node = head;
  
  // delete the primary list data
  while (node != NULL) {
    deleteDataHolder(node->data);
    node = node->next;
  }
  
  // delete the pool list data
  node = emptyHead;
  while (node != NULL) {
    deleteDataHolder(node->data);
    node = node->next;
  }
}

void OrderedMessageQueue::insertMessage(MessageObject *messageObject, int outletIndex, PdMessage *message) {
  LinkedListNode *newNode = getEmptyNode();
  MessageDestination *destination = (MessageDestination *) newNode->data;
  destination->object = messageObject;
  destination->message = message;
  destination->index = outletIndex;
  
  LinkedListNode *node = head;
  while (node != NULL) {
    destination = (MessageDestination *) node->data;
    if (message->getTimestamp() < destination->message->getTimestamp()) {
      insertBefore(newNode, node);
      return;
    } else {
      node = node->next;
    }
  }
  insertAfter(newNode, tail);
}

void OrderedMessageQueue::removeMessage(MessageObject *messageObject, int outletIndex, PdMessage *message) {
  MessageDestination *destination;
  LinkedListNode *iteratorNode = head;
  while ((iteratorNode != NULL) && (destination = (MessageDestination *) iteratorNode->data) != NULL) {
    if (destination->object == messageObject &&
        destination->message == message &&
        destination->index == outletIndex) {
      remove(iteratorNode);
      return;
    } else {
      iteratorNode = iteratorNode->next;
    }
  }
}

void *OrderedMessageQueue::newDataHolder() {
  return malloc(sizeof(MessageDestination));
}

void OrderedMessageQueue::deleteDataHolder(void *data) {
  free((MessageDestination *) data);
}
