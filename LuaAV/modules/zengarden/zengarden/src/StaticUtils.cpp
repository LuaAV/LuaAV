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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StaticUtils.h"

StaticUtils::StaticUtils() {
  // nothing to do
}

StaticUtils::~StaticUtils() {
  // nothing to do
}

char *StaticUtils::copyString(char *str) {
  if (str == NULL) {
    return NULL;
  } else {
    char *newStr = (char *) malloc((strlen(str)+1) * sizeof(char));
    strcpy(newStr, str);
    return newStr;
  }
}

bool StaticUtils::isNumeric(char *str) {
  int length = strlen(str);
  bool foundSign = str[0] == '+' || str[0] == '-';
  bool foundDecimal = false;
  for (int i = foundSign ? 1 : 0; i < length; i++) {
    if (str[i] == '0' ||
        str[i] == '1' ||
        str[i] == '2' ||
        str[i] == '3' ||
        str[i] == '4' ||
        str[i] == '5' ||
        str[i] == '6' ||
        str[i] == '7' ||
        str[i] == '8' ||
        str[i] == '9') {
      continue;
    }
    if (str[i] == '.' && foundDecimal) {
      return false;
    } else if (str[i] == '.' && !foundDecimal) {
      foundDecimal = true;
      continue;
    }
    if (str[i] == '+' || str[i] == '-') {
      return false;
    }
    return false;
  }  
  return true;
}

char *StaticUtils::concatStrings(const char *path0, const char *path1) {
  if (path0 == NULL || path1 == NULL) {
    return NULL;
  } else {
    char *joinedPaths = (char *) malloc((strlen(path0) + strlen(path1) + 1) * sizeof(char));
    strcpy(joinedPaths, path0);
    strcat(joinedPaths, path1);
    return joinedPaths;
  }
}

float StaticUtils::sineApprox(float x) {
  const static float a = 4.0f / M_PI; // 1.273239544735163
  const static float b = 4.0f / (M_PI * M_PI); // 0.405284734569351
  return (a * x) - (b * x) * abs(x);
  // NOTE(mhroth): use for former method instead of the latter for now (unless performance issues arise)
  // in order to avoid type-punning pointer warnings.
  //int y = *(int *)&x | 0x80000000;
  //return (1.273239544735163f * x) - (0.405284734569351f * x) * (*(float *)&y);
}

List *StaticUtils::tokenizeString(char *str, const char *delim) {
  List *tokenizedStrings = new List();
  char *head = str;
  char *tail = NULL;
  while ((tail = strstr(head, delim)) != NULL) {
    int numBytes = tail-head;
    char *nextToken = (char *) malloc(numBytes+1);
    memcpy(nextToken, head, numBytes);
    nextToken[numBytes] = '\0';
    tokenizedStrings->add(nextToken);
    head = tail + strlen(delim);
  }
  if (head < str + strlen(str)) {
    int numBytes = str + strlen(str) - head;
    char *nextToken = (char *) malloc(numBytes+1);
    memcpy(nextToken, head, numBytes);
    nextToken[numBytes] = '\0';
    tokenizedStrings->add(nextToken);
  }
  return tokenizedStrings;
}

void StaticUtils::destroyTokenizedStringList(List *list) {
  for (int i = 0; i < list->size(); i++) {
    char *str = (char *) list->get(i);
    free(str);
  }
  delete list;
}

const char *StaticUtils::messageElementTypeToString(MessageElementType type) {
  switch (type) {
    case FLOAT: {
      return "FLOAT";
    }
    case SYMBOL: {
      return "SYMBOL";
    }
    case BANG: {
      return "BANG";
    }
    case ANYTHING: {
      return "ANYTHING";
    }
    default: {
      return "UNKNOWN TYPE";
    }
  }
}

bool StaticUtils::fileExists(char *path) {
  if (FILE *fp = fopen(path, "r")) {
    fclose(fp);
    return true;
  }
  return false;
}
