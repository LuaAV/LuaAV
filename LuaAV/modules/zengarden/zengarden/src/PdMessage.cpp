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

#include <stdio.h>
#include "PdGraph.h"
#include "PdMessage.h"
#include "StaticUtils.h"

#define RES_BUFFER_LENGTH 1024

char *PdMessage::resolutionBuffer = NULL;
int PdMessage::resBufferRefCount = 0;
int PdMessage::globalMessageId = 0;

PdMessage::PdMessage() {
  elementList = new List();
  messageId = globalMessageId++;
  timestamp = 0.0;
  reservationCount = 0;
  
  retainResBuffer();
}

PdMessage::PdMessage(char *initString) {
  elementList = new List();
  messageId = globalMessageId++;
  timestamp = 0.0;
  reservationCount = 0;
  
  retainResBuffer();
  
  // generate the elements by tokenizing the string
  initWithString(initString);
}

PdMessage::PdMessage(char *initString, PdMessage *arguments) {
  elementList = new List();
  messageId = globalMessageId++;
  timestamp = 0.0;
  reservationCount = 0;
  
  retainResBuffer();
  
  // resolve entire string with offset 0 (allow for $0)
  char *buffer = PdMessage::resolveString(initString, arguments, 0);
  
  // generate the elements by tokenizing the string
  initWithString(buffer);
}

void PdMessage::retainResBuffer() {
  PdMessage::resBufferRefCount++;
  if (PdMessage::resolutionBuffer == NULL) {
    PdMessage::resolutionBuffer = (char *) calloc(RES_BUFFER_LENGTH, sizeof(char));
  }
}

void PdMessage::releaseResBuffer() {
  PdMessage::resBufferRefCount--;
  if (PdMessage::resBufferRefCount == 0) {
    free(PdMessage::resolutionBuffer);
    PdMessage::resolutionBuffer = NULL;
  }
}

void PdMessage::initWithString(char *initString) {
  char *token = strtok(initString, " ;");
  if (token != NULL) {
    do {
      if (StaticUtils::isNumeric(token)) {
        addElement(atof(token));
      } else {
        // element is symbolic
        addElement(token);
      }
    } while ((token = strtok(NULL, " ;")) != NULL);
  }
}

PdMessage::~PdMessage() {
  // delete the element list
  MessageElement *messageElement = NULL;
  int i = 0;
  while ((messageElement = (MessageElement *) elementList->getFromBackingArray(i++)) != NULL) {
    delete messageElement;
  }
  delete elementList;
  
  releaseResBuffer();
}

void PdMessage::resolveElement(char *templateString, PdMessage *arguments,
    MessageElement *messageElement) {
  char *buffer = resolveString(templateString, arguments, 1);
  if (StaticUtils::isNumeric(buffer)) {
    messageElement->setFloat(atof(buffer));
  } else {
    messageElement->setSymbol(buffer);
  }
}

