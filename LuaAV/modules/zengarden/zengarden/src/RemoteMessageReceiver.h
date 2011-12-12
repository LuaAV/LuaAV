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

#ifndef _REMOTE_MESSAGE_RECEIVER_H_
#define _REMOTE_MESSAGE_RECEIVER_H_

#include "MessageObject.h"

/**
 * This is a superclass of all <code>MessageObject</code>s which receive messages
 * remotely. This includes objects such as <code>MessageReceive</code> and <code>MessageNotein</code>.
 */
class RemoteMessageReceiver : public MessageObject {
  
  public:
    RemoteMessageReceiver(int numMessageInlets, int numMessageOutlets, PdGraph *graph);
    virtual ~RemoteMessageReceiver();
  
    char *getName();
  
  protected:
    // This class provides the name variable, but subclasses are responsible for managing it.
    char *name;
};

#endif // _REMOTE_MESSAGE_RECEIVER_H_
