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

#ifndef _TABLE_RECEIVER_INTERFACE_H_
#define _TABLE_RECEIVER_INTERFACE_H_

#include "MessageTable.h"

/**
 * This class has a similar function to DelayReceiver in that it is an interface
 * for all objects which interact with MessageTables. This includes DspTableRead, DspTablePlay,
 * DspTableRead4, MessageTableRead, and MessageTableWrite.
 */
class TableReceiverInterface {
  public:
    virtual ~TableReceiverInterface() { /* nothing to do */ }

    virtual char *getName() = 0;
  
    virtual void setTable(MessageTable *table) = 0;
};

#endif // _TABLE_RECEIVER_INTERFACE_H_
