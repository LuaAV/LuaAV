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

#ifndef _MESSAGE_LINE_H_
#define _MESSAGE_LINE_H_

#include "MessageObject.h"

class PdGraph;

/** [line], [line f], [line f f] */
class MessageLine : public MessageObject {
  
  public:
    MessageLine(PdMessage *initMessage, PdGraph *graph);
    MessageLine(float initValue, PdGraph *graph);
    MessageLine(float initValue, float grainRate, PdGraph *graph);
    ~MessageLine();

  private:
    void processMessage(int inletIndex, PdMessage *message);
  
    double grainRate;
    float slope;
    float currentValue;
    float targetValue;
    PdMessage *pendingMessage;
    static const double DEFAULT_GRAIN_RATE = 20.0; // 20ms
};

#endif // _MESSAGE_LINE_H_
