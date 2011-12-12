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

#ifndef _DSP_PHASOR_H_
#define _DSP_PHASOR_H_

#include "DspObject.h"

/** [phasor~], [phasor~ float] */
class DspPhasor : public DspObject {

  public:
    DspPhasor(PdMessage *initMessage, PdGraph *graph); // and Phasorillator of default zero frequency
    ~DspPhasor();

    const char *getObjectLabel();

  protected:
    void processMessage(int inletIndex, PdMessage *message);
    void processDspWithIndex(int fromIndex, int toIndex);

  private:
    float sampleRate;
    float frequency; // frequency and phase are stored as integers because they are used
    int phase;     // in for-loops to traverse the lookup table
    float index; // indexes the current place in the lookup table
    static float *phasor_table; // the phasor lookup table
    static int refCount; // a reference counter for phasor table. Now we know when to free it.
  
    double step;
    double xphase;
};

#endif // _DSP_PHASOR_H_
