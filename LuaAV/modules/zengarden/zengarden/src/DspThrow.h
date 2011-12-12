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

#ifndef _DSP_THROW_H_
#define _DSP_THROW_H_

#include "DspObject.h"

class DspCatch;

/**
 * [throw~ symbol]
 * Implements the sending end of a many-to-one audio connection.
 */
class DspThrow : public DspObject {
  
  public:
    DspThrow(PdMessage *initMessage, PdGraph *graph);
    ~DspThrow();
    
    /** Returns a pointer to the current local input buffer. */
    float *getBuffer();
  
    char *getName();
  
    const char *getObjectLabel();
  
    void processMessage(int inletIndex, PdMessage *message);
    
  private:
    void processDspWithIndex(int fromIndex, int toIndex);
  
    char *name;
    float *buffer;
};

#endif // _DSP_THROW_H_
