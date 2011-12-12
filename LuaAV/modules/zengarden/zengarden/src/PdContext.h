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

#ifndef _PD_CONTEXT_H_
#define _PD_CONTEXT_H_

#include <pthread.h>

#include "OrderedMessageQueue.h"
#include "PdGraph.h"
#include "ZGCallbackFunction.h"

class DspCatch;
class DelayReceiver;
class DspDelayWrite;
class DspReceive;
class DspSend;
class DspThrow;
class MessageSendController;
class MessageTable;
class PdFileParser;
class RemoteMessageReceiver;
class TableReceiverInterface;

/**
 * The <code>PdContext</code> is a container for a set of <code>PdGraph</code>s operating in
 * a common environment. The <code>PdContext</code> tracks all relevent global variables.
 */
class PdContext {
  
  public:
    PdContext(int numInputChannels, int numOutputChannels, int blockSize, float sampleRate,
        void (*function)(ZGCallbackFunction, void *, void *), void *userData);
    ~PdContext();
  
    int getNumInputChannels();
    int getNumOutputChannels();
    int getBlockSize();
    float getSampleRate();
  
    /**
     * Create a new graph using the given file, with the given arguments and parent graph.
     * The parent graph may be <code>NULL</code> in the case that it is a top-level graph.
     */
    PdGraph *newGraph(char *directory, char *filename, PdMessage *initMessage, PdGraph *graph);
  
    /**
     * Attach the given <code>graph</code> to this <code>context</code>, also registering all
     * necessary objects, and computing the dsp object compute order if necessary.
     */
    void attachGraph(PdGraph *graph);
    
    void process(float *inputBuffers, float *outputBuffers);
  
    void lock();
    void unlock();
  
    /** Globally register a remote message receiver (e.g. [send] or [notein]). */
    void registerRemoteMessageReceiver(RemoteMessageReceiver *receiver);
  
    /** Globally register a [receive~] object. Connect to registered [send~] objects with the same name. */
    void registerDspReceive(DspReceive *dspReceive);
    
    /** Globally register a [send~] object. Connect to registered [receive~] objects with the same name. */
    void registerDspSend(DspSend *dspSend);
  
    /** Returns the named global <code>DspSend</code> object. */
    DspSend *getDspSend(char *name);
    
    /**
     * Globally register a [delwrite~] object. Registration is necessary such that they can
     * be connected to [delread~] and [vd~] objects as are they are added to the graph.
     */
    void registerDelayline(DspDelayWrite *delayline);
    
    /** Returns the named global <code>DspDelayWrite</code> object. */
    DspDelayWrite *getDelayline(char *name);
    
    /**
     * Globally register a [delread~] or [vd~] object. Registration is necessary such that they can
     * be connected to [delwrite~] objects are they are added to the graph.
     */
    void registerDelayReceiver(DelayReceiver *delayReceiver);
    
    void registerDspThrow(DspThrow *dspThrow);
    
    void registerDspCatch(DspCatch *dspCatch);
    
    void registerTable(MessageTable *table);
    
    void registerTableReceiver(TableReceiverInterface *tableReceiver);
    
    MessageTable *getTable(char *name);
    
    /** Returns the named global <code>DspCatch</code> object. */
    DspCatch *getDspCatch(char *name);
    
    /**
     * Sends the given message to all [receive] objects with the given <code>name</code>.
     * This function is used by message boxes to send messages described be the syntax:
     * ;
     * name message;
     */
    void sendMessageToNamedReceivers(char *name, PdMessage *message);
  
    /**
     * Schedules a message to be sent to all receivers at the start of the next block.
     * @returns The <code>PdMessage</code> which will be sent. It is intended that the programmer
     * will set the values of the message with a call to <code>setMessage()</code>.
     */
    void scheduleExternalMessageV(const char *receiverName, double timestamp,
        const char *messageFormat, va_list ap);
  
