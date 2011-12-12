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

#include "DspAdc.h"
#include "PdGraph.h"

DspAdc::DspAdc(PdGraph *graph) : DspObject(0, 0, 0, graph->getNumInputChannels(), graph) {
  /* In order to avoid lots of <code>memcpy</code>ing to separate input buffers, all ADCs refer to
   * the same input buffers.
   * This is accomplished by freeing the normally allocated output buffers, and replacing the
   * pointers to them with pointers to the global input buffers. In this way,
   * <code>DspObject</code>s can continue to refer to <code>localDspBufferAtOutlet</code>
   * and still get access to the global audio input buffers.
   */
  free(localDspOutletBuffers);
  localDspOutletBuffers = NULL;
  dspBufferAtOutlet0 = graph->getGlobalDspBufferAtInlet(0);
  for (int i = 1; i < numDspOutlets; i++) {
    dspBufferAtOutlet[i] = graph->getGlobalDspBufferAtInlet(i);
  }
}

DspAdc::~DspAdc() {
  // nothing to do
}

const char *DspAdc::getObjectLabel() {
  return "adc~";
}

void DspAdc::processDsp() {
  // nothing to do
}
