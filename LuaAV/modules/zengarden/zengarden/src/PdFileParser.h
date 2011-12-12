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

#ifndef _PD_FILE_PARSER_H_
#define _PD_FILE_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StaticUtils.h"

/**
 * The class parses a Pd file and iterates through each message, returning <code>NULL</code> when 
 * no more are available. Messages are returned as strings (<code>char*</code>), which represent
 * the entire logical message (though the original message may have been broken up over several
 * lines in the file.
 */
class PdFileParser {

  public:
    PdFileParser(char *fullPath);
    ~PdFileParser();
    
    /**
     * Returns the next logical message in the file, or <code>NULL</code> if the end of the file
     * has been reached.
     */
    char *nextMessage();
  
  private:
    /**
     * Reads the next line and puts the contents into the <code>line</code> buffer.
     * A pointer to the <code>line</code> buffer is returned, or <code>NULL</code> if the end of the
     * file has been reached.
     */
    char *nextLine();
  
    FILE *fp;
    char *line;
    static const int LINE_LENGTH = 256;
    char *buffer;
    int bufferLength;
    bool isDone;
};

#endif // _PD_FILE_PARSER_H_
