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

#ifndef _MESSAGE_UNTIL_H_
#define _MESSAGE_UNTIL_H_

#include "MessageObject.h"

class PdGraph;

/** [until] */
class MessageUntil : public MessageObject {
  
  public:
    MessageUntil(PdGraph *graph);
    ~MessageUntil();
    
    const char *getObjectLabel();
    
  private:
    void processMessage(int inletIndex, PdMessage *message);
    
    unsigned int maxIterations;
};

#endif // _MESSAGE_UNTIL_H_
