/*
 *  Copyright 2009, 2010 Reality Jockey, Ltd.
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

#ifndef _MESSAGE_LOG_H_
#define _MESSAGE_LOG_H_

#include <math.h>
#include "MessageObject.h"

/** [log] */
class MessageLog : public MessageObject {

  public:
    MessageLog(PdMessage *initMessage, PdGraph *graph);
    MessageLog(PdGraph *graph);
    ~MessageLog();

    const char *getObjectLabel();

  private:
    void processMessage(int inletIndex, PdMessage *message);
};

#endif // _MESSAGE_LOG_H_
