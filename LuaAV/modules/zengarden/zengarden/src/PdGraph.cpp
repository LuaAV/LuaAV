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

#include "DeclareList.h"
#include "DspInlet.h"
#include "DspOutlet.h"
#include "DspTablePlay.h"
#include "DspTableRead.h"
#include "DspTableRead4.h"
#include "MessageInlet.h"
#include "MessageOutlet.h"
#include "MessageTableRead.h"
#include "MessageTableWrite.h"
#include "PdContext.h"
#include "PdGraph.h"
#include "StaticUtils.h"

#pragma mark -
#pragma mark Constructor/Deconstructor

// a PdGraph begins with zero inlets and zero outlets. thses will be added as inlet/~ and outlet/~
// objects are added to the graph
PdGraph::PdGraph(PdMessage *initMessage, PdGraph *parentGraph, PdContext *context, int graphId) :
    DspObject(0, 0, 0, 0, (parentGraph == NULL) ? context->getBlockSize() : parentGraph->getBlockSize(), parentGraph) {
  this->parentGraph = parentGraph; // == NULL if this is a root graph
  this->context = context;
  nodeList = new ZGLinkedList();
  dspNodeList = new List();
  inletList = new ZGLinkedList();
  outletList = new ZGLinkedList();
  declareList = new DeclareList();
  // all graphs start out unattached to any context, though they exist in a context
  isAttachedToContext = false;
  switched = true; // graphs are switched on by default
      
  // initialise the graph arguments
  this->graphId = graphId;
  graphArguments = new PdMessage();
  graphArguments->addElement((float) graphId); // $0
  for (int i = 0; i < initMessage->getNumElements(); i++) {
    graphArguments->addElement(initMessage->getElement(i));
  }
}

PdGraph::~PdGraph() {
  delete dspNodeList;
  delete inletList;
  delete outletList;
  delete graphArguments;
  delete declareList;

  // delete all constituent nodes
  nodeList->resetIterator();
  MessageObject *messageObject = NULL;
  while ((messageObject = (MessageObject *) nodeList->getNext()) != NULL) {
    delete messageObject;
  }
  delete nodeList;
}

const char *PdGraph::getObjectLabel() {
  return "pd";
}

ObjectType PdGraph::getObjectType() {
  return OBJECT_PD;
}

#pragma mark -
#pragma mark Lock/Unlock Context

void PdGraph::lockContextIfAttached() {
  if (isAttachedToContext) {
    context->lock();
  }
}

void PdGraph::unlockContextIfAttached() {
  if (isAttachedToContext) {
    context->unlock();
  }
}

#pragma mark -
#pragma mark Add or Remove Objects, Connections, or Contexts

void PdGraph::addObject(int canvasX, int canvasY, MessageObject *messageObject) {
  lockContextIfAttached();
  
  nodeList->add(messageObject); // all nodes are added to the node list regardless
  
  switch (messageObject->getObjectType()) {
    case MESSAGE_INLET: {
      MessageInlet *letObject = (MessageInlet *) messageObject;
      letObject->setCanvasPosition(canvasX);
      addLetObjectToLetList(letObject, canvasX, inletList);
      break;
    }
    case DSP_INLET: {
      DspInlet *letObject = (DspInlet *) messageObject;
      letObject->setCanvasPosition(canvasX);
      addLetObjectToLetList(letObject, canvasX, inletList);
      break;
    }
    case MESSAGE_OUTLET: {
      MessageOutlet *letObject = (MessageOutlet *) messageObject;
      letObject->setCanvasPosition(canvasX);
      addLetObjectToLetList(letObject, canvasX, outletList);
      break;
    }
    case DSP_OUTLET:{
      DspOutlet *letObject = (DspOutlet *) messageObject;
      letObject->setCanvasPosition(canvasX);
      addLetObjectToLetList(letObject, canvasX, outletList);
      break;
    }
    default: {
      // only register objects with the context if the graph is attached
      if (isAttachedToContext) {
        registerObjectIfRequiresRegistration(messageObject);
      }
    }
  }
  
  unlockContextIfAttached();
}

