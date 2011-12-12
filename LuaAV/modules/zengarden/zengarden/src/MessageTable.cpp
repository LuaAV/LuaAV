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

#include "MessageTable.h"
#include "PdGraph.h"

#define DEFAULT_BUFFER_LENGTH 1024

MessageTable::MessageTable(PdMessage *initMessage, PdGraph *graph) : RemoteMessageReceiver(0, 0, graph) {
  if (initMessage->isSymbol(0)) {
    name = StaticUtils::copyString(initMessage->getSymbol(0));
    // by default, the buffer length is 1024. The buffer should never be NULL.
    bufferLength = initMessage->isFloat(1) ? (int) initMessage->getFloat(1) : DEFAULT_BUFFER_LENGTH;
    buffer = (float *) calloc(bufferLength, sizeof(float));
  } else {
    name = NULL;
    buffer = NULL;
    bufferLength = 0;
    graph->printErr("Object \"table\" must be initialised with a name.");
  }
}

MessageTable::~MessageTable() {
  free(name);
  free(buffer);
}

const char *MessageTable::getObjectLabel() {
  return "table";
}

ObjectType MessageTable::getObjectType() {
  return MESSAGE_TABLE;
}

float *MessageTable::getBuffer(int *bufferLength) {
  *bufferLength = this->bufferLength;
  return buffer;
}

float *MessageTable::resizeBuffer(int newBufferLength) {
  buffer = (float *) realloc(buffer, newBufferLength);
  if (newBufferLength > bufferLength) {
    memset(buffer+bufferLength, 0, (newBufferLength-bufferLength) * sizeof(float));
  }
  bufferLength = newBufferLength;
  return buffer;
}

void MessageTable::processMessage(int inletIndex, PdMessage *message) {
  // TODO(mhroth): process all of the commands which can be sent to tables
  if (message->isSymbol(0, "read")) {
    if (message->isSymbol(1))  {
      // read the file and fill the table
    }
  } else if (message->isSymbol(0, "write")) {
    // write the contents of the table to file
  } else if (message->isSymbol(0, "normalize")) {
    // normalise the contents of the table to the given value. Default to 1.
    float total = 0.0f;
    for (int i = 0; i < bufferLength; i++) {
      total += buffer[i];
    }
    if (total != 0.0f) {
      float normalisedTotal = message->isFloat(1) ? message->getFloat(1) : 1.0f;
      total /= normalisedTotal;
      for (int i = 0; i < bufferLength; i++) {
        buffer[i] /= total;
      }
    }
  } else if (message->isSymbol(0, "resize")) {
    if (message->isFloat(1)) {
      int newBufferLength = (int) message->getFloat(1);
      if (newBufferLength > 0) {
        resizeBuffer(newBufferLength);
      }
    }
  }
}
