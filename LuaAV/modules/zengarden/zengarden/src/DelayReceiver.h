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

#ifndef _DELAY_RECEIVER_H_
#define _DELAY_RECEIVER_H_

#include "DspObject.h"

class DspDelayWrite;

/**
 * <code>DelayReceiver</code> is a subclass of <code>DspObject</code> which implements some basic
 * functionality which is used to treat <code>DspDelayRead</code> and <code>DspVariableDelay</code>
 * as the same kind of object. This is because both of these objects are connected to
 * <code>DspDelayWrite</code>s in the same way and it is convenient to treat them similarly as well.
 * NOTE(mhroth): In a better world it might make more sense to implement <code>DelayReceiver</code>
 * as an Java-style interface. But this isn't working for me very well in C++... :-/
 */
class DelayReceiver : public DspObject {
  public:
    DelayReceiver(int numMessageInlets, int numDspInlets, int numMessageOutlets, int numDspOutlets, PdGraph *graph);
    virtual ~DelayReceiver();
  
    char *getName();
    void setDelayline(DspDelayWrite *delayline);
    
  protected:
    char *name;
    DspDelayWrite *delayline;
};

#endif // _DELAY_RECEIVER_H_