void PdGraph::addLetObjectToLetList(MessageObject *inletObject, int newPosition, ZGLinkedList *letList) {
  for (int i = 0; i < letList->size(); i++) {
    MessageObject *object = (MessageObject *) letList->get(i);
    int position = -1;
    switch (object->getObjectType()) {
      case MESSAGE_INLET: {
        MessageInlet *inlet = (MessageInlet *) object;
        position = inlet->getCanvasPosition();
        break;
      }
      case DSP_INLET: {
        DspInlet *inlet = (DspInlet *) object;
        position = inlet->getCanvasPosition();
        break;
      }
      case MESSAGE_OUTLET: {
        MessageOutlet *outlet = (MessageOutlet *) object;
        position = outlet->getCanvasPosition();
        break;
      }
      case DSP_OUTLET: {
        DspOutlet *outlet = (DspOutlet *) object;
        position = outlet->getCanvasPosition();
        break;
      }
      default: {
        break;
      }
    }
    if (newPosition < position) {
      letList->add(i-1, inletObject);
      return;
    }
  }
  letList->add(inletObject);
}

float **PdGraph::getDspBufferRefAtOutlet(int outletIndex) {
  DspObject *dspOutlet = (DspObject *) outletList->get(outletIndex);
  return dspOutlet->getDspBufferRefAtOutlet(0);
}

void PdGraph::addConnection(MessageObject *fromObject, int outletIndex, MessageObject *toObject, int inletIndex) {
  lockContextIfAttached();
  toObject->addConnectionFromObjectToInlet(fromObject, outletIndex, inletIndex);
  fromObject->addConnectionToObjectFromOutlet(toObject, inletIndex, outletIndex);
  // NOTE(mhroth): very heavy handed approach. Always recompute the process order when adding connections
  computeLocalDspProcessOrder(); 
  unlockContextIfAttached();
}

void PdGraph::addConnection(int fromObjectIndex, int outletIndex, int toObjectIndex, int inletIndex) {
  MessageObject *fromObject = (MessageObject *) nodeList->get(fromObjectIndex);
  MessageObject *toObject = (MessageObject *) nodeList->get(toObjectIndex);
  addConnection(fromObject, outletIndex, toObject, inletIndex);
}

void PdGraph::attachToContext(bool isAttached) {
  // ensure that this function is only run on attachement change
  if (isAttachedToContext != isAttached) {
    isAttachedToContext = isAttached;
    // ensure that all subgraphs know if they are attached or not
    nodeList->resetIterator();
    MessageObject *messageObject = NULL;
    while ((messageObject = (MessageObject *) nodeList->getNext()) != NULL) {
      if (isAttachedToContext) {
        registerObjectIfRequiresRegistration(messageObject);
      } else {
        unregisterObjectIfRequiresUnregistration(messageObject);
      }
      if (messageObject->getObjectType() == OBJECT_PD) {
        PdGraph *pdGraph = (PdGraph *) messageObject;
        pdGraph->attachToContext(isAttached);
      }
    }
  }
}

void PdGraph::registerObjectIfRequiresRegistration(MessageObject *messageObject) {
  switch (messageObject->getObjectType()) {
    case MESSAGE_RECEIVE:
    case MESSAGE_NOTEIN: {
      context->registerRemoteMessageReceiver((RemoteMessageReceiver *) messageObject);
      break;
    }
    case MESSAGE_TABLE: {
      // tables must be registered globally as a table, but can also receive remote messages
      context->registerRemoteMessageReceiver((RemoteMessageReceiver *) messageObject);
      context->registerTable((MessageTable *) messageObject);
      break;
    }
    case MESSAGE_TABLE_READ: {
      context->registerTableReceiver((MessageTableRead *) messageObject);
      break;
    }
    case MESSAGE_TABLE_WRITE: {
      context->registerTableReceiver((MessageTableWrite *) messageObject);
      break;
    }
    case DSP_CATCH: {
      context->registerDspCatch((DspCatch *) messageObject);
      break;
    }
    case DSP_DELAY_READ:
    case DSP_VARIABLE_DELAY: {
      context->registerDelayReceiver((DelayReceiver *) messageObject);
      break;
    }
    case DSP_DELAY_WRITE: {
      context->registerDelayline((DspDelayWrite *) messageObject);
      break;
    }
    case DSP_SEND: {
      context->registerDspSend((DspSend *) messageObject);
      break;
    }
    case DSP_RECEIVE: {
      context->registerDspReceive((DspReceive *) messageObject);
      break;
    }
    case DSP_TABLE_PLAY: {
      context->registerTableReceiver((DspTablePlay *) messageObject);
      break;
    }
    case DSP_TABLE_READ4: {
      context->registerTableReceiver((DspTableRead4 *) messageObject);
      break;
    }
    case DSP_TABLE_READ: {
      context->registerTableReceiver((DspTableRead *) messageObject);
      break;
    }
    case DSP_THROW: {
      context->registerDspThrow((DspThrow *) messageObject);
      break;
    }
    default: {
      break; // nothing to do
    }
  }
}

