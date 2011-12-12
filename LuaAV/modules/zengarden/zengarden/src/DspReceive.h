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

#ifndef _DSP_RECEIVE_H_
#define _DSP_RECEIVE_H_

#include "DspObject.h"

/** [receive~ symbol], [r~ symbol] */
class DspReceive : public DspObject {
  
  public:
    DspReceive(PdMessage *initMessage, PdGraph *graph);
    ~DspReceive();
    
    /** Set a double-pointer at the send~'s input buffer. */
    void setBuffer(float **buffer);
    char *getName();
    const char *getObjectLabel();
    ObjectType getObjectType();
  
    void processDsp();
  
  private:
    char *name;
    float **sendBuffer; // double-pointer to the associated send~'s input buffer
    float *originalLocalOutletBuffer; // pointer to the original local outlet buffer
};

#endif // _DSP_RECEIVE_H_
