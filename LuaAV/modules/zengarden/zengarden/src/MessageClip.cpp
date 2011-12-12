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

#include "MessageClip.h"

MessageClip::MessageClip(PdMessage *initMessage, PdGraph *graph) : MessageObject(3, 1, graph) {
  if (initMessage->isFloat(0)) {
    if (initMessage->isFloat(1)) {
      init(initMessage->getFloat(0), initMessage->getFloat(1));
    } else {
      init(initMessage->getFloat(0), 0.0f);
    }
  } else {
    init(0.0f, 0.0f);
  }
}

MessageClip::~MessageClip() {
  // nothing to do
}

void MessageClip::init(float lowerBound, float upperBound) {
  this->lowerBound = lowerBound;
  this->upperBound = upperBound;
}

const char *MessageClip::getObjectLabel() {
  return "clip";
}

void MessageClip::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isFloat(0)) {
        float output = message->getFloat(0);
        if (output < lowerBound) {
          output = lowerBound;
        } else if (output > upperBound) {
          output = upperBound;
        }
        PdMessage *outgoingMessage = getNextOutgoingMessage(0);
        outgoingMessage->getElement(0)->setFloat(output);
        outgoingMessage->setTimestamp(message->getTimestamp());
        sendMessage(0, outgoingMessage);
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        lowerBound = message->getFloat(0);
      }
      break;
    }
    case 2: {
      if (message->isFloat(0)) {
        upperBound = message->getFloat(0);
      }
      break;
    }
    default: {
      break;
    }
  }
}
