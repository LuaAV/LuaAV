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

#include "PdFileParser.h"

PdFileParser::PdFileParser(char *filePath) {
  fp = fopen(filePath, "r");
  if (fp == NULL) {
    // error condition
    line = NULL;
    isDone = true;
  } else {
    line = (char *) calloc(LINE_LENGTH, sizeof(char));
    nextLine();
    isDone = false;
  }
  buffer = NULL;
}

PdFileParser::~PdFileParser() {
  free(line);
  free(buffer);
  fclose(fp);
  fp = NULL;
}

/*
 * This function creates and destroys lots of memory, basically every time that a new string is
 * considered. I find this a bit dirty and would like to make it one buffer. However, for the time-
 * being, this approach works and is robust.
 */
char *PdFileParser::nextMessage() {
  if (!isDone) {
    free(buffer);
    buffer = StaticUtils::copyString(line); // copy line to buffer 
    while (nextLine() != NULL &&
        !(strncmp(line, "#X", 2) == 0 || strncmp(line, "#N", 2) == 0 || 
        strncmp(line, "#A", 2) == 0)) {
      char *temp = buffer;
      // there is an implied space between lines
      buffer = StaticUtils::concatStrings(buffer, " ");
      free(temp);
      temp = buffer;
      buffer = StaticUtils::concatStrings(buffer, line);
      free(temp);
    }
    return buffer;
  } else {
    return NULL;
  }
}

char *PdFileParser::nextLine() {
  char *out = fgets(line, LINE_LENGTH, fp);
  if (out == NULL) {
    isDone = true;
  } else {
    // remove trailing '\n' from the line
    int length = strlen(line);
    line[length-1] = '\0';
  }
  return out;
}