void PdGraph::unregisterObjectIfRequiresUnregistration(MessageObject *messageObject) {
  // TODO(mhroth)
}

float PdGraph::getBlockIndex(PdMessage *message) {
  // sampleRate is in samples/second, but we need samples/millisecond
  return ((float) (message->getTimestamp() - context->getBlockStartTimestamp())) *
      context->getSampleRate() / 1000.0f;
}

float PdGraph::getSampleRate() {
  // there is no such thing as a local sample rate. Return the sample rate of the context.
  return context->getSampleRate();
}

int PdGraph::getGraphId() {
  return graphId;
}

float *PdGraph::getGlobalDspBufferAtInlet(int inletIndex) {
  return context->getGlobalDspBufferAtInlet(inletIndex);
}

float *PdGraph::getGlobalDspBufferAtOutlet(int outletIndex) {
  return context->getGlobalDspBufferAtOutlet(outletIndex);
}

char *PdGraph::resolveFullPath(char *filename) {
  if (DeclareList::isFullPath(filename)) {
    return StaticUtils::fileExists(filename) ? StaticUtils::copyString(filename) : NULL;
  } else {
    char *directory = findFilePath(filename);
    return (directory != NULL) ? StaticUtils::concatStrings(directory, filename) : NULL;
  }
}

char *PdGraph::findFilePath(char *filename) {
  char *directory = NULL;
  declareList->resetIterator();
  while ((directory = (char *) declareList->getNext()) != NULL) {
    char *fullPath = StaticUtils::concatStrings(directory, filename);
    if (StaticUtils::fileExists(fullPath)) {
      free(fullPath);
      return directory;
    } else {
      free(fullPath);
    }
  }
  return isRootGraph() ? NULL : parentGraph->findFilePath(filename);
}

void PdGraph::addDeclarePath(char *path) {
  if (isRootGraph()) {
    declareList->addPath(path);
  } else {
    if (graphId == parentGraph->getGraphId()) {
      // this graph is a subgraph (not an abstraction) of the parent graph
      // so the parent should handle the declared path
      parentGraph->addDeclarePath(path);
    } else {
      declareList->addPath(path);
    }
  }
}

#pragma mark -
#pragma mark Manage Messages

void PdGraph::scheduleMessage(MessageObject *messageObject, int outletIndex, PdMessage *message) {
  context->scheduleMessage(messageObject, outletIndex, message);
}

void PdGraph::cancelMessage(MessageObject *messageObject, int outletIndex, PdMessage *message) {
  context->cancelMessage(messageObject, outletIndex, message);
}

void PdGraph::sendMessageToNamedReceivers(char *name, PdMessage *message) {
  context->sendMessageToNamedReceivers(name, message);
}

#pragma mark -
#pragma mark Message/DspObject Functions

void PdGraph::receiveMessage(int inletIndex, PdMessage *message) {
  MessageInlet *inlet = (MessageInlet *) inletList->get(inletIndex);
  inlet->receiveMessage(0, message);
}

