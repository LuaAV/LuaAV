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

#include "MessageLine.h"
#include "PdGraph.h"

MessageLine::MessageLine(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  switch (initMessage->getNumElements()) {
    case 1: {
      if (initMessage->getElement(0)->getType() == FLOAT) {
        currentValue = initMessage->getElement(0)->getFloat();
      } else {
        currentValue = 0.0f;
      }
      grainRate = DEFAULT_GRAIN_RATE;
      slope = 0.0f;
      break;
    }
    case 2: {
      if (initMessage->getElement(0)->getType() == FLOAT &&
          initMessage->getElement(1)->getType() == FLOAT) {
        currentValue = initMessage->getElement(0)->getFloat();
        grainRate = (double) initMessage->getElement(1)->getFloat();
      } else {
        currentValue = 0.0f;
        grainRate = DEFAULT_GRAIN_RATE;
      }
      slope = 0.0f;
      break;
    }
    default: {
      currentValue = 0.0f;
      grainRate = DEFAULT_GRAIN_RATE;
      slope = 0.0f;
      break;
    }
  }
}

MessageLine::MessageLine(float initValue, PdGraph *graph) : MessageObject(1, 1, graph) {
  this->currentValue = initValue;
  grainRate = DEFAULT_GRAIN_RATE;
  slope = 0.0f;
}

MessageLine::MessageLine(float initValue, float grainRate, PdGraph *graph) : MessageObject(1, 1, graph) {
  this->currentValue = initValue;
  this->grainRate = (double) grainRate;
  slope = 0.0f;
}

MessageLine::~MessageLine() {
  // nothing to do
}

void MessageLine::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      switch (message->getNumElements()) {
        case 1: {
          if (message->getElement(0)->getType() == FLOAT) {
            // jump to the given value
            PdMessage *outgoingMessage = getNextOutgoingMessage(0);
            outgoingMessage->setTimestamp(message->getTimestamp());
            
            currentValue = message->getElement(0)->getFloat();
            // TODO(mhroth): cancel any callbacks
            
            outgoingMessage->getElement(0)->setFloat(currentValue);
            sendMessage(0, outgoingMessage);
          }
          break;
        }
        case 2: {
          if (message->getElement(0)->getType() == FLOAT &&
              message->getElement(1)->getType() == FLOAT) {
            // set value and target
            targetValue = message->getElement(0)->getFloat();
            float duration = message->getElement(1)->getFloat();
            slope = (targetValue - currentValue) / duration;
            
            // send the current message
            PdMessage *outgoingMessage = getNextOutgoingMessage(0);
            outgoingMessage->setTimestamp(message->getTimestamp());
            outgoingMessage->getElement(0)->setFloat(currentValue);
            sendMessage(0, outgoingMessage);
            
            // schedule the next message
            pendingMessage = getNextOutgoingMessage(0);
            pendingMessage->setTimestamp(message->getTimestamp() + grainRate);
            pendingMessage->getElement(0)->setFloat(currentValue + slope);
            graph->scheduleMessage(this, 0, pendingMessage);
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case 1: {
      // not sure what to do in this case
      break;
    }
  }
}