char *PdMessage::resolveString(char *initString, PdMessage *arguments, int offset) {
  char *buffer = PdMessage::resolutionBuffer;
  int bufferPos = 0;
  int initPos = 0;
  char *argPos = NULL;
  int numCharsWritten = 0;
  
  if (initString == NULL) {
    buffer[0] = '\0'; // a NULL string input yields a string of length zero
  } else if (arguments == NULL) {
    strcpy(buffer, initString); // NULL arguments returns the original string
  } else {
    int numArguments = arguments->getNumElements();
    while ((argPos = strstr(initString + initPos, "\\$")) != NULL) {
      int numCharsRead = argPos - initString - initPos;
      memcpy(buffer + bufferPos, initString + initPos, numCharsRead);
      bufferPos += numCharsRead;
      initPos += numCharsRead + 3;
      //int argumentIndex = argPos[2] - '0';
      int argumentIndex = 0;
      switch (argPos[2]) {
        case '0': { argumentIndex = 0; break; }
        case '1': { argumentIndex = 1; break; }
        case '2': { argumentIndex = 2; break; }
        case '3': { argumentIndex = 3; break; }
        case '4': { argumentIndex = 4; break; }
        case '5': { argumentIndex = 5; break; }
        case '6': { argumentIndex = 6; break; }
        case '7': { argumentIndex = 7; break; }
        case '8': { argumentIndex = 8; break; }
        case '9': { argumentIndex = 9; break; }
        default: { continue; }
      }
      argumentIndex -= offset;
      if (argumentIndex >= 0 && argumentIndex < numArguments) {
        switch (arguments->getType(argumentIndex)) {
          case FLOAT: {
            numCharsWritten = snprintf(buffer + bufferPos, RES_BUFFER_LENGTH - bufferPos,
                "%g", arguments->getFloat(argumentIndex));
            bufferPos += numCharsWritten;
            if (bufferPos >= 1023) {
              printf("WTF: %s", buffer);
            }
            break;
          }
          case SYMBOL: {
            numCharsWritten = snprintf(buffer + bufferPos, RES_BUFFER_LENGTH - bufferPos,
                "%s", arguments->getSymbol(argumentIndex));
            bufferPos += numCharsWritten;
            if (bufferPos >= 1023) {
              printf("WTF: %s", buffer);
            }
            break;
          }
          default: {
            break;
          }
        }
      }
    }
    
    // no more arguments remaining. copy the remainder of the string including '\0'
    strcpy(buffer + bufferPos, initString + initPos);
  }
  
  return buffer;
}

void PdMessage::resolveSymbolsToType() {
  int numElements = elementList->size();
  for (int i = 0; i < numElements; i++) {
    MessageElement *messageElement = (MessageElement *) elementList->get(i);
    if (messageElement->isSymbol()) {
      if (messageElement->isSymbolSymbolOrS()) {
        // do nothing, but leave the symbol as is
      } else if (messageElement->isSymbolAnythingOrA()) {
        messageElement->setAnything();
      } else if (messageElement->isSymbolBangOrB()) {
        messageElement->setBang();
      } else if (messageElement->isSymbolFloatOrF()) {
        messageElement->setFloat(0.0f);
      } else if (messageElement->isSymbolListOrL()) {
        messageElement->setList();
      } else {
        // if the symbol string is unknown, leave is as ANYTHING
        messageElement->setAnything();
      }
    }
  }
}

int PdMessage::getMessageId() {
  return messageId;
}

int PdMessage::getNumElements() {
  return elementList->size();
}

MessageElement *PdMessage::getElement(int index) {
  return (MessageElement *) elementList->get(index);
}

void PdMessage::setTimestamp(double timestamp) {
  this->timestamp = timestamp;
}

double PdMessage::getTimestamp() {
  return timestamp;
}

#pragma mark -
#pragma mark isElement

bool PdMessage::isFloat(int index) {
  if (index >= 0 && index < elementList->size()) {
    return getElement(index)->isFloat();
  } else {
    return false;
  }
}

bool PdMessage::isSymbol(int index) {
  if (index >= 0 && index < elementList->size()) {
    return getElement(index)->isSymbol();
  } else {
    return false;
  }
}