void PdGraph::processDsp() {
  if (switched) {
    // when inlets are processed, they will resolve their buffers and everything will proceed as normal
    
    // process all dsp objects
    // DSP processing elements are only executed if the graph is switched on
    int numNodes = dspNodeList->numElements;
    DspObject **dspNodeArray = (DspObject **) dspNodeList->arrayList;
    //for (int i = 0; i < 1; i++) { // TODO(mhroth): iterate depending on local blocksize relative to parent
    // execute all nodes which process audio
    while (numNodes--) {
      (*dspNodeArray++)->processDsp();
    }    
    //}
  }
}

void PdGraph::addConnectionFromObjectToInlet(MessageObject *messageObject, int outletIndex, int inletIndex) {
  switch (messageObject->getConnectionType(outletIndex)) {
    case MESSAGE: {
      MessageObject *inletObject = (MessageObject *) inletList->get(inletIndex);
      if (inletObject->getObjectType() == MESSAGE_INLET) {
        MessageInlet *messageInlet = (MessageInlet *) inletObject;
        messageInlet->addConnectionFromObjectToInlet(messageObject, outletIndex, 0);
      } else {
        printErr("Connection [%s]:%i is of type DSP and cannot be connected to inlet.",
            messageObject->getObjectLabel(), outletIndex);
      }
      break;
    }
    case DSP: {
      MessageObject *inletObject = (MessageObject *) inletList->get(inletIndex);
      if (inletObject->getObjectType() == DSP_INLET) {
        DspInlet *dspInlet = (DspInlet *) inletObject;
        dspInlet->addConnectionFromObjectToInlet(messageObject, outletIndex, 0);
      } else {
        printErr("Connection [%s]:%i is of type MESSAGE and cannot be connected to inlet~.",
            messageObject->getObjectLabel(), outletIndex);
      }
      break;
    }
    default: {
      break;
    }
  }
}

void PdGraph::addConnectionToObjectFromOutlet(MessageObject *messageObject, int inletIndex, int outletIndex) {
  switch (getConnectionType(outletIndex)) {
    case MESSAGE: {
      MessageOutlet *messageOutlet = (MessageOutlet *) outletList->get(inletIndex);
      messageOutlet->addConnectionToObjectFromOutlet(messageObject, inletIndex, 0);
      break;
    }
    case DSP: {
      DspOutlet *dspOutlet = (DspOutlet *) outletList->get(outletIndex);
      dspOutlet->addConnectionToObjectFromOutlet(messageObject, inletIndex, 0);
      break;
    }
    default: {
      break;
    }
  }
}

List *PdGraph::getProcessOrder() {
  if (isOrdered) {
    return new List();
  } else {
    isOrdered = true;
    List *processOrder = new List();
    MessageObject *messageObject = NULL;
    inletList->resetIterator();
    while ((messageObject = (MessageObject *) inletList->getNext()) != NULL) {
      switch (messageObject->getObjectType()) {
        case MESSAGE_INLET: {
          MessageInlet *messgeInlet = (MessageInlet *) messageObject;
          List *inletProcessOrder = messgeInlet->getProcessOrderFromInlet();
          processOrder->add(inletProcessOrder);
          delete inletProcessOrder;
          break;
        }
        case DSP_INLET: {
          DspInlet *dspInlet = (DspInlet *) messageObject;
          List *inletProcessOrder = dspInlet->getProcessOrderFromInlet();
          processOrder->add(inletProcessOrder);
          delete inletProcessOrder;
          break;
        }
        default: {
          break;
        }
      }
    }
    processOrder->add(this);
    return processOrder;
  }
}

bool PdGraph::isLeafNode() {
  MessageObject *messageObject = NULL;
  outletList->resetIterator();
  while ((messageObject = (MessageObject *) outletList->getNext()) != NULL) {
    if (!messageObject->isLeafNode()) {
      return false;
    }
  }
  return true;
}

