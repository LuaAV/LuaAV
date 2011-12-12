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

#ifndef _ORDERED_MESSAGE_QUEUE_H_
#define _ORDERED_MESSAGE_QUEUE_H_

#include "MessageDestination.h"
#include "PdMessage.h"
#include "ZGLinkedList.h"

class OrderedMessageQueue : public ZGLinkedList {
  
  public:
    OrderedMessageQueue();
    ~OrderedMessageQueue();
    
    /** Inserts the message into the ordered queue based on its scheduled time. */
    void insertMessage(MessageObject *messageObject, int outletIndex, PdMessage *message);
  
    /** Removes the given message addressed to the given <code>MessageObject</code> from the queue. */
    void removeMessage(MessageObject *messageObject, int outletIndex, PdMessage *message);
  
  protected:
    void *newDataHolder();
    void deleteDataHolder(void *data);
};

#endif // _ORDERED_MESSAGE_QUEUE_H_