bool PdMessage::isSymbol(int index, const char *test) {
  if (index >= 0 && index < elementList->size()) {
    MessageElement *messageElement = getElement(index);
    if (messageElement->isSymbol()) {
      return (strcmp(messageElement->getSymbol(), test) == 0);
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool PdMessage::isBang(int index) {
  if (index >= 0 && index < elementList->size()) {
    return getElement(index)->isBang();
  } else {
    return false;
  }
}

MessageElementType PdMessage::getType(int index) {
  if (index >= 0 && index < elementList->size()) {
    return getElement(index)->getType();
  } else {
    return ANYTHING;
  }
}

#pragma mark -
#pragma mark get/setElement

float PdMessage::getFloat(int index) {
  return getElement(index)->getFloat();
}

void PdMessage::setFloat(int index, float value) {
  getElement(index)->setFloat(value);
}

char *PdMessage::getSymbol(int index) {
  return getElement(index)->getSymbol();
}

void PdMessage::setSymbol(int index, char *symbol) {
  getElement(index)->setSymbol(symbol);
}

#pragma mark -
#pragma mark reserve/unreserve

void PdMessage::reserve() {
  reservationCount++;
}

void PdMessage::unreserve() {
  reservationCount--;
}

bool PdMessage::isReserved() {
  return (reservationCount > 0);
}

#pragma mark -
#pragma mark copy/clear

PdMessage *PdMessage::copy() {
  PdMessage *messageCopy = new PdMessage();
  for (int i = 0; i < elementList->size(); i++) {
    MessageElement *messageElement = (MessageElement *) elementList->get(i);
    messageCopy->addElement(messageElement);
  }
  return messageCopy;
}

void PdMessage::clear() {
  elementList->clear();
}

#pragma mark -
#pragma mark toString

char *PdMessage::toString() {
  // http://stackoverflow.com/questions/295013/using-sprintf-without-a-manually-allocated-buffer
  int listlen = elementList->size();
  int lengths[listlen]; // how long is the string of each atom
  char *finalString; // the final buffer we will pass back after concatenating all strings - user should free it
  int size = 0; // the total length of our final buffer
  int pos = 0;
  
  // loop through every element in our list of atoms
  // first loop figures out how long our buffer should be
  // chrism: apparently this might fail under MSVC because of snprintf(NULL) - do we care?
  for (int i = 0; i < listlen; i++) {
    lengths[i] = 0;
    switch (getType(i)) {
      case FLOAT: {
        lengths[i] = snprintf(NULL, 0, "%g", getFloat(i));
        break;
      }
      case BANG: {
        lengths[i] = 4; //snprintf(NULL, 0, "%s", "bang");
        break;
      }
      case SYMBOL: {
        lengths[i] = snprintf(NULL, 0, "%s", getSymbol(i));
        break;
      }
      default: {
        break;
      }
    }
    // total length of our string is each atom plus a space, or \0 on the end
    size += lengths[i] + 1;
  }
  
  // now we do the piecewise concatenation into our final string
  finalString = (char *)malloc(size);
  for (int i = 0; i < listlen; i++) {
    // first element doesn't have a space before it
    if (i > 0) {
      strncat(finalString, " ", 1);
      pos += 1;
    }
    // put a string representation of each atom into the final string
    switch (getType(i)) {
      case FLOAT: {
        snprintf(&finalString[pos], lengths[i] + 1, "%g", getFloat(i));
        break;
      }
      case BANG: {
        snprintf(&finalString[pos], lengths[i] + 1, "%s", "bang");
        break;
      }
      case SYMBOL: {
        snprintf(&finalString[pos], lengths[i] + 1, "%s", getSymbol(i));
        break;
      }
      default: {
        break;
      }
    }
    pos += lengths[i];
  }
  return finalString;
}

#pragma mark -
#pragma mark addElement

void PdMessage::addElement(float f) {
  int numElements = elementList->size();
  MessageElement *messageElement = (MessageElement *) elementList->getFromBackingArray(numElements);
  if (messageElement == NULL) {
    messageElement = new MessageElement(f);
  } else {
    messageElement->setFloat(f);
  }
  elementList->add(messageElement);
}

void PdMessage::addElement(char *symbol) {
  int numElements = elementList->size();
  MessageElement *messageElement = (MessageElement *) elementList->getFromBackingArray(numElements);
  if (messageElement == NULL) {
    messageElement = new MessageElement(symbol);
  } else {
    messageElement->setSymbol(symbol);
  }
  elementList->add(messageElement);
}

void PdMessage::addElement() {
  int numElements = elementList->size();
  MessageElement *messageElement = (MessageElement *) elementList->getFromBackingArray(numElements);
  if (messageElement == NULL) {
    messageElement = new MessageElement();
  } else {
    messageElement->setBang();
  }
  elementList->add(messageElement);
}

void PdMessage::addElement(MessageElement *messageElement) {
  switch (messageElement->getType()) {
    case FLOAT: {
      addElement(messageElement->getFloat());
      break;
    }
    case SYMBOL: {
      addElement(messageElement->getSymbol());
      break;
    }
    case BANG: {
      addElement();
      break;
    }
    default: {
      break;
    }
  }
}
