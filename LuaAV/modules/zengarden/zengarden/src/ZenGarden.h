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

#ifndef _ZENGARDEN_H_
#define _ZENGARDEN_H_

#include "ZGCallbackFunction.h"

/**
 * This header file defines the C interface to ZenGarden to the outside world. Include this header
 * along with the <code>libzengarden</code> library in your project in order to integrate it.
 */
#ifdef __cplusplus
class PdContext;
class PdGraph;
class MessageObject;
typedef PdContext ZGContext;
typedef PdGraph ZGGraph;
typedef MessageObject ZGObject;
extern "C" {
#else
  typedef void ZGGraph;
  typedef void ZGContext;
  typedef void ZGObject;
#endif
  
  /** Create a new context to which graphs can be added. */
  ZGContext *zg_new_context(int numInputChannels, int numOutputChannels, int blockSize, float sampleRate,
      void (*callbackFunction)(ZGCallbackFunction function, void *userData, void *ptr), void *userData);

  /** Create a new empty graph in the given context. Ideal for building graphs on the fly. */
  ZGGraph *zg_new_empty_graph(ZGContext *context);
  
  /* Create a new graph from a Pd file. */
  ZGGraph *zg_new_graph(ZGContext *context, char *directory, char *filename);
  
  /** Attach a graph to the context. */
  void zg_attach_graph(ZGContext *context, ZGGraph *graph);
  
  /** Remove the graph from the context. */
  //void zg_remove_graph(ZGContext *context, ZGGraph *graph);
  
  /**
   * Delete the given context. All attached graphs are also deleted. Unattached graphs are not
   * automatically deleted, but should be by the user. They are thereafter useless.
   */
  void zg_delete_context(ZGContext *context);
  
  /** Deletes the given graph. If attached, the graph is automatically removed from its context. */
  void zg_delete_graph(ZGGraph *graph);
  
  /**
   * Add an object to a graph. If the graph is currently attached then audio may be interrupted
   * while the object is attached the and graph reconfigured (if necessary). If the graph is unattached
   * then no audio interruption will take place, even if reconfiguration takes place.
   */
  void zg_add_object(ZGGraph *graph, ZGObject *object);
  
  /**
   * Removes the object from the graph. Any connections that this object may have had in the graph
   * are also deleted.
   */
  void zg_remove_object(ZGGraph *graph, ZGObject *object);

  /** Process the given context. */
  void zg_process(ZGContext *context, float *inputBuffers, float *outputBuffers);
  
  /**
   * Send a message to the named receiver with the given format at the beginning of the next audio block.
   * If no receiver exists with the given name, then this funtion does nothing.
   * E.g., zg_send_message(graph, "#accelerate", "fff", 0.0f, 0.0f, 0.0f);
   * sends a message containing three floats, each with value 0.0f, to all receivers named "#accelerate".
   * Messages may also be formatted with "s" and "b" for symbols and bangs, respectively.
   * E.g., zg_send_message(graph, "test", "s", "hello");
   * E.g., zg_send_message(graph, "test", "b");
   */
  void zg_send_message(ZGContext *context, const char *receiverName, const char *messageFormat, ...);
  
  /**
   * Send a message to the named receiver with the given format at the given block index. If the
   * block index is negative or greater than the block size (given when instantiating the graph)
   * the the message will be sent at the very beginning of the next block. A fractional block index
   * may be given, and the message will be evaluated between rendered samples. If the given block
   * index falls outside of the block size (either positive or negative), then the message will be
   * delivered at the beginning of the block. If no receiver exists with the given name, then this
   * funtion does nothing.
   * This function is equivalent to e.g., zg_send_message(graph, "#accelerate", 0.0, "fff", 0.0f, 0.0f, 0.0f)
   * E.g., zg_send_message_at_blockindex(graph, "#accelerate", 56.3, "fff", 0.0f, 0.0f, 0.0f);
   * sends a message containing three floats, each with value 0.0f, to all receivers named "#accelerate"
   * between samples 56th and 57th samples (counting from zero) of the block.
   */
  void zg_send_message_at_blockindex(ZGContext *context, const char *receiverName, double blockIndex,
      const char *messageFormat, ...);
  
  /**
   * Send a midi note message on the given channel to occur at the given block index. The
   * <code>blockIndex</code> parameter behaves in the same way as in <code>zg_send_message_at_blockindex()</code>.
   * All messages are sent to <code>notein</code> objects, i.e. omni. Channels are zero-index and only
   * 16 are supported. A note off message is generally interpreted as having velocity zero.
   */
  void zg_send_midinote(ZGContext *context, int channel, int noteNumber, int velocity, double blockIndex);
  
#ifdef __cplusplus
}
#endif

#endif // _ZENGARDEN_H_