void PdGraph::computeLocalDspProcessOrder() {
  lockContextIfAttached();
  
  /* clear/reset dspNodeList
   * Find all leaf nodes in nodeList. this includes PdGraphs as they are objects as well.
   * For each leaf node, generate an ordering for all of the nodes in the current graph.
   * the basic idea is to compute the full process order in each subgraph.
   * Finally, all non-dsp nodes must be removed from this list in order to derive the dsp process order.
   */

  // generate the leafnode list
  List *leafNodeList = new List();
  MessageObject *object = NULL;
  nodeList->resetIterator();
  while ((object = (MessageObject *) nodeList->getNext()) != NULL) {
    object->resetOrderedFlag(); // reset the ordered flag on all local objects
    if (object->isLeafNode()) { // isLeafNode() takes into account send/~ and throw~ objects
      leafNodeList->add(object);
    }
  }

  // for all leaf nodes, order the tree
  List *processList = new List();
  for (int i = 0; i < leafNodeList->size(); i++) {
    object = (MessageObject *) leafNodeList->get(i);
    List *processSubList = object->getProcessOrder();
    processList->add(processSubList);
    delete processSubList;
  }

  delete leafNodeList;

  // add only those nodes which process audio to the final list
  dspNodeList->clear(); // reset the dsp node list
  for (int i = 0; i < processList->size(); i++) {
    // reverse order of process list such that the dsp elements at the top of the graph are processed first
    object = (MessageObject *) processList->get(i);
    if (object->doesProcessAudio()) {
      dspNodeList->add(object);
    }
  }

  delete processList;
  
  /* print out process order of local dsp objects (for debugging) */
  /*
  if (dspNodeList->size() > 0) {
    // print dsp evaluation order for debugging, but only if there are any nodes to list
    printStd("--- ordered evaluation list ---");
    for (int i = 0; i < dspNodeList->size(); i++) {
      MessageObject *messageObject = (MessageObject *) dspNodeList->get(i);
      printStd(messageObject->getObjectLabel());
    }
  }
  */
  
  unlockContextIfAttached();
}

#pragma mark -
#pragma mark Print

void PdGraph::printErr(const char *msg, ...) {
  int maxStringLength = 1024;
  char stringBuffer[maxStringLength];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(stringBuffer, maxStringLength-1, msg, ap);
  va_end(ap);
  
  context->printErr(stringBuffer);
}

void PdGraph::printStd(const char *msg, ...) {
  int maxStringLength = 1024;
  char stringBuffer[maxStringLength];
  va_list ap;
  va_start(ap, msg);
  vsnprintf(stringBuffer, maxStringLength-1, msg, ap);
  va_end(ap);

  context->printStd(stringBuffer);
}

#pragma mark -
#pragma mark Get Attributes

PdMessage *PdGraph::getArguments() {
  return graphArguments;
}

int PdGraph::getNumInputChannels() {
  return context->getNumInputChannels();
}

int PdGraph::getNumOutputChannels() {
  return context->getNumOutputChannels();
}

int PdGraph::getBlockSize() {
  return blockSizeInt;
}

bool PdGraph::isRootGraph() {
  return (parentGraph == NULL);
}

MessageTable *PdGraph::getTable(char *name) {
  return context->getTable(name);
}

ConnectionType PdGraph::getConnectionType(int outletIndex) {
  // return the connection type depending on the type of outlet object
  MessageObject *messageObject = (MessageObject *) outletList->get(outletIndex);
  return messageObject->getConnectionType(0);
}

bool PdGraph::doesProcessAudio() {
  // This graph processes audio if it contains any nodes which process audio.
  // This works because graph objects are only created after they have been filled with objects.
  return (dspNodeList->size() > 0);
}

void PdGraph::setBlockSize(int blockSize) {
  // only update blocksize if it is <= the parent's
  if (blockSize <= parentGraph->getBlockSize()) {
    // TODO(mhroth)
    blockSizeInt = blockSize;
    blockSizeFloat = (float) blockSize;
    // update blockDurationMs, etc.
    // notify all dsp objects and get them to resize their buffers accordingly
  }
}

PdGraph *PdGraph::getParentGraph() {
  return parentGraph;
}

void PdGraph::setSwitch(bool switched) {
  this->switched = switched;
}

bool PdGraph::isSwitchedOn() {
  return switched;
}

void PdGraph::setValueForName(char *name, float constant) {
  context->setValueForName(name, constant);
}

float PdGraph::getValueForName(char *name) {
  return context->getValueForName(name);
}
