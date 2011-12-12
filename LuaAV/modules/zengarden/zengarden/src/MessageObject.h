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

#ifndef _MESSAGE_OBJECT_H_
#define _MESSAGE_OBJECT_H_

#include "ConnectionType.h"
#include "ObjectLetPair.h"
#include "ObjectType.h"
#include "PdMessage.h"
#include "StaticUtils.h"

class PdGraph;

class MessageObject {
  
  public:
    MessageObject(int numMessageInlets, int numMessageOutlets, PdGraph *graph);
    virtual ~MessageObject();
    
    /**
     * The generic entrypoint of a message to an object. This function usually
     * either passes the message directly to <code>processMessage</code> in the
     * case of an object which only processes messages, or queues the message for
     * later processing.
     */
    virtual void receiveMessage(int inletIndex, PdMessage *message);
    
    /** The message logic of an object. */
    virtual void processMessage(int inletIndex, PdMessage *message);
  
    /**
     * Sends the given message to all connected objects at the given outlet index.
     * This function can be overridden in order to take some other action, such as additionally
     * scheduling a new message as in the case of <code>MessageMetro</code>.
     */
    virtual void sendMessage(int outletIndex, PdMessage *message);
  
    /** Returns the connection type of the given outlet. */
    virtual ConnectionType getConnectionType(int outletIndex);
  
    /** Establish a connection from another object to this object. */
    virtual void addConnectionFromObjectToInlet(MessageObject *messageObject, int outletIndex, int inletIndex);
  
    /** Establish a connection to another object from this object. */
    virtual void addConnectionToObjectFromOutlet(MessageObject *messageObject, int inletIndex, int outletIndex);
  
    /**
     * The destination inlet of an outgoing message connection can change if an [inlet] object
     * in a graph is moved (and the inlet ordering changes). The connection index change has no
     * effect on the graph ordering and thus it is not necessary to remove and readd a connection.
     * However, the connection must be updated such that message will still be addressed to the
     * correct inlet.
     */
    void updateOutgoingMessageConnection(MessageObject *messageObject, int oldInletIndex,
        int outletIndex, int newInletIndex);
  
    void updateIncomingMessageConnection(MessageObject *messageObject, int oldOutletIndex,
        int inletIndex, int newOutletIndex);
  
    /** Returns the label for this object. */
    virtual const char *getObjectLabel() = 0;
  
    virtual ObjectType getObjectType();
  
    /** Returns <code>true</code> if this object processes audio, <code>false</code> otherwise. */
    virtual bool doesProcessAudio();
  
    /**
     * Returns <code>true</code> if this object should distribute the elements of the incoming
     * message across the inlets. A message is otherwise only distributed if the message arrives
     * on the left-most inlet, has more than one inlet, and has exactly as many elements as there
     * object inlets. This function returns <code>true</code> by default and should be overridden
     * to return <code>false</code> if this behaviour is not desired (e.g., as in the case of the
     * <code>line</code> object). This behaviour is set to <code>false</code> for all
     * <code>DspObject</code> objects.
     */
    virtual bool shouldDistributeMessageToInlets();
  
    /**
     * Returns <code>true</code> if this object is a leaf in the Pd tree. <code>false</code> otherwise.
     * This function is used only while computing the process order of objects. For this reason it also
     * returns true in the cases when the object is send, send~, or throw~.
     */
    virtual bool isLeafNode();
  
    /** Returns an ordered list of all parent objects of this object. */
    virtual List *getProcessOrder();
  
    /**
     * Reset the <code>isOrdered</code> flag to <code>false</code>. This is necessary in order to
     * recompute the process order.
     */
    void resetOrderedFlag();
    
  protected:
    /** Returns a message that can be sent from the given outlet. */
    PdMessage *getNextOutgoingMessage(int outletIndex);
    
    /** Returns a new message for use at the given outlet. */
    virtual PdMessage *newCanonicalMessage(int outletIndex);
  
    PdGraph *graph;    
    int numMessageInlets;
    int numMessageOutlets;
    List **incomingMessageConnectionsListAtInlet;
    List **outgoingMessageConnectionsListAtOutlet;
    List **messageOutletPools;
  
    /** A flag indicating that this object has already been considered when ordering the process tree. */
    bool isOrdered;
  
    /** A <code>PdMessage</code> used only to distribute an incoming message across inlets, when necessary. */
    PdMessage *distributedMessage;
};

#endif // _MESSAGE_OBJECT_H_
