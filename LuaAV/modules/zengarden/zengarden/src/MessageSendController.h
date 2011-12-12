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

#ifndef _MESSAGE_SEND_CONTROLLER_H_
#define _MESSAGE_SEND_CONTROLLER_H_

#include "MessageObject.h"
#include "RemoteMessageReceiver.h"

class PdContext;

/**
 * Because of features such as external message injection and implicit message sending from message
 * boxes, it must be possible to [send] a message to associated [receive]ers without explicitly
 * requiring a [send] object to exist in the graph. The <code>MessageSendController</code> is
 * intended as a central dispatcher for all sent messages, delivering a message to all associated
 * [receive]ers without requiring the existance of a [send] object.
 *
 * It can deliver messages via the usual <code>receiveMessage()</code> function, where the inlet
 * index identifies a receiver name. The index for a receiver name can be determined with
 * <code>getNameIndex()</code>. This is useful for messages that must be scheduled in the global
 * message queue and must then be dispatched via the <code>receiveMessage()</code> function.
 *
 * Alternatively, a message can be sent to receivers using <code>receiveMessage()</code> with
 * name and message arguments (instead of inlet index and message). Messages sent using this
 * alternative will be sent right away (avoiding the message queue).
 */
class MessageSendController : public MessageObject {
  
  public:
    MessageSendController(PdContext *context);
    ~MessageSendController();
  
    const char *getObjectLabel();
  
    /** Sends the message on to all receivers with the given name. */
    void receiveMessage(char *name, PdMessage *message);
  
    void sendMessage(int outletIndex, PdMessage *message);
  
    /**
     * Returns the index to which the given receiver name is referenced.
     * Used with <code>receiveMessage(int, PdMessage *)</code>.
     */
    int getNameIndex(char *name);
  
    void addReceiver(RemoteMessageReceiver *receiver);
  
  private:
    void processMessage(int inletIndex, PdMessage *message);
  
    PdContext *context;
  
    ZGLinkedList *nameList;
    ZGLinkedList *receiverLists;
  
    // a special index for referencing the system "pd" receiver
    static const int SYSTEM_NAME_INDEX = 0x7FFFFFFF;
};

#endif // _MESSAGE_SEND_CONTROLLER_H_
