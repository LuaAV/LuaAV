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

#include <stdlib.h>
#include <string.h>
#include "List.h"

List::List() {
  maxLength = 1; // initial length of 1
  numElements = 0;
  arrayList = (void **) calloc(maxLength, sizeof(void *));
}

List::~List() {
  free(arrayList);
}

int List::size() {
  return numElements;
}

List *List::add(void *element) {
  if (numElements + 1 > maxLength) {
    growArrayList();
  }
  arrayList[numElements++] = element;
  return this;
}

List *List::add(List *list) {
  while (numElements + list->size() > maxLength) {
    growArrayList();
  }
  memcpy(arrayList + numElements, list->arrayList, list->size() * sizeof(void *));
  numElements += list->size();
  return this;
}

void *List::remove(int index) {
  if (index < 0 || index >= numElements) {
    return NULL; // a no-op
  } else {
    void *object = arrayList[index];
    memmove(arrayList + index, arrayList + index + 1, (numElements-index-1) * sizeof(void *));
    // the last element is set to NULL as it no longer exists in the array
    arrayList[numElements-1] = NULL;
    numElements--;
    return object;
  }
}

void List::clear() {
  numElements = 0;
}

void List::growArrayList() {
  maxLength <<= 1;
  arrayList = (void **) realloc(arrayList, maxLength * sizeof(void *));
  memset(arrayList + numElements, 0, (maxLength - numElements) * sizeof(void *));
}

void *List::get(int index) {
  if (index < 0 || index >= numElements) {
    return NULL;
  } else {
    return arrayList[index];
  }
}

void *List::getFromBackingArray(int index) {
  if (index < 0 || index >= maxLength) {
    return NULL;
  } else {
    return arrayList[index];
  }
}
