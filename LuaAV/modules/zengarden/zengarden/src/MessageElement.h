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

#ifndef _PD_MESSAGE_ELEMENT_H_
#define _PD_MESSAGE_ELEMENT_H_

#include <string.h>
#include "MessageElementType.h"

#define SYMBOL_BUFFER_LENGTH 56

/** Implements a Pd message element. */
class MessageElement {
  
  public:
    MessageElement();
    MessageElement(float newConstant);
    MessageElement(char *newSymbol);
    ~MessageElement();
    
    MessageElementType getType();
    bool isFloat();
    bool isSymbol();
    bool isBang();
    bool isSymbolAnythingOrA();
    bool isSymbolBangOrB();
    bool isSymbolFloatOrF();
    bool isSymbolListOrL();
    bool isSymbolSymbolOrS();
    
    void setFloat(float constant);
    float getFloat();
    
    void setSymbol(char *symbol);
    char *getSymbol();
  
    void setBang();
  
    /** Sets the type of this element to ANYTHING. What that means is otherwise undefined. */
    void setAnything();
  
    /** Sets the type of this element to LIST. What that means is otherwise undefined. */
    void setList();
  
    MessageElement *copy();
  
    bool equals(MessageElement *messageElement);
    
  private:
    MessageElementType currentType;
    float constant;
    char symbol[SYMBOL_BUFFER_LENGTH];
};

#endif // _PD_MESSAGE_ELEMENT_H_
