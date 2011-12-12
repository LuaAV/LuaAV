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

#ifndef _ZG_LINKED_LIST_H_
#define _ZG_LINKED_LIST_H_

#include <stdlib.h>

struct LinkedListNode {
  void *data;
  LinkedListNode *previous;
  LinkedListNode *next;
};

class ZGLinkedList {
  public:
    ZGLinkedList();
    virtual ~ZGLinkedList();

    /** Adds a new node to the list. Returns a pointer to the data pointer of the new node.*/
    void **add();
    void add(void *data);
    void add(int index, void *data);

    /** Removes the given element from the list. */
    void remove(void *element);

    /** Remove the item at the given index from the list. */
    void *remove(int index);
  
    /** Remove the head of the list. The data contained in the head is returned. */
    void *pop();
  
    /** Returns the data contained at the head of the list. */
    void *peek();

    virtual bool isDataEqualTo(void *dataA, void *dataB) { return (dataA == dataB); }

    /** Returns the number of elements in the list. */
    int size();

    /** Returns the data at the given index. <code>NULL</code> if out-of-bounds. */
    void *get(int index);
  
    /** Reset the iterator for this list. */
    void resetIterator();
    
    /** Get the data from the next node in the list according to the iterator. */
    void *getNext();
  
    /** The list is cleared. */
    void clear();
  
    int numElements;

  protected:
    LinkedListNode *getEmptyNode();

    /** Inserts node A after node B. */
    void insertAfter(LinkedListNode *nodeA, LinkedListNode *nodeB);

    /** Inserts node A before node B. */
    void insertBefore(LinkedListNode *nodeA, LinkedListNode *nodeB);

    void *remove(LinkedListNode *node);

    virtual void *newDataHolder() { return NULL; }

    virtual void deleteDataHolder(void *data) { /* nothing to do */ }

    LinkedListNode *iteratorNode;
  
    LinkedListNode *head;
    LinkedListNode *tail;

    LinkedListNode *emptyHead;
    LinkedListNode *emptyTail;
    int numEmptyElements;
};

#endif // _ZG_LINKED_LIST_H_
