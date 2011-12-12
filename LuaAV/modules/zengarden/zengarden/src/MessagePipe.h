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

#ifndef _MESSAGE_PIPE_H_
#define _MESSAGE_PIPE_H_

#include "MessageObject.h"

class MessagePipe : public MessageObject {
  
  public:
    MessagePipe(PdMessage *initMessage, PdGraph *pdGraph);
    ~MessagePipe();
  
    const char *getObjectLabel();
  
    bool shouldDistributeMessageToInlets();
  
    // override <code>sendMessage</code> in order to remove the message from the scheduled
    // message list as it is sent out.
    void sendMessage(int outletIndex, PdMessage *message);
    
  private:
    void processMessage(int inletIndex, PdMessage *message);
  
    double delayMs; // the delay in milliseconds
    ZGLinkedList *scheduledMessagesList;
};

#endif // _MESSAGE_PIPE_H_
