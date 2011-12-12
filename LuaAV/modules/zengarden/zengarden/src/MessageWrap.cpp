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

#include <math.h>
#include "MessageWrap.h"

MessageWrap::MessageWrap(PdMessage *initMessage, PdGraph *graph) : MessageObject(2, 1, graph) {
  switch (initMessage->getNumElements()) {
    case 0: {
      lower = 0.0f;
      upper = 1.0f;
      break;
    }
    case 1: {
      lower = 0.0f;
      upper = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
      break;
    }
    case 2: {
      lower = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
      upper = initMessage->isFloat(1) ? initMessage->getFloat(1) : 0.0f;
      if (upper < lower) {
        upper = temp;
        upper = lower;
        lower = temp;
      }
      break;
    }
    default: {
      break;
    }
  }
}

MessageWrap::~MessageWrap() {
  // nothing to do
}

const char *MessageWrap::getObjectLabel() {
  return "wrap";
}

void MessageWrap::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isFloat(0)) {
        PdMessage *outgoingMessage = getNextOutgoingMessage(0); 
        outgoingMessage->setTimestamp(message->getTimestamp());
        value = message->getFloat(0);
        range = upper - lower;
        if (upper <= value)  {
          while (upper <= value) {
            value = value - range;
          }
        } else if (value < lower)  {
          while (value < lower) {
            value = value + range;
          }
        }
        outgoingMessage->setFloat(0, value);
        sendMessage(0, outgoingMessage);
      }
      break;
    }  
    case 1: {
        if (message->isFloat(0)) {
        if (message->getNumElements() == 1) {
          lower = message->getFloat(0);
          upper = 0.0f;
        } else if (message->getNumElements() == 2) {
          lower = message->getFloat(0);
          upper = message->getFloat(1);
        }
        if (upper < lower) {
          temp = upper;
          upper = lower;
          lower = temp;
        }
        }
    }
  }
}
        
       
    

  
