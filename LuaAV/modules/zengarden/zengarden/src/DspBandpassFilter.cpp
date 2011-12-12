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

#include "DspBandpassFilter.h"
#include "PdGraph.h"

DspBandpassFilter::DspBandpassFilter(PdMessage *initMessage, PdGraph *graph) : DspObject(3, 1, 0, 1, graph) {
  sampleRate = graph->getSampleRate();
  tap_0 = 0.0f;
  tap_1 = 0.0f;
  
  centerFrequency = initMessage->isFloat(0) ? initMessage->getFloat(0) : sampleRate/2.0f;
  q = initMessage->isFloat(1) ? initMessage->getFloat(1) : 1.0f;
  calculateFilterCoefficients(centerFrequency, q);
}

DspBandpassFilter::~DspBandpassFilter() {
  // nothing to do
}

const char *DspBandpassFilter::getObjectLabel() {
  return "bp~";
}

void DspBandpassFilter::calculateFilterCoefficients(float f, float q) {
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
  gain = 2.0f * oneminusr * (oneminusr + r * omega);
}

float DspBandpassFilter::sigbp_qcos(float f) {
  if (f >= -(0.5f * M_PI) && f <= (0.5f * M_PI)) {
    float g = f*f;
    return (((g*g*g * (-1.0f/720.0f) + g*g*(1.0f/24.0f)) - g*0.5) + 1);
  } else {
    return 0.0f;
  }
}

void DspBandpassFilter::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: {
      if (message->isSymbol(0, "clear")) {
        tap_0 = 0.0f; // TODO(mhroth): how to handle filter resets?
        tap_1 = 0.0f;
      }
      break;
    }
    case 1: {
      if (message->isFloat(0)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        calculateFilterCoefficients(message->getFloat(0), q);
      }
      break;
    }
    case 2: {
      if (message->isFloat(0)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        calculateFilterCoefficients(centerFrequency, message->getFloat(0));
      }
      break;
    }
    default: {
      break;
    }
  }
}

void DspBandpassFilter::processDspWithIndex(int fromIndex, int toIndex) {
  for (int i = fromIndex; i < toIndex; i++) {
    dspBufferAtOutlet0[i] = dspBufferAtInlet0[i] + (coef1 * tap_0) + (coef2 * tap_1);
    tap_1 = tap_0;
    tap_0 = dspBufferAtOutlet0[i];
    dspBufferAtOutlet0[i] *= gain;
  }
}
