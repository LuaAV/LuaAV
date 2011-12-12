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

#include "DspNoise.h"
#include "PdGraph.h"

DspNoise::DspNoise(PdGraph *graph) : DspObject(1, 0, 0, 1, graph->getBlockSize(), graph) {
  twister = new MTRand(); // use new seed
}

DspNoise::~DspNoise() {
  delete twister;
}

const char *DspNoise::getObjectLabel() {
  return "noise~";
}

void DspNoise::processDsp() {
  for (int i = 0; i < blockSizeInt; i++) {
    dspBufferAtOutlet0[i] = ((float) twister->rand(2.0)) - 1.0f;
  }
}
