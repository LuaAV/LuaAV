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

#include <math.h>
#include "DspVCF.h"

DspVCF::DspVCF(int blockSize, int sampleRate, char *initString) : 
    DspMessageInputDspOutputObject(3, 1, blockSize, initString) {
  this->sampleRate = (float) sampleRate;
  calculateFilterCoefficients(this->sampleRate/2.0f, 1.0f); // initialise the filter completely open
  tap_0 = tap_1 = 0.0f;
}

DspVCF::DspVCF(float q, int blockSize, int sampleRate, char *initString) : 
    DspMessageInputDspOutputObject(3, 1, blockSize, initString) {
  this->sampleRate = (float) sampleRate;
  calculateFilterCoefficients(this->sampleRate/2.0f, q);
  tap_0 = tap_1 = 0.0f;
}

DspVCF::~DspVCF() {
  // nothing to do
}

void DspVCF::calculateFilterCoefficients(float f, float q) {
  float r, oneminusr, omega;
  if (f < 0.001f) f = 10.0f;
  if (q < 0.0f) q = 0.0f;
  this->centerFrequency = f;
  this->q = q;
  omega = f * (2.0f * M_PI) / sampleRate;
  if (q < 0.001) oneminusr = 1.0f;
  else oneminusr = omega/q;
  if (oneminusr > 1.0f) oneminusr = 1.0f;
  r = 1.0f - oneminusr;
  coef1 = 2.0f * sigbp_qcos(omega) * r;
  coef2 = - r * r;
  gain = 2 * oneminusr * (oneminusr + r * omega);
}

float DspVCF::sigbp_qcos(float f) {
  if (f >= -(0.5f * M_PI) && f <= (0.5f * M_PI)) {
    float g = f*f;
    return (((g*g*g * (-1.0f/720.0f) + g*g*(1.0f/24.0f)) - g*0.5) + 1);
  } else {
    return 0.0f;
  }
}

inline void DspVCF::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      MessageElement *messageElement = message->getElement(0);
      if (messageElement != NULL && messageElement->getType() == SYMBOL) {
        // TODO(mhroth): how to handle filter resets? What type of message is this?
        tap_0 = tap_1 = 0.0f;
      }
      break;
    }
    case 2: {
      // update the filter resonance
      MessageElement *messageElement = message->getElement(0);
      if (messageElement != NULL && messageElement->getType() == FLOAT) {
        processDspToIndex(message->getBlockIndex());
        calculateFilterCoefficients(centerFrequency, messageElement->getFloat());
        blockIndexOfLastMessage = message->getBlockIndex();
      }
      break;
    }
    default: {
      break;
    }
  }
}

inline void DspVCF::processDspToIndex(int newBlockIndex) {
  // DspVCF only supports signalPresedence == DSP_DSP
  if (signalPresedence == DSP_DSP) {
    float *inputBuffer0 = localDspBufferAtInlet[0];
    float *inputBuffer1 = localDspBufferAtInlet[1];
    float *outputBuffer = localDspBufferAtOutlet[0];
    for (int i = blockIndexOfLastMessage; i < newBlockIndex; i++) {
      calculateFilterCoefficients(inputBuffer1[i], q);
      outputBuffer[i] = inputBuffer0[i] + (coef1 * tap_0) + (coef2 * tap_1);
      tap_1 = tap_0;
      tap_0 = outputBuffer[i];
      outputBuffer[i] *= gain;
    }
  }
}
