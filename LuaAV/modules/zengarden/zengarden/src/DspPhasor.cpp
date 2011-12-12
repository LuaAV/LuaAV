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

#include "DspPhasor.h"
#include "PdGraph.h"

#define UNITBIT32 1572864.  /* 3*2^19; bit 32 has place value 1 */

// initialise the static class variables
float *DspPhasor::phasor_table = NULL;
int DspPhasor::refCount = 0;

union tabfudge {
  double tf_d;
  int tf_i[2];
};

DspPhasor::DspPhasor(PdMessage *initMessage, PdGraph *graph) : DspObject(2, 2, 0, 1, graph) {
  frequency = initMessage->isFloat(0) ? initMessage->getFloat(0) : 0.0f;
  
  step = 1.0f / graph->getSampleRate();
  xphase = 0.0f;

  this->sampleRate = graph->getSampleRate();
  phase = 0.0f;
  index = 0.0f;
  refCount++;
  if (phasor_table == NULL) {
    int sampleRateInt = (int) sampleRate;
    phasor_table = (float *) malloc((sampleRateInt + 1) * sizeof(float));
    phasor_table[0] = 0.0f;
    for (int i = 1; i < sampleRateInt; i++) {
      phasor_table[i] = phasor_table[i-1] + 1.0f / sampleRate;
    }
    phasor_table[sampleRateInt] = phasor_table[0];
  }
}

DspPhasor::~DspPhasor() {
  if (--refCount == 0) {
    free(phasor_table);
    phasor_table = NULL;
  }
}

const char *DspPhasor::getObjectLabel() {
  return "phasor~";
}

void DspPhasor::processMessage(int inletIndex, PdMessage *message) {
  switch (inletIndex) {
    case 0: { // update the frequency
      if (message->isFloat(0)) {
        processDspWithIndex(blockIndexOfLastMessage, graph->getBlockIndex(message));
        frequency = message->getFloat(0);
      }
      break;
    }
    case 1: { // update the phase
      // TODO(mhroth)
      break;
    }
    default: {
      break;
    }
  }
}

void DspPhasor::processDspWithIndex(int fromIndex, int toIndex) {
  switch (signalPrecedence) {
    case DSP_DSP: {
      // TODO(mhroth)
      break;
    }
    case DSP_MESSAGE: {
      for (int i = fromIndex; i < toIndex; index += dspBufferAtInlet0[i++]) {
        if (index < 0.0f) {
          index += sampleRate; // account for negative frequencies
        } else if (index >= sampleRate) {
          index -= sampleRate;
        }
        dspBufferAtOutlet0[i] = phasor_table[(int) index];
      }

      /*
      double dphase = xphase + (double) UNITBIT32;
      
      union tabfudge tf;
      tf.tf_d = UNITBIT32;
      int normhipart = tf.tf_i[1]; // HIOFFSET == 1
      tf.tf_d = dphase;
      
      for (int i = getStartSampleIndex(); i < endSampleIndex; i++) {
        tf.tf_i[1] = normhipart;
        dphase += inputBuffer[i] * step;
        outputBuffer[i] = tf.tf_d - UNITBIT32;
        tf.tf_d = dphase;
      }
      
      tf.tf_i[1] = normhipart;
      xphase = tf.tf_d - UNITBIT32;
      */
      break;
    }
    case MESSAGE_DSP: {
      // TODO(mhroth)
      break;
    }
    case MESSAGE_MESSAGE: {
      for (int i = fromIndex; i < toIndex; i++, index += frequency) {
        if (index < 0.0f) {
          index += sampleRate; // account for negative frequencies
        } else if (index >= sampleRate) {
          index -= sampleRate;
        }
        dspBufferAtOutlet0[i] = phasor_table[(int) index];
      }
      break;
    }
  }
}
