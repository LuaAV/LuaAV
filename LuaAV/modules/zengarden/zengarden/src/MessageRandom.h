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

#ifndef _MESSAGE_RANDOM_H_
#define _MESSAGE_RANDOM_H_

#include "MersenneTwister.h"
#include "MessageObject.h"

class PdGraph;

/** [random], [random float] */
class MessageRandom : public MessageObject {

  public:
    MessageRandom(PdMessage *initMessage, PdGraph *graph);
    MessageRandom(int N, PdGraph *graph);
    ~MessageRandom();

    const char *getObjectLabel();

    bool shouldDistributeMessageToInlets();

  protected:
    void processMessage(int inletIndex, PdMessage *message);

  private:
    int max_inc; // random output is in range [0, max_inc]
    MTRand *twister;
};

#endif // _MESSAGE_RANDOM_H_
