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
#include "MessageMidiToFrequency.h"

MessageMidiToFrequency::MessageMidiToFrequency(PdGraph *graph) : MessageObject(1, 1, graph) {
  // nothing to do
}

MessageMidiToFrequency::~MessageMidiToFrequency() {
  // nothing to do
}

const char *MessageMidiToFrequency::getObjectLabel() {
  return "mtof";
}

void MessageMidiToFrequency::processMessage(int inletIndex, PdMessage *message) {
  if (message->isFloat(0)) {
    PdMessage *outgoingMessage = getNextOutgoingMessage(0);
    outgoingMessage->setFloat(0, 440.0f * powf(2.0f, (message->getFloat(0) - 69.0f) / 12.0f));
    outgoingMessage->setTimestamp(message->getTimestamp());
    sendMessage(0, outgoingMessage);
  }
}
