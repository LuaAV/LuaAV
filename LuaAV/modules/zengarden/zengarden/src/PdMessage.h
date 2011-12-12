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

#ifndef _PD_MESSAGE_H_
#define _PD_MESSAGE_H_

#include <stdarg.h>
#include "List.h"
#include "MessageElement.h"
#include "ZGLinkedList.h"

class MessageObject;
class PdGraph;

/** Implements a Pd message. */
class PdMessage {
  
  public:
    /** Creates an empty message. */
    PdMessage();
  
    /** Creates a new message by tokenizing the given string and creating floats and strings. */
    PdMessage(char *initString);
  
    /** Creates a message by tokenizing the given string and immediately resolving it using the given arguments. */
    PdMessage(char *initString, PdMessage *arguments);
  
    ~PdMessage();
  
    /**
     * Resolve the string using the arguments into the element. The string is expected to refer to
     * one message element only, i.e., it contains no spaces, though the underlying resolution
     * algorithm can handle any string.
     */
    static void resolveElement(char *templateString, PdMessage *arguments, MessageElement *messageElement);
  
    /**
     * Resolve arguments in a string with a given arugment list. The returned value is a pointer
     * to the global resolution buffer. This buffer should NOT be freed by the user.
     * The <code>offset</code> parameter adjusts the resolution of the <code>$</code> argument.
     * In the case of message boxes, <code>$0</code> refers to the graph id, which is the first
     * argument in a graph's list of arguments. For all other objects, <code>$1</code> refers
     * to the first (0th) element of the argument. The offset is used to distinguish between these
     * cases, by offsetting the argument index resolution.
     */
    static char *resolveString(char *initString, PdMessage *arguments, int offset);
  
    /**
     * Converts symbolic elements referring to message element types (e.g., float or f) to those
     * types. For instance, if an element has a symbolic value of "float", then that element is
     * converted into one of type <code>FLOAT</code>.
     */
    void resolveSymbolsToType();
  
    MessageElement *getElement(int index);
  
    int getNumElements();
  
    /** Get the global timestamp of this message (in milliseconds). */
    double getTimestamp();
  
    /** Set the global timestamp of this message (in milliseconds). */
    void setTimestamp(double timestamp);
  
    bool isReserved();
    void reserve();
    void unreserve();
  
    /** Returns a copy of the message. */
    PdMessage *copy();
  
    /** The message is cleared of all MessageElements. Its length is 0. */
    void clear();
    
    /**
     * Create a string representation of the message. Suitable for use by the print object.
     * The resulting string must be <code>free()</code>ed by the caller.
     */
    char *toString();
  
    /** Returns the message id, a globally unique identifier for this message. */ 
    int getMessageId();
  
    /** Convenience function to determine if a particular message element is a float. */
    bool isFloat(int index);
    bool isSymbol(int index);
    bool isSymbol(int index, const char *test);
    bool isBang(int index);
    MessageElementType getType(int index);
  
    /**
     * Convenience function to get the float value from a particular message element. The user
     * is responsible for checking that the indexed <code>MessageElement</code> is truly a float.
     * This function does not check for the existence of the message element.
     */
    float getFloat(int index);
    char *getSymbol(int index);
  
    /**
     * Convenience function to set a message element to a float value. This function does not check
     * for the existence of a message element.
     */
    void setFloat(int index, float value);
    void setSymbol(int index, char *symbol);
  
    /** Add a new element to the message, making a copy of the value of the given element. */
    void addElement(MessageElement *messageElement);
  
    /** Add a new float element. */
    void addElement(float f);
  
    /** Add a new symbol element. */
    void addElement(char *symbol);
  
    /** Add a new bang element. */
    void addElement(); // bang

  private:
    /**
     * Adds elements to the message by tokenizing the given string. Is a token is numeric then it is
     * automatically resolved to a float. Otherwise the string is interpreted as a symbol.
     * Meant for use in the constructor.
     */
    void initWithString(char *initString);
  
    /** Increment the reference count of the resolution buffer, also allocating it if necessary. */
    void retainResBuffer();
  
    /** Decrement the reference count of the resolution buffer, also deallocating it if necessary. */
    void releaseResBuffer();
  
    /** A global string buffer used for message resolution. */
    static char *resolutionBuffer;
  
    /**
     * The resolution buffer refernece counter. It is incremented when a new message is created, and
     * decremented when a message is destroyed. If the reference counter goes to zero then the
     * resolution buffer is freed.
     */
    static int resBufferRefCount;
  
    /**
     * A variable keeping track of the current global message id. It is incremented whenever a new message
     * is created.
     */
    static int globalMessageId;
  
    int messageId;
    double timestamp;
    int reservationCount;
    List *elementList;
};

#endif // _PD_MESSAGE_H_
