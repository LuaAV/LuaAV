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

#include "PdContext.h"
#include "PdGraph.h"
#include "ZenGarden.h"

ZGContext *zg_new_context(int numInputChannels, int numOutputChannels, int blockSize, float sampleRate,
    void (*callbackFunction)(ZGCallbackFunction function, void *userData, void *ptr), void *userData) {
  PdContext *context = new PdContext(numInputChannels, numOutputChannels, blockSize, sampleRate,
      callbackFunction, userData);
  return context;
}

ZGGraph *zg_new_empty_graph(PdContext *context) {
  PdMessage *initMessage = new PdMessage(); // create an empty message to use for initialisation
  // the new graph has no parent grpah and is created in the given context
  PdGraph *graph = new PdGraph(initMessage, NULL, context, context->getNextGraphId());
  delete initMessage; // destroy the dummy initial message
  return graph;
}

ZGGraph *zg_new_graph(PdContext *context, char *directory, char *filename) {
  PdMessage *initMessage = new PdMessage(); // create an empty initMessage
  // no parent graph
  PdGraph *graph = context->newGraph(directory, filename, initMessage, NULL);
  delete initMessage;
  return graph;
}

void zg_attach_graph(PdContext *context, PdGraph *graph) {
  context->attachGraph(graph);
}
/*
void zg_remove_graph(PdContext *context, PdGraph *graph) {
  context->removeGraph(graph);
}
*/
void zg_add_object(PdGraph *graph, MessageObject *object) {
  graph->addObject(0, 0, object);
}
/*
void zg_remove_object(PdGraph *graph, MessageObject *object) {
  graph->removeObject(object);
}
*/
void zg_delete_context(ZGContext *context) {
  if (context != NULL) {
    delete context;
  }
}

void zg_delete_graph(ZGGraph *graph) {
  if (graph != NULL) {
    /*
    if (graph->isAttachedToContext()) {
      context->removeGraph(graph);
    }
    */
    delete graph;
  }
}

void zg_process(PdContext *context, float *inputBuffers, float *outputBuffers) {
  context->process(inputBuffers, outputBuffers);
}

void zg_send_message(PdContext *context, const char *receiverName, const char *messageFormat, ...) {
  va_list ap;
  va_start(ap, messageFormat);
  context->scheduleExternalMessageV(receiverName, 0.0, messageFormat, ap);
  va_end(ap); // release the va_list
}

void zg_send_message_at_blockindex(PdContext *context, const char *receiverName, double blockIndex,
    const char *messageFormat, ...) {
  va_list ap;
  va_start(ap, messageFormat);
  double timestamp = context->getBlockStartTimestamp();
  if (blockIndex >= 0.0 && blockIndex <= (double) (context->getBlockSize()-1)) {
    timestamp += blockIndex / context->getSampleRate();
  }
  context->scheduleExternalMessageV(receiverName, timestamp, messageFormat, ap);
  va_end(ap);
}

void zg_send_midinote(PdContext *context, int channel, int noteNumber, int velocity, double blockIndex) {
  const char *receiverName;
  switch (channel) {
    case 0:  { receiverName = "zg_notein_0";  break; }
    case 1:  { receiverName = "zg_notein_1";  break; }
    case 2:  { receiverName = "zg_notein_2";  break; }
    case 3:  { receiverName = "zg_notein_3";  break; }
    case 4:  { receiverName = "zg_notein_4";  break; }
    case 5:  { receiverName = "zg_notein_5";  break; }
    case 6:  { receiverName = "zg_notein_6";  break; }
    case 7:  { receiverName = "zg_notein_7";  break; }
    case 8:  { receiverName = "zg_notein_8";  break; }
    case 9:  { receiverName = "zg_notein_9";  break; }
    case 10: { receiverName = "zg_notein_10"; break; }
    case 11: { receiverName = "zg_notein_11"; break; }
    case 12: { receiverName = "zg_notein_12"; break; }
    case 13: { receiverName = "zg_notein_13"; break; }
    case 14: { receiverName = "zg_notein_14"; break; }
    case 15: { receiverName = "zg_notein_15"; break; }
    default: {
      return;
    }
  }
  
  zg_send_message_at_blockindex(context, receiverName, blockIndex, "fff",
      (float) noteNumber, (float) velocity, (float) channel);
  
  // all message are also sent to the omni listener
  zg_send_message_at_blockindex(context, "zg_notein_omni", blockIndex, "fff",
      (float) noteNumber, (float) velocity, (float) channel);
}
