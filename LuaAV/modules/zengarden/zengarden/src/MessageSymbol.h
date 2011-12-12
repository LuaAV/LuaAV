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

#ifndef _MESSAGE_SYMBOL_H_
#define _MESSAGE_SYMBOL_H_

#include "MessageObject.h"

class MessageSymbol : public MessageObject {
  
  public:
    MessageSymbol(PdMessage *initMessage, PdGraph *graph);
    MessageSymbol(char *initSymbol, PdGraph *graph);
    ~MessageSymbol();
  
    const char *getObjectLabel();
    
  private:
    void processMessage(int inletIndex, PdMessage *message);

    // a MessageElement is used to store the symbol instead of a normal char array because MessageElement
    // will easily (and slightly more efficiently because it does not reallocate any buffers) store
    // a char array. It is mainly a matter of preference.
    MessageElement *symbol;
};

#endif // _MESSAGE_SYMBOL_H_
