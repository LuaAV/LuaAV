/*
 *  Copyright 2009 Reality Jockey, Ltd.
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

#ifndef _DSP_VOLTAGE_CONTROLLED_BAND_PASS_FILTER_H_
#define _DSP_VOLTAGE_CONTROLLED_BAND_PASS_FILTER_H_

#include "DspMessageInputDspOutputObject.h"

/**
 * vcf~, a voltage controlled bandpass filter
 */
class DspVCF : public DspMessageInputDspOutputObject {
  
  public:
    DspVCF(int blockSize, int sampleRate, char *initString);
    DspVCF(float q, int blockSize, int sampleRate, char *initString);
    ~DspVCF();
    
  protected:
    void processMessage(int inletIndex, PdMessage *message);
    void processDspToIndex(int newBlockIndex);
    
  private:
    void calculateFilterCoefficients(float f, float q);
    float sigbp_qcos(float f); // not entirely sure what this is doing. From Pd.
    
    float sampleRate;
    float centerFrequency;
    float q;
    float coef1;
    float coef2;
    float gain;
    float tap_0;
    float tap_1;
};

#endif // _DSP_VOLTAGE_CONTROLLED_BAND_PASS_FILTER_H_