    /**
     * Schedules a <code>PdMessage</code> to be sent by the <code>MessageObject</code> from the
     * <code>outletIndex</code> at the specified <code>time</code>.
     */
    void scheduleMessage(MessageObject *messageObject, int outletIndex, PdMessage *message);
  
    /** Cancel a scheduled <code>PdMessage</code> according to its id. */
    void cancelMessage(MessageObject *messageObject, int outletIndex, PdMessage *message);
  
    /** Receives and processes messages sent to the Pd system by sending to "pd". */
    void receiveSystemMessage(PdMessage *message);
  
    /** Returns the global dsp buffer at the given inlet. Exclusively used by <code>DspAdc</code>. */
    float *getGlobalDspBufferAtInlet(int inletIndex);
    
    /** Returns the global dsp buffer at the given outlet. Exclusively used by <code>DspDac</code>. */
    float *getGlobalDspBufferAtOutlet(int outletIndex);
  
    /** Returns the timestamp of the beginning of the current block. */
    double getBlockStartTimestamp();
    
    /** Returns the duration in milliseconds of one block. */
    double getBlockDuration();
  
    /** Prints the given message to error output. */
    void printErr(char *msg);
    void printErr(const char *msg, ...);
    
    /** Prints the given message to standard output. */
    void printStd(char *msg);
    void printStd(const char *msg, ...);
  
    /** Returns the next globally unique graph id. */
    static int getNextGraphId();
  
    /** Used with MessageValue for keeping track of global variables. */
    void setValueForName(char *name, float constant);
    float getValueForName(char *name);
  
  private:
    /** Returns <code>true</code> if the graph was successfully configured. <code>false</code> otherwise. */
    bool configureEmptyGraphWithParser(PdGraph *graph, PdFileParser *fileParser);
  
    /** Create a new object based on its initialisation string. */
    MessageObject *newObject(char *objectType, char *objectLabel, PdMessage *initMessage, PdGraph *graph);
  
    /**
     * This is an analog of MessageObject::getNextOutgoingMessage(), but strictly for use with the
     * external messages.
     */
    PdMessage *getNextExternalMessage();
  
    List *externalMessagePool;
  
    int numInputChannels;
    int numOutputChannels;
    int blockSize;
    float sampleRate;
  
    /** Keeps track of the current global graph id. */
    static int globalGraphId;
  
    /** A list of all top-level graphs in this context. */
    ZGLinkedList *graphList;
  
    /** A thread lock used to access critical sections of this context. */
    pthread_mutex_t contextLock;
    
    int numBytesInInputBuffers;
    int numBytesInOutputBuffers;
    
    float *globalDspInputBuffers;
    float *globalDspOutputBuffers;
  
    /** A message queue keeping track of all scheduled messages. */
    OrderedMessageQueue *messageCallbackQueue;
  
    /** The start of the current block in milliseconds. */
    double blockStartTimestamp;
    
    /** The duration of one block in milliseconds. */
    double blockDurationMs;
  
    /** The global send controller. */
    MessageSendController *sendController;
  
    /** A global list of all [send~] objects. */
    ZGLinkedList *dspSendList;
    
    /** A global list of all [receive~] objects. */
    ZGLinkedList *dspReceiveList;
    
    /** A global list of all [delwite~] objects. */
    ZGLinkedList *delaylineList;
    
    /** A global list of all [delread~] and [vd~] objects. */
    ZGLinkedList *delayReceiverList;
    
    /** A global list of all [throw~] objects. */
    ZGLinkedList *throwList;
    
    /** A global list of all [catch~] objects. */
    ZGLinkedList *catchList;
    
    /** A global list of all [table] objects. */
    ZGLinkedList *tableList;
    
    /** A global list of all table receivers ([tabread4~] and [tabplay~]) */
    ZGLinkedList *tableReceiverList;
  
    /** The registered callback function for sending data outside of the graph. */
    void (*callbackFunction)(ZGCallbackFunction, void *, void *);
  
    /** User-provided data associated with the callback function. */
    void *callbackUserData;
};

#endif // _PD_CONTEXT_H_
