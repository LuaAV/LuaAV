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

#include "ZGLinkedList.h"

ZGLinkedList::ZGLinkedList() {
  numElements = 0;
  numEmptyElements = 0;
  head = NULL;
  tail = NULL;
  emptyHead = NULL;
  emptyTail = NULL;
  iteratorNode = NULL;
}

ZGLinkedList::~ZGLinkedList() {
  LinkedListNode* node = head;
  LinkedListNode* nextNode = NULL;
  
  // delete the primary list
  while (node != NULL) {
    nextNode = node->next;
    deleteDataHolder(node->data);
    free(node);
    node = nextNode;
  }
  
  // delete the pool list
  node = emptyHead;
  while (node != NULL) {
    nextNode = node->next;
    deleteDataHolder(node->data);
    free(node);
    node = nextNode;
  }
}

int ZGLinkedList::size() {
  return numElements;
}

void *ZGLinkedList::get(int index) {
  if (index < 0 || index >= numElements) {
    return NULL;
  } else {
    LinkedListNode *node = head;
    for (int i = 0; i < index; i++) {
      node = node->next;
    }
    return node->data;
  }
}

void ZGLinkedList::resetIterator() {
  iteratorNode = head;
}

void *ZGLinkedList::getNext() {
  void *data = NULL;
  if (iteratorNode != NULL) {
    data = iteratorNode->data;
    iteratorNode = iteratorNode->next;
  }
  return data;
}

void **ZGLinkedList::add() {
  LinkedListNode *newNode = getEmptyNode();
  insertAfter(newNode, tail);
  return &(newNode->data);
}

void ZGLinkedList::add(void *data) {
  void **nodeData = add();
  *nodeData = data;
}

void ZGLinkedList::add(int index, void *data) {
  if (index <= 0) {
    LinkedListNode *node = getEmptyNode();
    node->data = data;
    insertBefore(node, head);
  } else if (index >= numElements) {
    LinkedListNode *node = getEmptyNode();
    node->data = data;
    insertAfter(node, tail);
  } else {
    LinkedListNode *newNode = getEmptyNode();
    newNode->data = data;
    LinkedListNode *node = head;
    for (int i = 0; i < index; i++) {
      node = node->next;
    }
    insertBefore(newNode, node);
  }
}

LinkedListNode *ZGLinkedList::getEmptyNode() {
  LinkedListNode *newNode = NULL;
  if (numEmptyElements > 0) {
    // if a node is already available from the empty pool, use it
    newNode = emptyHead;
    emptyHead = emptyHead->next;
    numEmptyElements--;
    // newNode->data is already properly configured
  } else {
    // otherwise create a new one
    newNode = (LinkedListNode *) malloc(sizeof(LinkedListNode));
    newNode->data = newDataHolder();
  }
  return newNode;
}

void *ZGLinkedList::remove(LinkedListNode *node) {
  // remove the current node from the list
  if (numElements == 1) {
    // if there is only one node in the list and this node is it
    head = NULL;
    tail = NULL;
  } else if (node == head) {
    head = head->next;
    head->previous = NULL;
  } else if (node == tail) {
    tail = tail->previous;
    tail->next = NULL;
  } else {
    node->previous->next = node->next;
    node->next->previous = node->previous;
  }
  numElements--;
  
  // add the current node to the end of the empty list
  if (numEmptyElements == 0) {
    emptyHead = node;
    emptyTail = node;
    node->next = NULL;
    node->previous = NULL;
  } else {
    emptyTail->next = node;
    node->previous = emptyTail;
    node->next = NULL;
    emptyTail = node;
  }
  numEmptyElements++;
  
  return node->data;
}

void *ZGLinkedList::remove(int index) {
  if (index < 0 || index >= numElements) {
    return NULL;
  } else {
    LinkedListNode *node = head;
    for (int i = 0; i < index; i++) {
      node = node->next;
    }
    return remove(node);
  }
}

void *ZGLinkedList::pop() {
  return (numElements > 0) ? remove(head) : NULL;
}

void *ZGLinkedList::peek() {
  return (numElements > 0) ? head->data : NULL;
}

void ZGLinkedList::remove(void *element) {
  LinkedListNode *node = head;
  while (node != NULL) {
    if (isDataEqualTo(node->data, element)) {
      remove(node);
      return;
    } else {
      node = node->next;
    }
  }
}

void ZGLinkedList::clear() {
  if (numEmptyElements == 0) {
    emptyHead = head;
    emptyTail = tail;
  } else if (numEmptyElements > 0) {
    emptyTail->next = head;
    emptyTail = tail;
  }
  head = NULL;
  tail = NULL;
  numEmptyElements += numElements;
  numElements = 0;
}

void ZGLinkedList::insertAfter(LinkedListNode *nodeA, LinkedListNode *nodeB) {
  if (nodeB == NULL) {
    // no tail has been defined (the list must be empty)
    nodeA->next = NULL;
    nodeA->previous = NULL;
    head = nodeA;
    tail = nodeA;
  } else if (tail == nodeB) {
    // insert nodeA at tail
    tail->next = nodeA;
    nodeA->previous = tail;
    nodeA->next = NULL;
    tail = nodeA;
  } else {
    nodeA->next = nodeB->next;
    nodeA->previous = nodeB;
    nodeB->next = nodeA;
    nodeB->next->previous = nodeA;
  }
  numElements++;
}

void ZGLinkedList::insertBefore(LinkedListNode *nodeA, LinkedListNode *nodeB) {
  if (nodeB == NULL) {
    // no head has been defined (the list must be empty)
    nodeA->next = NULL;
    nodeA->previous = NULL;
    head = nodeA;
    tail = nodeA;
  } else if (nodeB == head) {
    // if nodeB is the head
    nodeA->previous = NULL;
    nodeA->next = nodeB;
    nodeB->previous = nodeA;
    head = nodeA;
  } else {
    nodeB->previous->next = nodeA;
    nodeA->previous = nodeB->previous;
    nodeA->next = nodeB;
    nodeB->previous = nodeA;
  }
  numElements++;
}
