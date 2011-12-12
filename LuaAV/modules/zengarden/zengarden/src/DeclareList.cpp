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

#include <string.h>
#include "DeclareList.h"
#include "StaticUtils.h"

DeclareList::DeclareList() : ZGLinkedList() {
  // nothing to do
}

DeclareList::~DeclareList() {
  resetIterator();
  char *path = NULL;
  while ((path = (char *) getNext()) != NULL) {
    free(path);
  }
}

void DeclareList::addPath(char *path) {
  if (isFullPath(path)) {
    // if the path is full, then just add it to the list
    if (hasTrailingSlash(path)) {
      // if a trailing slash exists, then it can be added to the list
      add(StaticUtils::copyString(path));
    } else {
      // if no trailing slash exists, then one must be added
      char *newPath = StaticUtils::concatStrings(path, "/");
      add(newPath);
    }
  } else {
    // if it is not a full path, then make it relative to the root path
    if (hasTrailingSlash(path)) {
      char *newPath = StaticUtils::concatStrings(getRootPath(), path);
      add(newPath);
    } else {
      // if no trailing slash exists, then one should be added
      char *a = StaticUtils::concatStrings(path, "/");
      char *newPath = StaticUtils::concatStrings(getRootPath(), a);
      free(a);
      add(newPath);
    }
  }
}

bool DeclareList::isFullPath(char *path) {
  return (path[0] == '/');
}

bool DeclareList::hasTrailingSlash(char *path) {
  return (path[strlen(path)-1] == '/');
}

char *DeclareList::getRootPath() {
  return (char *) head->data;
}
