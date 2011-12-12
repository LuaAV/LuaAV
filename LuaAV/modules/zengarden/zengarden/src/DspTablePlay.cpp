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

#include "DspTablePlay.h"
#include "MessageTable.h"
#include "PdGraph.h"

DspTablePlay::DspTablePlay(PdMessage *initMessage, PdGraph *graph) : DspObject(1, 0, 2, 1, graph) {
  name = initMessage->isSymbol(0) ? StaticUtils::copyString(initMessage->getSymbol(0)) : NULL;
  table = NULL;
  outgoingMessage = NULL;
  //localDspBufferAtOutletReserved = localDspBufferAtOutlet[0];
  currentTableIndex = 0;
  endTableIndex = 0;
}

DspTablePlay::~DspTablePlay() {
  free(name);
  // allow the original audio buffer to be properly freed
  //localDspBufferAtOutlet[0] = localDspBufferAtOutletReserved;
}

const char *DspTablePlay::getObjectLabel() {
  return "tabplay~";
}

ObjectType DspTablePlay::getObjectType() {
  return DSP_TABLE_PLAY;
}

char *DspTablePlay::getName() {
  return name;
}

void DspTablePlay::setTable(MessageTable *aTable) {
  table = aTable;
}

ConnectionType DspTablePlay::getConnectionType(int outletIndex) {
  // the right outlet is a message outlet, otherwise this object is considered to output audio
  return (outletIndex == 1) ? MESSAGE : DSP;
}

void DspTablePlay::sendMessage(int outletIndex, PdMessage *message) {
  MessageObject::sendMessage(outletIndex, message);
  outgoingMessage = NULL;
}

void DspTablePlay::processMessage(int inletIndex, PdMessage *message) {
  switch (message->getType(0)) {
    case FLOAT: {
      processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
      playTable((int) message->getFloat(0),
          message->isFloat(1) ? (int) message->getFloat(1) : -1,
          message->getTimestamp());
      break;
    }
    case SYMBOL: {
      if (message->isSymbol(0, "set") && message->isSymbol(1)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        table = graph->getTable(message->getSymbol(1));
      }
      break;
    }
    case BANG: {
      processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
      playTable(0, -1, message->getTimestamp());
      break;
    }
    default: {
      break;
    }
  }
}

void DspTablePlay::playTable(int startIndex, int duration, double startTime) {
  if (startIndex >= 0 && duration >= -1) {
    if (outgoingMessage != NULL) {
      // if the table is currently playing and there is an outstanding scheduled message, cancel it
      graph->cancelMessage(this, 1, outgoingMessage);
      outgoingMessage = NULL;
    }
    int bufferLength = 0;
    table->getBuffer(&bufferLength);
    if (startIndex < bufferLength) {
      // sanity check that table should be played from a point before it ends
      currentTableIndex = startIndex;
      endTableIndex = (duration == -1) ? bufferLength : startIndex + duration;
      if (endTableIndex > bufferLength) {
        endTableIndex = bufferLength;
      }
      outgoingMessage = getNextOutgoingMessage(1);
      double durationMs = 1000.0 * ((double) (endTableIndex-startIndex)) / (double) graph->getSampleRate();
      outgoingMessage->setTimestamp(startTime + durationMs);
      graph->scheduleMessage(this, 1, outgoingMessage);
    } else {
      currentTableIndex = bufferLength;
    }
  }
}

void DspTablePlay::processDspWithIndex(int fromIndex, int toIndex) {
  /*
  if (table != NULL) {
    int bufferLength = 0;
    float *tableBuffer = table->getBuffer(&bufferLength);
    if (bufferLength < endTableIndex) {
      // in case the table length has been reset while tabplay~ is playing the buffer
      endTableIndex = bufferLength;
    }
    int startSampleIndex = getStartSampleIndex();
    int endSampleIndex = getEndSampleIndex(blockIndex);
    int duration = endSampleIndex - startSampleIndex; // the duration of the output buffer to fill
    // the number of reamining samples in the table buffer
    int remainingTableSamples = endTableIndex - currentTableIndex;
    if (remainingTableSamples <= 0) {
      // if the entire buffer has already been read, fill the output with silence
      localDspBufferAtOutlet[0] = localDspBufferAtOutletReserved;
      memset(localDspBufferAtOutletReserved + startSampleIndex, 0, duration * sizeof(float));
    } else if (startSampleIndex == 0 && endSampleIndex == blockSizeInt &&
        duration <= remainingTableSamples) {
      // if the entire output must be filled and there are more than one buffer's worth of
      // samples still available from the table, just set the output buffer pointer
      localDspBufferAtOutlet[0] = tableBuffer + currentTableIndex;
      currentTableIndex += blockSizeInt;
    } else if (duration <= remainingTableSamples) {
      // if the number of remaining samples in the table is more than the number of samples
      // which need to be read to the output buffer
      localDspBufferAtOutlet[0] = localDspBufferAtOutletReserved;
      memcpy(localDspBufferAtOutletReserved + startSampleIndex,
             tableBuffer + currentTableIndex, duration * sizeof(float));
      currentTableIndex += duration;
    } else {
      // if the number of output buffer samples to fill is larger than the number of remaining table
      // samples, fill the output with the maximum available table samples, and fill in the remainder
      // with zero
      localDspBufferAtOutlet[0] = localDspBufferAtOutletReserved;
      memcpy(localDspBufferAtOutletReserved + startSampleIndex,
             tableBuffer + currentTableIndex, remainingTableSamples * sizeof(float));
      memset(localDspBufferAtOutletReserved + startSampleIndex + remainingTableSamples, 0,
             (duration-remainingTableSamples) * sizeof(float));
      currentTableIndex += duration;
    }
  }
  */
}
